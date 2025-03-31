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

    private:
        VkInstance                  instance{VK_NULL_HANDLE};
        VkSurfaceKHR                surface;
        VkPhysicalDevice            physicalDevice{VK_NULL_HANDLE};
        std::vector<const char*>    deviceExtensions;
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

        auto getDevice() const { return device; }

        auto getGraphicsQueueFamilyIndex() const { return graphicsQueueFamilyIndex; }

    private:
        VkDevice    device{VK_NULL_HANDLE};
        uint32_t    presentQueueFamilyIndex;
        uint32_t    graphicsQueueFamilyIndex;
        uint32_t    transferQueueFamilyIndex;
        uint32_t    computeQueueFamilyIndex;
    };

    class VKCommandQueue : public CommandQueue {
    public:
        VKCommandQueue(const VKDevice& device);
        ~VKCommandQueue() override;

        auto getCommandQueue() const { return commandQueue; }

    private:
        VkQueue commandQueue;
    };

    class VKRenderingBackEnd : public RenderingBackEnd {
    public:
        VKRenderingBackEnd();

        auto getVKInstance() const { return std::reinterpret_pointer_cast<VKInstance>(instance); }
        auto getVKPhysicalDevice() const { return std::reinterpret_pointer_cast<VKPhysicalDevice>(physicalDevice); }
        auto getVKDevice() const { return std::reinterpret_pointer_cast<VKDevice>(device); }
        auto getVKGraphicCommandQueue() const { return std::reinterpret_pointer_cast<VKCommandQueue>(graphicCommandQueue); }
    };

}