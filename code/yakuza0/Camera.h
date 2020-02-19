#include "process.h"

class Camera {
    private:
    HANDLE process = 0;
    uintptr_t moduleAddress = 0;
    
    bool isCameraReleased = false;
    float cameraSpeed = 0.05;
    bool panningEnabled = false;

    void releaseCamera();
    void moveCamera(int x, int y, int z);
    void resolveCameraPointers();

    uintptr_t pCamera;

    public:
    void handleKeyPresses();
    Camera(HANDLE hProcess, uintptr_t address);

};