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

void Camera::resolveCameraPointers() {
    uintptr_t baseAddress = 0x0;
    std::vector<unsigned int> offsets = {0xB8, 0x18, 0x8, 0xB8, 0xC8, 0x128, 0x0};
    unsigned int xOffset = 0x320, yOffset = 0x328, zOffset = 0x324;

    baseAddress = followDynamicPointer(process, moduleAddress + 0x01163F30, offsets);

    // std::cout << "Module Base " << std::hex << moduleAddress + 0x01163F30 << std::endl;
    // std::cout << "Base Address " << std::hex << baseAddress << std::endl;
    // std::cout << "New Address " << std::hex << (baseAddress + xOffset) << std::endl;

    xAddress = baseAddress + xOffset;
    yAddress = baseAddress + yOffset;
    zAddress = baseAddress + zOffset;
}

void Camera::moveCamera(int xDir, int yDir, int zDir) {
    float x, y, z;

    ReadProcessMemory(process, (BYTE*)xAddress, &x, sizeof(x), nullptr);
    ReadProcessMemory(process, (BYTE*)yAddress, &y, sizeof(y), nullptr);
    ReadProcessMemory(process, (BYTE*)zAddress, &z, sizeof(z), nullptr);

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
        cameraSpeed += .05;
        std::cout << "Speed: " << cameraSpeed << std::endl;
        Sleep(200);
    }

    if (GetAsyncKeyState(VK_DOWN) & 0x8000) {
        cameraSpeed -= .05;
        if (cameraSpeed < 0) cameraSpeed = 0;
        std::cout << "Speed: " << cameraSpeed << std::endl;
        Sleep(200);
    }

    moveCamera(xDir, yDir, zDir);
    if (!panningEnabled) {
        xDir = 0, yDir = 0, zDir = 0;
    }


    
}