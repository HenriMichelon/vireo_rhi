module;
#include "VKLibraries.h"
import std;

export module dxvk.backend.vulkan;

import dxvk.backend;

export namespace dxvk::backend {

    class VKInstance : public Instance {
    public:
        VKInstance();
        ~VKInstance() override;
        auto getInstance() const { return instance; }
    private:
        VkInstance instance{VK_NULL_HANDLE};
    };

    class VKPhysicalDevice : public PhysicalDevice {
    public:
        VKPhysicalDevice(VkInstance instance);
        ~VKPhysicalDevice() override;
        auto getPhysicalDevice() const { return physicalDevice; }
    private:
        VkInstance                  instance{VK_NULL_HANDLE};
        VkSurfaceKHR                surface;
        VkPhysicalDevice            physicalDevice{VK_NULL_HANDLE};
        VkPhysicalDeviceFeatures    deviceFeatures {};
        VkPhysicalDeviceProperties2 deviceProperties{
            VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2
        };
        VkPhysicalDeviceIDProperties physDeviceIDProps{
            VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ID_PROPERTIES
        };
        struct SwapChainSupportDetails {
            VkSurfaceCapabilitiesKHR        capabilities;
            std::vector<VkSurfaceFormatKHR> formats;
            std::vector<VkPresentModeKHR>   presentModes;
        };
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

        uint32_t VKPhysicalDevice::rateDeviceSuitability(
            VkPhysicalDevice            vkPhysicalDevice,
            const std::vector<const char *> &deviceExtensions) const;

        static bool checkDeviceExtensionSupport
            (VkPhysicalDevice            vkPhysicalDevice,
            const std::vector<const char *> &deviceExtensions);

        SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice vkPhysicalDevice) const;

        QueueFamilyIndices findQueueFamilies(VkPhysicalDevice vkPhysicalDevice) const;

    };

    class VKRenderingBackEnd : public RenderingBackEnd {
    public:
        VKRenderingBackEnd();

        auto getVKInstance() const { return std::reinterpret_pointer_cast<VKInstance>(instance); }
        auto getVKPhysicalDevice() const { return std::reinterpret_pointer_cast<VKPhysicalDevice>(physicalDevice); }
    };

}