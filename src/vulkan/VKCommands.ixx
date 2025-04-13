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
        VKSubmitQueue(CommandList::Type type, const VKDevice& device, const string& name);

        auto getCommandQueue() const { return commandQueue; }

        void submit(const shared_ptr<FrameData>& frameData, vector<shared_ptr<CommandList>> commandLists) override;

        void submit(vector<shared_ptr<CommandList>> commandLists) override;

        void waitIdle() override;

    private:
        VkQueue commandQueue;
    };

    class VKCommandAllocator : public CommandAllocator {
    public:
        VKCommandAllocator(CommandList::Type type, const VKDevice& device);
        ~VKCommandAllocator() override;

        shared_ptr<CommandList> createCommandList(shared_ptr<Pipeline>& pipeline) const override;

        shared_ptr<CommandList> createCommandList() const override;

    private:
        const VKDevice& device;
        VkCommandPool   commandPool;
    };

    class VKCommandList : public CommandList {
    public:
        VKCommandList(const VKDevice& device, VkCommandPool commandPool);

        ~VKCommandList() override;

        void reset() override;

        void begin(shared_ptr<Pipeline>& pipeline) override;

        void begin() override;

        void end() override;

        void cleanup() override;

        void bindVertexBuffer(shared_ptr<Buffer>& buffer) override;

        void drawInstanced(uint32_t vertexCountPerInstance, uint32_t instanceCount = 1) override;

        void upload(shared_ptr<Buffer>& destination, const void* source) override;

        void upload(shared_ptr<Image>& destination, const void* source) override;

        auto getCommandBuffer() const { return commandBuffer; }

        void pipelineBarrier(
            VkPipelineStageFlags srcStageMask,
            VkPipelineStageFlags dstStageMask,
            const vector<VkImageMemoryBarrier>& barriers) const;

        static VkImageMemoryBarrier imageMemoryBarrier(
           VkImage image,
           VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask,
           VkImageLayout oldLayout, VkImageLayout newLayout,
           uint32_t baseMipLevel = 0,
           uint32_t levelCount = VK_REMAINING_MIP_LEVELS
       );

    private:
        const VKDevice& device;
        VkCommandBuffer commandBuffer;
        vector<VkBuffer> stagingBuffers{};
        vector<VkDeviceMemory> stagingBuffersMemory{};
    };

}