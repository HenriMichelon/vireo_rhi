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

    enum class MSAA : uint8_t {
        NONE = 0,
        //! 2x MSAA
        X2   = 1,
        //! 4x MSAA
        X4   = 2,
        //! 8x MSAA
        X8   = 3,
        //! 16x MSAA
        X16  = 4,
        //! 32x MSAA
        X32  = 5,
        //! 64x MSAA
        X64  = 6
    };

    enum class VSyncMode : uint8_t {
        IMMEDIATE = 0,
        VSYNC     = 1,
    };

    enum class Backends {
        DIRECTX,
        VULKAN,
    };

    struct Configuration {
        void*    windowHandle           {nullptr};
        Backends backend                {Backends::DIRECTX};
        //! MSAA samples
        // MSAA             msaa           {MSAA::NONE};
        //! Presentation mode
        VSyncMode        vSyncMode      {VSyncMode::VSYNC};
    };
}