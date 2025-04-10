/*
* Copyright (c) 2025-present Henri Michelon
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/
module;
#include "backend/vulkan/Libraries.h"
export module vireo.backend.vulkan.descriptors;

import vireo.backend.buffer;
import vireo.backend.descriptors;
import vireo.backend.samplers;

export namespace vireo::backend {

    class VKDescriptorSet : public DescriptorSet {
    public:
        VKDescriptorSet(DescriptorType type, VkDevice device, size_t capacity);

        ~VKDescriptorSet() override;

        void update(DescriptorHandle handle, Buffer& buffer) override;

        uint64_t getGPUHandle(DescriptorHandle handle) const override;

        auto getSetLayout() const { return setLayout; }

    private:
        VkDevice              device;
        VkDescriptorPool      pool;
        VkDescriptorSetLayout setLayout;
        VkDescriptorSet       set;
    };

}