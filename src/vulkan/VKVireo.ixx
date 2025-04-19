/*
* Copyright (c) 2025-present Henri Michelon
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/
module;
#include "vireo/backend/vulkan/Libraries.h"
export module vireo.vulkan;

import vireo;
import vireo.vulkan.commands;
import vireo.vulkan.devices;
import vireo.vulkan.resources;
import vireo.vulkan.swapchains;

export namespace vireo {

    class VKVireo : public Vireo {
    public:
        VKVireo(void* windowHandle);

        void waitIdle() override;

        shared_ptr<SwapChain> createSwapChain(ImageFormat format, PresentMode presentMode, uint32_t framesInFlight) const override;

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
            const wstring& name = L"Pipeline") const override;

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

        auto getVKInstance() const { return reinterpret_pointer_cast<VKInstance>(instance); }

        auto getVKPhysicalDevice() const { return reinterpret_pointer_cast<VKPhysicalDevice>(physicalDevice); }

        auto getVKDevice() const { return reinterpret_pointer_cast<VKDevice>(device); }

        auto getVKGraphicCommandQueue() const { return reinterpret_pointer_cast<VKSubmitQueue>(graphicCommandQueue); }

    private:
#ifdef _WIN32
        HWND hWnd;
#endif
        const VkClearValue depthClearValue{.depthStencil = {1.0f, 0}};
    };

}