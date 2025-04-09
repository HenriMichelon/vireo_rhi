/*
* Copyright (c) 2025-present Henri Michelon
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/
module;
#include "DXLibraries.h"

export module dxvk.backend.directx.framedata;

import dxvk.backend.framedata;

export namespace dxvk::backend {

    struct DXFrameData : FrameData {
        UINT64 inFlightFenceValue{0};
    };

}