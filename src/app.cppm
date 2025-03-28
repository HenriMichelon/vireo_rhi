// https://alextardif.com/DX12Tutorial.html

module;
#include "libraries.h"

export module dx12.app;

export namespace dx12 {

    // Window callback function.
    LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

    void* appInstance{nullptr};

    class Application {
    public:

    };
}