/*
* Copyright (c) 2025-present Henri Michelon
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/
module;
#include "backend/vulkan/Libraries.h"
export module vireo.backend.vulkan.descriptors;

import vireo.backend.descriptors;
import vireo.backend.resources;

export namespace vireo::backend {

    class VKDescriptorSet : public DescriptorSet {
    public:
        VKDescriptorSet(DescriptorType type, VkDevice device, size_t capacity, const std::wstring& name);

        ~VKDescriptorSet() override;

        void update(DescriptorHandle handle, Buffer& buffer) override;

        auto getSetLayout() const { return setLayout; }

        auto getSet() const { return set; }

    private:
        VkDevice              device;
        VkDescriptorPool      pool;
        VkDescriptorSetLayout setLayout;
        VkDescriptorSet       set;
    };

}