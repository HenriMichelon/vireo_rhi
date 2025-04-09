/*
* Copyright (c) 2025-present Henri Michelon
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/
#include "Tools.h"

import vireo.app;
import vireo.app.win32;

int CALLBACK wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR lpCmdLine, int nCmdShow) {
    return vireo::Win32Application::run(800, 600, L"Vireo Hello World", hInstance, nCmdShow);
}
