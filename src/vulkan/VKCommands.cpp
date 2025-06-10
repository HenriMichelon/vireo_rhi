/*
* Copyright (c) 2025-present Henri Michelon
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/
module;
#include "vireo/backend/vulkan/Libraries.h"
#include <cassert>
module vireo.vulkan.commands;

import std;
import vireo.tools;
import vireo.vulkan.descriptors;
import vireo.vulkan.pipelines;
import vireo.vulkan.tools;
import vireo.vulkan.swapchains;

namespace vireo {

    VKSubmitQueue::VKSubmitQueue(
        const std::shared_ptr<const VKDevice>& device,
        const CommandType type,
        const std::wstring& name) {
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
        const std::shared_ptr<Fence>& fence,
        const std::shared_ptr<const SwapChain>& swapChain,
        const std::vector<std::shared_ptr<const CommandList>>& commandLists) const {
        assert(fence != nullptr);
        assert(swapChain != nullptr);
        assert(!commandLists.empty());
        const auto vkSwapChain = static_pointer_cast<const VKSwapChain>(swapChain);
        const auto vkFence = static_pointer_cast<const VKFence>(fence);
        auto submitInfos = std::vector<VkCommandBufferSubmitInfo>(commandLists.size());
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

    void VKSubmitQueue::submit(const std::vector<std::shared_ptr<const CommandList>>& commandLists) const {
        assert(!commandLists.empty());
        auto submitInfos = std::vector<VkCommandBufferSubmitInfo>(commandLists.size());
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

    void VKSubmitQueue::submit(
        const std::shared_ptr<Fence>& fence,
        const std::vector<std::shared_ptr<const CommandList>>& commandLists) const {
        assert(fence != nullptr);
        assert(!commandLists.empty());
        const auto vkFence = static_pointer_cast<const VKFence>(fence);
        auto submitInfos = std::vector<VkCommandBufferSubmitInfo>(commandLists.size());
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
        vkCheck(vkQueueSubmit2(commandQueue, 1, &submitInfo, vkFence->getFence()));
    }

    void VKSubmitQueue::submit(
           const std::shared_ptr<Semaphore>& waitSemaphore,
           const WaitStage waitStage,
           const WaitStage signalStage,
           const std::shared_ptr<Semaphore>& signalSemaphore,
           const std::vector<std::shared_ptr<const CommandList>>& commandLists) const {
        assert(waitSemaphore != nullptr || signalSemaphore != nullptr);
        assert(!commandLists.empty());
        const auto vkWaitSemaphore = static_pointer_cast<VKSemaphore>(waitSemaphore);
        const auto vkSignalSemaphore = static_pointer_cast<VKSemaphore>(signalSemaphore);
        auto submitInfos = std::vector<VkCommandBufferSubmitInfo>(commandLists.size());
        for (int i = 0; i < commandLists.size(); i++) {
            submitInfos[i] = {
                .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO,
                .commandBuffer = static_pointer_cast<const VKCommandList>(commandLists[i])->getCommandBuffer(),
            };
        }
        auto waitSemaphoreSubmitInfo = VkSemaphoreSubmitInfo{
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO
        };
        if (vkWaitSemaphore) {
            waitSemaphoreSubmitInfo.semaphore = vkWaitSemaphore->getSemaphore();
            waitSemaphoreSubmitInfo.stageMask = static_cast<VkPipelineStageFlags2>(waitStage);
            waitSemaphoreSubmitInfo.value = vkWaitSemaphore->getValue();
        }
        auto signalSemaphoreSubmitInfo = VkSemaphoreSubmitInfo{
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO
        };
        if (vkSignalSemaphore) {
            if (vkSignalSemaphore->getType() == SemaphoreType::TIMELINE) {
                vkSignalSemaphore->incrementValue();
            }
            signalSemaphoreSubmitInfo.semaphore = vkSignalSemaphore->getSemaphore();
            signalSemaphoreSubmitInfo.stageMask = static_cast<VkPipelineStageFlags2>(signalStage);
            signalSemaphoreSubmitInfo.value = vkSignalSemaphore->getValue();
        }
        const auto submitInfo = VkSubmitInfo2 {
            .sType                    = VK_STRUCTURE_TYPE_SUBMIT_INFO_2,
            .waitSemaphoreInfoCount   = waitSemaphore ? 1u : 0u,
            .pWaitSemaphoreInfos      = waitSemaphore ? &waitSemaphoreSubmitInfo : VK_NULL_HANDLE,
            .commandBufferInfoCount   = static_cast<uint32_t>(submitInfos.size()),
            .pCommandBufferInfos      = submitInfos.data(),
            .signalSemaphoreInfoCount = signalSemaphore ? 1u : 0u,
            .pSignalSemaphoreInfos    = signalSemaphore ? &signalSemaphoreSubmitInfo : VK_NULL_HANDLE,
        };
        vkCheck(vkQueueSubmit2(commandQueue, 1, &submitInfo, VK_NULL_HANDLE));
    }

    void VKSubmitQueue::submit(
           const std::shared_ptr<Semaphore>& waitSemaphore,
           const std::vector<WaitStage>& waitStages,
           const WaitStage signalStage,
           const std::shared_ptr<Semaphore>& signalSemaphore,
           const std::vector<std::shared_ptr<const CommandList>>& commandLists) const {
        assert(waitSemaphore != nullptr || signalSemaphore != nullptr);
        assert(!commandLists.empty());
        const auto vkWaitSemaphore = static_pointer_cast<VKSemaphore>(waitSemaphore);
        const auto vkSignalSemaphore = static_pointer_cast<VKSemaphore>(signalSemaphore);
        auto submitInfos = std::vector<VkCommandBufferSubmitInfo>(commandLists.size());
        for (int i = 0; i < commandLists.size(); i++) {
            submitInfos[i] = {
                .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO,
                .commandBuffer = static_pointer_cast<const VKCommandList>(commandLists[i])->getCommandBuffer(),
            };
        }
        std::vector<VkSemaphoreSubmitInfo> waitSemaphoreSubmitInfos(waitStages.size());
        if (vkWaitSemaphore) {
            assert(waitSemaphore->getType() == SemaphoreType::TIMELINE);
            assert(waitStages.size() > 0);
            for (int i = 0; i < waitStages.size(); i++) {
                waitSemaphoreSubmitInfos[i].sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
                waitSemaphoreSubmitInfos[i].semaphore = vkWaitSemaphore->getSemaphore();
                waitSemaphoreSubmitInfos[i].stageMask = static_cast<VkPipelineStageFlags2>(waitStages[i]);
                waitSemaphoreSubmitInfos[i].value = vkWaitSemaphore->getValue() + i;
            }
        }
        auto signalSemaphoreSubmitInfo = VkSemaphoreSubmitInfo{
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO
        };
        if (vkSignalSemaphore) {
            if (vkSignalSemaphore->getType() == SemaphoreType::TIMELINE) {
                vkSignalSemaphore->incrementValue();
            }
            signalSemaphoreSubmitInfo.semaphore = vkSignalSemaphore->getSemaphore();
            signalSemaphoreSubmitInfo.stageMask = static_cast<VkPipelineStageFlags2>(signalStage);
            signalSemaphoreSubmitInfo.value = vkSignalSemaphore->getValue();
        }
        const auto submitInfo = VkSubmitInfo2 {
            .sType                    = VK_STRUCTURE_TYPE_SUBMIT_INFO_2,
            .waitSemaphoreInfoCount   = waitSemaphore ? static_cast<uint32_t>(waitSemaphoreSubmitInfos.size()) : 0u,
            .pWaitSemaphoreInfos      = waitSemaphore ? waitSemaphoreSubmitInfos.data() : VK_NULL_HANDLE,
            .commandBufferInfoCount   = static_cast<uint32_t>(submitInfos.size()),
            .pCommandBufferInfos      = submitInfos.data(),
            .signalSemaphoreInfoCount = signalSemaphore ? 1u : 0u,
            .pSignalSemaphoreInfos    = signalSemaphore ? &signalSemaphoreSubmitInfo : VK_NULL_HANDLE,
        };
        vkCheck(vkQueueSubmit2(commandQueue, 1, &submitInfo, VK_NULL_HANDLE));
    }


    void VKSubmitQueue::submit(
           const std::shared_ptr<Semaphore>& waitSemaphore,
           WaitStage waitStage,
           const std::shared_ptr<Fence>& fence,
           const std::shared_ptr<const SwapChain>& swapChain,
           const std::vector<std::shared_ptr<const CommandList>>& commandLists) const {
        assert(waitSemaphore != nullptr);
        assert(fence != nullptr);
        assert(swapChain != nullptr);
        assert(!commandLists.empty());
        const auto vkSwapChain = static_pointer_cast<const VKSwapChain>(swapChain);
        const auto vkFence = static_pointer_cast<const VKFence>(fence);
        const auto vkWaitSemaphore = static_pointer_cast<VKSemaphore>(waitSemaphore);
        auto submitInfos = std::vector<VkCommandBufferSubmitInfo>(commandLists.size());
        for (int i = 0; i < commandLists.size(); i++) {
            submitInfos[i] = {
                .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO,
                .commandBuffer = static_pointer_cast<const VKCommandList>(commandLists[i])->getCommandBuffer(),
            };
        }

        auto waitSubmitInfos = std::vector {
            vkSwapChain->getCurrentImageAvailableSemaphoreInfo()
        };
        if (vkWaitSemaphore) {
            waitSubmitInfos.push_back(VkSemaphoreSubmitInfo{
                .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
                .semaphore = vkWaitSemaphore->getSemaphore(),
                .value = vkWaitSemaphore->getValue(),
                .stageMask = static_cast<VkPipelineStageFlags2>(waitStage),
            });
        }

        const auto submitInfo = VkSubmitInfo2 {
            .sType                    = VK_STRUCTURE_TYPE_SUBMIT_INFO_2,
            .waitSemaphoreInfoCount   = static_cast<uint32_t>(waitSubmitInfos.size()),
            .pWaitSemaphoreInfos      = waitSubmitInfos.data(),
            .commandBufferInfoCount   = static_cast<uint32_t>(submitInfos.size()),
            .pCommandBufferInfos      = submitInfos.data(),
            .signalSemaphoreInfoCount = 1,
            .pSignalSemaphoreInfos    = &vkSwapChain->getCurrentRenderFinishedSemaphoreInfo(),
        };
        vkCheck(vkQueueSubmit2(commandQueue, 1, &submitInfo, vkFence->getFence()));
    }

    void VKSubmitQueue::submit(
           const std::shared_ptr<Semaphore>& waitSemaphore,
           const std::vector<WaitStage>& waitStages,
           const std::shared_ptr<Fence>& fence,
           const std::shared_ptr<const SwapChain>& swapChain,
           const std::vector<std::shared_ptr<const CommandList>>& commandLists) const {
        assert(waitSemaphore != nullptr);
        assert(fence != nullptr);
        assert(swapChain != nullptr);
        assert(!commandLists.empty());
        const auto vkSwapChain = static_pointer_cast<const VKSwapChain>(swapChain);
        const auto vkFence = static_pointer_cast<const VKFence>(fence);
        const auto vkWaitSemaphore = static_pointer_cast<VKSemaphore>(waitSemaphore);
        auto submitInfos = std::vector<VkCommandBufferSubmitInfo>(commandLists.size());
        for (int i = 0; i < commandLists.size(); i++) {
            submitInfos[i] = {
                .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO,
                .commandBuffer = static_pointer_cast<const VKCommandList>(commandLists[i])->getCommandBuffer(),
            };
        }

        auto waitSubmitInfos = std::vector {
            vkSwapChain->getCurrentImageAvailableSemaphoreInfo()
        };
        std::vector<VkSemaphoreSubmitInfo> waitSemaphoreSubmitInfos(waitStages.size());
        if (vkWaitSemaphore) {
            assert(waitSemaphore->getType() == SemaphoreType::TIMELINE);
            assert(waitStages.size() > 0);
            for (int i = 0; i < waitStages.size(); i++) {
                waitSubmitInfos.push_back(VkSemaphoreSubmitInfo{
                    .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
                    .semaphore = vkWaitSemaphore->getSemaphore(),
                    .value = vkWaitSemaphore->getValue() + i,
                    .stageMask = static_cast<VkPipelineStageFlags2>(waitStages[i]),
                });
            }
        }

        const auto submitInfo = VkSubmitInfo2 {
            .sType                    = VK_STRUCTURE_TYPE_SUBMIT_INFO_2,
            .waitSemaphoreInfoCount   = static_cast<uint32_t>(waitSubmitInfos.size()),
            .pWaitSemaphoreInfos      = waitSubmitInfos.data(),
            .commandBufferInfoCount   = static_cast<uint32_t>(submitInfos.size()),
            .pCommandBufferInfos      = submitInfos.data(),
            .signalSemaphoreInfoCount = 1,
            .pSignalSemaphoreInfos    = &vkSwapChain->getCurrentRenderFinishedSemaphoreInfo(),
        };
        vkCheck(vkQueueSubmit2(commandQueue, 1, &submitInfo, vkFence->getFence()));
    }

    VKCommandAllocator::VKCommandAllocator(const std::shared_ptr<const VKDevice>& device, const CommandType type):
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

    std::shared_ptr<CommandList> VKCommandAllocator::createCommandList(const Pipeline&) const {
        return createCommandList();
    }

    std::shared_ptr<CommandList> VKCommandAllocator::createCommandList() const {
        return std::make_shared<VKCommandList>(device, commandPool);
    }

    VKCommandList::VKCommandList(const std::shared_ptr<const VKDevice>& device, const VkCommandPool commandPool) :
        device{device} {
        const auto allocInfo = VkCommandBufferAllocateInfo {
            .sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .commandPool        = commandPool,
            .level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = 1,
        };
        vkCheck(vkAllocateCommandBuffers(device->getDevice(), &allocInfo, &commandBuffer));
    }

    VKCommandList::~VKCommandList() {
        cleanup();
    }

    void VKCommandList::bindVertexBuffers(const std::vector<std::shared_ptr<const Buffer>>& buffers, const std::vector<size_t> offsets) const {
        assert(!buffers.empty());
        assert(buffers.size() == offsets.size());
        std::vector<VkBuffer> vkBuffers(buffers.size());
        std::vector<VkDeviceSize> vkOffsets(buffers.size());
        for (int i = 0; i < buffers.size(); i++) {
            vkBuffers[i] = static_pointer_cast<const VKBuffer>(buffers[i])->getBuffer();
            vkOffsets[i] = offsets.empty() ? 0 : offsets[i];
        }
        vkCmdBindVertexBuffers(commandBuffer, 0, buffers.size(), vkBuffers.data(), vkOffsets.data());
    }

    void VKCommandList::bindVertexBuffer(const Buffer& buffer, const size_t offset) const {
        const auto& vkBuffer = static_cast<const VKBuffer&>(buffer);
        const VkBuffer     buffers[] = {vkBuffer.getBuffer()};
        const VkDeviceSize offsets[] = {offset};
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
    }

    void VKCommandList::bindIndexBuffer(const Buffer& buffer, IndexType indexType, const uint32_t firstIndex) const {
        const auto& vkBuffer = static_cast<const VKBuffer&>(buffer);
        vkCmdBindIndexBuffer(commandBuffer,
            vkBuffer.getBuffer(),
            firstIndex * indexTypeSize[static_cast<int>(indexType)],
            vkIndexTypes[static_cast<int>(indexType)]);
    }

    void VKCommandList::draw(
        const uint32_t vertexCountPerInstance,
        const uint32_t instanceCount,
        const uint32_t firstVertex,
        const uint32_t firstInstance) const {
        vkCmdDraw(commandBuffer, vertexCountPerInstance, instanceCount, firstVertex, firstInstance);
    }

    void VKCommandList::drawIndirect(
        const Buffer& buffer,
        const size_t offset,
        const uint32_t drawCount,
        const uint32_t stride,
        const uint32_t firstCommandOffset) {
        const auto& vkBuffer = static_cast<const VKBuffer&>(buffer);
        vkCmdDrawIndirect(commandBuffer, vkBuffer.getBuffer(), offset, drawCount, stride);
    }

    void VKCommandList::drawIndexed(
        const uint32_t indexCountPerInstance,
        const uint32_t instanceCount,
        const uint32_t firstIndex,
        const uint32_t vertexOffset,
        const uint32_t firstInstance) const {
        vkCmdDrawIndexed(commandBuffer, indexCountPerInstance, instanceCount, firstIndex, vertexOffset, firstInstance);
    }

    void VKCommandList::drawIndexedIndirect(
        const Buffer& buffer,
        const size_t offset,
        const uint32_t drawCount,
        const uint32_t stride,
        const uint32_t firstCommandOffset) {
        const auto& vkBuffer = static_cast<const VKBuffer&>(buffer);
        vkCmdDrawIndexedIndirect(
            commandBuffer,
            vkBuffer.getBuffer(),
            offset + firstCommandOffset,
            drawCount,
            stride);
    }

    void VKCommandList::drawIndexedIndirectCount(
        Buffer& buffer,
        const size_t offset,
        Buffer& countBuffer,
        const size_t countOffset,
        const uint32_t maxDrawCount,
        const uint32_t stride,
        const uint32_t firstCommandOffset) {
        const auto& vkBuffer = static_cast<const VKBuffer&>(buffer);
        const auto& vkCountBuffer = static_cast<const VKBuffer&>(countBuffer);
        vkCmdDrawIndexedIndirectCount(
            commandBuffer,
            vkBuffer.getBuffer(),
            offset + firstCommandOffset,
            vkCountBuffer.getBuffer(),
            countOffset,
            maxDrawCount, stride);
    }

    void VKCommandList::bindPipeline(Pipeline& pipeline) {
        if (pipeline.getType() == PipelineType::COMPUTE) {
            vkCmdBindPipeline(
                commandBuffer,
                VK_PIPELINE_BIND_POINT_COMPUTE ,
                static_cast<const VKComputePipeline&>(pipeline).getPipeline());
        } else {
            vkCmdBindPipeline(
                commandBuffer,
                VK_PIPELINE_BIND_POINT_GRAPHICS ,
                static_cast<const VKGraphicPipeline&>(pipeline).getPipeline());
        }
        currentlyBoundPipeline = &pipeline;
    }

    void VKCommandList::bindDescriptors(
        const std::vector<std::shared_ptr<const DescriptorSet>>& descriptors,
        const uint32_t firstSet) const {
        assert(!descriptors.empty());
        assert(currentlyBoundPipeline != nullptr);
        const auto vkLayout = static_pointer_cast<const VKPipelineResources>(currentlyBoundPipeline->getResources())->getPipelineLayout();
        std::vector<VkDescriptorSet> descriptorSets(descriptors.size());
        for (int i = 0; i < descriptors.size(); i++) {
            descriptorSets[i] = static_pointer_cast<const VKDescriptorSet>(descriptors[i])->getSet();
        }
        vkCmdBindDescriptorSets(commandBuffer,
                                currentlyBoundPipeline->getType() == PipelineType::COMPUTE ?
                                    VK_PIPELINE_BIND_POINT_COMPUTE :
                                    VK_PIPELINE_BIND_POINT_GRAPHICS,
                                vkLayout,
                                firstSet,
                                descriptorSets.size(),
                                descriptorSets.data(),
                                0,
                                nullptr);
    }

    void VKCommandList::bindDescriptor(
        const DescriptorSet& descriptor,
        const uint32_t set) const {
        assert(currentlyBoundPipeline != nullptr);
        const auto vkLayout = static_pointer_cast<const VKPipelineResources>(currentlyBoundPipeline->getResources())->getPipelineLayout();
        const auto& descriptorSet = static_cast<const VKDescriptorSet&>(descriptor).getSet();
        vkCmdBindDescriptorSets(commandBuffer,
                                currentlyBoundPipeline->getType() == PipelineType::COMPUTE ?
                                    VK_PIPELINE_BIND_POINT_COMPUTE :
                                    VK_PIPELINE_BIND_POINT_GRAPHICS,
                                vkLayout,
                                set,
                                1,
                                &descriptorSet,
                                0,
                                nullptr);
    }

    void VKCommandList::bindDescriptor(
        const DescriptorSet& descriptor,
        const uint32_t set,
        const uint32_t offset) const {
        assert(descriptor.getLayout()->isDynamicUniform());
        assert(currentlyBoundPipeline != nullptr);
        const auto vkLayout = static_pointer_cast<const VKPipelineResources>(currentlyBoundPipeline->getResources())->getPipelineLayout();
        const auto& descriptorSet = static_cast<const VKDescriptorSet&>(descriptor).getSet();
        vkCmdBindDescriptorSets(commandBuffer,
                                currentlyBoundPipeline->getType() == PipelineType::COMPUTE ?
                                    VK_PIPELINE_BIND_POINT_COMPUTE :
                                    VK_PIPELINE_BIND_POINT_GRAPHICS,
                                vkLayout,
                                set,
                                1,
                                &descriptorSet,
                                1,
                                &offset);
    }

    void VKCommandList::setStencilReference(const uint32_t reference) const {
        vkCmdSetStencilReference(commandBuffer, VK_STENCIL_FACE_FRONT_AND_BACK, reference);
    }

    void VKCommandList::setViewports(const std::vector<Viewport>& viewports) const {
        std::vector<VkViewport> vkViewports(viewports.size());
        for (int i = 0; i < viewports.size(); i++) {
            vkViewports[i].x = viewports[i].x;
            vkViewports[i].y = viewports[i].y;
            vkViewports[i].width = viewports[i].width;
            vkViewports[i].height = viewports[i].height;
            vkViewports[i].minDepth = viewports[i].minDepth;
            vkViewports[i].maxDepth = viewports[i].maxDepth;
        }
        vkCmdSetViewportWithCount(commandBuffer, vkViewports.size(), vkViewports.data());
    }

    void VKCommandList::setScissors(const std::vector<Rect>& rects) const {
        std::vector<VkRect2D> scissors(rects.size());
        for (int i = 0; i < scissors.size(); i++) {
            scissors[i].offset = {max(rects[i].x, 0), max(rects[i].y, 0)};
            scissors[i].extent.width = rects[i].width;
            scissors[i].extent.height = rects[i].height;
        }
        vkCmdSetScissorWithCount(commandBuffer, scissors.size(), scissors.data());
    }

    void VKCommandList::setViewport(const Viewport& viewport) const {
        const auto vkViewport = VkViewport {
            .x = viewport.x,
            .y = viewport.y + viewport.height,
            .width = viewport.width,
            .height = -viewport.height,
            .minDepth = viewport.minDepth,
            .maxDepth = viewport.maxDepth,
        };
        vkCmdSetViewportWithCount(commandBuffer, 1, &vkViewport);
    }

    void VKCommandList::setScissors(const Rect& rect) const {
        const auto scissor = VkRect2D{
            .offset = { max(rect.x, 0), max(rect.y, 0)},
            .extent = { rect.width, rect.height },
        };
        vkCmdSetScissorWithCount(commandBuffer, 1, &scissor);
    }

    void VKCommandList::beginRendering(const RenderingConfiguration& conf) {
        uint32_t width{0}, height{0};
        const auto vkDepthImage =
            conf.depthStencilRenderTarget ? static_pointer_cast<VKImage>(conf.depthStencilRenderTarget->getImage()) : nullptr;
        auto depthAttachmentInfo = VkRenderingAttachmentInfo {};
        auto stencilAttachmentInfo = VkRenderingAttachmentInfo {};
        if (vkDepthImage) {
            auto dsAttachmentInfo = VkRenderingAttachmentInfo {};
            dsAttachmentInfo.sType       = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR,
            dsAttachmentInfo.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
            dsAttachmentInfo.loadOp      = conf.clearDepthStencil ?
                VK_ATTACHMENT_LOAD_OP_CLEAR :
                VK_ATTACHMENT_LOAD_OP_LOAD,
            dsAttachmentInfo.storeOp     = conf.discardDepthStencilAfterRender ?
                VK_ATTACHMENT_STORE_OP_DONT_CARE :
                VK_ATTACHMENT_STORE_OP_STORE,
            dsAttachmentInfo.clearValue  = {
                .depthStencil = {
                    .depth = conf.depthStencilClearValue.depthStencil.depth,
                    .stencil = conf.depthStencilClearValue.depthStencil.stencil,
                }
            };
            width = vkDepthImage->getWidth();
            height = vkDepthImage->getHeight();

            const auto msaaDepth = conf.multisampledDepthStencilRenderTarget ?
                static_pointer_cast<VKImage>(conf.multisampledDepthStencilRenderTarget->getImage()) :
                VK_NULL_HANDLE;
            if (msaaDepth) {
                dsAttachmentInfo.imageView = msaaDepth->getImageView();
                dsAttachmentInfo.resolveMode = VK_RESOLVE_MODE_AVERAGE_BIT;
                dsAttachmentInfo.resolveImageLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
                dsAttachmentInfo.resolveImageView = vkDepthImage->getImageView();
            } else {
                dsAttachmentInfo.imageView   = vkDepthImage->getImageView();
            }

            if (conf.depthTestEnable) {
                depthAttachmentInfo = dsAttachmentInfo;
            }
            if (conf.stencilTestEnable) {
                stencilAttachmentInfo = dsAttachmentInfo;
            }
        }

        auto colorAttachmentsInfo = std::vector<VkRenderingAttachmentInfo>(conf.colorRenderTargets.size());
        for (int i = 0; i < conf.colorRenderTargets.size(); i++) {
            const auto vkSwapChain = conf.colorRenderTargets[i].swapChain ?
                static_pointer_cast<VKSwapChain>(conf.colorRenderTargets[i].swapChain) : nullptr;
            const auto vkColorImage = conf.colorRenderTargets[i].renderTarget ?
                static_pointer_cast<VKImage>(conf.colorRenderTargets[i].renderTarget->getImage()) : nullptr;
            const auto colorImageView =
                vkSwapChain ? vkSwapChain->getCurrentImageView() :
                vkColorImage ? vkColorImage->getImageView() :
                VK_NULL_HANDLE;

            if (colorImageView) {
                colorAttachmentsInfo[i].sType       = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
                colorAttachmentsInfo[i].imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                colorAttachmentsInfo[i].loadOp      = conf.colorRenderTargets[i].clear ?
                    VK_ATTACHMENT_LOAD_OP_CLEAR :
                    VK_ATTACHMENT_LOAD_OP_LOAD;
                colorAttachmentsInfo[i].storeOp     = conf.colorRenderTargets[i].discardAfterRender ?
                    VK_ATTACHMENT_STORE_OP_DONT_CARE :
                    VK_ATTACHMENT_STORE_OP_STORE;
                colorAttachmentsInfo[i].clearValue  = {
                    conf.colorRenderTargets[i].clearValue.color[0],
                    conf.colorRenderTargets[i].clearValue.color[1],
                    conf.colorRenderTargets[i].clearValue.color[2],
                    conf.colorRenderTargets[i].clearValue.color[3]
                };
                width = vkSwapChain ? vkSwapChain->getExtent().width : vkColorImage->getWidth();
                height = vkSwapChain ? vkSwapChain->getExtent().height : vkColorImage->getHeight();
            }

            if (conf.colorRenderTargets[i].multisampledRenderTarget) {
                const auto msaaColor =
                    static_pointer_cast<VKImage>(conf.colorRenderTargets[i].multisampledRenderTarget->getImage());
                barrier(msaaColor->getImage(), ResourceState::UNDEFINED, ResourceState::RENDER_TARGET_COLOR, 1, 1);
                colorAttachmentsInfo[i].imageView = msaaColor->getImageView(),
                colorAttachmentsInfo[i].resolveMode = VK_RESOLVE_MODE_AVERAGE_BIT;
                colorAttachmentsInfo[i].resolveImageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                colorAttachmentsInfo[i].resolveImageView = colorImageView;
            } else {
                colorAttachmentsInfo[i].imageView = colorImageView;
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
            .colorAttachmentCount = static_cast<uint32_t>(colorAttachmentsInfo.size()),
            .pColorAttachments    = colorAttachmentsInfo.empty() ? nullptr : colorAttachmentsInfo.data(),
            .pDepthAttachment     = depthAttachmentInfo.imageView != VK_NULL_HANDLE ? &depthAttachmentInfo : nullptr,
            .pStencilAttachment   = stencilAttachmentInfo.imageView != VK_NULL_HANDLE ? &stencilAttachmentInfo : nullptr,
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
        const ResourceState newState,
        const uint32_t firstMipLevel,
        const uint32_t levelCount) const {
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
                .baseMipLevel = firstMipLevel,
                .levelCount = levelCount,
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
        const std::vector<VkImage>& images,
        const ResourceState oldState,
        const ResourceState newState) const {
        assert(!images.empty());
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

        std::vector<VkImageMemoryBarrier> barriers(images.size());
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
            const ResourceState oldState,
            const ResourceState newState,
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
        } else if (oldState == ResourceState::SHADER_READ && newState == ResourceState::RENDER_TARGET_COLOR) {
            srcStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
            dstStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            srcAccess = VK_ACCESS_SHADER_READ_BIT;
            dstAccess = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            srcLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            dstLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        } else if (oldState == ResourceState::UNDEFINED &&
                  (newState == ResourceState::RENDER_TARGET_DEPTH_STENCIL || newState == ResourceState::RENDER_TARGET_DEPTH)) {
            srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            dstStage = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
            srcAccess = 0;
            dstAccess = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
            srcLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            dstLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            aspectFlag = newState == ResourceState::RENDER_TARGET_DEPTH ?
                VK_IMAGE_ASPECT_DEPTH_BIT :
                static_cast<VkImageAspectFlagBits>(VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT);
        } else if ((oldState == ResourceState::RENDER_TARGET_DEPTH_STENCIL || oldState == ResourceState::RENDER_TARGET_DEPTH) &&
                   (newState == ResourceState::RENDER_TARGET_DEPTH_STENCIL_READ || newState == ResourceState::RENDER_TARGET_DEPTH_READ)) {
            srcStage = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
            dstStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
            srcAccess = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
            dstAccess = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
            srcLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            dstLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
            aspectFlag = (newState == ResourceState::RENDER_TARGET_DEPTH_READ || oldState == ResourceState::RENDER_TARGET_DEPTH) ?
                VK_IMAGE_ASPECT_DEPTH_BIT :
                static_cast<VkImageAspectFlagBits>(VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT);
        } else if ((oldState == ResourceState::RENDER_TARGET_DEPTH_STENCIL_READ || oldState == ResourceState::RENDER_TARGET_DEPTH_READ) &&
                   (newState == ResourceState::RENDER_TARGET_DEPTH_STENCIL || newState == ResourceState::RENDER_TARGET_DEPTH)) {
            srcStage = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
            dstStage = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
            srcAccess = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
            dstAccess = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
            srcLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
            dstLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            aspectFlag = (oldState == ResourceState::RENDER_TARGET_DEPTH_READ || newState == ResourceState::RENDER_TARGET_DEPTH)?
                VK_IMAGE_ASPECT_DEPTH_BIT :
                static_cast<VkImageAspectFlagBits>(VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT);
        } else if (oldState == ResourceState::UNDEFINED &&
            (newState == ResourceState::RENDER_TARGET_DEPTH_STENCIL_READ || newState == ResourceState::RENDER_TARGET_DEPTH_READ)) {
            srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            dstStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
            srcAccess = 0;
            dstAccess = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
            srcLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            dstLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
            aspectFlag = newState == ResourceState::RENDER_TARGET_DEPTH_READ ?
                VK_IMAGE_ASPECT_DEPTH_BIT :
                static_cast<VkImageAspectFlagBits>(VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT);
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
        } else if (oldState == ResourceState::UNDEFINED && newState == ResourceState::SHADER_READ) {
            srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            dstStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
            srcAccess = 0;
            dstAccess = VK_ACCESS_SHADER_READ_BIT;
            srcLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            dstLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        } else if (oldState == ResourceState::COPY_DST && newState == ResourceState::SHADER_READ) {
            srcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            dstStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
            srcAccess = VK_ACCESS_TRANSFER_WRITE_BIT;
            dstAccess = VK_ACCESS_SHADER_READ_BIT;
            srcLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            dstLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        } else if (oldState == ResourceState::COPY_SRC && newState == ResourceState::SHADER_READ) {
            srcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            dstStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
            srcAccess = VK_ACCESS_TRANSFER_READ_BIT;
            dstAccess = VK_ACCESS_SHADER_READ_BIT;
            srcLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
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
        } else if ((oldState == ResourceState::RENDER_TARGET_DEPTH_STENCIL || oldState == ResourceState::RENDER_TARGET_DEPTH) &&
                   newState == ResourceState::UNDEFINED) {
            srcStage = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
            dstStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
            srcAccess = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
            dstAccess = 0;
            srcLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            dstLayout = VK_IMAGE_LAYOUT_GENERAL;
            aspectFlag = oldState == ResourceState::RENDER_TARGET_DEPTH ?
                VK_IMAGE_ASPECT_DEPTH_BIT :
                static_cast<VkImageAspectFlagBits>(VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT);
        } else if ((oldState == ResourceState::RENDER_TARGET_DEPTH_STENCIL_READ || oldState == ResourceState::RENDER_TARGET_DEPTH_READ) &&
                    newState == ResourceState::UNDEFINED) {
            srcStage = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
            dstStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
            srcAccess = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
            dstAccess = 0;
            srcLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
            dstLayout = VK_IMAGE_LAYOUT_GENERAL;
            aspectFlag = oldState == ResourceState::RENDER_TARGET_DEPTH_READ ?
                VK_IMAGE_ASPECT_DEPTH_BIT :
                static_cast<VkImageAspectFlagBits>(VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT);
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
        } else if (oldState == ResourceState::COPY_SRC && newState == ResourceState::COPY_DST) {
            srcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            srcAccess = VK_ACCESS_TRANSFER_READ_BIT;
            dstAccess = VK_ACCESS_TRANSFER_WRITE_BIT;
            srcLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            dstLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        } else if (oldState == ResourceState::COPY_DST && newState == ResourceState::COPY_SRC) {
            srcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            srcAccess = VK_ACCESS_TRANSFER_WRITE_BIT;
            dstAccess = VK_ACCESS_TRANSFER_READ_BIT;
            srcLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            dstLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        } else if (oldState == ResourceState::SHADER_READ && newState == ResourceState::COPY_SRC) {
            srcStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
            dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            srcAccess = VK_ACCESS_SHADER_READ_BIT;
            dstAccess = VK_ACCESS_TRANSFER_READ_BIT;
            srcLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            dstLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        } else if (oldState == ResourceState::COPY_DST && newState == ResourceState::UNDEFINED) {
            srcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            srcAccess = VK_ACCESS_TRANSFER_WRITE_BIT;
            dstAccess = 0;
            srcLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            dstLayout = VK_IMAGE_LAYOUT_GENERAL;
        } else {
            throw Exception("Not implemented");
        }
    }

    void VKCommandList::convertState(
        const ResourceState oldState,
        const ResourceState newState,
        VkPipelineStageFlags& srcStage,
        VkPipelineStageFlags& dstStage,
        VkAccessFlags& srcAccess,
        VkAccessFlags& dstAccess) {
        if (oldState == ResourceState::INDIRECT_DRAW && newState == ResourceState::COMPUTE_WRITE) {
            srcStage = VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT;
            dstStage = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
            srcAccess = VK_ACCESS_INDIRECT_COMMAND_READ_BIT;
            dstAccess = VK_ACCESS_SHADER_WRITE_BIT;
        }else if (oldState == ResourceState::INDIRECT_DRAW && newState == ResourceState::COPY_DST) {
            srcStage = VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT;
            dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            srcAccess = VK_ACCESS_INDIRECT_COMMAND_READ_BIT;
            dstAccess = VK_ACCESS_TRANSFER_WRITE_BIT;
        } else if (oldState == ResourceState::COMPUTE_WRITE && newState == ResourceState::INDIRECT_DRAW){
            srcStage = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
            dstStage = VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT;
            srcAccess = VK_ACCESS_SHADER_WRITE_BIT;
            dstAccess = VK_ACCESS_INDIRECT_COMMAND_READ_BIT;
        } else if (oldState == ResourceState::COPY_DST && newState == ResourceState::COMPUTE_WRITE) {
            srcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            dstStage = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
            srcAccess = VK_ACCESS_TRANSFER_WRITE_BIT;
            dstAccess = VK_ACCESS_SHADER_WRITE_BIT;
        } else if (oldState == ResourceState::COPY_DST && newState == ResourceState::SHADER_READ) {
            srcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            dstStage = VK_PIPELINE_STAGE_VERTEX_SHADER_BIT;
            srcAccess = VK_ACCESS_TRANSFER_WRITE_BIT;
            dstAccess = VK_ACCESS_SHADER_READ_BIT;
        } else if (oldState == ResourceState::SHADER_READ && newState == ResourceState::COPY_DST) {
            srcStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
            dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            srcAccess = VK_ACCESS_SHADER_READ_BIT;
            dstAccess = VK_ACCESS_TRANSFER_WRITE_BIT;
        } else if (oldState == ResourceState::COPY_SRC && newState == ResourceState::COMPUTE_WRITE) {
            srcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            dstStage = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
            srcAccess = VK_ACCESS_TRANSFER_READ_BIT;
            dstAccess = VK_ACCESS_SHADER_WRITE_BIT;
        } else if (oldState == ResourceState::SHADER_READ && newState == ResourceState::COMPUTE_WRITE) {
            srcStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
            dstStage = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
            srcAccess = VK_ACCESS_TRANSFER_READ_BIT;
            dstAccess = VK_ACCESS_SHADER_WRITE_BIT;
        } else if (oldState == ResourceState::COMPUTE_WRITE && newState == ResourceState::COPY_SRC) {
            srcStage = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
            dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            srcAccess = VK_ACCESS_SHADER_WRITE_BIT;
            dstAccess = VK_ACCESS_TRANSFER_READ_BIT;
        } else if (oldState == ResourceState::COMPUTE_WRITE && newState == ResourceState::SHADER_READ) {
            srcStage = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
            dstStage = VK_PIPELINE_STAGE_VERTEX_SHADER_BIT;
            srcAccess = VK_ACCESS_SHADER_WRITE_BIT;
            dstAccess = VK_ACCESS_SHADER_READ_BIT;
        } else if (oldState == ResourceState::COPY_DST && newState == ResourceState::SHADER_READ) {
            srcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            dstStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            srcAccess = VK_ACCESS_TRANSFER_WRITE_BIT;
            dstAccess = VK_ACCESS_SHADER_READ_BIT;
        } else {
            throw Exception("Not implemented");
        }
    }

    void VKCommandList::barrier(
        const Buffer& buffer,
        const ResourceState oldState,
        const ResourceState newState) const {
        VkPipelineStageFlags srcStage, dstStage;
        VkAccessFlags srcAccess, dstAccess;
        convertState(oldState, newState, srcStage, dstStage, srcAccess, dstAccess);
        const auto bufferBarrier = VkBufferMemoryBarrier {
            .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
            .srcAccessMask = srcAccess,
            .dstAccessMask = dstAccess,
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .buffer = static_cast<const VKBuffer&>(buffer).getBuffer(),
            .offset = 0,
            .size = VK_WHOLE_SIZE,
        };
        vkCmdPipelineBarrier(
            commandBuffer,
            srcStage,
            dstStage,
            0,
            0, nullptr,
            1, &bufferBarrier,
            0, nullptr
        );
    }

    void VKCommandList::barrier(
        const std::shared_ptr<const Image>& image,
        const ResourceState oldState,
        const ResourceState newState,
        const uint32_t firstMipLevel,
        const uint32_t levelCount) const {
        assert(image != nullptr);
        barrier(static_pointer_cast<const VKImage>(image)->getImage(), oldState, newState, firstMipLevel, levelCount);
    }

    void VKCommandList::barrier(
        const std::shared_ptr<const RenderTarget>& renderTarget,
        const ResourceState oldState,
        const ResourceState newState) const {
        assert(renderTarget != nullptr);
        barrier(static_pointer_cast<const VKImage>(renderTarget->getImage())->getImage(), oldState, newState);
    }

    void VKCommandList::barrier(
        const std::shared_ptr<const SwapChain>& swapChain,
        const ResourceState oldState,
        const ResourceState newState) const {
        assert(swapChain != nullptr);
        barrier(static_pointer_cast<const VKSwapChain>(swapChain)->getCurrentImage(), oldState, newState);
    }

    void VKCommandList::barrier(
        const std::vector<std::shared_ptr<const RenderTarget>>& renderTargets,
        const ResourceState oldState,
        const ResourceState newState) const {
        assert(!renderTargets.empty());
        const auto r = std::views::transform(renderTargets, [](const std::shared_ptr<const RenderTarget>& renderTarget) {
            return static_pointer_cast<const VKImage>(renderTarget->getImage())->getImage();
        });
        barrier(std::vector<VkImage>{r.begin(), r.end()}, oldState, newState);
    }

    void VKCommandList::pushConstants(
        const std::shared_ptr<const PipelineResources>& pipelineResources,
        const PushConstantsDesc& pushConstants,
        const void* data) const {
        assert(pipelineResources != nullptr);
        assert(data != nullptr);
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

    void VKCommandList::upload(const Buffer& destination, const void* source) {
        assert(source != nullptr);
        const auto& buffer = static_cast<const VKBuffer&>(destination);
        const auto stagingBuffer = std::make_shared<VKBuffer>(
            device,
            BufferType::BUFFER_UPLOAD,
            buffer.getInstanceSize(),
            buffer.getInstanceCount(),
            L"StagingBuffer for buffer");
        stagingBuffer->map();
        if ((buffer.getInstanceSizeAligned() == 1) || (buffer.getInstanceCount() == 1)) {
            stagingBuffer->write(source);
        } else {
            for (int i = 0; i < buffer.getInstanceCount(); i++) {
                stagingBuffer->write(
                    static_cast<const unsigned char*>(source) + i * buffer.getInstanceSize(),
                    buffer.getInstanceSize(),
                    buffer.getInstanceSizeAligned() * i);
            }
        }
        stagingBuffer->unmap();

        const auto copyRegion = VkBufferCopy{
            .srcOffset = 0,
            .dstOffset = 0,
            .size = buffer.getSize(),
        };
        vkCmdCopyBuffer(
            commandBuffer,
            stagingBuffer->getBuffer(),
            buffer.getBuffer(),
            1,
            &copyRegion);

        stagingBuffers.push_back(stagingBuffer);
    }

    void VKCommandList::copy(
        const Buffer& source,
        const Buffer& destination,
        const size_t size,
        const uint32_t sourceOffset,
        const uint32_t destinationOffset) const {
        const auto copySize = size == Buffer::WHOLE_SIZE ? min(source.getSize(), destination.getSize()) : size;
        assert(source.getSize() >= (copySize + sourceOffset));
        assert(destination.getSize() >= (copySize + destinationOffset));
        const auto copyRegion = VkBufferCopy{
            .srcOffset = sourceOffset,
            .dstOffset = destinationOffset,
            .size = copySize,
        };
        vkCmdCopyBuffer(
            commandBuffer,
            static_cast<const VKBuffer&>(source).getBuffer(),
            static_cast<const VKBuffer&>(destination).getBuffer(),
            1,
            &copyRegion);
    }

    void VKCommandList::copy(
        const Buffer& source,
        const Buffer& destination,
        const std::vector<BufferCopyRegion>& regions) const {
        std::vector<VkBufferCopy> copyRegions(regions.size());
        for (int i = 0; i < regions.size(); i++) {
            copyRegions[i].srcOffset = regions[i].srcOffset;
            copyRegions[i].dstOffset = regions[i].dstOffset;
            copyRegions[i].size = regions[i].size;
        }
        vkCmdCopyBuffer(
            commandBuffer,
            static_cast<const VKBuffer&>(source).getBuffer(),
            static_cast<const VKBuffer&>(destination).getBuffer(),
            copyRegions.size(),
            copyRegions.data());
    }

    void VKCommandList::upload(
        const Image& destination,
        const void* source,
        const uint32_t firstMipLevel) {
        assert(source != nullptr);
        assert(firstMipLevel < destination.getMipLevels());
        const auto& image = static_cast<const VKImage&>(destination);
        const auto stagingBuffer = std::make_shared<VKBuffer>(
           device,
           BufferType::IMAGE_UPLOAD,
           image.getImageSize(),
           image.getArraySize(),
           L"StagingBuffer for image");
        stagingBuffer->map();
        if (image.getArraySize() == 1) {
            stagingBuffer->write(source);
        } else {
            for (int i = 0; i < image.getArraySize(); i++) {
                stagingBuffer->write(
                    static_cast<const unsigned char*>(source) + i * image.getImageSize(),
                    image.getImageSize(),
                    stagingBuffer->getInstanceSizeAligned() * i);
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
                .mipLevel = firstMipLevel,
                .baseArrayLayer = 0,
                .layerCount = destination.getArraySize(),
            },
            .imageOffset = {0, 0, 0},
            .imageExtent = {image.getWidth(), image.getHeight(), 1},
        };

        vkCmdCopyBufferToImage(
                commandBuffer,
                stagingBuffer->getBuffer(),
                image.getImage(),
                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                1,
                &region);

        stagingBuffers.push_back(stagingBuffer);
    }

    void VKCommandList::copy(
        const Buffer& source,
        const Image& destination,
        const uint32_t sourceOffset,
        const uint32_t mipLevel,
        const bool rowPitchAlignment) const {
        assert(mipLevel < destination.getMipLevels());
        const auto& image = static_cast<const VKImage&>(destination);
        const auto& buffer = static_cast<const VKBuffer&>(source);
        const auto region = VkBufferImageCopy {
            .bufferOffset = sourceOffset,
            .bufferRowLength = rowPitchAlignment ? image.getAlignedRowLength(mipLevel) : 0,
            .bufferImageHeight = 0,
            .imageSubresource = {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .mipLevel = mipLevel,
                .baseArrayLayer = 0,
                .layerCount = destination.getArraySize(),
            },
            .imageOffset = {0, 0, 0},
            .imageExtent = {image.getWidth() >> mipLevel, image.getHeight() >> mipLevel, 1},
        };
        vkCmdCopyBufferToImage(
                commandBuffer,
                buffer.getBuffer(),
                image.getImage(),
                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                1,
                &region);
    }

    void VKCommandList::copy(
        const Buffer& source,
        const Image& destination,
        const std::vector<size_t>& sourceOffsets,
        const bool rowPitchAlignment) const {
        const auto& buffer = static_cast<const VKBuffer&>(source);
        const auto& image = static_cast<const VKImage&>(destination);
        auto copyRegions = std::vector<VkBufferImageCopy>{};
        copyRegions.reserve(destination.getMipLevels());
        for (uint32_t mip_level = 0; mip_level < image.getMipLevels(); mip_level++) {
            const auto buffer_copy_region = VkBufferImageCopy{
                .bufferOffset       = sourceOffsets[mip_level],
                .bufferRowLength    = rowPitchAlignment ? image.getAlignedRowLength(mip_level) : 0,
                .imageSubresource {
                    .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
                    .mipLevel       = mip_level,
                    .layerCount     = 1,
                },
                .imageExtent {
                    .width          = image.getWidth() >> mip_level,
                    .height         = image.getHeight() >> mip_level,
                    .depth          = 1,
                },
            };
            copyRegions.emplace_back(buffer_copy_region);
        }
        vkCmdCopyBufferToImage(
                       commandBuffer,
                       buffer.getBuffer(),
                       image.getImage(),
                       VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                       copyRegions.size(),
                       copyRegions.data());
    }

    void VKCommandList::copy(
        const Image& source,
        const Buffer& destination,
        const uint32_t destinationOffset,
        const uint32_t firstMipLevel) const {
        assert(firstMipLevel < source.getMipLevels());
        const auto& image = static_cast<const VKImage&>(source);
        const auto& buffer = static_cast<const VKBuffer&>(destination);
        const auto region = VkBufferImageCopy {
            .bufferOffset = destinationOffset,
            .bufferRowLength = 0,
            .bufferImageHeight = 0,
            .imageSubresource = {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .mipLevel = firstMipLevel,
                .baseArrayLayer = 0,
                .layerCount = source.getArraySize(),
            },
            .imageOffset = {0, 0, 0},
            .imageExtent = {image.getWidth() >> firstMipLevel, image.getHeight() >> firstMipLevel, 1},
        };
        vkCmdCopyImageToBuffer(
                commandBuffer,
                image.getImage(),
                VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                buffer.getBuffer(),
                1,
                &region);
    }

    void VKCommandList::uploadArray(
        const Image& destination,
        const std::vector<void*>& sources,
        const uint32_t firstMipLevel) {
        assert(sources.size() == destination.getArraySize());
        assert(firstMipLevel < destination.getMipLevels());
        const auto& image = static_cast<const VKImage&>(destination);
        const auto stagingBuffer = std::make_shared<VKBuffer>(
           device,
           BufferType::IMAGE_UPLOAD,
           image.getImageSize(),
           image.getArraySize(),
           L"StagingBuffer for image array");
        stagingBuffer->map();
        for (int i = 0; i < image.getArraySize(); i++) {
            stagingBuffer->write(
                sources[i],
                image.getImageSize(),
                stagingBuffer->getInstanceSizeAligned() * i);
        }
        stagingBuffer->unmap();

        const auto region = VkBufferImageCopy {
            .bufferOffset = 0,
            .bufferRowLength = 0,
            .bufferImageHeight = 0,
            .imageSubresource = {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .mipLevel = firstMipLevel,
                .baseArrayLayer = 0,
                .layerCount = destination.getArraySize(),
            },
            .imageOffset = {0, 0, 0},
            .imageExtent = {image.getWidth() >> firstMipLevel, image.getHeight() >> firstMipLevel, 1},
        };
        vkCmdCopyBufferToImage(
                commandBuffer,
                stagingBuffer->getBuffer(),
                image.getImage(),
                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                1,
                &region);
        stagingBuffers.push_back(stagingBuffer);
    }

    void VKCommandList::copy(
        const Image& source,
        const SwapChain& swapChain) const {
        const auto& vkSource = static_cast<const VKImage&>(source);
        const auto& vkSwapChain = static_cast<const VKSwapChain&>(swapChain);
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
        copyRegion.extent = {source.getWidth(), source.getHeight(), 1};

        vkCmdCopyImage(commandBuffer,
                       vkSource.getImage(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                       vkSwapChain.getCurrentImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                       1, &copyRegion);
    }

    VKFence::VKFence(const bool createSignaled, const std::shared_ptr<const VKDevice>& device, const std::wstring& name):
        device{device->getDevice()} {
        const auto fenceInfo = VkFenceCreateInfo {
            .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
            .flags = createSignaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0u,
        };
        vkCheck(vkCreateFence(device->getDevice(), &fenceInfo, nullptr, &fence));
#ifdef _DEBUG
        vkSetObjectName(device->getDevice(), reinterpret_cast<uint64_t>(fence), VK_OBJECT_TYPE_FENCE,
    "VKFence : " + to_string(name));
#endif
    }

    void VKFence::wait() const {
        vkWaitForFences(device, 1, &fence, VK_TRUE, UINT64_MAX);
    }

    void VKFence::reset() {
        vkResetFences(device, 1, &fence);
    }

    VKFence::~VKFence() {
        vkDestroyFence(device, fence, nullptr);
    }

     VKSemaphore::VKSemaphore(const std::shared_ptr<const VKDevice>& device,
                             const SemaphoreType type,
                             const std::wstring& name):
        Semaphore{type},
        device{device->getDevice()} {
        const auto createInfo = VkSemaphoreTypeCreateInfo {
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO,
            .semaphoreType = type == SemaphoreType::TIMELINE ? VK_SEMAPHORE_TYPE_TIMELINE : VK_SEMAPHORE_TYPE_BINARY,
            .initialValue = value,
        };
        const auto semaphoreInfo = VkSemaphoreCreateInfo {
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
            .pNext = &createInfo
        };
        vkCreateSemaphore(this->device, &semaphoreInfo, nullptr, &semaphore);
#ifdef _DEBUG
        vkSetObjectName(device->getDevice(), reinterpret_cast<uint64_t>(semaphore), VK_OBJECT_TYPE_SEMAPHORE,
    "VKSemaphore : " + to_string(name));
#endif
    }

    VKSemaphore::~VKSemaphore() {
        vkDestroySemaphore(device, semaphore, nullptr);
    }

}