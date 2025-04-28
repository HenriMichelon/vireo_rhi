/*
* Copyright (c) 2025-present Henri Michelon
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/
module;
#include "vireo/backend/vulkan/Libraries.h"
export module vireo.vulkan.devices;

import std;
import vireo;

export namespace vireo {

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
        static constexpr VkSampleCountFlagBits vkSampleCountFlag[] {
            VK_SAMPLE_COUNT_1_BIT,
            VK_SAMPLE_COUNT_2_BIT,
            VK_SAMPLE_COUNT_4_BIT,
            VK_SAMPLE_COUNT_8_BIT,
            VK_SAMPLE_COUNT_16_BIT,
            VK_SAMPLE_COUNT_32_BIT,
            VK_SAMPLE_COUNT_64_BIT,
        };

        VKPhysicalDevice(VkInstance instance);

        ~VKPhysicalDevice() override;

        auto getPhysicalDevice() const { return physicalDevice; }

        auto getInstance() const { return instance; }

        const auto& getDeviceExtensions() const { return deviceExtensions; }

        const auto& getDeviceProperties() const { return deviceProperties.properties; }

        struct QueueFamilyIndices {
            std::optional<uint32_t> graphicsFamily;
            std::optional<uint32_t> transferFamily;
            std::optional<uint32_t> computeFamily;

            bool isComplete() const {
                return graphicsFamily.has_value() &&
                        transferFamily.has_value() &&
                        computeFamily.has_value();
            }
        };

        // Get the supported queues families for a particular GPU
        static QueueFamilyIndices findQueueFamilies(VkPhysicalDevice vkPhysicalDevice);

        // Find a dedicated transfer queue
        uint32_t findTransferQueueFamily() const;

        // Find a dedicated compute & transfer queue
        uint32_t findComputeQueueFamily() const;

        uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const;

        auto getSampleCount() const { return sampleCount; }

        const PhysicalDeviceDesc getDescription() const override;

    private:
        VkInstance                   instance{VK_NULL_HANDLE};
        VkPhysicalDevice             physicalDevice{VK_NULL_HANDLE};
        std::vector<const char*>          deviceExtensions;
        VkPhysicalDeviceFeatures     deviceFeatures {};
        VkPhysicalDeviceProperties2  deviceProperties{
            VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2
        };
        VkPhysicalDeviceIDProperties physDeviceIDProps{
            VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ID_PROPERTIES
        };
        VkSampleCountFlagBits        sampleCount;

        struct SwapChainSupportDetails {
            VkSurfaceCapabilitiesKHR   capabilities;
            std::vector<VkSurfaceFormatKHR> formats;
            std::vector<VkPresentModeKHR>   presentModes;
        };

        // Rate physical device by properties to find the best suitable GPU
        static uint32_t rateDeviceSuitability(
            VkPhysicalDevice            vkPhysicalDevice,
            const std::vector<const char *> &deviceExtensions);

        static bool checkDeviceExtensionSupport
            (VkPhysicalDevice            vkPhysicalDevice,
            const std::vector<const char *> &deviceExtensions);

        VkSampleCountFlagBits getMaxUsableMSAASampleCount() const;
    };

    class VKDevice : public Device {
    public:
        VKDevice(const VKPhysicalDevice& physicalDevice, const std::vector<const char *>& requestedLayers);
        ~VKDevice() override;

        inline auto getDevice() const { return device; }

        inline const auto& getPhysicalDevice() const { return physicalDevice; }

        auto getGraphicsQueueFamilyIndex() const { return graphicsQueueFamilyIndex; }

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

    private:
        const VKPhysicalDevice& physicalDevice;
        VkDevice    device{VK_NULL_HANDLE};
        uint32_t    graphicsQueueFamilyIndex;
        uint32_t    transferQueueFamilyIndex;
        uint32_t    computeQueueFamilyIndex;
    };

    class VKFence : public Fence {
    public:
        VKFence(const std::shared_ptr<const VKDevice>& device, const std::wstring& name);

        ~VKFence() override;

        auto& getFence() const { return fence; }

    private:
        const VkDevice device;
        VkFence        fence;
    };

}