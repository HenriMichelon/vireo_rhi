/*
* Copyright (c) 2025-present Henri Michelon
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/
module;
#include "vireo/backend/vulkan/Libraries.h"
export module vireo.vulkan.commands;

import vireo;

import vireo.vulkan.devices;
import vireo.vulkan.descriptors;
import vireo.vulkan.resources;

export namespace vireo {

    class VKFence : public Fence {
    public:
        VKFence(bool createSignaled, const std::shared_ptr<const VKDevice>& device, const std::wstring& name);

        void wait() const override;

        void reset() override;

        ~VKFence() override;

        auto& getFence() const { return fence; }

    private:
        const VkDevice device;
        VkFence        fence;
    };

    class VKSemaphore : public Semaphore {
    public:
        static constexpr VkPipelineStageFlagBits2 vkWaitStageFlags[] {
            0,
            VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT,
            VK_PIPELINE_STAGE_2_VERTEX_INPUT_BIT,
            VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT,
            VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT ,
            VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT ,
            VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT ,
            VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT ,
            VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT ,
            VK_PIPELINE_STAGE_2_ALL_TRANSFER_BIT ,
            VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT ,
            VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT ,
            VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT,
            VK_PIPELINE_STAGE_2_COPY_BIT ,
            VK_PIPELINE_STAGE_2_RESOLVE_BIT ,
            VK_PIPELINE_STAGE_2_BLIT_BIT ,
            VK_PIPELINE_STAGE_2_CLEAR_BIT ,
        };
        VKSemaphore(const std::shared_ptr<const VKDevice>& device, SemaphoreType type, const std::wstring& name);

        auto getSemaphore() const { return semaphore; }

        ~VKSemaphore() override;

    private:
        const VkDevice device;
        VkSemaphore semaphore;
    };

    class VKSubmitQueue : public SubmitQueue {
    public:
        VKSubmitQueue(const std::shared_ptr<const VKDevice>& device, CommandType type, const std::wstring& name);

        auto getCommandQueue() const { return commandQueue; }

        void submit(
            const std::shared_ptr<Fence>& fence,
            const std::shared_ptr<const SwapChain>& swapChain,
            const std::vector<std::shared_ptr<const CommandList>>& commandLists) const override;

        void submit(
            const std::shared_ptr<Semaphore>& waitSemaphore,
            WaitStage waitStage,
            const std::shared_ptr<Fence>& fence,
            const std::shared_ptr<const SwapChain>& swapChain,
            const std::vector<std::shared_ptr<const CommandList>>& commandLists) const override;

        void submit(const std::vector<std::shared_ptr<const CommandList>>& commandLists) const override;

        void submit(
            const std::shared_ptr<Fence>& fence,
            const std::vector<std::shared_ptr<const CommandList>>& commandLists) const override;

        void submit(
            const std::shared_ptr<Semaphore>& waitSemaphore,
            WaitStage waitStage,
            WaitStage signalStage,
            const std::shared_ptr<Semaphore>& signalSemaphore,
            const std::vector<std::shared_ptr<const CommandList>>& commandLists) const override;

        void waitIdle() const override;

    private:
        VkQueue commandQueue;

    };

    class VKCommandAllocator : public CommandAllocator {
    public:
        VKCommandAllocator(const std::shared_ptr<const VKDevice>& device, CommandType type);

        ~VKCommandAllocator() override;

        void reset() const override;

        std::shared_ptr<CommandList> createCommandList(const std::shared_ptr<const Pipeline>& pipeline) const override;

        std::shared_ptr<CommandList> createCommandList() const override;

    private:
        const std::shared_ptr<const VKDevice> device;
        VkCommandPool                    commandPool;
    };

    class VKCommandList : public CommandList {
    public:
        static constexpr VkIndexType vkIndexTypes[] {
            VK_INDEX_TYPE_UINT16,
            VK_INDEX_TYPE_UINT32,
        };

        VKCommandList(const std::shared_ptr<const VKDevice>& device, VkCommandPool commandPool);

        ~VKCommandList() override;

        void begin() const override;

        void end() const override;

        void cleanup() override;

        void upload(const std::shared_ptr<const Buffer>& destination, const void* source) override;

        void upload(
            const std::shared_ptr<const Image>& destination,
            const void* source,
            uint32_t firstMipLevel) override;

        void copy(
            const std::shared_ptr<Buffer>& source,
            const std::shared_ptr<const Image>& destination,
            uint32_t sourceOffset,
            uint32_t firstMipLevel) override;

        void uploadArray(
            const std::shared_ptr<const Image>& destination,
            const std::vector<void*>& sources,
            uint32_t firstMipLevel) override;

        void copy(
            const std::shared_ptr<const Image>& source,
            const std::shared_ptr<const SwapChain>& swapChain) const override;

        void beginRendering(const RenderingConfiguration& conf) override;

        void endRendering() override;

        void dispatch(uint32_t x, uint32_t y, uint32_t z) const override;

        void bindVertexBuffers(
            const std::vector<std::shared_ptr<const Buffer>>& buffers,
            std::vector<size_t> offsets = {}) const override;

        void bindVertexBuffer(const std::shared_ptr<const Buffer>& buffer, size_t offset) const override;

        void bindIndexBuffer(const std::shared_ptr<const Buffer>& buffer, IndexType indexType, size_t offset) const override;

        void bindPipeline(const std::shared_ptr<const Pipeline>& pipeline) override;

        void bindDescriptors(
            const std::shared_ptr<const Pipeline>& pipeline,
            const std::vector<std::shared_ptr<const DescriptorSet>>& descriptors,
            uint32_t firstSet) const override;

        void bindDescriptor(
            const std::shared_ptr<const Pipeline>& pipeline,
            const std::shared_ptr<const DescriptorSet>& descriptor,
            uint32_t set) const override;

        void bindDescriptor(
            const std::shared_ptr<const Pipeline>& pipeline,
            const std::shared_ptr<const DescriptorSet>& descriptor,
            uint32_t set,
            uint32_t offset) const override;

        void draw(
            uint32_t vertexCountPerInstance,
            uint32_t instanceCount = 1,
            uint32_t firstVertex = 0,
            uint32_t firstInstance = 0) const override;

        void drawIndexed(
            uint32_t indexCountPerInstance,
            uint32_t instanceCount = 0,
            uint32_t firstIndex = 0,
            uint32_t vertexOffset = 0,
            uint32_t firstInstance = 0) const override;

        void setViewports(const std::vector<Extent>& extents) const override;

        void setScissors(const std::vector<Extent>& extents) const override;

        void setViewport(const Extent& extent) const override;

        void setScissors(const Extent& extent) const override;

        void setStencilReference(uint32_t reference) const override;

        void barrier(
            const std::shared_ptr<const Image>& image,
            ResourceState oldState,
            ResourceState newState,
            uint32_t firstMipLevel,
            uint32_t levelCount) const override;

        void barrier(
            const std::shared_ptr<const RenderTarget>& renderTarget,
            ResourceState oldState,
            ResourceState newState) const override;

        void barrier(
            const std::shared_ptr<const SwapChain>& swapChain,
            ResourceState oldState,
            ResourceState newState) const override;

        void barrier(
            const std::vector<std::shared_ptr<const RenderTarget>>& renderTargets,
            ResourceState oldState,
            ResourceState newState) const override;

        void pushConstants(
            const std::shared_ptr<const PipelineResources>& pipelineResources,
            const PushConstantsDesc& pushConstants,
            const void* data) const override;

        auto getCommandBuffer() const { return commandBuffer; }

    private:
        const std::shared_ptr<const VKDevice>   device;
        VkCommandBuffer                         commandBuffer;
        std::vector<std::shared_ptr<VKBuffer>>  stagingBuffers{};

        static void convertState(
            ResourceState oldState,
            ResourceState newState,
            VkPipelineStageFlags& srcStage,
            VkPipelineStageFlags& dstStage,
            VkAccessFlags& srcAccess,
            VkAccessFlags& dstAccess,
            VkImageLayout& srcLayout,
            VkImageLayout& dstLayout,
            VkImageAspectFlagBits& aspectFlag);

        void barrier(const std::vector<VkImage>& images,
           ResourceState oldState,
           ResourceState newState) const;

        void barrier(
            VkImage image,
            ResourceState oldState,
            ResourceState newState,
            uint32_t firstMipLevel = 0,
            uint32_t levelCount = 1) const;

    };

}