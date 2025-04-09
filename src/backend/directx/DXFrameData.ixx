/*
* Copyright (c) 2025-present Henri Michelon
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/
module;
#include "DXLibraries.h"

export module vireo.backend.directx.framedata;

import vireo.backend.framedata;

export namespace vireo::backend {

    struct DXFrameData : FrameData {
        UINT64 inFlightFenceValue{0};
    };

}