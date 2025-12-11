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
        VKVireo();

        void waitIdle() override;

        std::shared_ptr<SwapChain> createSwapChain(
            ImageFormat format,
            const std::shared_ptr<const SubmitQueue>& submitQueue,
            void* windowHandle,
            PresentMode presentMode,
            uint32_t framesInFlight) const override;

        std::shared_ptr<SubmitQueue> createSubmitQueue(
            CommandType commandType,
            const std::string& name) const override;

        std::shared_ptr<Fence> createFence(
            bool createSignaled,
            const std::string& name) const override;

        std::shared_ptr<Semaphore> createSemaphore(
            SemaphoreType type,
            const std::string& name) const override;

        std::shared_ptr<CommandAllocator> createCommandAllocator(CommandType type) const override;

        std::shared_ptr<VertexInputLayout> createVertexLayout(
            size_t size,
            const std::vector<VertexAttributeDesc>& attributesDescriptions) const override;

        std::shared_ptr<ShaderModule> createShaderModule(const std::string& fileName) const override;

        // std::shared_ptr<ShaderModule> createShaderModule(
            // std::ifstream input,
            // size_t size) const override;

        std::shared_ptr<ShaderModule> createShaderModule(
            const std::vector<char>& data,
            const std::string& name) const override;


        std::shared_ptr<PipelineResources> createPipelineResources(
            const std::vector<std::shared_ptr<DescriptorLayout>>& descriptorLayouts,
            const PushConstantsDesc& pushConstant,
            const std::string& name) const override;

        std::shared_ptr<ComputePipeline> createComputePipeline(
            const std::shared_ptr<PipelineResources>& pipelineResources,
            const std::shared_ptr<const ShaderModule>& shader,
            const std::string& name) const override;

        std::shared_ptr<GraphicPipeline> createGraphicPipeline(
            const GraphicPipelineConfiguration& configuration,
            const std::string& name) const override;

        std::shared_ptr<Buffer> createBuffer(
            BufferType type,
            size_t size,
            size_t count,
            const std::string& name) const override;

        std::shared_ptr<Image> createImage(
            ImageFormat format,
            uint32_t width,
            uint32_t height,
            uint32_t mipLevels,
            uint32_t arraySize,
            const std::string& name) const override;

        std::shared_ptr<Image> createReadWriteImage(
            ImageFormat format,
            uint32_t width,
            uint32_t height,
            uint32_t mipLevels,
            uint32_t arraySize,
            const std::string& name) const override;

        std::shared_ptr<RenderTarget> createRenderTarget(
            ImageFormat format,
            uint32_t width,
            uint32_t height,
            RenderTargetType type,
            ClearValue clearValue,
            uint32_t arraySize,
            MSAA msaa,
            const std::string& name) const override;

        std::shared_ptr<RenderTarget> createRenderTarget(
            const std::shared_ptr<const SwapChain>& swapChain,
            ClearValue clearValue,
            MSAA msaa,
            const std::string& name) const override;

        std::shared_ptr<DescriptorLayout> createDescriptorLayout(
            const std::string& name) const override;

        std::shared_ptr<DescriptorLayout> createSamplerDescriptorLayout(
            const std::string& name) const override;

        std::shared_ptr<DescriptorLayout> _createDynamicUniformDescriptorLayout(
            const std::string& name) const override;

        std::shared_ptr<DescriptorSet> createDescriptorSet(
            const std::shared_ptr<const DescriptorLayout>& layout,
            const std::string& name) const override;

        std::shared_ptr<Sampler> createSampler(
            Filter minFilter,
            Filter magFilter,
            AddressMode addressModeU,
            AddressMode addressModeV,
            AddressMode addressModeW,
            float minLod,
            float maxLod,
            bool anisotropyEnable,
            MipMapMode mipMapMode,
            CompareOp compareOp) const override;

        constexpr std::string getShaderFileExtension() const override {
            return ".spv";
        }

        auto getVKInstance() const { return reinterpret_pointer_cast<VKInstance>(instance); }

        auto getVKPhysicalDevice() const { return reinterpret_pointer_cast<VKPhysicalDevice>(physicalDevice); }

        auto getVKDevice() const { return reinterpret_pointer_cast<VKDevice>(device); }
    };

}