#include "process.h"

DWORD getProcId(const wchar_t* procName) {
    DWORD procId = 0;
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnap == INVALID_HANDLE_VALUE) // case of invalid handle
        return 0;

    PROCESSENTRY32 procEntry;
    procEntry.dwSize = sizeof(procEntry);

    if (Process32First(hSnap, &procEntry)) {
        do {
            if (!_wcsicmp(procEntry.szExeFile, procName)) {
                procId = procEntry.th32ProcessID;
                break;
            }
        } while (Process32Next(hSnap, &procEntry));
    }
    
    CloseHandle(hSnap);
    return procId;
}

uintptr_t getModuleBaseAddress(DWORD procId, const wchar_t* modName) {
    uintptr_t modBaseAddr = 0;
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, procId);
    if (hSnap == INVALID_HANDLE_VALUE)
        return 0;
    
    MODULEENTRY32 modEntry;
    modEntry.dwSize = sizeof(modEntry);
    if (Module32First(hSnap, &modEntry)) {
        do {
            if (!_wcsicmp(modEntry.szModule, modName)) {
                modBaseAddr = (uintptr_t) modEntry.modBaseAddr;
                break;
            }
        } while (Module32Next(hSnap, &modEntry));
    }

    CloseHandle(hSnap);
    return modBaseAddr;
}

uintptr_t followDynamicPointer(HANDLE hProc, uintptr_t ptr, std::vector<unsigned int> offsets) {
    uintptr_t addr = ptr;
    for (unsigned int i = 0; i < offsets.size(); ++i) {
        ReadProcessMemory(hProc, (BYTE*)addr, &addr, sizeof(addr), 0);
        addr += offsets[i];
    }

    return addr;
}


void getByteVector(HANDLE hProc, uintptr_t ptr, std::vector<BYTE> &target, int n) {
    uintptr_t addr = ptr;
    BYTE cValue = 0x0;
    for (unsigned int i = 0; i < n; i++) {
        ReadProcessMemory(hProc, (BYTE*)addr, &cValue, sizeof(cValue), nullptr);
        (target).push_back(cValue);
        addr += 1;
    }
}

void writeByteVector(HANDLE hProc, uintptr_t ptr, std::vector<BYTE> &source) {
    uintptr_t addr = ptr;
    BYTE cValue = 0x0;
    for (unsigned int i = 0; i < source.size(); i++) {
        cValue = source[i];
        WriteProcessMemory(hProc, (BYTE*)addr, &cValue, sizeof(cValue), nullptr);
        addr += 1;
    }
}