#include "header.h"
#include "Camera.h"

Camera::Camera(HANDLE hProcess, uintptr_t address) {
    process = hProcess;
    moduleAddress = address;
    resolveCameraPointers();
}

void Camera::releaseCamera() {
    std::vector<std::vector<BYTE> > originalInfo = {
        {0x66, 0x0F, 0x7F, 0x87, 0x20, 0x03, 0x00, 0x00},
        {0x0F, 0x29, 0x06}
    };
    std::vector<unsigned int> offsets = {0x18D24F, 0x18B1E5};
    
    uintptr_t cAddress;

    if (!isCameraReleased) {
        for (int i = 0; i < originalInfo.size(); i++) {
            cAddress = moduleAddress + offsets[i];
            std::vector<BYTE> nops(originalInfo[i].size(), 0x90);
            writeByteVector(process, cAddress, nops);
        }
        isCameraReleased = true;
        std::cout << "Camera Released" << std::endl;
    } else {
        for (int i = 0; i < originalInfo.size(); i++) {
            cAddress = moduleAddress + offsets[i];
            writeByteVector(process, cAddress, originalInfo[i]);
        }
        isCameraReleased = false;
        std::cout << "Camera Unreleased" << std::endl;
    }


}

__declspec(naked) void shellcode()
{
    // rax -> jmpBackAddress
    // rax+8 -> direccion de la camara
     __asm__ volatile (
         ".intel_syntax noprefix;"
         "lea rax,[rip+0x200];"
         "push rbx;"
         "mov rbx,rcx;"
         "mov [rax+8],rbx;"
         "pop rbx;"
         "movaps xmm1,[rcx+0x00000320];"
         "jmp [rax];"
         "nop;nop;nop;nop;" // ending function signature
        ".att_syntax;"
     );

}

void Camera::resolveCameraPointers() {

    void *pFunc = (void *)shellcode;
    int funcSize = 0;
    // calc the size of the function
    for(funcSize = 0; *((UINT32 *)(&((unsigned char *)pFunc)[funcSize])) != 0x90909090; ++funcSize);

    // try allocate near module
    void *pShellCode = nullptr;
    for (int i = 1; pShellCode == 0; i++)
        pShellCode = VirtualAllocEx(process, (BYTE*)(moduleAddress - (0x1000 * i)), funcSize, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);

    // absolute jmp
    uintptr_t jmpBackAddress = (moduleAddress + 0x18ABC2) + 7;

    WriteProcessMemory(process, pShellCode, (LPCVOID)shellcode, funcSize, nullptr);

    // 0x207 -> jmp back address
    // 0x207 + 0x8 -> camera address
    WriteProcessMemory(process, pShellCode + 0x207, &jmpBackAddress, 8, nullptr);
    pCamera = (uintptr_t)(pShellCode + 0x207 + 0x8);
    
    std::cout << "pCamera " << pCamera;
    std::cout << " Assigned address " << std::hex << pShellCode << std::endl;

    hookFunction(process, (moduleAddress + 0x18ABC2), (uintptr_t)pShellCode, 7);
}

void Camera::moveCamera(int xDir, int yDir, int zDir) {
    unsigned int xOffset = 0x320, yOffset = 0x328, zOffset = 0x324;
    uintptr_t camera;
    ReadProcessMemory(process, (LPCVOID)pCamera, &camera, 8, nullptr);

    uintptr_t xAddress = camera + xOffset;
    uintptr_t yAddress = camera + yOffset;
    uintptr_t zAddress = camera + zOffset;
    float x, y, z;

    ReadProcessMemory(process, (BYTE*)xAddress, &x, sizeof(x), nullptr);
    ReadProcessMemory(process, (BYTE*)yAddress, &y, sizeof(y), nullptr);
    ReadProcessMemory(process, (BYTE*)zAddress, &z, sizeof(z), nullptr);
    // std::cout << "pCamera " << camera;
    // std::cout << "x: " << x << std::endl;
    // std::cout << "y: " << y << std::endl;
    // std::cout << "z: " << z << std::endl;

    x += xDir * cameraSpeed;
    y += yDir * cameraSpeed;
    z += zDir * cameraSpeed;

    WriteProcessMemory(process, (BYTE*)xAddress, &x, sizeof(x), nullptr);
    WriteProcessMemory(process, (BYTE*)yAddress, &y, sizeof(y), nullptr);
    WriteProcessMemory(process, (BYTE*)zAddress, &z, sizeof(z), nullptr);
}

void Camera::handleKeyPresses() {
    enum keys {
        K_U = 0x55,
        K_J = 0x4A,
        K_H = 0x48,
        K_K = 0x4B,
        K_Y = 0x59,
        K_I = 0x49
    };

    static int xDir = 0, yDir = 0, zDir = 0;

    if (GetAsyncKeyState(VK_END) & 0x8000) {
        releaseCamera();
        Sleep(500);
    }

    if (GetAsyncKeyState(K_U) & 0x8000) {
        xDir = 1;;
    }

    if (GetAsyncKeyState(K_J) & 0x8000) {
        xDir = -1;
    }

    if (GetAsyncKeyState(K_H) & 0x8000) {
        yDir = -1;
    }

    if (GetAsyncKeyState(K_K) & 0x8000) {
        yDir = 1;
    }

    if (GetAsyncKeyState(K_Y) & 0x8000) {
        zDir = 1;
    }

    if (GetAsyncKeyState(K_I) & 0x8000) {
        zDir = -1;
    }

    if (GetAsyncKeyState(VK_F8) & 0x8000) {
        panningEnabled = !panningEnabled;
        std::cout << "Panning: " << panningEnabled << std::endl;
        Sleep(500); // TODO: Find a better way of handling boolean key presses.
    }

    if (GetAsyncKeyState(VK_UP) & 0x8000) {
        cameraSpeed += .01;
        std::cout << "Speed: " << cameraSpeed << std::endl;
        Sleep(200);
    }

    if (GetAsyncKeyState(VK_DOWN) & 0x8000) {
        cameraSpeed -= .01;
        if (cameraSpeed < 0) cameraSpeed = 0;
        std::cout << "Speed: " << cameraSpeed << std::endl;
        Sleep(200);
    }

    moveCamera(xDir, yDir, zDir);
    if (!panningEnabled) {
        xDir = 0, yDir = 0, zDir = 0;
    }


    
}