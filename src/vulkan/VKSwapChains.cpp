/*
* Copyright (c) 2025-present Henri Michelon
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/
module;
#include "vireo/backend/vulkan/Libraries.h"
module vireo.vulkan.swapchains;

import vireo.tools;

import vireo.vulkan.commands;
import vireo.vulkan.resources;
import vireo.vulkan.tools;

namespace vireo {

    VKSwapChain::VKSwapChain(
        const shared_ptr<const VKDevice>& device,
        const VkQueue presentQueue,
        void* windowHandle,
        const ImageFormat format,
        const PresentMode vSyncMode,
        const uint32_t framesInFlight):
        SwapChain{format, vSyncMode, framesInFlight},
        device{device},
        physicalDevice{device->getPhysicalDevice()},
#ifdef _WIN32
        hWnd{static_cast<HWND>(windowHandle)},
#endif
        presentQueue{presentQueue}
    {
        imageIndex.resize(framesInFlight);
        imageAvailableSemaphore.resize(framesInFlight);
        renderFinishedSemaphore.resize(framesInFlight);
        imageAvailableSemaphoreInfo.resize(framesInFlight);
        renderFinishedSemaphoreInfo.resize(framesInFlight);

        constexpr VkSemaphoreCreateInfo semaphoreInfo{
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO
        };
        for (int i = 0; i < framesInFlight; i++) {
            if (vkCreateSemaphore(device->getDevice(), &semaphoreInfo, nullptr, &imageAvailableSemaphore[i]) != VK_SUCCESS
                || vkCreateSemaphore(device->getDevice(), &semaphoreInfo, nullptr, &renderFinishedSemaphore[i]) != VK_SUCCESS) {
                throw Exception("failed to create semaphores!");
            }
            renderFinishedSemaphoreInfo[i] = VkSemaphoreSubmitInfo {
                .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
                .semaphore = renderFinishedSemaphore[i],
                .stageMask = VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT,
                .deviceIndex = 0
            };
            imageAvailableSemaphoreInfo[i] = VkSemaphoreSubmitInfo {
                .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
                .semaphore = imageAvailableSemaphore[i],
                .value = 1,
                .stageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR,
                .deviceIndex = 0
            };
#ifdef _DEBUG
            vkSetObjectName(device->getDevice(), reinterpret_cast<uint64_t>(imageAvailableSemaphore[i]), VK_OBJECT_TYPE_SEMAPHORE,
                "VKFrameData image Semaphore : " + to_string(i));
            vkSetObjectName(device->getDevice(), reinterpret_cast<uint64_t>(renderFinishedSemaphore[i]), VK_OBJECT_TYPE_SEMAPHORE,
        "VKFrameData render Semaphore : " + to_string(i));
#endif
        }
        create();
    }

    void VKSwapChain::create() {
        // https://vulkan-tutorial.com/Drawing_a_triangle/Presentation/Swap_chain
        const SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice.getPhysicalDevice(), physicalDevice.getSurface());
        const VkSurfaceFormatKHR surfaceFormat= chooseSwapSurfaceFormat(swapChainSupport.formats);
        const VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
        swapChainExtent = chooseSwapExtent(swapChainSupport.capabilities);

        if (swapChainSupport.capabilities.maxImageCount > 0 &&
            framesInFlight > swapChainSupport.capabilities.maxImageCount) {
            framesInFlight = swapChainSupport.capabilities.maxImageCount;
        }

        {
            auto createInfo = VkSwapchainCreateInfoKHR {
                .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
                .surface = physicalDevice.getSurface(),
                .minImageCount = framesInFlight,
                .imageFormat = surfaceFormat.format,
                .imageColorSpace = surfaceFormat.colorSpace,
                .imageExtent = swapChainExtent,
                .imageArrayLayers = 1,
                .imageUsage =
                    VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
                    VK_IMAGE_USAGE_TRANSFER_DST_BIT ,
                .preTransform = swapChainSupport.capabilities.currentTransform,
                .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
                .presentMode = presentMode,
                .clipped = VK_TRUE,
            };
            if (device->getPresentQueueFamilyIndex() != device->getGraphicsQueueFamilyIndex()) {
                const uint32_t queueFamilyIndices[] = {device->getPresentQueueFamilyIndex(), device->getGraphicsQueueFamilyIndex()};
                createInfo.imageSharingMode      = VK_SHARING_MODE_CONCURRENT;
                createInfo.queueFamilyIndexCount = 2;
                createInfo.pQueueFamilyIndices   = queueFamilyIndices;
            } else {
                createInfo.imageSharingMode      = VK_SHARING_MODE_EXCLUSIVE;
                createInfo.queueFamilyIndexCount = 0;
                createInfo.pQueueFamilyIndices   = nullptr;
            }
            // Need VK_KHR_SWAPCHAIN extension, or it will crash (no validation error)
            vkCheck(vkCreateSwapchainKHR(device->getDevice(), &createInfo, nullptr, &swapChain));
#ifdef _DEBUG
            vkSetObjectName(device->getDevice(), reinterpret_cast<uint64_t>(swapChain), VK_OBJECT_TYPE_SWAPCHAIN_KHR,
                "VKSwapChain");
#endif
        }
        vkGetSwapchainImagesKHR(device->getDevice(), swapChain, &framesInFlight, nullptr);
        swapChainImages.resize(framesInFlight);
        swapChainImageViews.resize(swapChainImages.size());
        swapChainImageFormat = surfaceFormat.format;

        vkGetSwapchainImagesKHR(device->getDevice(), swapChain, &framesInFlight, swapChainImages.data());
        extent      = Extent{ swapChainExtent.width, swapChainExtent.height };
        aspectRatio = static_cast<float>(swapChainExtent.width) / static_cast<float>(swapChainExtent.height);

        for (uint32_t i = 0; i < swapChainImages.size(); i++) {
            swapChainImageViews[i] = device->createImageView(swapChainImages[i],
                                                     swapChainImageFormat,
                                                     VK_IMAGE_ASPECT_COLOR_BIT,
                                                     1);
#ifdef _DEBUG
            vkSetObjectName(device->getDevice(), reinterpret_cast<uint64_t>(swapChainImageViews[i]), VK_OBJECT_TYPE_IMAGE_VIEW,
                "VKSwapChain Image View " + std::to_string(i));
            vkSetObjectName(device->getDevice(), reinterpret_cast<uint64_t>(swapChainImages[i]), VK_OBJECT_TYPE_IMAGE,
                "VKSwapChain Image " + std::to_string(i));
#endif
        }
    }

    void VKSwapChain::recreate() {
        const SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice.getPhysicalDevice(), physicalDevice.getSurface());
        const auto newExtent = chooseSwapExtent(swapChainSupport.capabilities);
        if (newExtent.width != swapChainExtent.width || newExtent.height != swapChainExtent.height) {
            vkDeviceWaitIdle(device->getDevice());
            cleanup();
            create();
        }
    }

    void VKSwapChain::cleanup() const {
        // https://vulkan-tutorial.com/Drawing_a_triangle/Swap_chain_recreation#page_Recreating-the-swap-chain
        for (const auto &swapChainImageView : swapChainImageViews) {
            vkDestroyImageView(device->getDevice(), swapChainImageView, nullptr);
        }
        vkDestroySwapchainKHR(device->getDevice(), swapChain, nullptr);
    }

    VKSwapChain::SwapChainSupportDetails VKSwapChain::querySwapChainSupport(
        const VkPhysicalDevice vkPhysicalDevice,
        const VkSurfaceKHR surface) {
        // https://vulkan-tutorial.com/Drawing_a_triangle/Presentation/Swap_chain#page_Querying-details-of-swap-chain-support
        SwapChainSupportDetails details;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vkPhysicalDevice, surface, &details.capabilities);
        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(vkPhysicalDevice, surface, &formatCount, nullptr);
        if (formatCount != 0) {
            details.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(vkPhysicalDevice, surface, &formatCount, details.formats.data());
        }
        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(vkPhysicalDevice, surface, &presentModeCount, nullptr);
        if (presentModeCount != 0) {
            details.presentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(vkPhysicalDevice,
                                                      surface,
                                                      &presentModeCount,
                                                      details.presentModes.data());
        }
        return details;
    }

    VkSurfaceFormatKHR VKSwapChain::chooseSwapSurfaceFormat(const vector<VkSurfaceFormatKHR> &availableFormats) {
        // https://vulkan-tutorial.com/Drawing_a_triangle/Presentation/Swap_chain#page_Choosing-the-right-settings-for-the-swap-chain
        for (const auto &availableFormat : availableFormats) {
            if (availableFormat.format == VKImage::vkFormats[static_cast<int>(format)]) { return availableFormat; }
        }
        return availableFormats[0];
    }

    VkPresentModeKHR VKSwapChain::chooseSwapPresentMode(const vector<VkPresentModeKHR> &availablePresentModes) const {
        // https://vulkan-tutorial.com/Drawing_a_triangle/Presentation/Swap_chain#page_Presentation-mode
        for (const auto &availablePresentMode : availablePresentModes) {
            if (availablePresentMode == vkPresentModes[static_cast<int>(presentMode)]) {
                return availablePresentMode;
            }
        }
        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VkExtent2D VKSwapChain::chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities) const {
        // https://vulkan-tutorial.com/Drawing_a_triangle/Presentation/Swap_chain#page_Swap-extent
#ifdef _WIN32
        RECT windowRect{};
        if (GetClientRect(hWnd, &windowRect) == 0) {
            throw Exception("Error getting window rect");
        }
        const VkExtent2D actualExtent{
            .width = static_cast<uint32_t>(windowRect.right - windowRect.left),
            .height = static_cast<uint32_t>(windowRect.bottom - windowRect.top)
        };
#endif
        // actualExtent.width = glm::max(
        //         capabilities.minImageExtent.width,
        //         glm::min(capabilities.maxImageExtent.width, actualExtent.width));
        // actualExtent.height = glm::max(
        //         capabilities.minImageExtent.height,
        //         glm::min(capabilities.maxImageExtent.height, actualExtent.height));
        return actualExtent;
    }

    void VKSwapChain::nextSwapChain() {
        currentFrameIndex = (currentFrameIndex + 1) % framesInFlight;
    }

    void VKSwapChain::present() {
        const VkSwapchainKHR   swapChains[] = { swapChain };
        const VkPresentInfoKHR presentInfo{
            .sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
            .waitSemaphoreCount = 1,
            .pWaitSemaphores    = &renderFinishedSemaphore[currentFrameIndex],
            .swapchainCount     = 1,
            .pSwapchains        = swapChains,
            .pImageIndices      = &imageIndex[currentFrameIndex],
            .pResults           = nullptr // Optional
        };
        const auto result = vkQueuePresentKHR(presentQueue, &presentInfo);
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
            recreate();
        } else if (result != VK_SUCCESS) {
            throw Exception("failed to present swap chain image!");
        }
    }

    bool VKSwapChain::acquire(const shared_ptr<Fence>& fence) {
        const auto vkFence = static_pointer_cast<VKFence>(fence);
        // wait until the GPU has finished rendering the frame.
        if (vkWaitForFences(device->getDevice(), 1, &vkFence->getFence(), VK_TRUE, UINT64_MAX) == VK_TIMEOUT) {
            throw Exception("timeout waiting for inFlightFence");
        }
        // get the next available swap chain image
        const auto result = vkAcquireNextImageKHR(
             device->getDevice(),
             swapChain,
             UINT64_MAX,
             imageAvailableSemaphore[currentFrameIndex],
             VK_NULL_HANDLE,
             &imageIndex[currentFrameIndex]);
        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            return false;
        }
        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw Exception("failed to acquire swap chain image :", to_string(result));
        }
        vkResetFences(device->getDevice(), 1, &vkFence->getFence());
        return true;
    }

    VKSwapChain::~VKSwapChain() {
        cleanup();
        for (int i = 0; i < framesInFlight; i++) {
            vkDestroySemaphore(device->getDevice(), imageAvailableSemaphore[i], nullptr);
            vkDestroySemaphore(device->getDevice(), renderFinishedSemaphore[i], nullptr);
        }
    }

}