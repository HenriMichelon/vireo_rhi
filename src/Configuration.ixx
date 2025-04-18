/*
 * Copyright (c) 2024-2025 Henri Michelon
 * 
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
*/
module;
#include "vireo/Libraries.h"
export module vireo.config;

export namespace vireo {

    enum class PresentMode : uint8_t {
        IMMEDIATE = 0,
        VSYNC     = 1,
    };

    enum class Backends {
        UNDEFINED,
        DIRECTX,
        VULKAN,
    };

    struct Configuration {
        void*    windowHandle           {nullptr};
        Backends backend                {Backends::DIRECTX};
        //! Presentation mode
        PresentMode        presentMode  {PresentMode::VSYNC};
    };
}