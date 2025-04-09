/*
* Copyright (c) 2025-present Henri Michelon
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/
module;
#include "backend/vulkan/Tools.h"

export module vireo.backend.vulkan.device;

import vireo.backend;
import vireo.backend.device;

export namespace vireo::backend {

    class VKInstance : public Instance {
    public:
        VKInstance();
        ~VKInstance() override;

        auto getInstance() const { return instance; }

        auto& getRequestedLayers() const { return requestedLayers; }

    private:
        VkInstance instance{VK_NULL_HANDLE};
        // Used to redirect validation layers to the logging system
        VkDebugUtilsMessengerEXT debugMessenger;
        // All the vulkan layers used
        std::vector<const char *>requestedLayers{};
    };

    class VKPhysicalDevice : public PhysicalDevice {
    public:
        VKPhysicalDevice(VkInstance instance);
        ~VKPhysicalDevice() override;

        auto getPhysicalDevice() const { return physicalDevice; }

        auto& getDeviceExtensions() const { return deviceExtensions; }

        struct QueueFamilyIndices {
            std::optional<uint32_t> graphicsFamily;
            std::optional<uint32_t> presentFamily;
            std::optional<uint32_t> transferFamily;
            std::optional<uint32_t> computeFamily;

            bool isComplete() const {
                return graphicsFamily.has_value() &&
                        presentFamily.has_value() &&
                        transferFamily.has_value() &&
                        computeFamily.has_value();
            }
        };

        // Get the supported queues families for a particular GPU
        QueueFamilyIndices findQueueFamilies(VkPhysicalDevice vkPhysicalDevice) const;

        // Find a dedicated transfer queue
        uint32_t findTransferQueueFamily() const;

        // Find a dedicated compute & transfer queue
        uint32_t findComputeQueueFamily() const;

        auto getSurface() const { return surface; }

        auto getMemoryTypeDeviceLocalIndex() const { return memoryTypeDeviceLocalIndex; }

        auto getMemoryTypeHostVisibleIndex() const { return memoryTypeHostVisibleIndex; }

    private:
        VkInstance                   instance{VK_NULL_HANDLE};
        VkSurfaceKHR                 surface;
        VkPhysicalDevice             physicalDevice{VK_NULL_HANDLE};
        std::vector<const char*>     deviceExtensions;
        VkPhysicalDeviceFeatures     deviceFeatures {};
        VkPhysicalDeviceProperties2  deviceProperties{
            VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2
        };
        VkPhysicalDeviceIDProperties physDeviceIDProps{
            VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ID_PROPERTIES
        };
        uint32_t                     memoryTypeDeviceLocalIndex;
        uint32_t                     memoryTypeHostVisibleIndex;

        struct SwapChainSupportDetails {
            VkSurfaceCapabilitiesKHR        capabilities;
            std::vector<VkSurfaceFormatKHR> formats;
            std::vector<VkPresentModeKHR>   presentModes;
        };

        // Rate physical device by properties to find the best suitable GPU
        uint32_t VKPhysicalDevice::rateDeviceSuitability(
            VkPhysicalDevice            vkPhysicalDevice,
            const std::vector<const char *> &deviceExtensions) const;

        static bool checkDeviceExtensionSupport
            (VkPhysicalDevice            vkPhysicalDevice,
            const std::vector<const char *> &deviceExtensions);

        // Get the swap chain capabilities
        SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice vkPhysicalDevice) const;
    };

    class VKDevice : public Device {
    public:
        VKDevice(const VKPhysicalDevice& physicalDevice, const std::vector<const char *>& requestedLayers);
        ~VKDevice() override;

        inline auto getDevice() const { return device; }

        inline const auto& getPhysicalDevice() const { return physicalDevice; }

        auto getGraphicsQueueFamilyIndex() const { return graphicsQueueFamilyIndex; }

        auto getPresentQueueFamilyIndex() const { return presentQueueFamilyIndex; }

        auto getComputeQueueFamilyIndex() const { return computeQueueFamilyIndex; }

        auto getTransferQueueFamilyIndex() const { return transferQueueFamilyIndex; }

        VkImageView createImageView(VkImage            image,
                                    VkFormat           format,
                                    VkImageAspectFlags aspectFlags,
                                    uint32_t           mipLevels = 1,
                                    VkImageViewType    type      = VK_IMAGE_VIEW_TYPE_2D,
                                    uint32_t           baseArrayLayer = 0,
                                    uint32_t           layers = 1,
                                    uint32_t           baseMipLevel = 0) const;

        static VkImageMemoryBarrier imageMemoryBarrier(
          VkImage image,
          VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask,
          VkImageLayout oldLayout, VkImageLayout newLayout,
          uint32_t baseMipLevel = 0,
          uint32_t levelCount = VK_REMAINING_MIP_LEVELS);

    private:
        const VKPhysicalDevice& physicalDevice;
        VkDevice    device{VK_NULL_HANDLE};
        uint32_t    presentQueueFamilyIndex;
        uint32_t    graphicsQueueFamilyIndex;
        uint32_t    transferQueueFamilyIndex;
        uint32_t    computeQueueFamilyIndex;
    };

}