/*
* Copyright (c) 2025-present Henri Michelon
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/
module;
#include "vireo/backend/directx/Libraries.h"
export module vireo.directx;

import vireo;
import vireo.directx.commands;
import vireo.directx.descriptors;
import vireo.directx.devices;
import vireo.directx.swapchains;

export namespace vireo {

    class DXVireo : public Vireo {
    public:
        DXVireo(uint32_t maxDescriptors, uint32_t maxSamplers);

        std::shared_ptr<SwapChain> createSwapChain(
            ImageFormat format,
            const std::shared_ptr<const SubmitQueue>& submitQueue,
            void* windowHandle,
            PresentMode presentMode,
            uint32_t framesInFlight) const override;

        std::shared_ptr<SubmitQueue> createSubmitQueue(
        CommandType commandType,
        const std::wstring& name) const override;

        std::shared_ptr<Fence> createFence(
            bool createSignaled,
            const std::wstring& name) const override;

        std::shared_ptr<Semaphore> createSemaphore(
            SemaphoreType type,
            const std::wstring& name) const override;

        std::shared_ptr<CommandAllocator> createCommandAllocator(CommandType type) const override;

        std::shared_ptr<VertexInputLayout> createVertexLayout(
            size_t size,
            const std::vector<VertexAttributeDesc>& attributesDescriptions) const override;

        std::shared_ptr<ShaderModule> createShaderModule(const std::string& fileName) const override;

        std::shared_ptr<PipelineResources> createPipelineResources(
            const std::vector<std::shared_ptr<DescriptorLayout>>& descriptorLayouts,
            const PushConstantsDesc& pushConstant,
            const std::wstring& name) const override;

        std::shared_ptr<ComputePipeline> createComputePipeline(
            const std::shared_ptr<PipelineResources>& pipelineResources,
            const std::shared_ptr<const ShaderModule>& shader,
            const std::wstring& name) const override;

        std::shared_ptr<GraphicPipeline> createGraphicPipeline(
            const GraphicPipelineConfiguration& configuration,
            const std::wstring& name) const override;

        std::shared_ptr<Buffer> createBuffer(
            BufferType type,
            size_t size,
            size_t count,
            const std::wstring& name) const override;

        std::shared_ptr<Image> createImage(
            ImageFormat format,
            uint32_t width,
            uint32_t height,
            uint32_t mipLevels,
            uint32_t arraySize,
            const std::wstring& name) const override;

        std::shared_ptr<Image> createReadWriteImage(
            ImageFormat format,
            uint32_t width,
            uint32_t height,
            uint32_t mipLevels,
            uint32_t arraySize,
            const std::wstring& name) const override;

        std::shared_ptr<RenderTarget> createRenderTarget(
            ImageFormat format,
            uint32_t width,
            uint32_t height,
            RenderTargetType type,
            ClearValue clearValue,
            MSAA msaa,
            const std::wstring& name) const override;

        std::shared_ptr<RenderTarget> createRenderTarget(
            const std::shared_ptr<const SwapChain>& swapChain,
            ClearValue clearValue,
            MSAA msaa,
            const std::wstring& name) const override;

        std::shared_ptr<DescriptorLayout> createDescriptorLayout(
            const std::wstring& name) const override;

        std::shared_ptr<DescriptorLayout> createSamplerDescriptorLayout(
            const std::wstring& name) const override;

        std::shared_ptr<DescriptorLayout> _createDynamicUniformDescriptorLayout(
            const std::wstring& name) const override;

        std::shared_ptr<DescriptorSet> createDescriptorSet(
            const std::shared_ptr<const DescriptorLayout>& layout,
            const std::wstring& name) const override;

        std::shared_ptr<Sampler> createSampler(
               Filter minFilter,
               Filter magFilter,
               AddressMode addressModeU,
               AddressMode addressModeV,
               AddressMode addressModeW,
               float minLod,
               float maxLod,
               bool anisotropyEnable,
               MipMapMode mipMapMode) const override;

        auto getDXInstance() const { return reinterpret_pointer_cast<DXInstance>(instance); }

        auto getDXPhysicalDevice() const { return reinterpret_pointer_cast<DXPhysicalDevice>(physicalDevice); }

        auto getDXDevice() const { return reinterpret_pointer_cast<DXDevice>(device); }

        auto getCbvSrcUavDescriptorHeap() const { return cbvSrvUavDescriptorHeap.get(); }

        auto getSamplerDescriptorHeap() const { return samplerDescriptorHeap.get(); }

    private:
        // D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV  global heap
        std::shared_ptr<DXDescriptorHeap> cbvSrvUavDescriptorHeap;
        // D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER global heap
        std::shared_ptr<DXDescriptorHeap> samplerDescriptorHeap;
   };

}