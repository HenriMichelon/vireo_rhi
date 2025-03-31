module;
#include "Libraries.h"
import std;

export module dxvk.app.win32;

import dxvk.app;

namespace dxvk {

    export class Win32Application {
    public:
        static int run(UINT width, UINT height, std::wstring name, HINSTANCE hInstance, int nCmdShow);
        static HWND getHwnd() { return hwnd; }
        static auto& getApp() { return app; }

    protected:
        static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

    private:
        static HWND hwnd;
        static std::unique_ptr<BaseApplication> app;
    };
}
