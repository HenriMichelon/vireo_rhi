/*
* Copyright (c) 2025-present Henri Michelon
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/
module;
#include "vireo/Tools.h"
export module vireo.app.win32;

import vireo.app;
import vireo.backend;

namespace vireo {

    export class Win32Application {
    public:
        static int run(
            backend::RenderingBackends backendType,
            UINT width, UINT height,
            const std::wstring& name, HINSTANCE hInstance, int nCmdShow);
        static HWND getHwnd() { return hwnd; }
        static auto& getApp() { return app; }

    protected:
        static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

    private:
        static HWND hwnd;
        static std::unique_ptr<Application> app;
    };
}
