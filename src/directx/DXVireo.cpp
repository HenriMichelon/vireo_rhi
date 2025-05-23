/*
* Copyright (c) 2025-present Henri Michelon
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/
module;
#include "vireo/backend/directx/Libraries.h"
module vireo.directx;

import vireo.directx.pipelines;
import vireo.directx.resources;

namespace vireo {

    DXVireo::DXVireo(const uint32_t maxDescriptors, const uint32_t maxSamplers)  {
        instance = std::make_shared<DXInstance>();
        physicalDevice = std::make_shared<DXPhysicalDevice>(getDXInstance()->getFactory());
        device = std::make_shared<DXDevice>(getDXPhysicalDevice()->getHardwareAdapter());
        cbvSrvUavDescriptorHeap = std::make_shared<DXDescriptorHeap>(
            getDXDevice()->getDevice(),
            D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
            maxDescriptors);
        samplerDescriptorHeap = std::make_shared<DXDescriptorHeap>(
            getDXDevice()->getDevice(),
            D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER,
            maxSamplers);
    }

    std::shared_ptr<SwapChain> DXVireo::createSwapChain(
        const ImageFormat format,
        const std::shared_ptr<const SubmitQueue>& submitQueue,
        void* windowHandle,
        const PresentMode presentMode,
        const uint32_t framesInFlight) const {
        return std::make_shared<DXSwapChain>(
            getDXInstance()->getFactory(),
            getDXDevice(),
            static_pointer_cast<const DXSubmitQueue>(submitQueue)->getCommandQueue(),
            format,
            static_cast<HWND>(windowHandle),
            presentMode,
            framesInFlight);
    }

    std::shared_ptr<SubmitQueue> DXVireo::createSubmitQueue(
            CommandType commandType,
            const std::wstring& name) const {
        return std::make_shared<DXSubmitQueue>(getDXDevice()->getDevice(), commandType);
    }

    std::shared_ptr<PipelineResources> DXVireo::createPipelineResources(
        const std::vector<std::shared_ptr<DescriptorLayout>>& descriptorLayouts,
        const PushConstantsDesc& pushConstant,
        const std::wstring& name ) const {
        return std::make_shared<DXPipelineResources>(getDXDevice()->getDevice(), descriptorLayouts, pushConstant, name);
    }

    std::shared_ptr<GraphicPipeline> DXVireo::createGraphicPipeline(
        const GraphicPipelineConfiguration& configuration,
        const std::wstring& name) const {
        return std::make_shared<DXGraphicPipeline>(
            getDXDevice()->getDevice(),
            configuration,
            name);
    }

    std::shared_ptr<ComputePipeline> DXVireo::createComputePipeline(
        const std::shared_ptr<PipelineResources>& pipelineResources,
        const std::shared_ptr<const ShaderModule>& shader,
        const std::wstring& name) const {
            return std::make_shared<DXComputePipeline>(
                getDXDevice()->getDevice(),
                pipelineResources,
                shader,
                name);
    }

    std::shared_ptr<VertexInputLayout> DXVireo::createVertexLayout(
        size_t,
        const std::vector<VertexAttributeDesc>& attributesDescriptions) const {
        return std::make_shared<DXVertexInputLayout>(attributesDescriptions);
    }

    std::shared_ptr<ShaderModule> DXVireo::createShaderModule(const std::string& fileName) const {
        return std::make_shared<DXShaderModule>(fileName);
    }

    std::shared_ptr<Buffer> DXVireo::createBuffer(
        const BufferType type,
        const size_t size,
        const size_t count,
        const std::wstring& name) const {
        return std::make_shared<DXBuffer>(getDXDevice()->getDevice(), type, size, count, name);
    }

    std::shared_ptr<Image> DXVireo::createImage(
        const ImageFormat format,
        const uint32_t width,
        const uint32_t height,
        const uint32_t mipLevels,
        const uint32_t arraySize,
        const std::wstring& name) const {
        return std::make_shared<DXImage>(
            getDXDevice()->getDevice(),
            format,
            width, height,
            mipLevels,
            arraySize,
            name,
            false, false, false,
            DXImage::defaultClearValue,
            MSAA::NONE);
    }

    std::shared_ptr<Image> DXVireo::createReadWriteImage(
        const ImageFormat format,
        const uint32_t width,
        const uint32_t height,
        const uint32_t mipLevels,
        const uint32_t arraySize,
        const std::wstring& name) const {
            return std::make_shared<DXImage>(
                getDXDevice()->getDevice(),
                format,
                width, height,
                mipLevels,
                arraySize,
                name,
                true, false, false,
                DXImage::defaultClearValue,
                MSAA::NONE);
    }

    std::shared_ptr<RenderTarget> DXVireo::createRenderTarget(
           const ImageFormat format,
           const uint32_t width,
           const uint32_t height,
           const RenderTargetType type,
           const ClearValue clearValue,
           const MSAA msaa,
           const std::wstring& name) const {
        return std::make_shared<DXRenderTarget>(
            getDXDevice()->getDevice(),
            std::make_shared<DXImage>(
                getDXDevice()->getDevice(),
                format,
                width,
                height,
                1,
                1,
                name,
                false,
                true,
                type == RenderTargetType::DEPTH || type == RenderTargetType::DEPTH_STENCIL,
                clearValue,
                msaa),
            type);
    }

    std::shared_ptr<RenderTarget> DXVireo::createRenderTarget(
           const std::shared_ptr<const SwapChain>& swapChain,
           const ClearValue clearValue,
           const MSAA msaa,
           const std::wstring& name) const {
        return std::make_shared<DXRenderTarget>(
            getDXDevice()->getDevice(),
            std::make_shared<DXImage>(
                getDXDevice()->getDevice(),
                swapChain->getFormat(),
                swapChain->getExtent().width,
                swapChain->getExtent().height,
                1,
                1,
                name,
                false,
                true,
                false,
                clearValue,
                msaa),
            RenderTargetType::COLOR);
    }
    std::shared_ptr<DescriptorLayout> DXVireo::createDescriptorLayout(
        const std::wstring& name) const {
        return std::make_shared<DXDescriptorLayout>(false, false);
    }

    std::shared_ptr<DescriptorLayout> DXVireo::createSamplerDescriptorLayout(
        const std::wstring& name) const {
        return std::make_shared<DXDescriptorLayout>(true, false);
    }

    std::shared_ptr<DescriptorLayout> DXVireo::_createDynamicUniformDescriptorLayout(
    const std::wstring& name) const {
        return std::make_shared<DXDescriptorLayout>(false, true);
    }

    std::shared_ptr<DescriptorSet> DXVireo::createDescriptorSet(
        const std::shared_ptr<const DescriptorLayout>& layout,
        const std::wstring&) const {
        const auto dxLayout = static_pointer_cast<const DXDescriptorLayout>(layout);
        return std::make_shared<DXDescriptorSet>(
            dxLayout->isSamplers() ? samplerDescriptorHeap : cbvSrvUavDescriptorHeap,
            layout,
            getDXDevice()->getDevice());
    }

    std::shared_ptr<Sampler> DXVireo::createSampler(
           Filter minFilter,
           Filter magFilter,
           AddressMode addressModeU,
           AddressMode addressModeV,
           AddressMode addressModeW,
           float minLod,
           float maxLod,
           bool anisotropyEnable,
           MipMapMode mipMapMode) const {
        return std::make_shared<DXSampler>(
            minFilter, magFilter, addressModeU, addressModeV, addressModeW,
            minLod, maxLod, anisotropyEnable, mipMapMode);
    }

    std::shared_ptr<Fence> DXVireo::createFence(const bool, const std::wstring& name) const {
        return std::make_shared<DXFence>(getDXDevice()->getDevice());
    }

    std::shared_ptr<Semaphore> DXVireo::createSemaphore(
        SemaphoreType type,
        const std::wstring& name) const {
        return std::make_shared<DXSemaphore>(getDXDevice()->getDevice(), type);
    }

    std::shared_ptr<CommandAllocator> DXVireo::createCommandAllocator(const CommandType type) const {
        return std::make_shared<DXCommandAllocator>(
            getDXDevice()->getDevice(),
            type,
            std::vector{cbvSrvUavDescriptorHeap, samplerDescriptorHeap});
    }

}
