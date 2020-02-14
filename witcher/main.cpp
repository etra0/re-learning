#include <iostream>
#include <vector>
#include "process.h"

int wmain(int argc, char** argv) {
    DWORD procId = getProcId(L"witcher.exe");
    std::cout << "procId " << procId << std::endl;

    // 400000 es 32 bits, don't forget that!
    uintptr_t moduleBase = getModuleBaseAddress(procId, L"witcher.exe");

    HANDLE hProcess = 0;
    hProcess = OpenProcess(PROCESS_ALL_ACCESS, NULL, procId);
    std::cout << hProcess << std::endl;

    // base offset para el primer puntero.
    uintptr_t dynamicPtrBaseAddr = moduleBase + 0x00DC09E4;

    // pointerOffsets dedicados a 
    std::vector<unsigned int> pointerOffsets = {0x4, 0x28c, 0x4, 0x90, 0x14, 0xa40};

    uintptr_t moneyAddr = followDynamicPointer(hProcess, dynamicPtrBaseAddr, pointerOffsets);

    std::cout << "DMA = " << std::hex << dynamicPtrBaseAddr << std::endl;
    std::cout << "moneyAddr = " << std::hex << moneyAddr << std::endl;

    int money = 0;

    ReadProcessMemory(hProcess, (BYTE*)moneyAddr, &money, sizeof(money), nullptr);
    std::cout << "Current money " << std::dec << money << std::endl;

    std::cout << "Press enter to inject new money" << std::endl;

    getchar();

    int newMoney = 0xD34D; // equivalente a $54.189

    WriteProcessMemory(hProcess, (BYTE*)moneyAddr, &newMoney, sizeof(money), nullptr);

    ReadProcessMemory(hProcess, (BYTE*)moneyAddr, &money, sizeof(money), nullptr);
    std::cout << "new ammount " << std::dec << money << " in hex " << std::hex << money << std::endl;

    getchar();
    
    
    return 1;
}