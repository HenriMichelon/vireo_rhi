/*
* Copyright (c) 2025-present Henri Michelon
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/
module;
#include "vireo/Tools.h"
export module vireo.framedata;

import vireo.descriptors;

export namespace vireo {

    struct FrameData {
        FrameData(const vector<shared_ptr<DescriptorSet>>& descriptorSets): descriptorSets{descriptorSets} {}

        virtual ~FrameData() = default;

        vector<shared_ptr<DescriptorSet>> descriptorSets;
    };

}