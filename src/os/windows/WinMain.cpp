/*
* Copyright (c) 2025-present Henri Michelon
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/
#include "vireo/Tools.h"

import vireo.app;
import vireo.app.win32;
import vireo.backend;
import vireo.backend.directx;
import vireo.backend.vulkan;

constexpr auto ID_VULKAN{1001};
constexpr auto ID_DIRECTX{1002};
HBRUSH g_GrayBrush = nullptr;

LRESULT CALLBACK WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case ID_VULKAN:
            PostQuitMessage(ID_VULKAN);
            break;
        case ID_DIRECTX:
            PostQuitMessage(ID_DIRECTX);
            break;
        default: ;
        }
        break;
    case WM_CLOSE:
        PostQuitMessage(IDCANCEL);
        break;
    default: ;
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}

bool dirExists(const std::string& dirName_in) {
    const DWORD ftyp = GetFileAttributesA(dirName_in.c_str());
    return (ftyp != INVALID_FILE_ATTRIBUTES) && (ftyp & FILE_ATTRIBUTE_DIRECTORY);
}


int CALLBACK wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR lpCmdLine, int nCmdShow) {
    if (!dirExists("shaders")) {
        MessageBox(nullptr, L"Shaders directory not found, please run the application from the root of the project", L"Error", MB_OK);
        return 0;
    }

    // SetProcessDPIAware();
    //
    // const auto className = L"ApiSelectorWindow";
    //
    // const WNDCLASS wc {
    //     .lpfnWndProc = WindowProc,
    //     .hInstance = hInstance,
    //     .lpszClassName = className,
    // };
    // RegisterClass(&wc);
    //
    // const HWND hWnd = CreateWindowEx(0,
    //        className,
    //        L"Select Graphics API",
    //        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
    //        CW_USEDEFAULT,
    //        CW_USEDEFAULT,
    //        215,
    //        100,
    //        nullptr,
    //        nullptr,
    //        hInstance,
    //        nullptr);
    //
    // auto hFont = CreateFont(
    //     -MulDiv(9, GetDeviceCaps(GetDC(nullptr), LOGPIXELSY), 72),
    //     0,
    //     0,
    //     0,
    //     FW_NORMAL,
    //     FALSE,
    //     FALSE,
    //     FALSE,
    //     ANSI_CHARSET,
    //     OUT_DEFAULT_PRECIS,
    //     CLIP_DEFAULT_PRECIS,
    //     DEFAULT_QUALITY,
    //     DEFAULT_PITCH | FF_DONTCARE,
    //     L"Segoe UI");
    //
    // const auto hVulkan = CreateWindow(L"BUTTON",
    //       L"Vulkan",
    //       WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
    //       20,
    //       15,
    //       70,
    //       25,
    //       hWnd,
    //       reinterpret_cast<HMENU>(ID_VULKAN),
    //       hInstance,
    //       nullptr);
    // SendMessage(hVulkan, WM_SETFONT, reinterpret_cast<WPARAM>(hFont), TRUE);
    //
    // const auto hDirectX = CreateWindow(L"BUTTON",
    //        L"DirectX",
    //        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
    //        110,
    //        15,
    //        70,
    //        25,
    //        hWnd,
    //        reinterpret_cast<HMENU>(ID_DIRECTX),
    //        hInstance,
    //        nullptr);
    // SendMessage(hDirectX, WM_SETFONT, reinterpret_cast<WPARAM>(hFont), TRUE);
    //
    // RECT rect;
    // GetWindowRect(hWnd, &rect);
    // const auto x = (GetSystemMetrics(SM_CXSCREEN) - (rect.right - rect.left)) / 2;
    // const auto y = (GetSystemMetrics(SM_CYSCREEN) - (rect.bottom - rect.top)) / 2;
    // // SetWindowPos(hWnd, nullptr, x, y, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
    // ShowWindow(hWnd, SW_SHOW);
    // UpdateWindow(hWnd);
    //
    // MSG msg;
    // int result = IDCANCEL;
    // while (GetMessage(&msg, nullptr, 0, 0)) {
    //     TranslateMessage(&msg);
    //     DispatchMessage(&msg);
    // }
    // CloseWindow(hWnd);
    // result = static_cast<int>(msg.wParam);

    std::wstring title{L"Vireo Hello World - "};
    vireo::backend::RenderingBackends backendType{vireo::backend::RenderingBackends::VULKAN}; title.append(L"Vulkan 1.3");
    // vireo::backend::RenderingBackends backendType{vireo::backend::RenderingBackends::DIRECTX}; title.append(L"DirectX 12");

    // vireo::backend::RenderingBackends backendType;
    //
    // switch (result) {
    // case ID_VULKAN: {
    //     backendType = vireo::backend::RenderingBackends::VULKAN;
    //     title.append(L"Vulkan 1.3");
    //     break;
    // }
    // case ID_DIRECTX: {
    //     backendType = vireo::backend::RenderingBackends::DIRECTX;
    //     title.append(L"DirectX 12");
    //     break;
    // }
    // default: {
    //     return 0;
    // }
    // }

    return vireo::Win32Application::run(backendType, 800, 600, title, hInstance, nCmdShow);
}
