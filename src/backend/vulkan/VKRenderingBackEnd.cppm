module;
#include "VKLibraries.h"
import std;

export module dxvk.backend.vulkan;

import dxvk.backend;
import dxvk.backend.framedata;

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

        auto getSurface() const { return surface; }

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

        auto getPresentQueueFamilyIndex() const { return presentQueueFamilyIndex; }

        VkImageView createImageView(VkImage            image,
                                    VkFormat           format,
                                    VkImageAspectFlags aspectFlags,
                                    uint32_t           mipLevels = 1,
                                    VkImageViewType    type      = VK_IMAGE_VIEW_TYPE_2D,
                                    uint32_t           baseArrayLayer = 0,
                                    uint32_t           layers = 1,
                                    uint32_t           baseMipLevel = 0) const;

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

    class VKSwapChain : public SwapChain {
    public:
        VKSwapChain(const VKPhysicalDevice& physicalDevice, const VKDevice& device, uint32_t width, uint32_t height);
        ~VKSwapChain() override;

        auto getSwapChain() { return swapChain; }

        auto getCurrentFrameIndex() const { return currentFrameIndex; }

        void nextSwapChain() override;

        void present(const FrameData& frameData) override;

        void prepare(FrameData& frameData) override;

    private:
        VkDevice                    device;
        VkSwapchainKHR              swapChain;
        std::vector<VkImage>        swapChainImages;
        VkFormat                    swapChainImageFormat;
        VkExtent2D                  swapChainExtent;
        float                       swapChainRatio;
        std::vector<VkImageView>    swapChainImageViews;
        uint32_t                    currentFrameIndex{0};
        VkImageBlit                 colorImageBlit{};
        VkQueue                     presentQueue;

        // For Device::querySwapChainSupport()
        struct SwapChainSupportDetails {
            VkSurfaceCapabilitiesKHR        capabilities;
            std::vector<VkSurfaceFormatKHR> formats;
            std::vector<VkPresentModeKHR>   presentModes;
        };

        // Get the swap chain capabilities
        SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice vkPhysicalDevice, VkSurfaceKHR surface) const;

        // Get the swap chain format, default for sRGB/NON-LINEAR
        static VkSurfaceFormatKHR chooseSwapSurfaceFormat(
                const std::vector<VkSurfaceFormatKHR> &availableFormats);

        // Get the swap chain present mode, default to MAILBOX, if not available FIFO (V-SYNC)
        static VkPresentModeKHR chooseSwapPresentMode(
                const std::vector<VkPresentModeKHR> &availablePresentModes);

        // Get the swap chain images sizes
        VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities, uint32_t width, uint32_t height) const;


    };

    class VKRenderingBackEnd : public RenderingBackEnd {
    public:
        VKRenderingBackEnd(uint32_t width, uint32_t height);

        auto getVKInstance() const { return std::reinterpret_pointer_cast<VKInstance>(instance); }
        auto getVKPhysicalDevice() const { return std::reinterpret_pointer_cast<VKPhysicalDevice>(physicalDevice); }
        auto getVKDevice() const { return std::reinterpret_pointer_cast<VKDevice>(device); }
        auto getVKGraphicCommandQueue() const { return std::reinterpret_pointer_cast<VKCommandQueue>(graphicCommandQueue); }
    };

}