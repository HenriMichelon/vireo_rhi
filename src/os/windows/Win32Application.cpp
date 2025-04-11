/*
* Copyright (c) 2025-present Henri Michelon
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/
module;
#include "vireo/Tools.h"

module vireo.app.win32;

import vireo.app;

namespace vireo {

    HWND Win32Application::hwnd = nullptr;
    std::unique_ptr<Application> Win32Application::app{};

    int Win32Application::run(backend::RenderingBackends backendType,
                              const UINT width,
                              const UINT height,
                              const std::wstring& name,
                              const HINSTANCE hInstance,
                              const int nCmdShow) {
        // Initialize the window class.
        WNDCLASSEX windowClass = {0};
        windowClass.cbSize = sizeof(WNDCLASSEX);
        windowClass.style = CS_HREDRAW | CS_VREDRAW;
        windowClass.lpfnWndProc = WindowProc;
        windowClass.hInstance = hInstance;
        windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
        windowClass.lpszClassName = L"DXSampleClass";
        RegisterClassEx(&windowClass);

        RECT windowRect = {0, 0, static_cast<LONG>(width), static_cast<LONG>(height)};
        AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

        // Create the window and store a handle to it.
        hwnd = CreateWindow(
            windowClass.lpszClassName,
            name.c_str(),
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            windowRect.right - windowRect.left,
            windowRect.bottom - windowRect.top,
            nullptr,
            // We have no parent window.
            nullptr,
            // We aren't using menus.
            hInstance,
            nullptr);

        RECT rect;
        GetWindowRect(hwnd, &rect);
        const auto x = (GetSystemMetrics(SM_CXSCREEN) - (rect.right - rect.left)) / 2;
        const auto y = (GetSystemMetrics(SM_CYSCREEN) - (rect.bottom - rect.top)) / 2;
        SetWindowPos(hwnd, nullptr, x, y, 0, 0, SWP_NOZORDER | SWP_NOSIZE);

        app = std::make_unique<Application>(backendType, name);
        app->onInit();

        ShowWindow(hwnd, nCmdShow);

        MSG msg = {};
        while (msg.message != WM_QUIT) {
            if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }

        app->onDestroy();

        // Return this part of the WM_QUIT message to Windows.
        return static_cast<char>(msg.wParam);
    }

    // Main message handler for the sample.
    LRESULT CALLBACK Win32Application::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
        auto& pSample = getApp();

        switch (message) {
        case WM_CREATE: {
            // Save the DXSample* passed in to CreateWindow.
            LPCREATESTRUCT pCreateStruct = reinterpret_cast<LPCREATESTRUCT>(lParam);
            SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pCreateStruct->lpCreateParams));
        }
            return 0;

        case WM_KEYDOWN:
            if (pSample) {
                pSample->onKeyDown(static_cast<uint32_t>(wParam));
            }
            return 0;

        case WM_KEYUP:
            if (pSample) {
                pSample->onKeyUp(static_cast<uint32_t>(wParam));
            }
            return 0;

        case WM_PAINT:
            if (pSample) {
                pSample->onUpdate();
                pSample->onRender();
            }
            return 0;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        }

        // Handle any messages the switch statement didn't.
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
}
