/*
 * Copyright (c) 2024-2025 Henri Michelon
 * 
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
*/
module;
#ifdef _WIN32
    #include <window.h>
#elifdef USE_SDL3
    #include <SDL3/SDL_video.h>
#endif
export module vireo.platform;

export namespace vireo {
#ifdef _WIN32
    using PlatformWindowHandle = HWND;
#elifdef USE_SDL3
    using PlatformWindowHandle = SDL_Window*;
#endif
}