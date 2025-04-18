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

    VKVireo::VKVireo(const Configuration& configuration) :
    Vireo{configuration},
#ifdef _WIN32
    hWnd{static_cast<HWND>(configuration.windowHandle)}
#endif
    {
        instance = make_shared<VKInstance>();
        physicalDevice = make_shared<VKPhysicalDevice>(getVKInstance()->getInstance(), hWnd);
        device = make_shared<VKDevice>(*getVKPhysicalDevice(), getVKInstance()->getRequestedLayers());
        computeCommandQueue = make_shared<VKSubmitQueue>(getVKDevice(), CommandType::COMPUTE, "Compute");
        graphicCommandQueue = make_shared<VKSubmitQueue>(getVKDevice(), CommandType::GRAPHIC, "Graphic");
        transferCommandQueue = make_shared<VKSubmitQueue>(getVKDevice(), CommandType::TRANSFER, "Transfer");
        swapChain = make_shared<VKSwapChain>(getVKDevice(),
#ifdef _WIN32
            hWnd,
#endif
            configuration.presentMode
        );
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
        const GraphicPipeline::Configuration& configuration,
        const wstring& name) const {
        return make_shared<VKGraphicPipeline>(
            getVKDevice(),
            *getVKSwapChain(),
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
        const size_t alignment,
        const wstring& name) const  {
        return make_shared<VKBuffer>(
           getVKDevice(), type,
           size, count, alignment,
           name);
    }

    shared_ptr<Image> VKVireo::createImage(
            const ImageFormat format,
            const uint32_t width,
            const uint32_t height,
            const wstring& name) const {
        return make_shared<VKImage>(
            getVKDevice(),
            format,
            width,
            height,
            name,
            false,
            false);
    }

    shared_ptr<Image> VKVireo::createReadWriteImage(
            const ImageFormat format,
            const uint32_t width,
            const uint32_t height,
            const wstring& name) const {
        return make_shared<VKImage>(
            getVKDevice(),
            format,
            width,
            height,
            name,
            true,
            false);
    }

    shared_ptr<RenderTarget> VKVireo::createRenderTarget(
            const ImageFormat format,
            const uint32_t width,
            const uint32_t height,
            const wstring& name) const {
        return make_shared<RenderTarget>(
            make_shared<VKImage>(
                getVKDevice(),
                format,
                width,
                height,
                name,
                false,
                true));
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