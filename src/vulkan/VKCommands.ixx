/*
* Copyright (c) 2025-present Henri Michelon
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/
module;
#include "vireo/backend/vulkan/Tools.h"
export module vireo.vulkan.commands;

import vireo;

import vireo.vulkan.devices;
import vireo.vulkan.descriptors;
import vireo.vulkan.resources;

export namespace vireo {

    class VKSubmitQueue : public SubmitQueue {
    public:
        VKSubmitQueue(const shared_ptr<const VKDevice>& device, CommandType type, const string& name);

        auto getCommandQueue() const { return commandQueue; }

        void submit(const shared_ptr<const FrameData>& frameData, const vector<shared_ptr<const CommandList>>& commandLists) const override;

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
        static constexpr VkPrimitiveTopology vkPrimitives[]{
            VK_PRIMITIVE_TOPOLOGY_POINT_LIST,
            VK_PRIMITIVE_TOPOLOGY_LINE_LIST,
            VK_PRIMITIVE_TOPOLOGY_LINE_STRIP,
            VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
            VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP
        };

        VKCommandList(const shared_ptr<const VKDevice>& device, VkCommandPool commandPool);

        void begin() const override;

        void end() const override;

        void cleanup() override;

        void upload(const shared_ptr<const Buffer>& destination, const void* source) override;

        void upload(const shared_ptr<const Image>& destination, const void* source) override;

        void copy(
            const shared_ptr<const Image>& source,
            const shared_ptr<const FrameData>& frameData,
            const shared_ptr<const SwapChain>& swapChain) const override;

        void beginRendering(
            const shared_ptr<FrameData>& frameData,
            const shared_ptr<SwapChain>& swapChain,
            const float clearColor[]) const override;

        void beginRendering(
            const shared_ptr<RenderTarget>& renderTarget,
            const float clearColor[]) const override;

        void endRendering() const override;

        void dispatch(uint32_t x, uint32_t y, uint32_t z) const override;

        void bindVertexBuffer(const shared_ptr<const Buffer>& buffer) const override;

        void bindPipeline(const shared_ptr<const Pipeline>& pipeline) override;

        void bindDescriptors(
            const shared_ptr<const Pipeline>& pipeline,
            const vector<shared_ptr<const DescriptorSet>>& descriptors) const override;

        void drawInstanced(uint32_t vertexCountPerInstance, uint32_t instanceCount = 1) const override;

        void setViewports(uint32_t count, const vector<Extent>& extent) const override;

        void setScissors(uint32_t count, const vector<Extent>& extent) const override;

        void setPrimitiveTopology(PrimitiveTopology primitiveTopology) const override;

        void barrier(
            const shared_ptr<const Image>& image,
            ResourceState oldState,
            ResourceState newState) const override;

        void barrier(
            const shared_ptr<const FrameData>& frameData,
            const shared_ptr<const SwapChain>& swapChain,
            ResourceState oldState,
            ResourceState newState) const override;

        void pushConstants(
            const shared_ptr<const PipelineResources>& pipelineResources,
            const PushConstantsDesc& pushConstants,
            const void* data) const;

        auto getCommandBuffer() const { return commandBuffer; }


    private:
        const shared_ptr<const VKDevice> device;
        VkCommandBuffer                  commandBuffer;
        vector<VkBuffer>                 stagingBuffers{};
        vector<VkDeviceMemory>           stagingBuffersMemory{};

        void beginRendering(
          VkImageView imageView,
          uint32_t width, uint32_t height,
          const float clearColor[]) const;

        void barrier(
           VkImage image,
           ResourceState oldState,
           ResourceState newState) const;
    };

}