#include "process.h"


class Camera {
    private:
    HANDLE process = 0;
    uintptr_t moduleAddress = 0;
    
    bool is_camera_released = false;
    float camera_speed = 0.05;
    bool panning_enabled = false;

    #ifdef KIWAMI
    std::vector<unsigned int> offsets = {0x309AB3, 0x307A25};
    DWORD jumpOffset = 0x30743F;
    #else
    DWORD jumpOffset = 0x18ABC2;
    std::vector<unsigned int> offsets = {0x18D24F, 0x18B1E5};
    #endif



    void release_camera();
    void move_camera(int x, int y, int z);
    void resolve_camera_pointers();

    uintptr_t pCamera;

    public:
    void handle_key_presses();
    Camera(HANDLE hProcess, uintptr_t address);

};