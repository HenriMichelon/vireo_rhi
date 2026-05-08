/*
 * Copyright (c) 2024-2025 Henri Michelon
 * 
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
*/
module;
#ifdef _WIN32
    #include <windows.h>
#elifdef USE_SDL3
    #include <SDL3/SDL_video.h>
#endif
export module vireo.platform;

export namespace vireo {

    /** Identifies the host operating system. */
    enum class Platform {
        //! Microsoft Windows
        WINDOWS = 0,
        //! Linux
        LINUX = 1,
    };

#ifdef _WIN32
    /** Opaque native window handle. `HWND` on Windows, `SDL_Window*` on Linux with SDL3, `void*` otherwise. */
    using PlatformWindowHandle = HWND;
    /** Returns the platform the binary was compiled for. */
    consteval Platform getPlatform() { return Platform::WINDOWS; }
#elifdef USE_SDL3
    /** Opaque native window handle. `HWND` on Windows, `SDL_Window*` on Linux with SDL3, `void*` otherwise. */
    using PlatformWindowHandle = SDL_Window*;
#else
    /** Opaque native window handle. `HWND` on Windows, `SDL_Window*` on Linux with SDL3, `void*` otherwise. */
    using PlatformWindowHandle = void*;
#endif

#ifdef __linux__
    /** Returns the platform the binary was compiled for. */
    consteval Platform getPlatform() { return Platform::LINUX; }
#endif

}