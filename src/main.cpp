#include "Libraries.h"

import dxvk.app;
import dxvk.baseapp;
import dxvk.win32app;

int CALLBACK wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nCmdShow) {
    dxvk::DX12Application sample(1280, 720, L"DXVK Hello Triangle");
    return dxvk::Win32Application::Run(reinterpret_cast<dxvk::BaseApplication*>(&sample), hInstance, nCmdShow);
    return 0;
}
