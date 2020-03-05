#include "header.h"
#include "Camera.h"

Camera::Camera(HANDLE hProcess, uintptr_t address) {
    process = hProcess;
    moduleAddress = address;
    resolve_camera_pointers();
}

void Camera::release_camera() {
    std::vector<std::vector<BYTE> > originalInfo = {
        {0x66, 0x0F, 0x7F, 0x87, 0x20, 0x03, 0x00, 0x00},
        {0x0F, 0x29, 0x06}
    };
    
    uintptr_t cAddress;

    if (!is_camera_released) {
        for (int i = 0; i < originalInfo.size(); i++) {
            cAddress = moduleAddress + this->offsets[i];
            std::vector<BYTE> nops(originalInfo[i].size(), 0x90);
            writeByteVector(process, cAddress, nops);
        }
        is_camera_released = true;
        std::cout << "Camera Released" << std::endl;
    } else {
        for (int i = 0; i < originalInfo.size(); i++) {
            cAddress = moduleAddress + this->offsets[i];
            writeByteVector(process, cAddress, originalInfo[i]);
        }
        is_camera_released = false;
        std::cout << "Camera Unreleased" << std::endl;
    }


}

__declspec(naked) void shellcode()
{
    // rax -> jmpBackAddress
    // rax+8 -> direccion de la camara
     __asm__ volatile (
         ".intel_syntax noprefix;"
         "lea r11,[rip+0x200];"
         "push rbx;"
         "mov rbx,rcx;"
         "mov [r11+8],rbx;"
         "pop rbx;"
         "movaps xmm1,[rcx+0x00000320];"
         "jmp [r11];"
         "nop;nop;nop;nop;" // ending function signature
        ".att_syntax;"
     );

}

void Camera::resolve_camera_pointers() {

    void *p_func = (void *)shellcode;
    int f_size = 0;
    // calc the size of the function
    for(f_size = 0; *((UINT32 *)(&((unsigned char *)p_func)[f_size])) != 0x90909090; ++f_size);

    // try allocate near module
    void *p_shellcode = nullptr;
    int instruction_size = 7;
    for (int i = 1; p_shellcode == 0; i++)
        p_shellcode = VirtualAllocEx(process, (BYTE*)(moduleAddress - (0x1000 * i)), f_size, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);

    // absolute jmp
    uintptr_t moduleInjectionAddress = (moduleAddress + this->jumpOffset);
    uintptr_t jmpBackAddress = moduleInjectionAddress + instruction_size;

    WriteProcessMemory(process, p_shellcode, (LPCVOID)shellcode, f_size, nullptr);

    // 0x207 -> jmp back address
    // 0x207 + 0x8 -> camera address
    WriteProcessMemory(process, p_shellcode + 0x207, &jmpBackAddress, 8, nullptr);
    pCamera = (uintptr_t)(p_shellcode + 0x207 + 0x8);
    
    std::cout << "pCamera " << pCamera;
    std::cout << " Assigned address " << std::hex << p_shellcode << std::endl;

    // this function hooks with the process the jump.
    hookFunction(process, moduleInjectionAddress, (uintptr_t)p_shellcode, instruction_size);
}

void Camera::move_camera(int xDir, int yDir, int zDir) {
    unsigned int x_offset = 0x320, y_offset = 0x328, z_offset = 0x324;
    uintptr_t camera;
    ReadProcessMemory(process, (LPCVOID)pCamera, &camera, 8, nullptr);

    uintptr_t x_address = camera + x_offset;
    uintptr_t y_address = camera + y_offset;
    uintptr_t z_address = camera + z_offset;
    float x, y, z;

    ReadProcessMemory(process, (BYTE*)x_address, &x, sizeof(x), nullptr);
    ReadProcessMemory(process, (BYTE*)y_address, &y, sizeof(y), nullptr);
    ReadProcessMemory(process, (BYTE*)z_address, &z, sizeof(z), nullptr);

    x += xDir * camera_speed;
    y += yDir * camera_speed;
    z += zDir * camera_speed;

    WriteProcessMemory(process, (BYTE*)x_address, &x, sizeof(x), nullptr);
    WriteProcessMemory(process, (BYTE*)y_address, &y, sizeof(y), nullptr);
    WriteProcessMemory(process, (BYTE*)z_address, &z, sizeof(z), nullptr);
}

void Camera::handle_key_presses() {
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
        release_camera();
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
        panning_enabled = !panning_enabled;
        std::cout << "Panning: " << panning_enabled << std::endl;
        Sleep(500); // TODO: Find a better way of handling boolean key presses.
    }

    if (GetAsyncKeyState(VK_UP) & 0x8000) {
        camera_speed += .01;
        std::cout << "Speed: " << camera_speed << std::endl;
        Sleep(200);
    }

    if (GetAsyncKeyState(VK_DOWN) & 0x8000) {
        camera_speed -= .01;
        if (camera_speed < 0) camera_speed = 0;
        std::cout << "Speed: " << camera_speed << std::endl;
        Sleep(200);
    }

    if (xDir || yDir || zDir)
        move_camera(xDir, yDir, zDir);
    if (!panning_enabled) {
        xDir = 0, yDir = 0, zDir = 0;
    }


    
}