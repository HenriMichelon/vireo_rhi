module;
#include "Libraries.h"
import std;

export module dxvk.app.win32;

import dxvk.app;

namespace dxvk {

    export class Win32Application {
    public:
        static int Run(UINT width, UINT height, std::wstring name, HINSTANCE hInstance, int nCmdShow);
        static HWND GetHwnd() { return m_hwnd; }
        static auto& getApp() { return app; }

    protected:
        static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

    private:
        static HWND m_hwnd;
        static std::unique_ptr<BaseApplication> app;
    };
}
