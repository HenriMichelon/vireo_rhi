// https://github.com/microsoft/DirectX-Graphics-Samples/tree/master/Samples/Desktop/D3D12HelloWorld/src/HelloTriangle
import dx12.app;
#include "libraries.h"
#include "Win32Application.h"

int CALLBACK wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nCmdShow) {
    dx12::Application sample(1280, 720, L"D3D12 Hello Triangle");
    return Win32Application::Run(&sample, hInstance, nCmdShow);
    return 0;
}
