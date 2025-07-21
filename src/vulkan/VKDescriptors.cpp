/*
* Copyright (c) 2025-present Henri Michelon
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/
module;
#include "vireo/backend/vulkan/Libraries.h"
#include <cassert>
module vireo.vulkan.descriptors;

import vireo.tools;

import vireo.vulkan.resources;
import vireo.vulkan.tools;

namespace vireo {

    VKDescriptorLayout::VKDescriptorLayout(const VkDevice device, const bool samplers, const bool dynamic, const std::string& name):
        DescriptorLayout{samplers, dynamic}, device{device}, name{name} {
    }

    DescriptorLayout& VKDescriptorLayout::add(const DescriptorIndex index, const DescriptorType type, const size_t count) {
        if (isSamplers() && type != DescriptorType::SAMPLER) {
            throw Exception("Sampler descriptor layout only accepts SAMPLER resources");
        }
        if ((!isSamplers()) && type == DescriptorType::SAMPLER) {
            throw Exception("Use Sampler descriptor layout for SAMPLER resources");
        }
        if (isDynamicUniform() && type != DescriptorType::UNIFORM_DYNAMIC) {
            throw Exception("Uniform dynamic descriptor layout only accepts UNIFORM_DYNAMIC resources");
        }
        if ((!isDynamicUniform()) && type == DescriptorType::UNIFORM_DYNAMIC) {
            throw Exception("Use uniform dynamic descriptor layout for UNIFORM_DYNAMIC resources");
        }
        poolSizes[index] = {
            .type =
                type == DescriptorType::UNIFORM ? VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER :
                type == DescriptorType::UNIFORM_DYNAMIC ? VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC :
                type == DescriptorType::STORAGE ? VK_DESCRIPTOR_TYPE_STORAGE_BUFFER :
                type == DescriptorType::DEVICE_STORAGE ? VK_DESCRIPTOR_TYPE_STORAGE_BUFFER :
                type == DescriptorType::READWRITE_STORAGE ? VK_DESCRIPTOR_TYPE_STORAGE_BUFFER :
                type == DescriptorType::SAMPLED_IMAGE ? VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE :
                type == DescriptorType::READWRITE_IMAGE ? VK_DESCRIPTOR_TYPE_STORAGE_IMAGE :
                VK_DESCRIPTOR_TYPE_SAMPLER,
            .descriptorCount = static_cast<uint32_t>(count),
        };
        capacity += count;
        return *this;
    }

    void VKDescriptorLayout::build() {
        std::vector<VkDescriptorSetLayoutBinding> bindings;
        for (const auto& poolSize : poolSizes) {
            auto binding = VkDescriptorSetLayoutBinding{
                .binding = poolSize.first,
                .descriptorType = poolSize.second.type,
                .descriptorCount = poolSize.second.descriptorCount,
                .stageFlags = VK_SHADER_STAGE_ALL
            };
            bindings.push_back(binding);
        }

        const auto layoutInfo = VkDescriptorSetLayoutCreateInfo {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
            .bindingCount = static_cast<uint32_t>(bindings.size()),
            .pBindings = bindings.data(),
        };
        vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &setLayout);
#ifdef _DEBUG
        vkSetObjectName(device, reinterpret_cast<uint64_t>(setLayout), VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT,
            "VKDescriptorLayout : " + name);
#endif
    }

    VKDescriptorLayout::~VKDescriptorLayout() {
        vkDestroyDescriptorSetLayout(device, setLayout, nullptr);
    }

    VKDescriptorSet::VKDescriptorSet(
        const std::shared_ptr<const DescriptorLayout>& layout,
        const std::string& name):
        DescriptorSet {layout} {
        const auto vkLayout = static_pointer_cast<const VKDescriptorLayout>(layout);
        const auto setLayout = vkLayout->getSetLayout();
        device = vkLayout->getDevice();

        std::vector<VkDescriptorPoolSize> poolSizes;
        for (const auto &poolSize : vkLayout->getPoolSizes()) {
            poolSizes.push_back(poolSize.second);
        }
        const auto poolInfo = VkDescriptorPoolCreateInfo {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
            .maxSets = static_cast<uint32_t>(vkLayout->getCapacity()),
            .poolSizeCount = static_cast<uint32_t>(poolSizes.size()),
            .pPoolSizes = poolSizes.data(),
        };
        vkCreateDescriptorPool(device, &poolInfo, nullptr, &pool);

        const auto allocInfo = VkDescriptorSetAllocateInfo {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            .descriptorPool = pool,
            .descriptorSetCount = 1,
            .pSetLayouts = &setLayout,
        };
        vkAllocateDescriptorSets(device, &allocInfo, &set);

#ifdef _DEBUG
        vkSetObjectName(device, reinterpret_cast<uint64_t>(pool), VK_OBJECT_TYPE_DESCRIPTOR_POOL,
             "VKDescriptorSet Pool : " + name);
        vkSetObjectName(device, reinterpret_cast<uint64_t>(set), VK_OBJECT_TYPE_DESCRIPTOR_SET,
            "VKDescriptorSet : " + name);
#endif
    }

    VKDescriptorSet::~VKDescriptorSet() {
        vkDestroyDescriptorPool(device, pool, nullptr);
        // vkFreeDescriptorSets(static_cast<const VKDescriptorLayout&>(layout).getDevice(), set, nullptr);
    }

    void VKDescriptorSet::update(const DescriptorIndex index, const Buffer& buffer) {
        assert(!layout->isSamplers());
        const auto& vkBuffer = static_cast<const VKBuffer&>(buffer);
        const auto bufferInfo = VkDescriptorBufferInfo {
            .buffer = vkBuffer.getBuffer(),
            .range = vkBuffer.getInstanceSizeAligned(),
        };
        const auto write = VkWriteDescriptorSet {
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .dstSet = set,
            .dstBinding = index,
            .dstArrayElement = 0,
            .descriptorCount = 1,
            .descriptorType =
                buffer.getType() == BufferType::UNIFORM ?
                layout->isDynamicUniform() ?
                VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC :
                VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER :
                VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
            .pBufferInfo = &bufferInfo,
        };
        vkUpdateDescriptorSets(static_pointer_cast<const VKDescriptorLayout>(layout)->getDevice(), 1, &write, 0, nullptr);
    }

    void VKDescriptorSet::update(const DescriptorIndex index, const Image& image) {
        assert(!layout->isDynamicUniform());
        assert(!layout->isSamplers());
        const auto& vkImage = static_cast<const VKImage&>(image);
        const auto imageInfo = VkDescriptorImageInfo {
            .sampler = VK_NULL_HANDLE,
            .imageView = vkImage.getImageView(),
            .imageLayout = image.isReadWrite() ? VK_IMAGE_LAYOUT_GENERAL : VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        };
        const auto write = VkWriteDescriptorSet {
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .dstSet = set,
            .dstBinding = index,
            .dstArrayElement = 0,
            .descriptorCount = 1,
            .descriptorType = image.isReadWrite() ? VK_DESCRIPTOR_TYPE_STORAGE_IMAGE : VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
            .pImageInfo = &imageInfo,
        };
        vkUpdateDescriptorSets(static_pointer_cast<const VKDescriptorLayout>(layout)->getDevice(), 1, &write, 0, nullptr);
    }

    void VKDescriptorSet::update(const DescriptorIndex index, const Sampler& sampler) {
        assert(layout->isSamplers());
        const auto& vkSampler = static_cast<const VKSampler&>(sampler);
        const auto imageInfo = VkDescriptorImageInfo {
            .sampler = vkSampler.getSampler(),
            .imageView = VK_NULL_HANDLE,
            .imageLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        };
        const auto write = VkWriteDescriptorSet {
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .dstSet = set,
            .dstBinding = index,
            .dstArrayElement = 0,
            .descriptorCount = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER,
            .pImageInfo = &imageInfo,
        };
        vkUpdateDescriptorSets(static_pointer_cast<const VKDescriptorLayout>(layout)->getDevice(), 1, &write, 0, nullptr);
    }

    void VKDescriptorSet::update(const DescriptorIndex index, const std::vector<std::shared_ptr<Buffer>>& buffers) {
        assert(!layout->isDynamicUniform());
        assert(!layout->isSamplers());
        assert(buffers.size() > 0);
        auto type = buffers[0]->getType() == BufferType::UNIFORM ? VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER : VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        auto buffersInfo = std::vector<VkDescriptorBufferInfo>(buffers.size());
        for (int i = 0; i < buffers.size(); i++) {
            const auto& vkBuffer = static_pointer_cast<const VKBuffer>(buffers[i]);
            assert(vkBuffer->getType() ==  buffers[0]->getType());
            buffersInfo[i].buffer = vkBuffer->getBuffer();
            buffersInfo[i].range = vkBuffer->getSize();
        }
        const auto write = VkWriteDescriptorSet {
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .dstSet = set,
            .dstBinding = index,
            .dstArrayElement = 0,
            .descriptorCount = static_cast<uint32_t>(buffersInfo.size()),
            .descriptorType = type,
            .pBufferInfo = buffersInfo.data(),
        };
        vkUpdateDescriptorSets(static_pointer_cast<const VKDescriptorLayout>(layout)->getDevice(), 1, &write, 0, nullptr);
    }

    void VKDescriptorSet::update(const DescriptorIndex index, const std::vector<std::shared_ptr<Image>>& images) {
        assert(!layout->isDynamicUniform());
        assert(!layout->isSamplers());
        assert(images.size() > 0);
        auto imagesInfo = std::vector<VkDescriptorImageInfo>(images.size());
        bool isStorage = false;
        for (int i = 0; i < images.size(); i++) {
            assert(images[i] != nullptr);
            imagesInfo[i].sampler = VK_NULL_HANDLE;
            imagesInfo[i].imageView = static_pointer_cast<const VKImage>(images[i])->getImageView();
            imagesInfo[i].imageLayout = images[i]->isReadWrite() ? VK_IMAGE_LAYOUT_GENERAL : VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            isStorage |= images[i]->isReadWrite();
        }
        const auto write = VkWriteDescriptorSet {
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .dstSet = set,
            .dstBinding = index,
            .dstArrayElement = 0,
            .descriptorCount = static_cast<uint32_t>(imagesInfo.size()),
            .descriptorType = isStorage ? VK_DESCRIPTOR_TYPE_STORAGE_IMAGE : VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
            .pImageInfo = imagesInfo.data(),
        };
        vkUpdateDescriptorSets(static_pointer_cast<const VKDescriptorLayout>(layout)->getDevice(), 1, &write, 0, nullptr);
    }

    void VKDescriptorSet::update(const DescriptorIndex index, const std::vector<std::shared_ptr<Sampler>>&samplers) {
        assert(layout->isSamplers());
        assert(samplers.size() > 0);
        auto imagesInfo = std::vector<VkDescriptorImageInfo>(samplers.size());
        for (int i = 0; i < samplers.size(); i++) {
            imagesInfo[i].sampler = static_pointer_cast<const VKSampler>(samplers[i])->getSampler();
            imagesInfo[i].imageView = VK_NULL_HANDLE;
            imagesInfo[i].imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        }
        const auto write = VkWriteDescriptorSet {
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .dstSet = set,
            .dstBinding = index,
            .dstArrayElement = 0,
            .descriptorCount = static_cast<uint32_t>(imagesInfo.size()),
            .descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER,
            .pImageInfo = imagesInfo.data(),
        };
        vkUpdateDescriptorSets(static_pointer_cast<const VKDescriptorLayout>(layout)->getDevice(), 1, &write, 0, nullptr);

    }
}