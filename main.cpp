#include <iostream>
#include <vector>
#include "process.h"

int main(int argc, char** argv) {
    DWORD procId = getProcId(L"witcher.exe");

    uintptr_t moduleBase = getModuleBaseAddress(procId, L"witcher.exe");

    HANDLE hProcess = 0;
    hProcess = OpenProcess(PROCESS_ALL_ACCESS, NULL, procId);

    uintptr_t dynamicPtrBaseAddr = moduleBase + 0x00DC09E4;

    std::vector<unsigned int> moneyOffsets = {0x4, 0x28c, 0x4, 0x90, 0x14, 0xa40};
    uintptr_t moneyAddr = findDMAAddy(hProcess, dynamicPtrBaseAddr, moneyOffsets);

    std::cout << "DMA = " << std::hex << dynamicPtrBaseAddr << std::endl;

    getchar();
    
    
    return 1;
}