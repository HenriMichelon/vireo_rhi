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
        void* windowHandle,
        const PresentMode vSyncMode):
        device{device},
        physicalDevice{device->getPhysicalDevice()},
        vSyncMode{vSyncMode},
#ifdef _WIN32
        hWnd{static_cast<HWND>(windowHandle)}
#endif
    {
        imageIndex.resize(FRAMES_IN_FLIGHT);
        imageAvailableSemaphore.resize(FRAMES_IN_FLIGHT);
        renderFinishedSemaphore.resize(FRAMES_IN_FLIGHT);

        constexpr VkSemaphoreCreateInfo semaphoreInfo{
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO
        };
        for (int i = 0; i < FRAMES_IN_FLIGHT; i++) {
            if (vkCreateSemaphore(device->getDevice(), &semaphoreInfo, nullptr, &imageAvailableSemaphore[i]) != VK_SUCCESS
                || vkCreateSemaphore(device->getDevice(), &semaphoreInfo, nullptr, &renderFinishedSemaphore[i]) != VK_SUCCESS) {
                throw Exception("failed to create semaphores!");
            }
#ifdef _DEBUG
            vkSetObjectName(device->getDevice(), reinterpret_cast<uint64_t>(imageAvailableSemaphore[i]), VK_OBJECT_TYPE_SEMAPHORE,
                "VKFrameData image Semaphore : " + to_string(i));
            vkSetObjectName(device->getDevice(), reinterpret_cast<uint64_t>(renderFinishedSemaphore[i]), VK_OBJECT_TYPE_SEMAPHORE,
        "VKFrameData render Semaphore : " + to_string(i));
#endif
        }

        vkGetDeviceQueue(
            device->getDevice(),
            device->getPresentQueueFamilyIndex(),
            0,
            &presentQueue);
        create();
    }

    void VKSwapChain::create() {
        // https://vulkan-tutorial.com/Drawing_a_triangle/Presentation/Swap_chain
        const SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice.getPhysicalDevice(), physicalDevice.getSurface());
        const VkSurfaceFormatKHR surfaceFormat= chooseSwapSurfaceFormat(swapChainSupport.formats);
        const VkPresentModeKHR presentMode = chooseSwapPresentMode(vSyncMode, swapChainSupport.presentModes);
        swapChainExtent = chooseSwapExtent(swapChainSupport.capabilities);

        uint32_t imageCount = FRAMES_IN_FLIGHT;
        if (swapChainSupport.capabilities.maxImageCount > 0 &&
            imageCount > swapChainSupport.capabilities.maxImageCount) {
            imageCount = swapChainSupport.capabilities.maxImageCount;
        }

        {
            VkSwapchainCreateInfoKHR createInfo = {
                .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
                .surface = physicalDevice.getSurface(),
                .minImageCount = imageCount,
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
                .clipped = VK_TRUE
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
        vkGetSwapchainImagesKHR(device->getDevice(), swapChain, &imageCount, nullptr);
        swapChainImages.resize(imageCount);
        swapChainImageViews.resize(swapChainImages.size());
        swapChainImageFormat = surfaceFormat.format;

        vkGetSwapchainImagesKHR(device->getDevice(), swapChain, &imageCount, swapChainImages.data());
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
            // Using sRGB no-linear color space
            // https://learnopengl.com/Advanced-Lighting/Gamma-Correction
            if (availableFormat.format == VKImage::vkFormats[static_cast<int>(RENDER_FORMAT)]) { return availableFormat; }
        }
        return availableFormats[0];
    }

    VkPresentModeKHR VKSwapChain::chooseSwapPresentMode(
        const PresentMode vSyncMode,
        const vector<VkPresentModeKHR> &availablePresentModes) {
        // https://vulkan-tutorial.com/Drawing_a_triangle/Presentation/Swap_chain#page_Presentation-mode
        for (const auto &availablePresentMode : availablePresentModes) {
            if (availablePresentMode == vkPresentModes[static_cast<int>(vSyncMode)]) {
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
        currentFrameIndex = (currentFrameIndex + 1) % FRAMES_IN_FLIGHT;
    }

    void VKSwapChain::present() {
        {
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
    }

    bool VKSwapChain::acquire(const shared_ptr<Fence>& fence) {
        const auto vkFence = static_pointer_cast<VKFence>(fence);
        // wait until the GPU has finished rendering the frame.
        if (vkWaitForFences(device->getDevice(), 1, &vkFence->getFence(), VK_TRUE, UINT64_MAX) == VK_TIMEOUT) {
            throw Exception("timeout waiting for inFlightFence");
        }
        // get the next available swap chain image
        {
            const auto result = vkAcquireNextImageKHR(
                 device->getDevice(),
                 swapChain,
                 UINT64_MAX,
                 imageAvailableSemaphore[currentFrameIndex],
                 VK_NULL_HANDLE,
                 &imageIndex[currentFrameIndex]);
            if (result == VK_ERROR_OUT_OF_DATE_KHR) {
                recreate();
                return false;
            }
            if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
                throw Exception("failed to acquire swap chain image :", to_string(result));
            }
        }
        vkResetFences(device->getDevice(), 1, &vkFence->getFence());
        return true;
    }

    VKSwapChain::~VKSwapChain() {
        cleanup();
        for (int i = 0; i < FRAMES_IN_FLIGHT; i++) {
            vkDestroySemaphore(device->getDevice(), imageAvailableSemaphore[i], nullptr);
            vkDestroySemaphore(device->getDevice(), renderFinishedSemaphore[i], nullptr);
        }
    }

}