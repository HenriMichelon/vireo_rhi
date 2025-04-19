/*
* Copyright (c) 2025-present Henri Michelon
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/
module;
#include "vireo/backend/vulkan/Libraries.h"
module vireo.vulkan;

import vireo.tools;

import vireo.vulkan.descriptors;
import vireo.vulkan.pipelines;
import vireo.vulkan.tools;

namespace vireo {

    VKSubmitQueue::VKSubmitQueue(const shared_ptr<const VKDevice>& device, const CommandType type, const string& name) {
        vkGetDeviceQueue(
            device->getDevice(),
            type == CommandType::COMPUTE ? device->getComputeQueueFamilyIndex() :
            type == CommandType::TRANSFER ?  device->getTransferQueueFamilyIndex() :
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
        const shared_ptr<Fence>& fence,
        const shared_ptr<const SwapChain>& swapChain,
        const vector<shared_ptr<const CommandList>>& commandLists) const {
        const auto vkSwapChain = static_pointer_cast<const VKSwapChain>(swapChain);
        const auto vkFence = static_pointer_cast<const VKFence>(fence);
        auto submitInfos = vector<VkCommandBufferSubmitInfo>(commandLists.size());
        for (int i = 0; i < commandLists.size(); i++) {
            submitInfos[i] = {
                .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO,
                .commandBuffer = static_pointer_cast<const VKCommandList>(commandLists[i])->getCommandBuffer(),
            };
        }
        const auto waitSemaphoreInfo = VkSemaphoreSubmitInfo {
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
            .semaphore = vkSwapChain->getCurrentImageAvailableSemaphore(),
            .value = 1,
            .stageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR,
            .deviceIndex = 0
        };
        const auto renderFinishedSemaphoreSubmitInfo = VkSemaphoreSubmitInfo {
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
            .semaphore = vkSwapChain->getCurrentRenderFinishedSemaphore(),
            .stageMask = VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT,
            .deviceIndex = 0
        };
        const auto submitInfo = VkSubmitInfo2  {
            .sType                    = VK_STRUCTURE_TYPE_SUBMIT_INFO_2,
            .waitSemaphoreInfoCount   = 1,
            .pWaitSemaphoreInfos      = &waitSemaphoreInfo,
            .commandBufferInfoCount   = static_cast<uint32_t>(submitInfos.size()),
            .pCommandBufferInfos      = submitInfos.data(),
            .signalSemaphoreInfoCount = 1,
            .pSignalSemaphoreInfos    = &renderFinishedSemaphoreSubmitInfo
        };
        vkCheck(vkQueueSubmit2(commandQueue, 1, &submitInfo, vkFence->getFence()));
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
        vkCheck(vkQueueSubmit2(commandQueue, 1, &submitInfo, VK_NULL_HANDLE));
    }

    VKCommandAllocator::VKCommandAllocator(const shared_ptr<const VKDevice>& device, const CommandType type):
        CommandAllocator{type},
        device{device} {
        const auto poolInfo = VkCommandPoolCreateInfo {
            .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT, // TODO optional
            .queueFamilyIndex =
                type == CommandType::COMPUTE ? device->getComputeQueueFamilyIndex() :
                type == CommandType::TRANSFER ?  device->getTransferQueueFamilyIndex() :
                device->getGraphicsQueueFamilyIndex()
        };
        vkCheck(vkCreateCommandPool(device->getDevice(), &poolInfo, nullptr, &commandPool));
    }

    void VKCommandAllocator::reset() const {
        vkResetCommandPool(device->getDevice(), commandPool, 0);
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
            .commandBufferCount = 1,
        };
        vkCheck(vkAllocateCommandBuffers(device->getDevice(), &allocInfo, &commandBuffer));
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

    void VKCommandList::bindPipeline(const shared_ptr<const Pipeline>& pipeline) {
        if (pipeline->getType() == Pipeline::COMPUTE) {
            vkCmdBindPipeline(
                commandBuffer,
                VK_PIPELINE_BIND_POINT_COMPUTE ,
                static_pointer_cast<const VKComputePipeline>(pipeline)->getPipeline());
        } else {
            vkCmdBindPipeline(
                commandBuffer,
                VK_PIPELINE_BIND_POINT_GRAPHICS ,
                static_pointer_cast<const VKGraphicPipeline>(pipeline)->getPipeline());
        }
    }

    void VKCommandList::bindDescriptors(
        const shared_ptr<const Pipeline>& pipeline,
        const vector<shared_ptr<const DescriptorSet>>& descriptors) const {
        if (descriptors.empty()) { return; }
        const auto vkLayout = static_pointer_cast<const VKPipelineResources>(pipeline->getResources())->getPipelineLayout();

        vector<VkDescriptorSet> descriptorSets(descriptors.size());
        for (int i = 0; i < descriptors.size(); i++) {
            descriptorSets[i] = static_pointer_cast<const VKDescriptorSet>(descriptors[i])->getSet();
        }
        vkCmdBindDescriptorSets(commandBuffer,
                                pipeline->getType() == Pipeline::COMPUTE ?
                                    VK_PIPELINE_BIND_POINT_COMPUTE :
                                    VK_PIPELINE_BIND_POINT_GRAPHICS,
                                vkLayout,
                                0,
                                descriptorSets.size(),
                                descriptorSets.data(),
                                0,
                                nullptr);
    }

    void VKCommandList::setViewports(const uint32_t count, const vector<Extent>& extent) const {
        vector<VkViewport> viewports(count);
        for (int i = 0; i < count; i++) {
            viewports[i].x = 0.0f;
            viewports[i].y = static_cast<float>(extent[i].height),
            viewports[i].width = static_cast<float>(extent[i].width);
            viewports[i].height = -static_cast<float>(extent[i].height);
            viewports[i].minDepth = 0.0f;
            viewports[i].maxDepth = 1.0f;
        }
        vkCmdSetViewportWithCount(commandBuffer, count, viewports.data());
    }

    void VKCommandList::setScissors(const uint32_t count, const vector<Extent>& extent) const {
        vector<VkRect2D> scissors(count);
        for (int i = 0; i < count; i++) {
            scissors[i].offset = {0, 0};
            scissors[i].extent.width = extent[i].width;
            scissors[i].extent.height = extent[i].height;
        }
        vkCmdSetScissorWithCount(commandBuffer, count, scissors.data());
    }

    void VKCommandList::beginRendering(
          const shared_ptr<SwapChain>& swapChain,
          const float clearColor[]) const {
        const auto vkSwapChain = static_pointer_cast<VKSwapChain>(swapChain);
        beginRendering(
            vkSwapChain->getCurrentImageView(),
            vkSwapChain->getExtent().width,
            vkSwapChain->getExtent().height,
            clearColor);
    }

    void VKCommandList::beginRendering(
      const shared_ptr<RenderTarget>& multisampledRenderTarget,
      const shared_ptr<SwapChain>& swapChain,
      const float clearColor[]) const {
        const auto vkMultiSampledImage = static_pointer_cast<VKImage>(multisampledRenderTarget->getImage());
        const auto vkSwapChain = static_pointer_cast<VKSwapChain>(swapChain);
        beginRendering(
            vkMultiSampledImage->getImage(),
            vkMultiSampledImage->getImageView(),
            vkSwapChain->getCurrentImageView(),
            vkSwapChain->getExtent().width,
            vkSwapChain->getExtent().height,
            clearColor);
    }

    void VKCommandList::beginRendering(
      const shared_ptr<RenderTarget>& renderTarget,
      const float clearColor[]) const {
        const auto vkImage = static_pointer_cast<VKImage>(renderTarget->getImage());
        beginRendering(
            vkImage->getImageView(),
            vkImage->getWidth(),
            vkImage->getHeight(),
            clearColor);
    }

    void VKCommandList::beginRendering(
        const shared_ptr<RenderTarget>& multisampledRenderTarget,
        const shared_ptr<RenderTarget>& renderTarget,
        const float clearColor[]) const {
        const auto vkMultiSampledImage = static_pointer_cast<VKImage>(multisampledRenderTarget->getImage());
        const auto vkImage = static_pointer_cast<VKImage>(renderTarget->getImage());
        beginRendering(
            vkMultiSampledImage->getImage(),
            vkMultiSampledImage->getImageView(),
            vkImage->getImageView(),
            vkImage->getWidth(),
            vkImage->getHeight(),
            clearColor);
    }

    void VKCommandList::beginRendering(
        const VkImage multisampledImage,
        const VkImageView multisampledImageView,
        const VkImageView imageView,
        const uint32_t width,
        const uint32_t height,
        const float clearColor[]) const {
        barrier(multisampledImage, ResourceState::UNDEFINED, ResourceState::RENDER_TARGET);
        const auto colorAttachmentInfo = VkRenderingAttachmentInfo {
            .sType              = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR,
            .imageView          = multisampledImageView,
            .imageLayout        = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            .resolveMode        = VK_RESOLVE_MODE_AVERAGE_BIT,
            .resolveImageView   = imageView,
            .resolveImageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            .loadOp             = VK_ATTACHMENT_LOAD_OP_CLEAR,
            .storeOp            = VK_ATTACHMENT_STORE_OP_STORE,
            .clearValue         = {
                clearColor[0], clearColor[1], clearColor[2], clearColor[3]
            },
        };
        const auto renderingInfo = VkRenderingInfo {
            .sType               = VK_STRUCTURE_TYPE_RENDERING_INFO_KHR,
            .pNext                = nullptr,
            .renderArea           = {
                {0, 0},
                {width, height}
            },
            .layerCount           = 1,
            .colorAttachmentCount = 1,
            .pColorAttachments    = &colorAttachmentInfo,
            .pDepthAttachment     = nullptr,
            .pStencilAttachment   = nullptr
        };
        vkCmdBeginRendering(commandBuffer, &renderingInfo);
    }

    void VKCommandList::beginRendering(
        const VkImageView imageView,
        const uint32_t width,
        const uint32_t height,
        const float clearColor[]) const {

        const auto colorAttachmentInfo = VkRenderingAttachmentInfo {
            .sType              = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR,
            .imageView          = imageView,
            .imageLayout        = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            .resolveMode        = VK_RESOLVE_MODE_NONE,
            .loadOp             = VK_ATTACHMENT_LOAD_OP_CLEAR,
            .storeOp            = VK_ATTACHMENT_STORE_OP_STORE,
            .clearValue         = {
                clearColor[0], clearColor[1], clearColor[2], clearColor[3]
            },
        };
        const auto renderingInfo = VkRenderingInfo {
            .sType               = VK_STRUCTURE_TYPE_RENDERING_INFO_KHR,
            .pNext                = nullptr,
            .renderArea           = {
                {0, 0},
                {width, height}
            },
            .layerCount           = 1,
            .colorAttachmentCount = 1,
            .pColorAttachments    = &colorAttachmentInfo,
            .pDepthAttachment     = nullptr,
            .pStencilAttachment   = nullptr
        };
        vkCmdBeginRendering(commandBuffer, &renderingInfo);
    }

    void VKCommandList::endRendering() const {
        vkCmdEndRendering(commandBuffer);
    }

    void VKCommandList::dispatch(const uint32_t x, const uint32_t y, const uint32_t z) const {
        vkCmdDispatch(commandBuffer, x, y, z);
    }

    void VKCommandList::barrier(
        const VkImage image,
        const ResourceState oldState,
        const ResourceState newState) const {
        VkPipelineStageFlags srcStage, dstStage;
        VkAccessFlags srcAccess, dstAccess;
        VkImageLayout srcLayout, dstLayout;

        if (oldState == ResourceState::UNDEFINED && newState == ResourceState::DISPATCH_TARGET) {
            srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            dstStage = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
            srcAccess = 0;
            dstAccess = VK_ACCESS_SHADER_WRITE_BIT;
            srcLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            dstLayout = VK_IMAGE_LAYOUT_GENERAL;
        } else if (oldState == ResourceState::UNDEFINED && newState == ResourceState::RENDER_TARGET) {
            srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            dstStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            srcAccess = 0;
            dstAccess = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            srcLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            dstLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        } else if (oldState == ResourceState::RENDER_TARGET && newState == ResourceState::COPY_SRC) {
            srcStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            srcAccess = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            dstAccess = 0;
            srcLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            dstLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        } else if (oldState == ResourceState::RENDER_TARGET && newState == ResourceState::PRESENT) {
            srcStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            dstStage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
            srcAccess = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            dstAccess = 0;
            srcLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            dstLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        } else if (oldState == ResourceState::UNDEFINED && newState == ResourceState::PRESENT) {
            srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            dstStage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
            srcAccess = 0;
            dstAccess = 0;
            srcLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            dstLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        } else if (oldState == ResourceState::COPY_DST && newState == ResourceState::PRESENT) {
            srcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            dstStage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
            srcAccess = VK_ACCESS_TRANSFER_WRITE_BIT;
            dstAccess = 0;
            srcLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            dstLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        } else if (oldState == ResourceState::UNDEFINED && newState == ResourceState::COPY_DST) {
            srcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            srcAccess = 0;
            dstAccess = VK_ACCESS_TRANSFER_WRITE_BIT;
            srcLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            dstLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        } else if (oldState == ResourceState::COPY_DST && newState == ResourceState::SHADER_READ) {
            srcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            dstStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
            srcAccess = VK_ACCESS_TRANSFER_WRITE_BIT;
            dstAccess = VK_ACCESS_SHADER_READ_BIT;
            srcLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            dstLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        } else if (oldState == ResourceState::DISPATCH_TARGET && newState == ResourceState::COPY_SRC) {
            srcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            srcAccess = 0;
            dstAccess = VK_ACCESS_TRANSFER_READ_BIT;
            srcLayout = VK_IMAGE_LAYOUT_GENERAL;
            dstLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        } else if (oldState == ResourceState::UNDEFINED && newState == ResourceState::COPY_SRC) {
            srcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            srcAccess = 0;
            dstAccess = VK_ACCESS_TRANSFER_READ_BIT;
            srcLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            dstLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        } else if (oldState == ResourceState::COPY_SRC && newState == ResourceState::UNDEFINED) {
            srcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            srcAccess = VK_ACCESS_TRANSFER_READ_BIT;
            dstAccess = 0;
            srcLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            dstLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        } else if (oldState == ResourceState::COPY_SRC && newState == ResourceState::DISPATCH_TARGET) {
            srcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            dstStage = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
            srcAccess = VK_ACCESS_TRANSFER_READ_BIT;
            dstAccess = VK_ACCESS_SHADER_WRITE_BIT;
            srcLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            dstLayout = VK_IMAGE_LAYOUT_GENERAL;
        } else {
            throw Exception("Not implemented");
            return;
        }
        const auto barrier = VkImageMemoryBarrier {
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
            .srcAccessMask =  srcAccess,
            .dstAccessMask = dstAccess,
            .oldLayout = srcLayout,
            .newLayout = dstLayout,
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .image = image,
            .subresourceRange = {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel = 0,
                .levelCount = VK_REMAINING_MIP_LEVELS,
                .baseArrayLayer = 0,
                .layerCount = VK_REMAINING_ARRAY_LAYERS,
            }
        };
        vkCmdPipelineBarrier(commandBuffer,
            srcStage,
            dstStage,
            0,
            0,
            nullptr,
            0,
            nullptr,
            1,
            &barrier);
    }

    void VKCommandList::barrier(
        const shared_ptr<const Image>& image,
        const ResourceState oldState,
        const ResourceState newState) const {
        const auto vkImage = static_pointer_cast<const VKImage>(image);
        barrier(vkImage->getImage(), oldState, newState);
    }

    void VKCommandList::barrier(
        const shared_ptr<const RenderTarget>& renderTarget,
        const ResourceState oldState,
        const ResourceState newState) const {
        const auto vkImage = static_pointer_cast<const VKImage>(renderTarget->getImage());
        barrier(vkImage->getImage(), oldState, newState);
    }

    void VKCommandList::barrier(
        const shared_ptr<const SwapChain>& swapChain,
        const ResourceState oldState,
        const ResourceState newState) const {
        const auto vkSwapChain = static_pointer_cast<const VKSwapChain>(swapChain);
        barrier(vkSwapChain->getCurrentImage(), oldState, newState);
    }

    void VKCommandList::pushConstants(
        const shared_ptr<const PipelineResources>& pipelineResources,
        const PushConstantsDesc& pushConstants,
        const void* data) const {
        const auto vkResources = static_pointer_cast<const VKPipelineResources>(pipelineResources);
        VkShaderStageFlags stageFlags;
        if (pushConstants.stage == ShaderStage::VERTEX) {
            stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        } else if (pushConstants.stage == ShaderStage::FRAGMENT) {
            stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
        }  else {
            stageFlags = VK_SHADER_STAGE_ALL_GRAPHICS;
        }
        vkCmdPushConstants(
            commandBuffer,
            vkResources->getPipelineLayout(),
            stageFlags,
            pushConstants.offset,
            pushConstants.size,
            data);
    }

    void VKCommandList::begin() const {
        constexpr auto beginInfo = VkCommandBufferBeginInfo{
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        };
        vkResetCommandBuffer(commandBuffer, 0);
        vkCheck(vkBeginCommandBuffer(commandBuffer, &beginInfo));
    }

    void VKCommandList::end() const {
        vkCheck(vkEndCommandBuffer(commandBuffer));
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

        vkCmdCopyBufferToImage(
                commandBuffer,
                stagingBuffer,
                image->getImage(),
                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                1,
                &region);

        stagingBuffers.push_back(stagingBuffer);
        stagingBuffersMemory.push_back(stagingBufferMemory);
    }

    void VKCommandList::copy(
        const shared_ptr<const Image>& source,
        const shared_ptr<const SwapChain>& swapChain) const {
        const auto vkSource = static_pointer_cast<const VKImage>(source);
        const auto vkSwapChain = static_pointer_cast<const VKSwapChain>(swapChain);
        auto copyRegion = VkImageCopy {
            .srcSubresource = {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .mipLevel = 0,
                .baseArrayLayer = 0,
                .layerCount = 1,
            },
            .srcOffset = {0, 0, 0},
        };
        copyRegion.dstSubresource = copyRegion.srcSubresource;
        copyRegion.dstOffset = {0, 0, 0};
        copyRegion.extent = {source->getWidth(), source->getHeight(), 1};

        vkCmdCopyImage(commandBuffer,
                       vkSource->getImage(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                       vkSwapChain->getCurrentImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                       1, &copyRegion);
    }

    void VKCommandList::blit(
        const shared_ptr<const Image>& source,
        const shared_ptr<const SwapChain>& swapChain,
        const Filter filter) const {
        const auto vkSource = static_pointer_cast<const VKImage>(source);
        const auto vkSwapChain = static_pointer_cast<const VKSwapChain>(swapChain);
        auto blitRegion = VkImageBlit {
            .srcSubresource = {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .mipLevel = 0,
                .baseArrayLayer = 0,
                .layerCount = 1,
            },
        };
        blitRegion.dstSubresource = blitRegion.srcSubresource;
        blitRegion.srcOffsets[1] = {
            static_cast<int32_t>(source->getWidth()),
            static_cast<int32_t>(source->getHeight()),
            1},
        blitRegion.dstOffsets[1] = {
            static_cast<int32_t>(vkSwapChain->getExtent().width),
            static_cast<int32_t>(vkSwapChain->getExtent().height),
            1},

        vkCmdBlitImage(commandBuffer,
                       vkSource->getImage(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                       vkSwapChain->getCurrentImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                       1, &blitRegion,
                       filter == Filter::LINEAR ? VK_FILTER_LINEAR  : VK_FILTER_NEAREST);
    }



}