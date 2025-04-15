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
        graphicCommandQueue = make_shared<VKSubmitQueue>(getVKDevice(), CommandType::GRAPHIC, "Graphic");
        transferCommandQueue = make_shared<VKSubmitQueue>(getVKDevice(), CommandType::TRANSFER, "Transfer");
        swapChain = make_shared<VKSwapChain>(getVKDevice(),
#ifdef _WIN32
            hWnd,
#endif
            configuration.vSyncMode
        );
    }

    void VKRenderingBackEnd::destroyFrameData(const shared_ptr<FrameData>& frameData) {
        const auto data = static_pointer_cast<VKFrameData>(frameData);
        vkDestroySemaphore(getVKDevice()->getDevice(), data->imageAvailableSemaphore, nullptr);
        vkDestroySemaphore(getVKDevice()->getDevice(), data->renderFinishedSemaphore, nullptr);
        vkDestroyFence(getVKDevice()->getDevice(), data->inFlightFence, nullptr);
    }

    shared_ptr<FrameData> VKRenderingBackEnd::createFrameData(const uint32_t frameIndex) {
        auto data = make_shared<VKFrameData>();
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

    shared_ptr<CommandAllocator> VKRenderingBackEnd::createCommandAllocator(CommandType type) const {
        return make_shared<VKCommandAllocator>(getVKDevice(), type);
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
        const shared_ptr<PipelineResources>& pipelineResources,
        const shared_ptr<const VertexInputLayout>& vertexInputLayout,
        const shared_ptr<const ShaderModule>& vertexShader,
        const shared_ptr<const ShaderModule>& fragmentShader,
        const Pipeline::Configuration& configuration,
        const wstring& name) const {
        return make_shared<VKPipeline>(
            getVKDevice()->getDevice(),
            *getVKSwapChain(),
            pipelineResources,
            vertexInputLayout,
            vertexShader,
            fragmentShader,
            configuration,
            name
        );
    }

    shared_ptr<Buffer> VKRenderingBackEnd::createBuffer(
        const BufferType type,
        const size_t size,
        const size_t count,
        const size_t alignment,
        const wstring& name) const  {
        return make_shared<VKBuffer>(
           getVKDevice(), type,
           size, count, alignment,
           name);
    }

    shared_ptr<Image> VKRenderingBackEnd::createImage(
            ImageFormat format,
            uint32_t width,
            uint32_t height,
            const wstring& name) const {
        return make_shared<VKImage>(
            getVKDevice(),
            format,
            width,
            height,
            name);
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
            const shared_ptr<const DescriptorLayout>& layout,
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
            getVKDevice(),
            minFilter, magFilter, addressModeU, addressModeV, addressModeW,
            minLod, maxLod, anisotropyEnable, mipMapMode);
    }

}