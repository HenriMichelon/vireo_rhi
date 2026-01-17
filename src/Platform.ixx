/*
 * Copyright (c) 2024-2025 Henri Michelon
 * 
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
*/
module;
#ifdef _WIN32
#include <window.h>
#elifdef __linux__
#include <X11/Xlib.h>
#endif
export module vireo.platform;

export namespace vireo {

#ifdef _WIN32
    using PlatformWindowHandle = HWND;
#elifdef __linux__
    struct PlatformWindowHandle {
        Display* display;
        Window window;
    };
#endif

}