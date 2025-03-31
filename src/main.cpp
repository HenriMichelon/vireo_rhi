#include "Libraries.h"

import dxvk.app;
import dxvk.app.win32;
import dxvk.app.directx;
import dxvk.app.vulkan;

int CALLBACK wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nCmdShow) {
    return dxvk::Win32Application::Run(1280, 720, L"DXVK Hello Triangle", hInstance, nCmdShow);
}
