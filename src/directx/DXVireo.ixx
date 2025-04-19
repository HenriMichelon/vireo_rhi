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
import vireo.directx.devices;
import vireo.directx.swapchains;

export namespace vireo {

    class DXVireo : public Vireo {
    public:
        DXVireo(void* windowHandle);

        shared_ptr<SwapChain> createSwapChain(
            ImageFormat format,
            const shared_ptr<const SubmitQueue>& submitQueue,
            PresentMode presentMode,
            uint32_t framesInFlight) const override;

        shared_ptr<SubmitQueue> createSubmitQueue(
        CommandType commandType,
        const wstring& name = L"SubmitQueue") const override;

        shared_ptr<Fence> createFence(const wstring& name) const override;

        shared_ptr<CommandAllocator> createCommandAllocator(CommandType type) const override;

        shared_ptr<VertexInputLayout> createVertexLayout(
            size_t size,
            const vector<VertexAttributeDesc>& attributesDescriptions) const override;

        shared_ptr<ShaderModule> createShaderModule(const string& fileName) const override;

        shared_ptr<PipelineResources> createPipelineResources(
            const vector<shared_ptr<DescriptorLayout>>& descriptorLayouts,
            const PushConstantsDesc& pushConstant = {},
            const wstring& name = L"PipelineResource") const override;

        shared_ptr<ComputePipeline> createComputePipeline(
            const shared_ptr<PipelineResources>& pipelineResources,
            const shared_ptr<const ShaderModule>& shader,
            const wstring& name = L"ComputePipeline") const override;

        shared_ptr<GraphicPipeline> createGraphicPipeline(
            const shared_ptr<PipelineResources>& pipelineResources,
            const shared_ptr<const VertexInputLayout>& vertexInputLayout,
            const shared_ptr<const ShaderModule>& vertexShader,
            const shared_ptr<const ShaderModule>& fragmentShader,
            const GraphicPipeline::Configuration& configuration,
            const wstring& name = L"GraphicPipeline") const override;

        shared_ptr<Buffer> createBuffer(
            BufferType type,
            size_t size,
            size_t count = 1,
            size_t alignment = 1,
            const wstring& name = L"Buffer") const override;

        shared_ptr<Image> createImage(
            ImageFormat format,
            uint32_t width,
            uint32_t height,
            const wstring& name = L"Image") const override;

        shared_ptr<Image> createReadWriteImage(
            ImageFormat format,
            uint32_t width,
            uint32_t height,
            const wstring& name = L"RWImage") const override;

        shared_ptr<RenderTarget> createRenderTarget(
            ImageFormat format,
            uint32_t width,
            uint32_t height,
            MSAA msaa = MSAA::NONE,
            const wstring& name = L"RenderTarget") const override;

        shared_ptr<RenderTarget> createRenderTarget(
            const shared_ptr<const SwapChain>& swapChain,
            MSAA msaa = MSAA::NONE,
            const wstring& name = L"RenderTarget") const override;

        shared_ptr<DescriptorLayout> createDescriptorLayout(
            const wstring& name) override;

        shared_ptr<DescriptorLayout> createSamplerDescriptorLayout(
            const wstring& name = L"createSamplerDescriptorLayout") override;

        shared_ptr<DescriptorSet> createDescriptorSet(
            const shared_ptr<const DescriptorLayout>& layout,
            const wstring& name) override;

        shared_ptr<Sampler> createSampler(
               Filter minFilter,
               Filter magFilter,
               AddressMode addressModeU,
               AddressMode addressModeV,
               AddressMode addressModeW,
               float minLod = 0.0f,
               float maxLod = 1.0f,
               bool anisotropyEnable = true,
               MipMapMode mipMapMode = MipMapMode::LINEAR) const override;

        auto getDXInstance() const { return reinterpret_pointer_cast<DXInstance>(instance); }

        auto getDXPhysicalDevice() const { return reinterpret_pointer_cast<DXPhysicalDevice>(physicalDevice); }

        auto getDXDevice() const { return reinterpret_pointer_cast<DXDevice>(device); }

    private:
        HWND hWnd;
    };

}