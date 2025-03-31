#include "Libraries.h"

import dxvk.app;
import dxvk.app.win32;
import dxvk.app.directx;
import dxvk.app.vulkan;

int CALLBACK wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nCmdShow) {
    // dxvk::VKApplication sample(1280, 720, L"DXVK Hello Triangle");
    dxvk::DXApplication sample(1280, 720, L"DXVK Hello Triangle");
    return dxvk::Win32Application::Run(reinterpret_cast<dxvk::BaseApplication*>(&sample), hInstance, nCmdShow);
}
