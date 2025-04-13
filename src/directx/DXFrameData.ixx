/*
* Copyright (c) 2025-present Henri Michelon
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/
module;
#include "vireo/backend/directx/Libraries.h"
export module vireo.directx.framedata;

import vireo;

export namespace vireo {

    struct DXFrameData : FrameData {
        UINT64 inFlightFenceValue{0};

        DXFrameData(const vector<shared_ptr<DescriptorSet>>& descriptorSet): FrameData{descriptorSet} {}
    };

}