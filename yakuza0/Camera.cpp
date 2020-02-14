#include "header.h"
#include "Camera.h"

Camera::Camera(HANDLE hProcess, uintptr_t address) {
    process = hProcess;
    moduleAddress = address;
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
            std::cout << "caddress " << std::hex << cAddress << std::endl;
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

void Camera::handleKeyPresses() {
        if (GetAsyncKeyState(VK_END) & 1) {
           releaseCamera();
        }
}