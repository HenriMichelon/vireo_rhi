/*
* Copyright (c) 2025-present Henri Michelon
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/
module;
#include "backend/vulkan/Libraries.h"
export module vireo.backend.vulkan.samplers;

import vireo.backend.samplers;
import vireo.backend.vulkan.device;

export namespace vireo::backend {

    class VKSampler : public Sampler {
    public:
        VKSampler(
            const VKPhysicalDevice& physicalDevice,
            VkDevice device,
            Filter minFilter,
            Filter magFilter,
            AddressMode addressModeU,
            AddressMode addressModeV,
            AddressMode addressModeW,
            float minLod = 0.0f,
            float maxLod = 1.0f,
            bool anisotropyEnable = true,
            MipMapMode mipMapMode = MipMapMode::LINEAR);

        ~VKSampler() override;

    private:
        VkDevice  device;
        VkSampler sampler;
    };

}