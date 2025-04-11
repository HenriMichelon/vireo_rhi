/*
* Copyright (c) 2025-present Henri Michelon
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/
module;
#include "backend/vulkan/Tools.h"
export module vireo.backend.vulkan.descriptors;

import vireo.backend.descriptors;
import vireo.backend.resources;

export namespace vireo::backend {

    class VKDescriptorLayout : public DescriptorLayout {
    public:
        VKDescriptorLayout(VkDevice device, const std::wstring& name);

        ~VKDescriptorLayout() override;

        DescriptorLayout& add(DescriptorIndex index, DescriptorType type, size_t count = 1) override;

        DescriptorLayout& add(DescriptorIndex index, const std::vector<std::shared_ptr<Sampler>>& staticSamplers) override;

        void build() override;

        auto getSetLayout() const { return setLayout; }

        auto getDevice() const { return device; }

        const auto& getPoolSizes() const { return poolSizes; }

    private:
        VkDevice device;
        VkDescriptorSetLayout setLayout{nullptr};
        const std::wstring name;
        std::vector<VkSampler> staticSamplers;
        std::map<DescriptorIndex, VkDescriptorPoolSize> poolSizes;
    };

    class VKDescriptorSet : public DescriptorSet {
    public:
        VKDescriptorSet(const DescriptorLayout& layout, const std::wstring& name);

        ~VKDescriptorSet() override;

        void update(DescriptorIndex index, Buffer& buffer) override;

        void update(DescriptorIndex index, Image& buffer) override;

        auto getSet() const { return set; }

        auto getPool() const { return pool; }

    private:
        VkDevice device;
        VkDescriptorSet       set;
        VkDescriptorPool      pool;
    };

}