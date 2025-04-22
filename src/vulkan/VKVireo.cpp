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

    VKVireo::VKVireo(void* windowHandle) :
#ifdef _WIN32
    hWnd{static_cast<HWND>(windowHandle)}
#endif
    {
        instance = make_shared<VKInstance>();
        physicalDevice = make_shared<VKPhysicalDevice>(getVKInstance()->getInstance(), hWnd);
        device = make_shared<VKDevice>(*getVKPhysicalDevice(), getVKInstance()->getRequestedLayers());
    }

    shared_ptr<SwapChain> VKVireo::createSwapChain(
        const ImageFormat format,
        const shared_ptr<const SubmitQueue>& submitQueue,
        const PresentMode presentMode,
        const uint32_t framesInFlight) const {
        return make_shared<VKSwapChain>(getVKDevice(),
            static_pointer_cast<const VKSubmitQueue>(submitQueue)->getCommandQueue(),
#ifdef _WIN32
            hWnd,
#endif
            format,
            presentMode,
            framesInFlight);
    }

    shared_ptr<SubmitQueue> VKVireo::createSubmitQueue(
            CommandType commandType,
            const wstring& name) const {
        return make_shared<VKSubmitQueue>(getVKDevice(), commandType, name);
    }

    shared_ptr<VertexInputLayout> VKVireo::createVertexLayout(
           size_t size,
           const vector<VertexAttributeDesc>& attributesDescriptions) const {
        return make_shared<VKVertexInputLayout>(size, attributesDescriptions);
    }

    shared_ptr<Fence> VKVireo::createFence(const wstring& name) const {
        return make_shared<VKFence>(getVKDevice(), name);
    }

    shared_ptr<CommandAllocator> VKVireo::createCommandAllocator(CommandType type) const {
        return make_shared<VKCommandAllocator>(getVKDevice(), type);
    }

    shared_ptr<ShaderModule> VKVireo::createShaderModule(const string& fileName) const {
        return make_shared<VKShaderModule>(getVKDevice()->getDevice(), fileName);
    }

    shared_ptr<PipelineResources> VKVireo::createPipelineResources(
        const vector<shared_ptr<DescriptorLayout>>& descriptorLayouts,
        const PushConstantsDesc& pushConstant,
        const wstring& name) const {
        return make_shared<VKPipelineResources>(getVKDevice()->getDevice(), descriptorLayouts, pushConstant, name);
    }

    shared_ptr<ComputePipeline> VKVireo::createComputePipeline(
        const shared_ptr<PipelineResources>& pipelineResources,
        const shared_ptr<const ShaderModule>& shader,
        const wstring& name) const {
        return make_shared<VKComputePipeline>(getVKDevice()->getDevice(), pipelineResources, shader, name);
    }

    shared_ptr<GraphicPipeline> VKVireo::createGraphicPipeline(
        const shared_ptr<PipelineResources>& pipelineResources,
        const shared_ptr<const VertexInputLayout>& vertexInputLayout,
        const shared_ptr<const ShaderModule>& vertexShader,
        const shared_ptr<const ShaderModule>& fragmentShader,
        const GraphicPipelineConfiguration& configuration,
        const wstring& name) const {
        return make_shared<VKGraphicPipeline>(
            getVKDevice(),
            pipelineResources,
            vertexInputLayout,
            vertexShader,
            fragmentShader,
            configuration,
            name
        );
    }

    shared_ptr<Buffer> VKVireo::createBuffer(
        const BufferType type,
        const size_t size,
        const size_t count,
        const wstring& name) const  {
        return make_shared<VKBuffer>(
           getVKDevice(), type,
           size, count,
           name);
    }

    shared_ptr<Image> VKVireo::createImage(
            const ImageFormat format,
            const uint32_t width,
            const uint32_t height,
            const uint32_t arraySize,
            const wstring& name) const {
        return make_shared<VKImage>(
            getVKDevice(),
            format,
            width,
            height,
            arraySize,
            name,
            false,
            false,
            false,
            MSAA::NONE);
    }

    shared_ptr<Image> VKVireo::createReadWriteImage(
            const ImageFormat format,
            const uint32_t width,
            const uint32_t height,
            const uint32_t arraySize,
            const wstring& name) const {
        return make_shared<VKImage>(
            getVKDevice(),
            format,
            width,
            height,
            arraySize,
            name,
            true,
            false,
            false,
            MSAA::NONE);
    }

    shared_ptr<RenderTarget> VKVireo::createRenderTarget(
            const ImageFormat format,
            const uint32_t width,
            const uint32_t height,
            const RenderTargetType type,
            const ClearValue clearValue,
            const MSAA msaa,
            const wstring& name) const {
        return make_shared<RenderTarget>(
            type,
            make_shared<VKImage>(
                getVKDevice(),
                format,
                width,
                height,
                1,
                name,
                false,
                true,
                type == RenderTargetType::DEPTH,
                msaa));
    }

    shared_ptr<RenderTarget> VKVireo::createRenderTarget(
        const shared_ptr<const SwapChain>& swapChain,
        const ClearValue clearValue,
        MSAA msaa,
        const wstring& name) const {
        return make_shared<RenderTarget>(
            RenderTargetType::COLOR,
            make_shared<VKImage>(
                getVKDevice(),
                swapChain->getFormat(),
                swapChain->getExtent().width,
                swapChain->getExtent().height,
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

    shared_ptr<DescriptorLayout> VKVireo::createDescriptorLayout(
        const wstring& name) {
        return make_shared<VKDescriptorLayout>(getVKDevice()->getDevice(), name);
    }

    shared_ptr<DescriptorLayout> VKVireo::createSamplerDescriptorLayout(
        const wstring& name) {
        return make_shared<VKDescriptorLayout>(getVKDevice()->getDevice(), name);
    }

    shared_ptr<DescriptorSet> VKVireo::createDescriptorSet(
            const shared_ptr<const DescriptorLayout>& layout,
            const wstring& name) {
        return make_shared<VKDescriptorSet>(layout, name);
    }

    shared_ptr<Sampler> VKVireo::createSampler(
           Filter minFilter,
           Filter magFilter,
           AddressMode addressModeU,
           AddressMode addressModeV,
           AddressMode addressModeW,
           float minLod,
           float maxLod,
           bool anisotropyEnable,
           MipMapMode mipMapMode) const {
        return make_shared<VKSampler>(
            getVKDevice(),
            minFilter, magFilter, addressModeU, addressModeV, addressModeW,
            minLod, maxLod, anisotropyEnable, mipMapMode);
    }

}