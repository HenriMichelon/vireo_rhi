/*
* Copyright (c) 2025-present Henri Michelon
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/
module;
#include "vireo/backend/vulkan/Libraries.h"
module vireo.vulkan;

import vireo.tools;

import vireo.vulkan.descriptors;
import vireo.vulkan.pipelines;
import vireo.vulkan.tools;

namespace vireo {

    VKVireo::VKVireo() {
        instance = std::make_shared<VKInstance>();
        physicalDevice = std::make_shared<VKPhysicalDevice>(getVKInstance()->getInstance());
        device = std::make_shared<VKDevice>(*getVKPhysicalDevice(), getVKInstance()->getRequestedLayers());
    }

    std::shared_ptr<SwapChain> VKVireo::createSwapChain(
        const ImageFormat format,
        const std::shared_ptr<const SubmitQueue>& submitQueue,
        void* windowHandle,
        const PresentMode presentMode,
        const uint32_t framesInFlight) const {
        return std::make_shared<VKSwapChain>(getVKDevice(),
            static_pointer_cast<const VKSubmitQueue>(submitQueue)->getCommandQueue(),
            windowHandle,
            format,
            presentMode,
            framesInFlight);
    }

    std::shared_ptr<SubmitQueue> VKVireo::createSubmitQueue(
            CommandType commandType,
            const std::wstring& name) const {
        return std::make_shared<VKSubmitQueue>(getVKDevice(), commandType, name);
    }

    std::shared_ptr<VertexInputLayout> VKVireo::createVertexLayout(
           size_t size,
           const std::vector<VertexAttributeDesc>& attributesDescriptions) const {
        return std::make_shared<VKVertexInputLayout>(size, attributesDescriptions);
    }

    std::shared_ptr<Fence> VKVireo::createFence(bool createSignaled, const std::wstring& name) const {
        return std::make_shared<VKFence>(createSignaled, getVKDevice(), name);
    }

    std::shared_ptr<CommandAllocator> VKVireo::createCommandAllocator(CommandType type) const {
        return std::make_shared<VKCommandAllocator>(getVKDevice(), type);
    }

    std::shared_ptr<ShaderModule> VKVireo::createShaderModule(const std::string& fileName) const {
        return std::make_shared<VKShaderModule>(getVKDevice()->getDevice(), fileName);
    }

    std::shared_ptr<PipelineResources> VKVireo::createPipelineResources(
        const std::vector<std::shared_ptr<DescriptorLayout>>& descriptorLayouts,
        const PushConstantsDesc& pushConstant,
        const std::wstring& name) const {
        return std::make_shared<VKPipelineResources>(getVKDevice()->getDevice(), descriptorLayouts, pushConstant, name);
    }

    std::shared_ptr<ComputePipeline> VKVireo::createComputePipeline(
        const std::shared_ptr<PipelineResources>& pipelineResources,
        const std::shared_ptr<const ShaderModule>& shader,
        const std::wstring& name) const {
        return std::make_shared<VKComputePipeline>(getVKDevice()->getDevice(), pipelineResources, shader, name);
    }

    std::shared_ptr<GraphicPipeline> VKVireo::createGraphicPipeline(
        const GraphicPipelineConfiguration& configuration,
        const std::wstring& name) const {
        return std::make_shared<VKGraphicPipeline>(
            getVKDevice(),
            configuration,
            name
        );
    }

    std::shared_ptr<Buffer> VKVireo::createBuffer(
        const BufferType type,
        const size_t size,
        const size_t count,
        const std::wstring& name) const  {
        return std::make_shared<VKBuffer>(
           getVKDevice(), type,
           size, count,
           name);
    }

    std::shared_ptr<Image> VKVireo::createImage(
            const ImageFormat format,
            const uint32_t width,
            const uint32_t height,
            const uint32_t mipLevels,
            const uint32_t arraySize,
            const std::wstring& name) const {
        return std::make_shared<VKImage>(
            getVKDevice(),
            format,
            width,
            height,
            mipLevels,
            arraySize,
            name,
            false,
            false,
            false,
            MSAA::NONE);
    }

    std::shared_ptr<Image> VKVireo::createReadWriteImage(
            const ImageFormat format,
            const uint32_t width,
            const uint32_t height,
            const uint32_t mipLevels,
            const uint32_t arraySize,
            const std::wstring& name) const {
        return std::make_shared<VKImage>(
            getVKDevice(),
            format,
            width,
            height,
            mipLevels,
            arraySize,
            name,
            true,
            false,
            false,
            MSAA::NONE);
    }

    std::shared_ptr<RenderTarget> VKVireo::createRenderTarget(
            const ImageFormat format,
            const uint32_t width,
            const uint32_t height,
            const RenderTargetType type,
            const ClearValue clearValue,
            const MSAA msaa,
            const std::wstring& name) const {
        return std::make_shared<RenderTarget>(
            type,
            std::make_shared<VKImage>(
                getVKDevice(),
                format,
                width,
                height,
                1,
                1,
                name,
                false,
                true,
                type == RenderTargetType::DEPTH,
                msaa));
    }

    std::shared_ptr<RenderTarget> VKVireo::createRenderTarget(
        const std::shared_ptr<const SwapChain>& swapChain,
        const ClearValue clearValue,
        MSAA msaa,
        const std::wstring& name) const {
        return std::make_shared<RenderTarget>(
            RenderTargetType::COLOR,
            std::make_shared<VKImage>(
                getVKDevice(),
                swapChain->getFormat(),
                swapChain->getExtent().width,
                swapChain->getExtent().height,
                1,
                1,
                name,
                false,
                true,
                false,
                msaa));
    }

    void VKVireo::waitIdle() {
        vkDeviceWaitIdle(getVKDevice()->getDevice());
    }

    std::shared_ptr<DescriptorLayout> VKVireo::createDescriptorLayout(
        const std::wstring& name) {
        return std::make_shared<VKDescriptorLayout>(getVKDevice()->getDevice(), name);
    }

    std::shared_ptr<DescriptorLayout> VKVireo::createSamplerDescriptorLayout(
        const std::wstring& name) {
        return std::make_shared<VKDescriptorLayout>(getVKDevice()->getDevice(), name);
    }

    std::shared_ptr<DescriptorSet> VKVireo::createDescriptorSet(
            const std::shared_ptr<const DescriptorLayout>& layout,
            const std::wstring& name) {
        return std::make_shared<VKDescriptorSet>(layout, name);
    }

    std::shared_ptr<Sampler> VKVireo::createSampler(
           Filter minFilter,
           Filter magFilter,
           AddressMode addressModeU,
           AddressMode addressModeV,
           AddressMode addressModeW,
           float minLod,
           float maxLod,
           bool anisotropyEnable,
           MipMapMode mipMapMode) const {
        return std::make_shared<VKSampler>(
            getVKDevice(),
            minFilter, magFilter, addressModeU, addressModeV, addressModeW,
            minLod, maxLod, anisotropyEnable, mipMapMode);
    }

}