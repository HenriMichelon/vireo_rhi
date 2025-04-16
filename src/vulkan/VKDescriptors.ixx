/*
* Copyright (c) 2025-present Henri Michelon
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/
module;
#include "vireo/backend/vulkan/Tools.h"
export module vireo.vulkan.descriptors;

import vireo;

export namespace vireo {

    class VKDescriptorLayout : public DescriptorLayout {
    public:
        VKDescriptorLayout(VkDevice device, const wstring& name);

        ~VKDescriptorLayout() override;

        DescriptorLayout& add(DescriptorIndex index, DescriptorType type, size_t count = 1) override;

        void build() override;

        auto getSetLayout() const { return setLayout; }

        auto getDevice() const { return device; }

        const auto& getPoolSizes() const { return poolSizes; }

    private:
        VkDevice device;
        VkDescriptorSetLayout setLayout{nullptr};
        const wstring name;
        map<DescriptorIndex, VkDescriptorPoolSize> poolSizes;
    };

    class VKDescriptorSet : public DescriptorSet {
    public:
        VKDescriptorSet(const shared_ptr<const DescriptorLayout>& layout, const wstring& name);

        ~VKDescriptorSet() override;

        void update(DescriptorIndex index, const shared_ptr<const Buffer>& buffer) const override;

        void update(DescriptorIndex index, const shared_ptr<const Image>& image, bool useByComputeShader = false) const override;

        void update(DescriptorIndex index, const shared_ptr<const Sampler>& sampler) const override;

        void update(DescriptorIndex index, const vector<shared_ptr<Buffer>>& buffers) const override;

        void update(DescriptorIndex index, const vector<shared_ptr<Image>>& images, bool useByComputeShader = false) const override;

        void update(DescriptorIndex index, const vector<shared_ptr<Sampler>>& samplers) const override;

        auto getSet() const { return set; }

        auto getPool() const { return pool; }

    private:
        VkDevice              device;
        VkDescriptorSet       set;
        VkDescriptorPool      pool;
    };

}