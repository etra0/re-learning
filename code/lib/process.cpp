#include "process.h"
/*
 * Some functions were based on the tutorial from Game Hacking's user
 * Rive, here's the tutorial: https://www.youtube.com/watch?v=wiX5LmdD5y
*/
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

// En el caso de que se quiera llegar al base pointer, no olvidar agregar un 0x0 al final del vector.
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
    DWORD protectionBytes;

    int nBytes = source.size();
    BYTE bytesToWrite[nBytes];
    
    for (int i = 0; i < nBytes; bytesToWrite[i] = source[i], i++);

    VirtualProtectEx(hProc, (BYTE*)addr, nBytes, PAGE_EXECUTE_READWRITE, &protectionBytes);
    WriteProcessMemory(hProc, (BYTE*)addr, bytesToWrite, nBytes, nullptr);
    VirtualProtectEx(hProc, (BYTE*)addr, nBytes, protectionBytes, &protectionBytes);
}

// function based on https://www.youtube.com/watch?v=jTl3MFVKSUM
bool hookFunction(HANDLE hProc, uintptr_t toHook, uintptr_t f, int len) {
    if (len < 5)
        return false;
    
    DWORD curProtection;
    VirtualProtectEx(hProc, (BYTE*)toHook, len, PAGE_EXECUTE_READWRITE, &curProtection);

    // Se setean todos los bytes a nop, just in case
    std::vector<BYTE> nops(len, 0x90);
    writeByteVector(hProc, (BYTE*)toHook, nops);

    uintptr_t relativeAddress = ((uintptr_t)f - (uintptr_t)toHook) - 5;

    BYTE instructions[5];
    instructions[0] = 0xE9;
    memcpy(instructions+1, &relativeAddress, sizeof(DWORD));

    WriteProcessMemory(hProc, (BYTE*)toHook, instructions, 5, nullptr);

    VirtualProtectEx(hProc, (BYTE*)toHook, len, curProtection, nullptr);

    return true;
}