/*
* Copyright (c) 2025-present Henri Michelon
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/
module;
#include "vireo/backend/vulkan/Tools.h"
export module vireo.backend.vulkan.framedata;

import vireo.backend.framedata;
import vireo.backend.descriptors;

export namespace vireo::backend {

    struct VKFrameData : FrameData {
        VkSemaphore             imageAvailableSemaphore{VK_NULL_HANDLE};
        VkSemaphoreSubmitInfo   imageAvailableSemaphoreSubmitInfo{};
        VkSemaphore             renderFinishedSemaphore{VK_NULL_HANDLE};
        VkSemaphoreSubmitInfo   renderFinishedSemaphoreSubmitInfo{};
        VkFence                 inFlightFence{VK_NULL_HANDLE};
        uint32_t                imageIndex{0};

        VKFrameData(const std::vector<std::shared_ptr<DescriptorSet>>& descriptorSet): FrameData{descriptorSet} {}
    };

}