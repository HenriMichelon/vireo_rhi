/*
* Copyright (c) 2025-present Henri Michelon
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/
module;
#include "vireo/backend/vulkan/Tools.h"
module vireo.vulkan;

import vireo.vulkan.framedata;
import vireo.vulkan.pipelines;

namespace vireo {

    VKSubmitQueue::VKSubmitQueue(const shared_ptr<const VKDevice>& device, const CommandList::Type type, const string& name) {
        vkGetDeviceQueue(
            device->getDevice(),
            type == CommandList::COMPUTE ? device->getComputeQueueFamilyIndex() :
            type == CommandList::TRANSFER ?  device->getTransferQueueFamilyIndex() :
            device->getGraphicsQueueFamilyIndex(),
            0,
            &commandQueue);
#ifdef _DEBUG
        vkSetObjectName(device->getDevice(), reinterpret_cast<uint64_t>(commandQueue), VK_OBJECT_TYPE_QUEUE,
            "VKSubmitQueue : " + name);
#endif
    }

    void VKSubmitQueue::waitIdle() const {
        vkQueueWaitIdle(commandQueue);
    }

    void VKSubmitQueue::submit(
        const shared_ptr<const FrameData>& frameData,
        const vector<shared_ptr<const CommandList>>& commandLists) const {
        const auto data = static_pointer_cast<const VKFrameData>(frameData);
        auto submitInfos = vector<VkCommandBufferSubmitInfo>(commandLists.size());
        for (int i = 0; i < commandLists.size(); i++) {
            submitInfos[i] = {
                .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO,
                .commandBuffer = static_pointer_cast<const VKCommandList>(commandLists[i])->getCommandBuffer(),
            };
        }
        const auto submitInfo = VkSubmitInfo2  {
            .sType                    = VK_STRUCTURE_TYPE_SUBMIT_INFO_2,
            .waitSemaphoreInfoCount   = 1,
            .pWaitSemaphoreInfos      = &data->imageAvailableSemaphoreSubmitInfo,
            .commandBufferInfoCount   = static_cast<uint32_t>(submitInfos.size()),
            .pCommandBufferInfos      = submitInfos.data(),
            .signalSemaphoreInfoCount = 1,
            .pSignalSemaphoreInfos    = &data->renderFinishedSemaphoreSubmitInfo
        };
        DieIfFailed(vkQueueSubmit2(commandQueue, 1, &submitInfo, data->inFlightFence));
    }

    void VKSubmitQueue::submit(const vector<shared_ptr<const CommandList>>& commandLists) const {
        auto submitInfos = vector<VkCommandBufferSubmitInfo>(commandLists.size());
        for (int i = 0; i < commandLists.size(); i++) {
            submitInfos[i] = {
                .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO,
                .commandBuffer = static_pointer_cast<const VKCommandList>(commandLists[i])->getCommandBuffer(),
            };
        }
        const auto submitInfo = VkSubmitInfo2 {
            .sType                    = VK_STRUCTURE_TYPE_SUBMIT_INFO_2,
            .waitSemaphoreInfoCount   = 0,
            .commandBufferInfoCount   = static_cast<uint32_t>(submitInfos.size()),
            .pCommandBufferInfos      = submitInfos.data(),
            .signalSemaphoreInfoCount = 0,
        };
        DieIfFailed(vkQueueSubmit2(commandQueue, 1, &submitInfo, VK_NULL_HANDLE));
    }

    VKCommandAllocator::VKCommandAllocator(const shared_ptr<const VKDevice>& device, const CommandList::Type type):
        CommandAllocator{type},
        device{device} {
        const auto poolInfo = VkCommandPoolCreateInfo {
            .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT, // TODO optional
            .queueFamilyIndex = type == CommandList::COMPUTE ? device->getComputeQueueFamilyIndex() :
                type == CommandList::TRANSFER ?  device->getTransferQueueFamilyIndex() :
                 device->getGraphicsQueueFamilyIndex()
        };
        DieIfFailed(vkCreateCommandPool(device->getDevice(), &poolInfo, nullptr, &commandPool));
    }

    VKCommandAllocator::~VKCommandAllocator() {
        vkDestroyCommandPool(device->getDevice(), commandPool, nullptr);
    }

    shared_ptr<CommandList> VKCommandAllocator::createCommandList(const shared_ptr<const Pipeline>&) const {
        return createCommandList();
    }

    shared_ptr<CommandList> VKCommandAllocator::createCommandList() const {
        return make_shared<VKCommandList>(device, commandPool);
    }

    VKCommandList::VKCommandList(const shared_ptr<const VKDevice>& device, const VkCommandPool commandPool) :
        device{device} {
        const auto allocInfo = VkCommandBufferAllocateInfo {
            .sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .commandPool        = commandPool,
            .level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = 1
        };
        DieIfFailed(vkAllocateCommandBuffers(device->getDevice(), &allocInfo, &commandBuffer));
    }

    void VKCommandList::bindVertexBuffer(const shared_ptr<const Buffer>& buffer) const {
        const auto vkBuffer = static_pointer_cast<const VKBuffer>(buffer);
        const VkBuffer         buffers[] = {vkBuffer->getBuffer()};
        constexpr VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
    }

    void VKCommandList::drawInstanced(const uint32_t vertexCountPerInstance, const uint32_t instanceCount) const {
        vkCmdDraw(commandBuffer, vertexCountPerInstance, instanceCount, 0, 0);
    }

    void VKCommandList::reset() const {
        vkResetCommandBuffer(commandBuffer, 0);
    }

    void VKCommandList::begin(const shared_ptr<const Pipeline>& pipeline) const {
        constexpr auto beginInfo = VkCommandBufferBeginInfo {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        };
        DieIfFailed(vkBeginCommandBuffer(commandBuffer, &beginInfo));
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS , static_pointer_cast<const VKPipeline>(pipeline)->getPipeline());
    }

    void VKCommandList::begin() const {
        constexpr auto beginInfo = VkCommandBufferBeginInfo{
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        };
        vkResetCommandBuffer(commandBuffer, 0);
        DieIfFailed(vkBeginCommandBuffer(commandBuffer, &beginInfo));
    }

    void VKCommandList::end() const {
        DieIfFailed(vkEndCommandBuffer(commandBuffer));
    }

    void VKCommandList::cleanup() {
        for (int i = 0; i < stagingBuffers.size(); i++) {
            vkDestroyBuffer(device->getDevice(), stagingBuffers[i], nullptr);
            vkFreeMemory(device->getDevice(), stagingBuffersMemory[i], nullptr);
        }
        stagingBuffers.clear();
        stagingBuffersMemory.clear();
    }

    void VKCommandList::upload(const shared_ptr<const Buffer>& destination, const void* source) {
        const auto buffer = static_pointer_cast<const VKBuffer>(destination);
        auto stagingBuffer = VkBuffer{VK_NULL_HANDLE};
        auto stagingBufferMemory = VkDeviceMemory {VK_NULL_HANDLE};
        VKBuffer::createBuffer(
            device,
            buffer->getSize(),
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            stagingBuffer, stagingBufferMemory);

        void* stagingData;
        vkMapMemory(device->getDevice(), stagingBufferMemory, 0, buffer->getSize(), 0, &stagingData);
        memcpy(stagingData, source, buffer->getSize());
        vkUnmapMemory(device->getDevice(), stagingBufferMemory);

        const auto copyRegion = VkBufferCopy{
            .srcOffset = 0,
            .dstOffset = 0,
            .size = buffer->getSize(),
        };
        vkCmdCopyBuffer(
            commandBuffer,
            stagingBuffer,
            buffer->getBuffer(),
            1,
            &copyRegion);

        stagingBuffers.push_back(stagingBuffer);
        stagingBuffersMemory.push_back(stagingBufferMemory);
    }

    void VKCommandList::upload(const shared_ptr<const Image>& destination, const void* source) {
        const auto image = static_pointer_cast<const VKImage>(destination);
        auto stagingBuffer = VkBuffer{VK_NULL_HANDLE};
        auto stagingBufferMemory = VkDeviceMemory{VK_NULL_HANDLE};
        VKBuffer::createBuffer(
            device,
            image->getSize(),
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            stagingBuffer, stagingBufferMemory);

        void* stagingData;
        vkMapMemory(device->getDevice(), stagingBufferMemory, 0, image->getSize(), 0, &stagingData);
        memcpy(stagingData, source, image->getSize());
        vkUnmapMemory(device->getDevice(), stagingBufferMemory);

        // https://vulkan-tutorial.com/Texture_mapping/Images#page_Copying-buffer-to-image
        const auto region = VkBufferImageCopy {
            .bufferOffset = 0,
            .bufferRowLength = 0,
            .bufferImageHeight = 0,
            .imageSubresource = {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .mipLevel = 0,
                .baseArrayLayer = 0,
                .layerCount = 1,
            },
            .imageOffset = {0, 0, 0},
            .imageExtent = {image->getWidth(), image->getHeight(), 1},
        };

        pipelineBarrier(
        VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        {
            imageMemoryBarrier(
                image->getImage(),
                0,
                VK_ACCESS_TRANSFER_WRITE_BIT,
                VK_IMAGE_LAYOUT_UNDEFINED,
                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
        });
        vkCmdCopyBufferToImage(
                commandBuffer,
                stagingBuffer,
                image->getImage(),
                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                1,
                &region);
        pipelineBarrier(
           VK_PIPELINE_STAGE_TRANSFER_BIT,
           VK_PIPELINE_STAGE_TRANSFER_BIT,
           {
               imageMemoryBarrier(
                   image->getImage(),
                   VK_ACCESS_TRANSFER_WRITE_BIT,
                   0,
                   VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                   VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
        });

        stagingBuffers.push_back(stagingBuffer);
        stagingBuffersMemory.push_back(stagingBufferMemory);
    }

    void VKCommandList::pipelineBarrier(
       const VkPipelineStageFlags srcStageMask,
       const VkPipelineStageFlags dstStageMask,
       const vector<VkImageMemoryBarrier>& barriers) const {
        vkCmdPipelineBarrier(commandBuffer,
            srcStageMask,
            dstStageMask,
            0,
            0,
            nullptr,
            0,
            nullptr,
            static_cast<uint32_t>(barriers.size()),
            barriers.data());
    }

    VkImageMemoryBarrier VKCommandList::imageMemoryBarrier(
           const VkImage image,
           const VkAccessFlags srcAccessMask, const VkAccessFlags dstAccessMask,
           const VkImageLayout oldLayout, const VkImageLayout newLayout,
           const uint32_t baseMipLevel,
           const uint32_t levelCount
       ) {
        return VkImageMemoryBarrier {
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
            .srcAccessMask =  srcAccessMask,
            .dstAccessMask = dstAccessMask,
            .oldLayout = oldLayout,
            .newLayout = newLayout,
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .image = image,
            .subresourceRange = {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel = baseMipLevel,
                .levelCount = levelCount,
                .baseArrayLayer = 0,
                .layerCount = VK_REMAINING_ARRAY_LAYERS,
            }
        };
    }

}