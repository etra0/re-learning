#include <iostream>
#include <vector>
#include "process.h"

void freeCamera(HANDLE hProcess, uintptr_t moduleAddress) {
    std::vector<std::vector<BYTE> > originalInfo = {
        {0x66, 0x0F, 0x7F, 0x87, 0x20, 0x03, 0x00, 0x00},
        {0x0F, 0x29, 0x06}
    };
    std::vector<unsigned int> offsets = {0x18D24F, 0x18B1E5};
    
    uintptr_t cAddress;

    for (int i = 0; i < originalInfo.size(); i++) {
        cAddress = moduleAddress + offsets[i];
        std::cout << "caddress " << std::hex << cAddress << std::endl;
        std::vector<BYTE> nops(originalInfo[i].size(), 0x90);
        writeByteVector(hProcess, cAddress, nops);
    }

    std::cout << "Noped bytes" << std::endl;

    getchar();

    for (int i = 0; i < originalInfo.size(); i++) {
        cAddress = moduleAddress + offsets[i];
        writeByteVector(hProcess, cAddress, originalInfo[i]);
    }

    std::cout << "restored bytes" << std::endl;

    getchar();
}

int wmain(int argc, char** argv) {
    DWORD procId = getProcId(L"Yakuza0.exe");
    std::cout << "procId " << procId << std::endl;

    // 400000 es 32 bits, don't forget that!
    uintptr_t moduleBase = getModuleBaseAddress(procId, L"Yakuza0.exe");
    std::cout << "moduleBaseAddr " << std::hex << moduleBase << std::endl; 

    HANDLE hProcess = 0;
    hProcess = OpenProcess(PROCESS_ALL_ACCESS, NULL, procId);

    freeCamera(hProcess, moduleBase);
    
    return 1;
}