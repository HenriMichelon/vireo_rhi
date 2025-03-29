module;
#include "Libraries.h"

export module dxvk.win32app;

import dxvk.baseapp;

namespace dxvk {

    export class Win32Application {
    public:
        static int Run(BaseApplication* pSample, HINSTANCE hInstance, int nCmdShow);
        static HWND GetHwnd() { return m_hwnd; }

    protected:
        static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

    private:
        static HWND m_hwnd;
    };
}
