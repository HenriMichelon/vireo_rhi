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

    class VKSubmitQueue : public SubmitQueue {
    public:
        VKSubmitQueue(const shared_ptr<const VKDevice>& device, CommandType type, const wstring& name);

        auto getCommandQueue() const { return commandQueue; }

        void submit(
            const shared_ptr<Fence>& fence,
            const shared_ptr<const SwapChain>& swapChain,
            const vector<shared_ptr<const CommandList>>& commandLists) const override;

        void submit(const vector<shared_ptr<const CommandList>>& commandLists) const override;

        void waitIdle() const override;

    private:
        VkQueue commandQueue;
    };

    class VKCommandAllocator : public CommandAllocator {
    public:
        VKCommandAllocator(const shared_ptr<const VKDevice>& device, CommandType type);

        ~VKCommandAllocator() override;

        void reset() const override;

        shared_ptr<CommandList> createCommandList(const shared_ptr<const Pipeline>& pipeline) const override;

        shared_ptr<CommandList> createCommandList() const override;

    private:
        const shared_ptr<const VKDevice> device;
        VkCommandPool                    commandPool;
    };

    class VKCommandList : public CommandList {
    public:
        static constexpr VkIndexType vkIndexTypes[] {
            VK_INDEX_TYPE_UINT16,
            VK_INDEX_TYPE_UINT32,
        };

        VKCommandList(const shared_ptr<const VKDevice>& device, VkCommandPool commandPool);

        void begin() const override;

        void end() const override;

        void cleanup() override;

        void upload(const shared_ptr<const Buffer>& destination, const void* source) override;

        void upload(const shared_ptr<const Image>& destination, const void* source) override;

        void upload(const shared_ptr<const Image>& destination, const vector<void*>& sources) override;

        void copy(
            const shared_ptr<const Image>& source,
            const shared_ptr<const SwapChain>& swapChain) const override;

        void blit(
            const shared_ptr<const Image>& source,
            const shared_ptr<const SwapChain>& swapChain,
            Filter filter) const override;

        void beginRendering(const RenderingConfiguration& conf) override;

        void endRendering() override;

        void dispatch(uint32_t x, uint32_t y, uint32_t z) const override;

        void bindVertexBuffers(
            const vector<shared_ptr<const Buffer>>& buffers,
            vector<size_t> offsets = {}) const override;

        void bindVertexBuffer(const shared_ptr<const Buffer>& buffer, size_t offset) const override;

        void bindIndexBuffer(const shared_ptr<const Buffer>& buffer, IndexType indexType, size_t offset) const override;

        void bindPipeline(const shared_ptr<const Pipeline>& pipeline) override;

        void bindDescriptors(
            const shared_ptr<const Pipeline>& pipeline,
            const vector<shared_ptr<const DescriptorSet>>& descriptors) const override;

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

        void setViewports(uint32_t count, const vector<Extent>& extent) const override;

        void setScissors(uint32_t count, const vector<Extent>& extent) const override;

        void barrier(
            const shared_ptr<const Image>& image,
            ResourceState oldState,
            ResourceState newState) const override;

        void barrier(
            const shared_ptr<const RenderTarget>& renderTarget,
            ResourceState oldState,
            ResourceState newState) const override;

        void barrier(
            const shared_ptr<const SwapChain>& swapChain,
            ResourceState oldState,
            ResourceState newState) const override;

        void barrier(
            const vector<shared_ptr<const RenderTarget>>& renderTargets,
            ResourceState oldState,
            ResourceState newState) const override;

        void pushConstants(
            const shared_ptr<const PipelineResources>& pipelineResources,
            const PushConstantsDesc& pushConstants,
            const void* data) const override;

        auto getCommandBuffer() const { return commandBuffer; }

    private:
        const shared_ptr<const VKDevice> device;
        VkCommandBuffer                  commandBuffer;
        vector<shared_ptr<VKBuffer>>     stagingBuffers{};

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

        void barrier(const vector<VkImage>& images,
           ResourceState oldState,
           ResourceState newState) const;

        void barrier(
           VkImage image,
           ResourceState oldState,
           ResourceState newState) const;

    };

}