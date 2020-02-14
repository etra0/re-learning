#include "process.h"

class Camera {
    private:
    HANDLE process = 0;
    uintptr_t moduleAddress = 0;
    bool isCameraReleased = false;

    public:
    void releaseCamera();
    void moveCamera();
    void handleKeyPresses();
    Camera(HANDLE hProcess, uintptr_t address);

};