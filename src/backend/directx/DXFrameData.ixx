module;
#include "VKLibraries.h"

export module dxvk.backend.directx.framedata;

import dxvk.backend.framedata;

export namespace dxvk::backend {

    struct DXFrameData : FrameData {
        UINT64 inFlightFenceValue{0};
    };

}