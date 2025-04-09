/*
* Copyright (c) 2025-present Henri Michelon
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/
module;
#include "VKLibraries.h"

export module vireo.backend.vulkan.framedata;

import vireo.backend.framedata;

export namespace vireo::backend {

    struct VKFrameData : FrameData {
        VkSemaphore             imageAvailableSemaphore;
        VkSemaphoreSubmitInfo   imageAvailableSemaphoreSubmitInfo;
        VkSemaphore             renderFinishedSemaphore;
        VkSemaphoreSubmitInfo   renderFinishedSemaphoreSubmitInfo;
        VkFence                 inFlightFence;
        uint32_t                imageIndex;
    };

}