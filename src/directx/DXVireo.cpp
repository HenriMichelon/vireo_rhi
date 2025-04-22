/*
* Copyright (c) 2025-present Henri Michelon
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/
module;
#include "vireo/backend/directx/Libraries.h"
module vireo.directx;

import vireo.directx.descriptors;
import vireo.directx.pipelines;
import vireo.directx.resources;

namespace vireo {

    DXVireo::DXVireo(void* windowHandle):
        hWnd{static_cast<HWND>(windowHandle)} {
        instance = make_shared<DXInstance>(hWnd);
        physicalDevice = make_shared<DXPhysicalDevice>(getDXInstance()->getFactory());
        device = make_shared<DXDevice>(getDXPhysicalDevice()->getHardwareAdapter());
    }

    shared_ptr<SwapChain> DXVireo::createSwapChain(
        const ImageFormat format,
        const shared_ptr<const SubmitQueue>& submitQueue,
        const PresentMode presentMode,
        const uint32_t framesInFlight) const {
        return make_shared<DXSwapChain>(
            getDXInstance()->getFactory(),
            getDXDevice(),
            static_pointer_cast<const DXSubmitQueue>(submitQueue)->getCommandQueue(),
            format,
            hWnd,
            presentMode,
            framesInFlight);
    }

    shared_ptr<SubmitQueue> DXVireo::createSubmitQueue(
            CommandType commandType,
            const wstring& name) const {
        return make_shared<DXSubmitQueue>(getDXDevice()->getDevice(), commandType);
    }

    shared_ptr<PipelineResources> DXVireo::createPipelineResources(
        const vector<shared_ptr<DescriptorLayout>>& descriptorLayouts,
        const PushConstantsDesc& pushConstant,
        const wstring& name ) const {
        return make_shared<DXPipelineResources>(getDXDevice()->getDevice(), descriptorLayouts, pushConstant, name);
    }

    shared_ptr<GraphicPipeline> DXVireo::createGraphicPipeline(
        const shared_ptr<PipelineResources>& pipelineResources,
        const shared_ptr<const VertexInputLayout>& vertexInputLayout,
        const shared_ptr<const ShaderModule>& vertexShader,
        const shared_ptr<const ShaderModule>& fragmentShader,
        const GraphicPipelineConfiguration& configuration,
        const wstring& name) const {
        return make_shared<DXGraphicPipeline>(
            getDXDevice()->getDevice(),
            pipelineResources,
            vertexInputLayout,
            vertexShader,
            fragmentShader,
            configuration,
            name);
    }

    shared_ptr<ComputePipeline> DXVireo::createComputePipeline(
        const shared_ptr<PipelineResources>& pipelineResources,
        const shared_ptr<const ShaderModule>& shader,
        const wstring& name) const {
            return make_shared<DXComputePipeline>(
                getDXDevice()->getDevice(),
                pipelineResources,
                shader,
                name);
    }

    shared_ptr<VertexInputLayout> DXVireo::createVertexLayout(
        size_t,
        const vector<VertexAttributeDesc>& attributesDescriptions) const {
        return make_shared<DXVertexInputLayout>(attributesDescriptions);
    }

    shared_ptr<ShaderModule> DXVireo::createShaderModule(const string& fileName) const {
        return make_shared<DXShaderModule>(fileName);
    }

    shared_ptr<Buffer> DXVireo::createBuffer(
        const BufferType type,
        const size_t size,
        const size_t count,
        const wstring& name) const {
        return make_shared<DXBuffer>(getDXDevice()->getDevice(), type, size, count, name);
    }

    shared_ptr<Image> DXVireo::createImage(
        const ImageFormat format,
        const uint32_t width,
        const uint32_t height,
        const uint32_t arraySize,
        const wstring& name) const {
        return make_shared<DXImage>(
            getDXDevice()->getDevice(),
            format,
            width, height,
            arraySize,
            name,
            false, false, false,
            DXImage::defaultClearValue,
            MSAA::NONE);
    }

    shared_ptr<Image> DXVireo::createReadWriteImage(
        const ImageFormat format,
        const uint32_t width,
        const uint32_t height,
        const uint32_t arraySize,
        const wstring& name) const {
            return make_shared<DXImage>(
                getDXDevice()->getDevice(),
                format,
                width, height,
                arraySize,
                name,
                true, false, false,
                DXImage::defaultClearValue,
                MSAA::NONE);
    }

    shared_ptr<RenderTarget> DXVireo::createRenderTarget(
           const ImageFormat format,
           const uint32_t width,
           const uint32_t height,
           const RenderTargetType type,
           const ClearValue clearValue,
           const MSAA msaa,
           const wstring& name) const {
        return make_shared<DXRenderTarget>(
            getDXDevice()->getDevice(),
            make_shared<DXImage>(
                getDXDevice()->getDevice(),
                format,
                width,
                height,
                1,
                name,
                false,
                true,
                type == RenderTargetType::DEPTH,
                clearValue,
                msaa),
            type);
    }

    shared_ptr<RenderTarget> DXVireo::createRenderTarget(
           const shared_ptr<const SwapChain>& swapChain,
           const ClearValue clearValue,
           const MSAA msaa,
           const wstring& name) const {
        return make_shared<DXRenderTarget>(
            getDXDevice()->getDevice(),
            make_shared<DXImage>(
                getDXDevice()->getDevice(),
                swapChain->getFormat(),
                swapChain->getExtent().width,
                swapChain->getExtent().height,
                1,
                name,
                false,
                true,
                false,
                clearValue,
                msaa),
            RenderTargetType::COLOR);
    }
    shared_ptr<DescriptorLayout> DXVireo::createDescriptorLayout(
        const wstring& name) {
        return make_shared<DXDescriptorLayout>(false);
    }

    shared_ptr<DescriptorLayout> DXVireo::createSamplerDescriptorLayout(
        const wstring& name) {
        return make_shared<DXDescriptorLayout>(true);
    }

    shared_ptr<DescriptorSet> DXVireo::createDescriptorSet(
        const shared_ptr<const DescriptorLayout>& layout,
        const wstring& name) {
        return make_shared<DXDescriptorSet>(layout, getDXDevice()->getDevice(), name);
    }

    shared_ptr<Sampler> DXVireo::createSampler(
           Filter minFilter,
           Filter magFilter,
           AddressMode addressModeU,
           AddressMode addressModeV,
           AddressMode addressModeW,
           float minLod,
           float maxLod,
           bool anisotropyEnable,
           MipMapMode mipMapMode) const {
        return make_shared<DXSampler>(
            minFilter, magFilter, addressModeU, addressModeV, addressModeW,
            minLod, maxLod, anisotropyEnable, mipMapMode);
    }

    shared_ptr<Fence> DXVireo::createFence(const wstring& name) const {
        return make_shared<DXFence>(getDXDevice()->getDevice());
    }

    shared_ptr<CommandAllocator> DXVireo::createCommandAllocator(const CommandType type) const {
        return make_shared<DXCommandAllocator>(getDXDevice()->getDevice(), type);
    }

}
