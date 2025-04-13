/*
* Copyright (c) 2025-present Henri Michelon
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/
module;
#include "vireo/backend/vulkan/Tools.h"
module vireo.vulkan;

import vireo.vulkan.descriptors;
import vireo.vulkan.framedata;
import vireo.vulkan.pipelines;

namespace vireo {

    VKRenderingBackEnd::VKRenderingBackEnd(const Configuration& configuration) :
    RenderingBackEnd{configuration},
#ifdef _WIN32
    hWnd{static_cast<HWND>(configuration.windowHandle)}
#endif
    {
        instance = make_shared<VKInstance>();
        physicalDevice = make_shared<VKPhysicalDevice>(getVKInstance()->getInstance(), hWnd);
        device = make_shared<VKDevice>(*getVKPhysicalDevice(), getVKInstance()->getRequestedLayers());
        graphicCommandQueue = make_shared<VKSubmitQueue>(CommandList::GRAPHIC, *getVKDevice(), "Graphic");
        transferCommandQueue = make_shared<VKSubmitQueue>(CommandList::TRANSFER, *getVKDevice(), "Transfer");
        swapChain = make_shared<VKSwapChain>(*getVKPhysicalDevice(), *getVKDevice(),
#ifdef _WIN32
            hWnd,
#endif
            configuration.vSyncMode
        );
    }

    void VKRenderingBackEnd::destroyFrameData(shared_ptr<FrameData>& frameData) {
        const auto data = static_pointer_cast<VKFrameData>(frameData);
        vkDestroySemaphore(getVKDevice()->getDevice(), data->imageAvailableSemaphore, nullptr);
        vkDestroySemaphore(getVKDevice()->getDevice(), data->renderFinishedSemaphore, nullptr);
        vkDestroyFence(getVKDevice()->getDevice(), data->inFlightFence, nullptr);
    }

    shared_ptr<FrameData> VKRenderingBackEnd::createFrameData(
        const uint32_t frameIndex,
        const vector<shared_ptr<DescriptorSet>>& descriptorSet) {
        auto data = make_shared<VKFrameData>(descriptorSet);
        constexpr VkSemaphoreCreateInfo semaphoreInfo{
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO
        };
        constexpr VkFenceCreateInfo fenceInfo{
            .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
            .flags = VK_FENCE_CREATE_SIGNALED_BIT
        };
        if (vkCreateSemaphore(getVKDevice()->getDevice(), &semaphoreInfo, nullptr, &data->imageAvailableSemaphore) != VK_SUCCESS
            || vkCreateSemaphore(getVKDevice()->getDevice(), &semaphoreInfo, nullptr, &data->renderFinishedSemaphore) != VK_SUCCESS
            || vkCreateFence(getVKDevice()->getDevice(), &fenceInfo, nullptr, &data->inFlightFence) != VK_SUCCESS) {
            die("failed to create semaphores!");
        }
        data->imageAvailableSemaphoreSubmitInfo = {
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
            .semaphore = data->imageAvailableSemaphore,
            .value = 1,
            .stageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR,
            .deviceIndex = 0
        };
        data->renderFinishedSemaphoreSubmitInfo = {
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
            .semaphore = data->renderFinishedSemaphore,
            .value = 1,
            .stageMask = VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT,
            .deviceIndex = 0
        };
#ifdef _DEBUG
        vkSetObjectName(getVKDevice()->getDevice(), reinterpret_cast<uint64_t>(data->imageAvailableSemaphore), VK_OBJECT_TYPE_SEMAPHORE,
            "VKFrameData image Semaphore : " + to_string(frameIndex));
        vkSetObjectName(getVKDevice()->getDevice(), reinterpret_cast<uint64_t>(data->renderFinishedSemaphore), VK_OBJECT_TYPE_SEMAPHORE,
    "VKFrameData render Semaphore : " + to_string(frameIndex));
        vkSetObjectName(getVKDevice()->getDevice(), reinterpret_cast<uint64_t>(data->inFlightFence), VK_OBJECT_TYPE_FENCE,
    "VKFrameData Fence: " + to_string(frameIndex));
#endif
        return data;
    }

    shared_ptr<VertexInputLayout> VKRenderingBackEnd::createVertexLayout(
           size_t size,
           const vector<VertexInputLayout::AttributeDescription>& attributesDescriptions) const {
        return make_shared<VKVertexInputLayout>(size, attributesDescriptions);
    }

    shared_ptr<CommandAllocator> VKRenderingBackEnd::createCommandAllocator(CommandList::Type type) const {
        return make_shared<VKCommandAllocator>(type, *getVKDevice());
    }

    shared_ptr<ShaderModule> VKRenderingBackEnd::createShaderModule(const string& fileName) const {
        return make_shared<VKShaderModule>(getVKDevice()->getDevice(), fileName);
    }

    shared_ptr<PipelineResources> VKRenderingBackEnd::createPipelineResources(
        const vector<shared_ptr<DescriptorLayout>>& descriptorLayouts,
        const wstring& name) const {
        return make_shared<VKPipelineResources>(getVKDevice()->getDevice(), descriptorLayouts, name);
    }

    shared_ptr<Pipeline> VKRenderingBackEnd::createPipeline(
        PipelineResources& pipelineResources,
        VertexInputLayout& vertexInputLayout,
        ShaderModule& vertexShader,
        ShaderModule& fragmentShader,
        const wstring& name) const {
        return make_shared<VKPipeline>(
            getVKDevice()->getDevice(),
            *getVKSwapChain(),
            pipelineResources,
            vertexInputLayout,
            vertexShader,
            fragmentShader,
            name
        );
    }

    shared_ptr<Buffer> VKRenderingBackEnd::createBuffer(
        const Buffer::Type type,
        const size_t size,
        const size_t count,
        const size_t alignment,
        const wstring& name) const  {
        return make_shared<VKBuffer>(
           *getVKDevice(), type,
           size, count, alignment,
           name);
    }

    shared_ptr<Image> VKRenderingBackEnd::createImage(
            ImageFormat format,
            uint32_t width,
            uint32_t height,
            const wstring& name) const {
        return make_shared<VKImage>(
            *getVKDevice(),
            format,
            width,
            height,
            name);
    }

    void VKRenderingBackEnd::beginRendering(
        shared_ptr<FrameData>& frameData,
        shared_ptr<PipelineResources>& pipelineResources,
        shared_ptr<Pipeline>& pipeline,
        shared_ptr<CommandList>& commandList) {
        const auto data = static_pointer_cast<VKFrameData>(frameData);
        const auto vkCommandList = static_pointer_cast<VKCommandList>(commandList);
        const auto vkPipelineResources = static_pointer_cast<VKPipelineResources>(pipelineResources);
        const auto& swapChain = *getVKSwapChain();

        vkCommandList->pipelineBarrier(
            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            {VKCommandList::imageMemoryBarrier(swapChain.getImages()[data->imageIndex],
                    0, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                    VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)});

        const auto vkClearColor = VkClearValue{{
            clearColor[0], clearColor[1], clearColor[2], clearColor[3]
        }};
        const VkExtent2D extent = {swapChain.getExtent().width, swapChain.getExtent().height};

        const auto colorAttachmentInfo = VkRenderingAttachmentInfo {
            .sType              = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR,
            .imageView          = swapChain.getImageViews()[data->imageIndex],
            .imageLayout        = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            .resolveMode        = VK_RESOLVE_MODE_NONE,
            .loadOp             = VK_ATTACHMENT_LOAD_OP_CLEAR,
            .storeOp            = VK_ATTACHMENT_STORE_OP_STORE,
            .clearValue         = vkClearColor,
        };
        const auto renderingInfo = VkRenderingInfo {
            .sType               = VK_STRUCTURE_TYPE_RENDERING_INFO_KHR,
            .pNext                = nullptr,
            .renderArea           = {
                {0, 0},
                extent
            },
            .layerCount           = 1,
            .colorAttachmentCount = 1,
            .pColorAttachments    = &colorAttachmentInfo,
            .pDepthAttachment     = nullptr,
            .pStencilAttachment   = nullptr
        };
        const auto& commandBuffer = vkCommandList->getCommandBuffer();
        vkCmdBeginRendering(commandBuffer, &renderingInfo);

        const VkViewport viewport{
            .x = 0.0f,
            .y = static_cast<float>(extent.height),
            .width = static_cast<float>(extent.width),
            .height = -static_cast<float>(extent.height),
            .minDepth = 0.0f,
            .maxDepth = 1.0f
        };
        const VkRect2D scissor{
            .offset = {0, 0},
            .extent = extent
        };
        vkCmdSetViewportWithCount(commandBuffer, 1, &viewport);
        vkCmdSetScissorWithCount(commandBuffer, 1, &scissor);
        vkCmdSetCullMode(commandBuffer, VK_CULL_MODE_NONE);

        if (!data->descriptorSets.empty()) {
            vector<VkDescriptorSet> descriptorSets(data->descriptorSets.size());
            for (int i = 0; i < data->descriptorSets.size(); i++) {
                descriptorSets[i] = static_pointer_cast<VKDescriptorSet>(data->descriptorSets[i])->getSet();
            }
            vkCmdBindDescriptorSets(commandBuffer,
                                    VK_PIPELINE_BIND_POINT_GRAPHICS,
                                    vkPipelineResources->getPipelineLayout(),
                                    0,
                                    descriptorSets.size(),
                                    descriptorSets.data(),
                                    0,
                                    nullptr);
        }
    }

    void VKRenderingBackEnd::endRendering(shared_ptr<CommandList>& commandList) {
        const auto vkCommandList = static_pointer_cast<VKCommandList>(commandList);
        vkCmdEndRendering(vkCommandList->getCommandBuffer());
    }

    void VKRenderingBackEnd::waitIdle() {
        vkDeviceWaitIdle(getVKDevice()->getDevice());
    }

    shared_ptr<DescriptorLayout> VKRenderingBackEnd::createDescriptorLayout(
        const wstring& name) {
        return make_shared<VKDescriptorLayout>(getVKDevice()->getDevice(), name);
    }

    shared_ptr<DescriptorLayout> VKRenderingBackEnd::createSamplerDescriptorLayout(
        const wstring& name) {
        return make_shared<VKDescriptorLayout>(getVKDevice()->getDevice(), name);
    }

    shared_ptr<DescriptorSet> VKRenderingBackEnd::createDescriptorSet(
            shared_ptr<DescriptorLayout>& layout,
            const wstring& name) {
        return make_shared<VKDescriptorSet>(layout, name);
    }

    shared_ptr<Sampler> VKRenderingBackEnd::createSampler(
           Filter minFilter,
           Filter magFilter,
           AddressMode addressModeU,
           AddressMode addressModeV,
           AddressMode addressModeW,
           float minLod,
           float maxLod,
           bool anisotropyEnable,
           MipMapMode mipMapMode) const {
        return make_shared<VKSampler>(
            *getVKPhysicalDevice(), getVKDevice()->getDevice(),
            minFilter, magFilter, addressModeU, addressModeV, addressModeW,
            minLod, maxLod, anisotropyEnable, mipMapMode);
    }

}