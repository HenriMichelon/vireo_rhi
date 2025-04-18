/*
* Copyright (c) 2025-present Henri Michelon
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/
module;
#include "vireo/backend/vulkan/Libraries.h"
export module vireo.vulkan.framedata;

import vireo;

import vireo.vulkan.resources;

export namespace vireo {

    struct VKFrameData : FrameData {
        VkSemaphore             imageAvailableSemaphore{VK_NULL_HANDLE};
        VkSemaphoreSubmitInfo   imageAvailableSemaphoreSubmitInfo{};
        VkSemaphore             renderFinishedSemaphore{VK_NULL_HANDLE};
        VkSemaphoreSubmitInfo   renderFinishedSemaphoreSubmitInfo{};
        VkFence                 inFlightFence{VK_NULL_HANDLE};
    };

}