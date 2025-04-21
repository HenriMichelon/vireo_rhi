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

        shared_ptr<SwapChain> createSwapChain(
            ImageFormat format,
            const shared_ptr<const SubmitQueue>& submitQueue,
            PresentMode presentMode,
            uint32_t framesInFlight) const override;

        shared_ptr<SubmitQueue> createSubmitQueue(
            CommandType commandType,
            const wstring& name) const override;

        shared_ptr<Fence> createFence(const wstring& name) const override;

        shared_ptr<CommandAllocator> createCommandAllocator(CommandType type) const override;

        shared_ptr<VertexInputLayout> createVertexLayout(
            size_t size,
            const vector<VertexAttributeDesc>& attributesDescriptions) const override;

        shared_ptr<ShaderModule> createShaderModule(const string& fileName) const override;

        shared_ptr<PipelineResources> createPipelineResources(
            const vector<shared_ptr<DescriptorLayout>>& descriptorLayouts,
            const PushConstantsDesc& pushConstant,
            const wstring& name) const override;

        shared_ptr<ComputePipeline> createComputePipeline(
            const shared_ptr<PipelineResources>& pipelineResources,
            const shared_ptr<const ShaderModule>& shader,
            const wstring& name) const override;

        shared_ptr<GraphicPipeline> createGraphicPipeline(
            const shared_ptr<PipelineResources>& pipelineResources,
            const shared_ptr<const VertexInputLayout>& vertexInputLayout,
            const shared_ptr<const ShaderModule>& vertexShader,
            const shared_ptr<const ShaderModule>& fragmentShader,
            const GraphicPipelineConfiguration& configuration,
            const wstring& name) const override;

        shared_ptr<Buffer> createBuffer(
            BufferType type,
            size_t size,
            size_t count,
            size_t alignment,
            const wstring& name) const override;

        shared_ptr<Image> createImage(
            ImageFormat format,
            uint32_t width,
            uint32_t height,
            uint32_t arraySize,
            const wstring& name) const override;

        shared_ptr<Image> createReadWriteImage(
            ImageFormat format,
            uint32_t width,
            uint32_t height,
            uint32_t arraySize,
            const wstring& name) const override;

        shared_ptr<RenderTarget> createRenderTarget(
            ImageFormat format,
            uint32_t width,
            uint32_t height,
            RenderTargetType type,
            ClearValue clearValue,
            MSAA msaa,
            const wstring& name) const override;

        shared_ptr<RenderTarget> createRenderTarget(
            const shared_ptr<const SwapChain>& swapChain,
            ClearValue clearValue,
            MSAA msaa,
            const wstring& name) const override;

        shared_ptr<DescriptorLayout> createDescriptorLayout(
            const wstring& name) override;

        shared_ptr<DescriptorLayout> createSamplerDescriptorLayout(
            const wstring& name) override;

        shared_ptr<DescriptorSet> createDescriptorSet(
            const shared_ptr<const DescriptorLayout>& layout,
            const wstring& name) override;

        shared_ptr<Sampler> createSampler(
           Filter minFilter,
           Filter magFilter,
           AddressMode addressModeU,
           AddressMode addressModeV,
           AddressMode addressModeW,
           float minLod,
           float maxLod,
           bool anisotropyEnable,
           MipMapMode mipMapMode) const override;

        auto getVKInstance() const { return reinterpret_pointer_cast<VKInstance>(instance); }

        auto getVKPhysicalDevice() const { return reinterpret_pointer_cast<VKPhysicalDevice>(physicalDevice); }

        auto getVKDevice() const { return reinterpret_pointer_cast<VKDevice>(device); }

    private:
#ifdef _WIN32
        HWND hWnd;
#endif
    };

}