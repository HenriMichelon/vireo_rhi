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
#include <cstring>
#include <cstdlib>
// #include <wayland-client.h>
// #include <wayland-client-protocol.h>
#endif
export module vireo.platform;


#ifdef _WIN32
    export using PlatformWindowHandle = HWND;
#elifdef __linux__
    export struct PlatformWindowHandleXLIB {
        Display* display;
        Window window;
    };
    export struct PlatformWindowHandleWayland {
        struct wl_display* display;
        struct wl_surface* surface;
    };
    export using PlatformWindowHandle  = void*;

    export bool is_wayland() {
        const char* xdg = getenv("XDG_SESSION_TYPE");
        if (xdg && strcmp(xdg, "wayland") == 0)
            return true;

        if (getenv("WAYLAND_DISPLAY"))
            return true;

        return false;
    }

    export bool is_x11() {
        const char* xdg = getenv("XDG_SESSION_TYPE");
        if (xdg && strcmp(xdg, "x11") == 0)
            return true;

        if (getenv("DISPLAY"))
            return true;

        return false;
    }

#endif

