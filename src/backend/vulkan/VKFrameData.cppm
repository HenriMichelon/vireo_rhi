module;
#include "VKLibraries.h"

export module dxvk.backend.vulkan.framedata;

import dxvk.backend.framedata;

export namespace dxvk::backend {

    struct VKFrameData : FrameData {
        VkSemaphore             imageAvailableSemaphore;
        VkSemaphore             renderFinishedSemaphore;
        uint32_t                imageIndex;
    };

}