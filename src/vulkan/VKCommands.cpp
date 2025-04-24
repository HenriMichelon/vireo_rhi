/*
* Copyright (c) 2025-present Henri Michelon
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/
module;
#include "vireo/backend/vulkan/Libraries.h"
module vireo.vulkan.commands;

import vireo.tools;

import vireo.vulkan.descriptors;
import vireo.vulkan.pipelines;
import vireo.vulkan.tools;
import vireo.vulkan.swapchains;

namespace vireo {

    VKSubmitQueue::VKSubmitQueue(
        const shared_ptr<const VKDevice>& device,
        const CommandType type,
        const wstring& name) {
        vkGetDeviceQueue(
            device->getDevice(),
            type == CommandType::COMPUTE ? device->getComputeQueueFamilyIndex() :
            type == CommandType::TRANSFER ?  device->getTransferQueueFamilyIndex() :
            device->getGraphicsQueueFamilyIndex(),
            0,
            &commandQueue);
#ifdef _DEBUG
        vkSetObjectName(device->getDevice(), reinterpret_cast<uint64_t>(commandQueue), VK_OBJECT_TYPE_QUEUE,
            "VKSubmitQueue : " + to_string(name));
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
        const auto submitInfo = VkSubmitInfo2  {
            .sType                    = VK_STRUCTURE_TYPE_SUBMIT_INFO_2,
            .waitSemaphoreInfoCount   = 1,
            .pWaitSemaphoreInfos      = &vkSwapChain->getCurrentImageAvailableSemaphoreInfo(),
            .commandBufferInfoCount   = static_cast<uint32_t>(submitInfos.size()),
            .pCommandBufferInfos      = submitInfos.data(),
            .signalSemaphoreInfoCount = 1,
            .pSignalSemaphoreInfos    = &vkSwapChain->getCurrentRenderFinishedSemaphoreInfo()
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

    void VKCommandList::bindVertexBuffers(const vector<shared_ptr<const Buffer>>& buffers, const vector<size_t> offsets) const {
        vector<VkBuffer> vkBuffers(buffers.size());
        vector<VkDeviceSize> vkOffsets(buffers.size());
        for (int i = 0; i < buffers.size(); i++) {
            vkBuffers[i] = static_pointer_cast<const VKBuffer>(buffers[i])->getBuffer();
            vkOffsets[i] = offsets.empty() ? 0 : offsets[i];
        }
        vkCmdBindVertexBuffers(commandBuffer, 0, buffers.size(), vkBuffers.data(), vkOffsets.data());
    }

    void VKCommandList::bindVertexBuffer(const shared_ptr<const Buffer>& buffer, const size_t offset) const {
        const auto vkBuffer = static_pointer_cast<const VKBuffer>(buffer);
        const VkBuffer     buffers[] = {vkBuffer->getBuffer()};
        const VkDeviceSize offsets[] = {offset};
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
    }

    void VKCommandList::bindIndexBuffer(const shared_ptr<const Buffer>& buffer, IndexType indexType, const size_t offset) const {
        const auto vkBuffer = static_pointer_cast<const VKBuffer>(buffer);
        vkCmdBindIndexBuffer(commandBuffer,
            vkBuffer->getBuffer(),
            offset,
            vkIndexTypes[static_cast<int>(indexType)]);
    }

    void VKCommandList::draw(
        const uint32_t vertexCountPerInstance,
        const uint32_t instanceCount,
        const uint32_t firstVertex,
        const uint32_t firstInstance) const {
        vkCmdDraw(commandBuffer, vertexCountPerInstance, instanceCount, firstVertex, firstInstance);
    }

    void VKCommandList::drawIndexed(
        const uint32_t indexCountPerInstance,
        const uint32_t instanceCount,
        const uint32_t firstIndex,
        const uint32_t vertexOffset,
        const uint32_t firstInstance) const {
        vkCmdDrawIndexed(commandBuffer, indexCountPerInstance, instanceCount, firstIndex, vertexOffset, firstInstance);
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

    void VKCommandList::beginRendering(const RenderingConfiguration& conf) {
        const auto vkSwapChain =
            conf.swapChain ? static_pointer_cast<VKSwapChain>(conf.swapChain) : nullptr;
        const auto vkColorImage =
            conf.colorRenderTarget ? static_pointer_cast<VKImage>(conf.colorRenderTarget->getImage()) : nullptr;
        const auto vkDepthImage =
            conf.depthRenderTarget ? static_pointer_cast<VKImage>(conf.depthRenderTarget->getImage()) : nullptr;

        const auto colorImageView =
            vkSwapChain ? vkSwapChain->getCurrentImageView() :
            vkColorImage ? vkColorImage->getImageView() :
            VK_NULL_HANDLE;
        uint32_t width{0}, height{0};

        auto depthAttachmentInfo = VkRenderingAttachmentInfo {};
        if (vkDepthImage) {
            depthAttachmentInfo.sType       = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR,
            depthAttachmentInfo.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
            depthAttachmentInfo.loadOp      = conf.clearDepth ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD,
            depthAttachmentInfo.storeOp     = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            depthAttachmentInfo.clearValue  = {
                .depthStencil = {
                    .depth = conf.depthClearValue.depthStencil.depth,
                    .stencil = conf.depthClearValue.depthStencil.stencil,
                }
            };
            width = vkDepthImage->getWidth();
            height = vkDepthImage->getHeight();
        }

        auto colorAttachmentInfo = VkRenderingAttachmentInfo{};
        if (colorImageView) {
            colorAttachmentInfo.sType       = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
            colorAttachmentInfo.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            colorAttachmentInfo.loadOp      = conf.clearColor ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;
            colorAttachmentInfo.storeOp     = VK_ATTACHMENT_STORE_OP_STORE;
            colorAttachmentInfo.clearValue  = {
                conf.clearColorValue.color[0],
                conf.clearColorValue.color[1],
                conf.clearColorValue.color[2],
                conf.clearColorValue.color[3]
            };
            width = vkSwapChain ? vkSwapChain->getExtent().width : vkColorImage->getWidth();
            height = vkSwapChain ? vkSwapChain->getExtent().height : vkColorImage->getHeight();
        }

        if (conf.multisampledColorRenderTarget) {
            const auto msaaColor = static_pointer_cast<VKImage>(conf.multisampledColorRenderTarget->getImage());
            barrier(msaaColor->getImage(), ResourceState::UNDEFINED, ResourceState::RENDER_TARGET_COLOR);
            colorAttachmentInfo.imageView = msaaColor->getImageView(),
            colorAttachmentInfo.resolveMode = VK_RESOLVE_MODE_AVERAGE_BIT;
            colorAttachmentInfo.resolveImageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            colorAttachmentInfo.resolveImageView = colorImageView;

            const auto msaaDepth = conf.multisampledDepthRenderTarget ?
                static_pointer_cast<VKImage>(conf.multisampledDepthRenderTarget->getImage()) :
                VK_NULL_HANDLE;
            if (msaaDepth) {
                depthAttachmentInfo.imageView = msaaDepth->getImageView();
                depthAttachmentInfo.resolveMode = VK_RESOLVE_MODE_AVERAGE_BIT;
                depthAttachmentInfo.resolveImageLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
                depthAttachmentInfo.resolveImageView = vkDepthImage->getImageView();
            }
        } else {
            colorAttachmentInfo.imageView = colorImageView;
            if (vkDepthImage) {
                depthAttachmentInfo.imageView = vkDepthImage->getImageView();
            }
        }
        const auto renderingInfo = VkRenderingInfo {
            .sType               = VK_STRUCTURE_TYPE_RENDERING_INFO_KHR,
            .pNext                = nullptr,
            .renderArea           = {
                {0, 0},
                {width, height}
            },
            .layerCount           = 1,
            .colorAttachmentCount = colorImageView ? 1u : 0u,
            .pColorAttachments    = &colorAttachmentInfo,
            .pDepthAttachment     = vkDepthImage ? &depthAttachmentInfo : nullptr,
            .pStencilAttachment   = nullptr
        };
        vkCmdBeginRendering(commandBuffer, &renderingInfo);
    }

    void VKCommandList::endRendering() {
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
        VkImageAspectFlagBits aspectFlag = VK_IMAGE_ASPECT_COLOR_BIT;
        convertState(oldState, newState, srcStage, dstStage, srcAccess, dstAccess, srcLayout, dstLayout, aspectFlag);
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
                .aspectMask = static_cast<uint32_t>(aspectFlag),
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
        const vector<VkImage>& images,
        const ResourceState oldState,
        const ResourceState newState) const {
        VkPipelineStageFlags srcStage, dstStage;
        VkAccessFlags srcAccess, dstAccess;
        VkImageLayout srcLayout, dstLayout;
        VkImageAspectFlagBits aspectFlag = VK_IMAGE_ASPECT_COLOR_BIT;
        convertState(
            oldState, newState,
            srcStage, dstStage,
            srcAccess, dstAccess,
            srcLayout, dstLayout,
            aspectFlag);

        vector<VkImageMemoryBarrier> barriers(images.size());
        for (int i = 0; i < images.size(); i++) {
            barriers[i].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            barriers[i].srcAccessMask =  srcAccess;
            barriers[i].dstAccessMask = dstAccess;
            barriers[i].oldLayout = srcLayout;
            barriers[i].newLayout = dstLayout;
            barriers[i].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barriers[i].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barriers[i].image = images[i];
            barriers[i].subresourceRange.aspectMask = static_cast<uint32_t>(aspectFlag);
            barriers[i].subresourceRange.baseMipLevel = 0;
            barriers[i].subresourceRange.levelCount = VK_REMAINING_MIP_LEVELS;
            barriers[i].subresourceRange.baseArrayLayer = 0;
            barriers[i].subresourceRange.layerCount = VK_REMAINING_ARRAY_LAYERS;
        }
        vkCmdPipelineBarrier(commandBuffer,
            srcStage,
            dstStage,
            0,
            0,
            nullptr,
            0,
            nullptr,
            barriers.size(),
            barriers.data());
    }

    void VKCommandList::convertState(
            ResourceState oldState,
            ResourceState newState,
            VkPipelineStageFlags& srcStage,
            VkPipelineStageFlags& dstStage,
            VkAccessFlags& srcAccess,
            VkAccessFlags& dstAccess,
            VkImageLayout& srcLayout,
            VkImageLayout& dstLayout,
            VkImageAspectFlagBits& aspectFlag) {
        if (oldState == ResourceState::UNDEFINED && newState == ResourceState::DISPATCH_TARGET) {
            srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            dstStage = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
            srcAccess = 0;
            dstAccess = VK_ACCESS_SHADER_WRITE_BIT;
            srcLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            dstLayout = VK_IMAGE_LAYOUT_GENERAL;
        } else if (oldState == ResourceState::UNDEFINED && newState == ResourceState::RENDER_TARGET_COLOR) {
            srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            dstStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            srcAccess = 0;
            dstAccess = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            srcLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            dstLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        } else if (oldState == ResourceState::UNDEFINED && newState == ResourceState::RENDER_TARGET_DEPTH_STENCIL) {
            srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            dstStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
            srcAccess = 0;
            dstAccess = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
            srcLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            dstLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            aspectFlag = VK_IMAGE_ASPECT_DEPTH_BIT;
        } else if (oldState == ResourceState::RENDER_TARGET_COLOR && newState == ResourceState::COPY_SRC) {
            srcStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            srcAccess = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            dstAccess = 0;
            srcLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            dstLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        } else if (oldState == ResourceState::RENDER_TARGET_COLOR && newState == ResourceState::COMPUTE_READ) {
            srcStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            dstStage = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
            srcAccess = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            dstAccess = VK_ACCESS_SHADER_READ_BIT;
            srcLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            dstLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        } else if (oldState == ResourceState::RENDER_TARGET_COLOR && newState == ResourceState::SHADER_READ) {
            srcStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            dstStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
            srcAccess = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            dstAccess = VK_ACCESS_SHADER_READ_BIT;
            srcLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            dstLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        } else if (oldState == ResourceState::SHADER_READ && newState == ResourceState::UNDEFINED) {
            srcStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
            dstStage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
            srcAccess = VK_ACCESS_SHADER_READ_BIT;
            dstAccess = 0;
            srcLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            dstLayout = VK_IMAGE_LAYOUT_GENERAL;
        } else if (oldState == ResourceState::COMPUTE_READ && newState == ResourceState::UNDEFINED) {
            srcStage = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
            dstStage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
            srcAccess = VK_ACCESS_SHADER_READ_BIT;
            dstAccess = 0;
            srcLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            dstLayout = VK_IMAGE_LAYOUT_GENERAL;
        } else if (oldState == ResourceState::COMPUTE_READ && newState == ResourceState::COPY_SRC) {
            srcStage = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
            dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            srcAccess = VK_ACCESS_SHADER_READ_BIT;
            dstAccess = VK_ACCESS_TRANSFER_READ_BIT;
            srcLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            dstLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        } else if (oldState == ResourceState::COMPUTE_READ && newState == ResourceState::UNDEFINED) {
            srcStage = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
            dstStage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
            srcAccess = VK_ACCESS_SHADER_READ_BIT;
            dstAccess = 0;
            srcLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            dstLayout = VK_IMAGE_LAYOUT_GENERAL;
        } else if (oldState == ResourceState::RENDER_TARGET_COLOR && newState == ResourceState::PRESENT) {
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
            srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
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
            dstLayout = VK_IMAGE_LAYOUT_GENERAL;
        } else if (oldState == ResourceState::RENDER_TARGET_COLOR && newState == ResourceState::UNDEFINED) {
            srcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            dstStage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
            srcAccess = 0;
            dstAccess = 0;
            srcLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            dstLayout = VK_IMAGE_LAYOUT_GENERAL;
        } else if (oldState == ResourceState::DISPATCH_TARGET && newState == ResourceState::UNDEFINED) {
            srcStage = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
            dstStage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
            srcAccess = 0;
            dstAccess = 0;
            srcLayout = VK_IMAGE_LAYOUT_GENERAL;
            dstLayout = VK_IMAGE_LAYOUT_GENERAL;
        } else if (oldState == ResourceState::RENDER_TARGET_DEPTH_STENCIL && newState == ResourceState::UNDEFINED) {
            srcStage = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
            dstStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
            srcAccess = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
            dstAccess = 0;
            srcLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            dstLayout = VK_IMAGE_LAYOUT_GENERAL;
            aspectFlag = VK_IMAGE_ASPECT_DEPTH_BIT;
        } else if (oldState == ResourceState::COPY_SRC && newState == ResourceState::DISPATCH_TARGET) {
            srcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            dstStage = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
            srcAccess = VK_ACCESS_TRANSFER_READ_BIT;
            dstAccess = VK_ACCESS_SHADER_WRITE_BIT;
            srcLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            dstLayout = VK_IMAGE_LAYOUT_GENERAL;
        } else if (oldState == ResourceState::COPY_SRC && newState == ResourceState::RENDER_TARGET_COLOR) {
            srcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            dstStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            srcAccess = 0;
            dstAccess = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            srcLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            dstLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        } else {
            throw Exception("Not implemented");
        }
    }

    void VKCommandList::barrier(
        const shared_ptr<const Image>& image,
        const ResourceState oldState,
        const ResourceState newState) const {
        barrier(static_pointer_cast<const VKImage>(image)->getImage(), oldState, newState);
    }

    void VKCommandList::barrier(
        const shared_ptr<const RenderTarget>& renderTarget,
        const ResourceState oldState,
        const ResourceState newState) const {
        barrier(static_pointer_cast<const VKImage>(renderTarget->getImage())->getImage(), oldState, newState);
    }

    void VKCommandList::barrier(
        const shared_ptr<const SwapChain>& swapChain,
        const ResourceState oldState,
        const ResourceState newState) const {
        barrier(static_pointer_cast<const VKSwapChain>(swapChain)->getCurrentImage(), oldState, newState);
    }

    void VKCommandList::barrier(
            const vector<shared_ptr<const RenderTarget>>& renderTargets,
            ResourceState oldState,
            ResourceState newState) const {
        const auto r = views::transform(renderTargets, [](const shared_ptr<const RenderTarget>& renderTarget) {
            return static_pointer_cast<const VKImage>(renderTarget->getImage())->getImage();
        });
        barrier(vector<VkImage>{r.begin(), r.end()}, oldState, newState);
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
        stagingBuffers.clear();
    }

    void VKCommandList::upload(const shared_ptr<const Buffer>& destination, const void* source) {
        const auto buffer = static_pointer_cast<const VKBuffer>(destination);
        const auto stagingBuffer = make_shared<VKBuffer>(
            device,
            BufferType::TRANSFER,
            buffer->getInstanceSize(),
            buffer->getInstanceCount(),
            L"StagingBuffer for buffer");
        stagingBuffer->map();
        if ((buffer->getAlignmentSize() == 1) || (buffer->getInstanceCount() == 1)) {
            stagingBuffer->write(source);
        } else {
            for (int i = 0; i < buffer->getInstanceCount(); i++) {
                stagingBuffer->write(
                    static_cast<const unsigned char*>(source) + i * buffer->getInstanceSize(),
                    buffer->getInstanceSize(),
                    buffer->getAlignmentSize() * i);
            }
        }
        stagingBuffer->unmap();

        const auto copyRegion = VkBufferCopy{
            .srcOffset = 0,
            .dstOffset = 0,
            .size = buffer->getSize(),
        };
        vkCmdCopyBuffer(
            commandBuffer,
            stagingBuffer->getBuffer(),
            buffer->getBuffer(),
            1,
            &copyRegion);

        stagingBuffers.push_back(stagingBuffer);
    }

    void VKCommandList::upload(const shared_ptr<const Image>& destination, const void* source) {
        const auto image = static_pointer_cast<const VKImage>(destination);
        const auto stagingBuffer = make_shared<VKBuffer>(
           device,
           BufferType::TRANSFER,
           image->getImageSize(),
           image->getArraySize(),
           L"StagingBuffer for image");
        stagingBuffer->map();
        if (image->getArraySize() == 1) {
            stagingBuffer->write(source);
        } else {
            for (int i = 0; i < image->getArraySize(); i++) {
                stagingBuffer->write(
                    static_cast<const unsigned char*>(source) + i * image->getImageSize(),
                    image->getImageSize(),
                    stagingBuffer->getAlignmentSize() * i);
            }
        }
        stagingBuffer->unmap();

        // https://vulkan-tutorial.com/Texture_mapping/Images#page_Copying-buffer-to-image
        const auto region = VkBufferImageCopy {
            .bufferOffset = 0,
            .bufferRowLength = 0,
            .bufferImageHeight = 0,
            .imageSubresource = {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .mipLevel = 0,
                .baseArrayLayer = 0,
                .layerCount = destination->getArraySize(),
            },
            .imageOffset = {0, 0, 0},
            .imageExtent = {image->getWidth(), image->getHeight(), 1},
        };

        vkCmdCopyBufferToImage(
                commandBuffer,
                stagingBuffer->getBuffer(),
                image->getImage(),
                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                1,
                &region);

        stagingBuffers.push_back(stagingBuffer);
    }

    void VKCommandList::upload(const shared_ptr<const Image>& destination, const vector<void*>& sources) {
        assert(sources.size() == destination->getArraySize());
        const auto image = static_pointer_cast<const VKImage>(destination);
        const auto stagingBuffer = make_shared<VKBuffer>(
           device,
           BufferType::TRANSFER,
           image->getImageSize(),
           image->getArraySize(),
           L"StagingBuffer for image array");
        stagingBuffer->map();
        for (int i = 0; i < image->getArraySize(); i++) {
            stagingBuffer->write(
                sources[i],
                image->getImageSize(),
                stagingBuffer->getAlignmentSize() * i);
        }
        stagingBuffer->unmap();

        const auto region = VkBufferImageCopy {
            .bufferOffset = 0,
            .bufferRowLength = 0,
            .bufferImageHeight = 0,
            .imageSubresource = {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .mipLevel = 0,
                .baseArrayLayer = 0,
                .layerCount = destination->getArraySize(),
            },
            .imageOffset = {0, 0, 0},
            .imageExtent = {image->getWidth(), image->getHeight(), 1},
        };
        vkCmdCopyBufferToImage(
                commandBuffer,
                stagingBuffer->getBuffer(),
                image->getImage(),
                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                1,
                &region);
        stagingBuffers.push_back(stagingBuffer);
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