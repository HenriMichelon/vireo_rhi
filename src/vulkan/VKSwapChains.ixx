/*
* Copyright (c) 2025-present Henri Michelon
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/
module;
#include "vireo/backend/vulkan/Libraries.h"
export module vireo.vulkan.swapchains;

import vireo;
import vireo.config;
import vireo.vulkan.devices;

export namespace vireo {

    class VKSwapChain : public SwapChain {
    public:
        VKSwapChain(
            const shared_ptr<const VKDevice>& device,
            void* windowHandle,
            ImageFormat format,
            PresentMode vSyncMode,
            uint32_t framesInFlight);

        ~VKSwapChain() override;

        auto getSwapChain() const { return swapChain; }

        auto getCurrentImage() const { return swapChainImages[imageIndex[currentFrameIndex]]; }

        auto getCurrentImageView() const { return swapChainImageViews[imageIndex[currentFrameIndex]]; }

        void nextSwapChain() override;

        bool acquire(const shared_ptr<Fence>& fence) override;

        void present() override;

        void recreate() override;

        auto& getCurrentImageAvailableSemaphore() const { return imageAvailableSemaphore[imageIndex[currentFrameIndex]]; }

        auto& getCurrentRenderFinishedSemaphore() const { return renderFinishedSemaphore[imageIndex[currentFrameIndex]]; }

    private:
        static constexpr VkPresentModeKHR vkPresentModes[] {
            VK_PRESENT_MODE_IMMEDIATE_KHR,
            VK_PRESENT_MODE_FIFO_KHR
        };
        // For Device::querySwapChainSupport()
        struct SwapChainSupportDetails {
            VkSurfaceCapabilitiesKHR        capabilities;
            vector<VkSurfaceFormatKHR> formats;
            vector<VkPresentModeKHR>   presentModes;
        };

        const shared_ptr<const VKDevice>         device;
        const VKPhysicalDevice& physicalDevice;
        VkSwapchainKHR          swapChain;
        vector<VkImage>         swapChainImages;
        VkFormat                swapChainImageFormat;
        VkExtent2D              swapChainExtent;
        vector<VkImageView>     swapChainImageViews;
        VkQueue                 presentQueue;
        vector<uint32_t>        imageIndex;
        vector<VkSemaphore>     imageAvailableSemaphore;
        vector<VkSemaphore>     renderFinishedSemaphore;

#ifdef _WIN32
        HWND hWnd;
#endif

        void create();

        void cleanup() const;

        // Get the swap chain capabilities
        static SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice vkPhysicalDevice, VkSurfaceKHR surface);

        // Get the swap chain format
        VkSurfaceFormatKHR chooseSwapSurfaceFormat(const vector<VkSurfaceFormatKHR> &availableFormats);

        // Get the swap chain present mode
        VkPresentModeKHR chooseSwapPresentMode(const vector<VkPresentModeKHR> &availablePresentModes) const;

        // Get the swap chain images sizes
        VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities) const;
    };

}