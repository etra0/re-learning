#include <iostream>
#include <vector>
#include "process.h"
#include "Camera.h"

#ifdef KIWAMI
const wchar_t *game_name = L"YakuzaKiwami.exe";
#else
const wchar_t *game_name = L"Yakuza0.exe";
#endif

int wmain(int argc, char** argv) {
    DWORD procId = getProcId(game_name);
    std::cout << "procId " << procId << std::endl;

    // 400000 is for 32 bits, don't forget that!
    uintptr_t moduleBase = getModuleBaseAddress(procId, game_name);
    // std::cout << "moduleBaseAddr " << std::hex << moduleBase << std::endl; 

    HANDLE hProcess = 0;
    hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, procId);

    if (!hProcess)
        return 1;

    // variables for the cheats

    bool mainLoop = true;
    Camera cam(hProcess, moduleBase);

    const char *welcomeMessage = 
        "Usage:\n\n"
        "END: Attach/Deattach the camera\n"
        "U, J: Move along X axis\n"
        "H, K: Move along Y axis\n"
        "Y, I: Move along Z axis\n"
        "UP, DOWN: Increase/Decrease camera speed\n\n"
        "Please check you don't have that keys mapped into the game (unmap controller buttons in settings)\n"
        "For panning:\n"
        "Press F8 then the direction you wish to move. F8 again to deactivate it.\n";
    std::cout << welcomeMessage;

    while (mainLoop) {
        cam.handle_key_presses();

        Sleep(10);
    }
    
    
    return 0;
}