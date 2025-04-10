/*
* Copyright (c) 2025-present Henri Michelon
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/
module;
#include "Tools.h"
export module vireo.backend.framedata;

import vireo.backend.descriptors;

export namespace vireo::backend {

    struct FrameData {
        FrameData(const std::vector<std::shared_ptr<DescriptorSet>>& descriptorSets): descriptorSets{descriptorSets} {}

        virtual ~FrameData() = default;

        std::vector<std::shared_ptr<DescriptorSet>> descriptorSets;
    };

}