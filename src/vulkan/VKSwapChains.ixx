/*
* Copyright (c) 2025-present Henri Michelon
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/
module;
#include "vireo/backend/vulkan/Libraries.h"
export module vireo.vulkan.swapchains;

import std;
import vireo;
import vireo.platform;
import vireo.vulkan.devices;

export namespace vireo {

    class VKSwapChain : public SwapChain {
    public:
        VKSwapChain(
            const std::shared_ptr<const VKDevice>& device,
            VkQueue presentQueue,
            PlatformWindowHandle windowHandle,
            ImageFormat format,
            PresentMode vSyncMode,
            uint32_t framesInFlight);

        ~VKSwapChain() override;

        auto getSwapChain() const { return swapChain; }

        auto getCurrentImage() const { return swapChainImages[imageIndex[currentFrameIndex]]; }

        auto getCurrentImageView() const { return swapChainImageViews[imageIndex[currentFrameIndex]]; }

        void nextFrameIndex() override;

        bool acquire(const std::shared_ptr<Fence>& fence) override;

        void present() override;

        void recreate() override;

        const auto& getCurrentImageAvailableSemaphoreInfo() const { return imageAvailableSemaphoreInfo[currentFrameIndex]; }

        const auto& getCurrentRenderFinishedSemaphoreInfo() const { return renderFinishedSemaphoreInfo[imageIndex[currentFrameIndex]]; }

        void waitIdle() override { vkDeviceWaitIdle(device->getDevice()); }

    private:
        static constexpr VkPresentModeKHR vkPresentModes[] {
            VK_PRESENT_MODE_IMMEDIATE_KHR,
            VK_PRESENT_MODE_FIFO_KHR
        };
        // For Device::querySwapChainSupport()
        struct SwapChainSupportDetails {
            VkSurfaceCapabilitiesKHR   capabilities;
            std::vector<VkSurfaceFormatKHR> formats;
            std::vector<VkPresentModeKHR>   presentModes;
        };

        const std::shared_ptr<const VKDevice> device;
        // Platform specific window handle
        PlatformWindowHandle    windowHandle;
        // Rendering window drawing surface
        VkSurfaceKHR             surface;
        VkSwapchainKHR           swapChain{VK_NULL_HANDLE};
        uint32_t                 imagesCount;
        // Frame buffers
        std::vector<VkImage>     swapChainImages;
        // Frame buffers image format
        VkFormat                 swapChainImageFormat;
        // Current surface size
        VkExtent2D               swapChainExtent;
        // Frame buffer vulkan image view
        std::vector<VkImageView> swapChainImageViews;
        // Submission queue used to present images on the surface
        VkQueue                  presentQueue;
        // Frame buffer index by frame index
        std::vector<uint32_t>        imageIndex;
        // Semaphore to wait the frame buffers to be ready to be used
        std::vector<VkSemaphore>     imageAvailableSemaphore;
        // Semaphore to wait for all the commands to be executed by the GPU
        std::vector<VkSemaphore>     renderFinishedSemaphore;
        std::vector<VkSemaphoreSubmitInfo> imageAvailableSemaphoreInfo;
        std::vector<VkSemaphoreSubmitInfo> renderFinishedSemaphoreInfo;

        void create();

        void cleanup() const;

        // Get the swap chain capabilities
        static SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice vkPhysicalDevice, VkSurfaceKHR surface);

        // Get the swap chain format
        VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats) const;

        // Get the swap chain present mode
        VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes) const;

        // Get the swap chain images sizes
        VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities) const;
    };

}