#include <iostream>
#include <vector>
#include "process.h"
#include "Camera.h"

int wmain(int argc, char** argv) {
    DWORD procId = getProcId(L"Yakuza0.exe");
    std::cout << "procId " << procId << std::endl;

    // 400000 es 32 bits, don't forget that!
    uintptr_t moduleBase = getModuleBaseAddress(procId, L"Yakuza0.exe");
    std::cout << "moduleBaseAddr " << std::hex << moduleBase << std::endl; 

    HANDLE hProcess = 0;
    hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, procId);

    if (!hProcess)
        return 1;

    // variables for the cheats
    bool bFreeCamera = false;

    bool mainLoop = true;
    Camera cam(hProcess, moduleBase);


    while (mainLoop) {
        cam.handleKeyPresses();

        Sleep(10);
    }
    
    
    return 0;
}