/*
* Copyright (c) 2025-present Henri Michelon
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/
module;
#include "vireo/backend/vulkan/Libraries.h"
export module vireo.vulkan.descriptors;

import std;
import vireo;

export namespace vireo {

    class VKDescriptorLayout : public DescriptorLayout {
    public:
        VKDescriptorLayout(VkDevice device, const std::wstring& name);

        ~VKDescriptorLayout() override;

        DescriptorLayout& add(DescriptorIndex index, DescriptorType type, size_t count = 1) override;

        void build() override;

        auto getSetLayout() const { return setLayout; }

        auto getDevice() const { return device; }

        const auto& getPoolSizes() const { return poolSizes; }

    private:
        VkDevice device;
        VkDescriptorSetLayout setLayout{nullptr};
        const std::wstring name;
        std::map<DescriptorIndex, VkDescriptorPoolSize> poolSizes;
    };

    class VKDescriptorSet : public DescriptorSet {
    public:
        VKDescriptorSet(const std::shared_ptr<const DescriptorLayout>& layout, const std::wstring& name);

        ~VKDescriptorSet() override;

        void update(DescriptorIndex index, const std::shared_ptr<const Buffer>& buffer) override;

        void update(DescriptorIndex index, const std::shared_ptr<const Image>& image) const override;

        void update(DescriptorIndex index, const std::shared_ptr<const Sampler>& sampler) const override;

        void update(DescriptorIndex index, const std::vector<std::shared_ptr<Buffer>>& buffers) override;

        void update(DescriptorIndex index, const std::vector<std::shared_ptr<Image>>& images) const override;

        void update(DescriptorIndex index, const std::vector<std::shared_ptr<Sampler>>& samplers) const override;

        auto getSet() const { return set; }

        auto getPool() const { return pool; }

    private:
        VkDevice              device;
        VkDescriptorSet       set;
        VkDescriptorPool      pool;
    };

}