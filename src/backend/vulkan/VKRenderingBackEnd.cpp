module;
#include "VKLibraries.h"
#include "VKTools.h"
import std;

module dxvk.backend.vulkan;

import dxvk.app.win32;
import dxvk.backend.vulkan.framedata;

namespace dxvk::backend {

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT,
                                                       VkDebugUtilsMessageTypeFlagsEXT,
                                                       const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                                                       void *) {
        std::cerr << "validation layer: " <<  pCallbackData->pMessage << std::endl;
        return VK_FALSE;
    }

    // vkCreateDebugUtilsMessengerEXT linker
    VkResult CreateDebugUtilsMessengerEXT(const VkInstance                          instance,
                                          const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
                                          const VkAllocationCallbacks              *pAllocator,
                                          VkDebugUtilsMessengerEXT                 *pDebugMessenger) {
        const auto func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
                vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));
        if (func != nullptr) {
            return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
        }
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }

    // vkDestroyDebugUtilsMessengerEXT linker
    void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger,
                                       const VkAllocationCallbacks *pAllocator) {
        const auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
                vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));
        if (func != nullptr) {
            func(instance, debugMessenger, pAllocator);
        }
    }


    VKRenderingBackEnd::VKRenderingBackEnd(uint32_t width, uint32_t height) {
        instance = std::make_shared<VKInstance>();
        physicalDevice = std::make_shared<VKPhysicalDevice>(getVKInstance()->getInstance());
        device = std::make_shared<VKDevice>(*getVKPhysicalDevice(), getVKInstance()->getRequestedLayers());
        graphicCommandQueue = std::make_shared<VKCommandQueue>(*getVKDevice());
        swapChain = std::make_shared<VKSwapChain>(*getVKPhysicalDevice(), *getVKDevice(), width, height);
    }

    VKInstance::VKInstance() {
        vulkanInitialize();
        std::vector<const char *>requestedLayers{};
#ifdef _DEBUG
        const char* validationLayerName = "VK_LAYER_KHRONOS_validation";
        requestedLayers.push_back(validationLayerName);
#endif
        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
        for (const char *layerName : requestedLayers) {
            bool layerFound = false;
            for (const auto &layerProperties : availableLayers) {
                if (strcmp(layerName, layerProperties.layerName) == 0) {
                    layerFound = true;
                    break;
                }
            }
            if (!layerFound) { die("A requested Vulkan layer is not supported"); }
        }

        std::vector<const char *> instanceExtensions{};
        instanceExtensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
        instanceExtensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
        instanceExtensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
#ifdef _DEBUG
        instanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif
        constexpr VkApplicationInfo applicationInfo{
            .sType      = VK_STRUCTURE_TYPE_APPLICATION_INFO,
            .apiVersion = VK_API_VERSION_1_4};
        const VkInstanceCreateInfo createInfo = {VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
                                                 nullptr,
                                                 0,
                                                 &applicationInfo,
                                                 static_cast<uint32_t>(requestedLayers.size()),
                                                 requestedLayers.data(),
                                                 static_cast<uint32_t>(instanceExtensions.size()),
                                                 instanceExtensions.data()};
        ThrowIfFailed(vkCreateInstance(&createInfo, nullptr, &instance));
        vulkanInitializeInstance(instance);

#ifdef _DEBUG
        // Initialize validating layer for logging
        constexpr VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{
            .sType           = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
            .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                    VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
            .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                    VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                    VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
            .pfnUserCallback = debugCallback,
            .pUserData       = nullptr,
        };
        if (CreateDebugUtilsMessengerEXT(instance, &debugCreateInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
            die("failed to set up debug messenger!");
        }
#endif
    }

    VKInstance::~VKInstance() {
#ifdef _DEBUG
        DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
#endif
        vkDestroyInstance(instance, nullptr);
        vulkanFinalize();
    }

#ifdef _WIN32
    PFN_vkCreateWin32SurfaceKHR vkCreateWin32SurfaceKHR;
#endif

    VKPhysicalDevice::VKPhysicalDevice(VkInstance instance):
        instance(instance),
    // Requested device extensions
        deviceExtensions {
            // Mandatory to create a swap chain
            VK_KHR_SWAPCHAIN_EXTENSION_NAME,
            // https://docs.vulkan.org/samples/latest/samples/extensions/dynamic_rendering/README.html
            VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME,
            // https://docs.vulkan.org/samples/latest/samples/extensions/shader_object/README.html
            VK_EXT_SHADER_OBJECT_EXTENSION_NAME,
            // for Vulkan Memory Allocator
            VK_KHR_GET_MEMORY_REQUIREMENTS_2_EXTENSION_NAME,
            VK_EXT_LINE_RASTERIZATION_EXTENSION_NAME,
#ifndef NDEBUG
            // To debugPrintEXT() from shaders :-)
            // See shader_debug_env.cmd for setup with environment variables
            VK_KHR_SHADER_NON_SEMANTIC_INFO_EXTENSION_NAME,
#endif
        }{
        //////////////////// Find the best GPU
        // Check for at least one supported Vulkan physical device
        // https://vulkan-tutorial.com/Drawing_a_triangle/Setup/Physical_devices_and_queue_families#page_Selecting-a-physical-device
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
        if (deviceCount == 0) {
            die("Failed to find GPUs with Vulkan support");
        }

        // Get a VkSurface for drawing in the window, must be done before picking the better physical device
        // since we need the VkSurface for vkGetPhysicalDeviceSurfaceCapabilitiesKHR
        // https://vulkan-tutorial.com/Drawing_a_triangle/Presentation/Window_surface#page_Window-surface-creation
#ifdef _WIN32
        const VkWin32SurfaceCreateInfoKHR surfaceCreateInfo{
                .sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
                .hinstance = GetModuleHandle(nullptr),
                .hwnd = Win32Application::getHwnd(),
        };
        vkCreateWin32SurfaceKHR = (PFN_vkCreateWin32SurfaceKHR)vkGetInstanceProcAddr(instance, "vkCreateWin32SurfaceKHR");
        if (vkCreateWin32SurfaceKHR(instance, &surfaceCreateInfo, nullptr, &surface) != VK_SUCCESS) {
            die("Failed to create window surface!");
        }
#endif


        // Use the better Vulkan physical device found
        // https://vulkan-tutorial.com/Drawing_a_triangle/Setup/Physical_devices_and_queue_families#page_Base-device-suitability-checks
        vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());
        // Use an ordered map to automatically sort candidates by increasing score
        multimap<uint32_t, VkPhysicalDevice> candidates;
        for (const auto &dev : devices) {
            uint32_t score = rateDeviceSuitability(dev, deviceExtensions);
            candidates.insert(make_pair(score, dev));
        }
        // Check if the best candidate is suitable at all
        if (candidates.rbegin()->first > 0) {
            // Select the better suitable device found
            physicalDevice = candidates.rbegin()->second;
            // Select the best MSAA samples count if requested
            // if (applicationConfig.msaa == MSAA::AUTO) {
                // samples = getMaxUsableMSAASampleCount();
            // }
            deviceProperties.pNext = &physDeviceIDProps;
            vkGetPhysicalDeviceProperties2(physicalDevice, &deviceProperties);
            // Get the GPU description and total memory
            // getAdapterDescFromOS();
            vkGetPhysicalDeviceFeatures(physicalDevice, &deviceFeatures);
        } else {
            die("Failed to find a suitable GPU!");
        }


    }

     VKPhysicalDevice::QueueFamilyIndices VKPhysicalDevice::findQueueFamilies(const VkPhysicalDevice vkPhysicalDevice) const {
        // https://vulkan-tutorial.com/Drawing_a_triangle/Setup/Physical_devices_and_queue_families#page_Queue-families
        QueueFamilyIndices indices;
        uint32_t           queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice, &queueFamilyCount, nullptr);
        vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice, &queueFamilyCount, queueFamilies.data());
        int i = 0;
        for (const auto &queueFamily : queueFamilies) {
            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                indices.graphicsFamily = i;
            }
            if (queueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT) {
                indices.transferFamily = i;
            }
            if (queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT) {
                indices.computeFamily = i;
            }
            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(vkPhysicalDevice, i, surface, &presentSupport);
            if (presentSupport) {
                indices.presentFamily = i;
            }
            if (indices.isComplete()) {
                break;
            }
            i++;
        }
        return indices;
    }

    uint32_t VKPhysicalDevice::findTransferQueueFamily() const {
        uint32_t           queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
        vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());
        uint32_t i = 0;
        for (const auto &queueFamily : queueFamilies) {
            if ((queueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT) &&
                (!(queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)) &&
                (!(queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT))) {
                return i;
                }
            i++;
        }
        die("Could not find dedicated transfer queue family");
        return i;
    }

    uint32_t VKPhysicalDevice::findComputeQueueFamily() const {
        uint32_t           queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
        vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());
        uint32_t i = 0;
        for (const auto &queueFamily : queueFamilies) {
            if ((queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT) &&
                (!(queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)) &&
                (queueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT)) {
                return i;
                }
            i++;
        }
        die("Could not find dedicated compute queue family");
        return i;
    }


    VKPhysicalDevice::~VKPhysicalDevice() {
        vkDestroySurfaceKHR(instance, surface, nullptr);
    }

    uint32_t VKPhysicalDevice::rateDeviceSuitability(
        VkPhysicalDevice            vkPhysicalDevice,
        const vector<const char *> &deviceExtensions) const {
        // https://vulkan-tutorial.com/Drawing_a_triangle/Setup/Physical_devices_and_queue_families#page_Base-device-suitability-checks
        VkPhysicalDeviceProperties _deviceProperties;
        vkGetPhysicalDeviceProperties(vkPhysicalDevice, &_deviceProperties);
        VkPhysicalDeviceFeatures deviceFeatures;
        vkGetPhysicalDeviceFeatures(vkPhysicalDevice, &deviceFeatures);

        uint32_t score = 0;
        // Discrete GPUs have a significant performance advantage
        if (_deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
            score += 1000;
        }
        // Maximum possible size of textures affects graphics quality
        score += _deviceProperties.limits.maxImageDimension2D;
        // Application can't function without geometry shaders
        if (!deviceFeatures.geometryShader) {
            return 0;
        }

        bool extensionsSupported = checkDeviceExtensionSupport(vkPhysicalDevice, deviceExtensions);
        bool swapChainAdequate   = false;
        if (extensionsSupported) {
            SwapChainSupportDetails swapChainSupport = querySwapChainSupport(vkPhysicalDevice);
            swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
        }
        QueueFamilyIndices indices = findQueueFamilies(vkPhysicalDevice);
        if ((!extensionsSupported) || (!indices.isComplete()) || (!swapChainAdequate)) {
            return 0;
        }
        return score;
    }

    bool VKPhysicalDevice::checkDeviceExtensionSupport(
        const VkPhysicalDevice            vkPhysicalDevice,
        const std::vector<const char *> &deviceExtensions) {
        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(vkPhysicalDevice, nullptr, &extensionCount, nullptr);
        vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(vkPhysicalDevice,
                                             nullptr,
                                             &extensionCount,
                                             availableExtensions.data());
        set<string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());
        for (const auto &extension : availableExtensions) { requiredExtensions.erase(extension.extensionName); }
        return requiredExtensions.empty();
    }

    VKPhysicalDevice::SwapChainSupportDetails VKPhysicalDevice::querySwapChainSupport(const VkPhysicalDevice vkPhysicalDevice) const {
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

    VKDevice::VKDevice(const VKPhysicalDevice& physicalDevice, const std::vector<const char *>& requestedLayers) {
         /// Select command queues
        vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        constexpr auto queuePriority = array{1.0f};
        const auto indices = physicalDevice.findQueueFamilies(physicalDevice.getPhysicalDevice());
        // Use a graphical command queue
        graphicsQueueFamilyIndex = indices.graphicsFamily.value();
        {
            const VkDeviceQueueCreateInfo queueCreateInfo{
                .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                .queueFamilyIndex = graphicsQueueFamilyIndex,
                .queueCount = 1,
                .pQueuePriorities = queuePriority.data(),
            };
            queueCreateInfos.push_back(queueCreateInfo);
        }
        // Use a presentation command queue if different from the graphical queue
        presentQueueFamilyIndex = indices.presentFamily.value();
        if (presentQueueFamilyIndex != graphicsQueueFamilyIndex) {
            const VkDeviceQueueCreateInfo queueCreateInfo{
                .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                .queueFamilyIndex = presentQueueFamilyIndex,
                .queueCount = 1,
                .pQueuePriorities = queuePriority.data(),
            };
            queueCreateInfos.push_back(queueCreateInfo);
        }
        // Use a compute command queue if different from the graphical queue
        computeQueueFamilyIndex = physicalDevice.findComputeQueueFamily();
        if (computeQueueFamilyIndex != graphicsQueueFamilyIndex) {
            const VkDeviceQueueCreateInfo queueCreateInfo{
                .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                .queueFamilyIndex = computeQueueFamilyIndex,
                .queueCount = 1,
                .pQueuePriorities = queuePriority.data(),
            };
            queueCreateInfos.push_back(queueCreateInfo);
        }
        // Use a dedicated transfer queue for DMA transfers
        transferQueueFamilyIndex = physicalDevice.findTransferQueueFamily();
        {
            const VkDeviceQueueCreateInfo queueCreateInfo{
                .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                .queueFamilyIndex = transferQueueFamilyIndex,
                .queueCount = 1,
                .pQueuePriorities = queuePriority.data(),
            };
            queueCreateInfos.push_back(queueCreateInfo);
        }

        // Initialize device extensions and create a logical device
        {
            VkPhysicalDeviceLineRasterizationFeaturesEXT lineRasterizationFeatures{
                .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_LINE_RASTERIZATION_FEATURES_EXT,
                .rectangularLines = VK_TRUE,
                // .smoothLines = VK_TRUE,
            };
            VkPhysicalDeviceSynchronization2FeaturesKHR sync2Features{
                .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES,
                .pNext = &lineRasterizationFeatures,
                .synchronization2 = VK_TRUE
            };
            VkPhysicalDeviceFeatures2 deviceFeatures2 {
                .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2,
                .pNext = &sync2Features,
                .features = {
                    // .fillModeNonSolid = VK_TRUE,
                    .samplerAnisotropy = VK_TRUE,
                }
            };

            // https://docs.vulkan.org/samples/latest/samples/extensions/shader_object/README.html
            VkPhysicalDeviceShaderObjectFeaturesEXT deviceShaderObjectFeatures{
                .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_OBJECT_FEATURES_EXT,
                .pNext = &deviceFeatures2,
                .shaderObject = VK_TRUE,
            };
            // https://lesleylai.info/en/vk-khr-dynamic-rendering/
            const VkPhysicalDeviceDynamicRenderingFeaturesKHR dynamicRenderingFeature{
                .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR,
                .pNext = &deviceShaderObjectFeatures,
                .dynamicRendering = VK_TRUE,
            };
            const VkDeviceCreateInfo createInfo{
                .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
                .pNext = &dynamicRenderingFeature,
                .queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size()),
                .pQueueCreateInfos = queueCreateInfos.data(),
                .enabledLayerCount = static_cast<uint32_t>(requestedLayers.size()),
                .ppEnabledLayerNames = requestedLayers.data(),
                .enabledExtensionCount = static_cast<uint32_t>(physicalDevice.getDeviceExtensions().size()),
                .ppEnabledExtensionNames = physicalDevice.getDeviceExtensions().data(),
                .pEnabledFeatures = VK_NULL_HANDLE,
            };
            if (vkCreateDevice(physicalDevice.getPhysicalDevice(), &createInfo, nullptr, &device) != VK_SUCCESS) {
                die("Failed to create logical device!");
            }
            vulkanInitializeDevice(device);
        }
    }

    VkImageView VKDevice::createImageView(const VkImage            image,
                                       const VkFormat           format,
                                       const VkImageAspectFlags aspectFlags,
                                       const uint32_t           mipLevels,
                                       const VkImageViewType    type,
                                       const uint32_t           baseArrayLayer,
                                       const uint32_t           layers,
                                       const uint32_t           baseMipLevel) const {
        // https://vulkan-tutorial.com/Drawing_a_triangle/Presentation/Image_views
        const VkImageViewCreateInfo viewInfo{
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image = image,
            .viewType = type,
            .format = format,
            .subresourceRange = {
                .aspectMask = aspectFlags,
                .baseMipLevel = baseMipLevel,
                .levelCount = mipLevels,
                .baseArrayLayer = baseArrayLayer,
                // Note : VK_REMAINING_ARRAY_LAYERS does not work for VK_IMAGE_VIEW_TYPE_2D_ARRAY
                // we have to specify the exact number of layers or texture() only read the first layer
                .layerCount = type == VK_IMAGE_VIEW_TYPE_CUBE ? VK_REMAINING_ARRAY_LAYERS : layers
            }
        };
        VkImageView imageView;
        if (vkCreateImageView(device, &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
            die("failed to create texture image view!");
        }
        return imageView;
    }


    VKDevice::~VKDevice() {
        vkDeviceWaitIdle(device);
        vkDestroyDevice(device, nullptr);
    }

    VKCommandQueue::VKCommandQueue(const VKDevice& device) {
        vkGetDeviceQueue(
            device.getDevice(),
            device.getGraphicsQueueFamilyIndex(),
            0,
            &commandQueue);
    }

    VKCommandQueue::~VKCommandQueue() {
        vkQueueWaitIdle(commandQueue);
    }

    VKSwapChain::VKSwapChain(const VKPhysicalDevice& physicalDevice, const VKDevice& device, uint32_t width, uint32_t height):
    device{device.getDevice()} {
        vkGetDeviceQueue(
            device.getDevice(),
            device.getPresentQueueFamilyIndex(),
            0,
            &presentQueue);

         // https://vulkan-tutorial.com/Drawing_a_triangle/Presentation/Swap_chain
        const SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice.getPhysicalDevice(), physicalDevice.getSurface());
        const VkSurfaceFormatKHR      surfaceFormat    = chooseSwapSurfaceFormat(swapChainSupport.formats);
        const VkPresentModeKHR        presentMode      = chooseSwapPresentMode(swapChainSupport.presentModes);
        swapChainExtent = chooseSwapExtent(swapChainSupport.capabilities, width, height);

        uint32_t imageCount = SwapChain::FRAMES_IN_FLIGHT + 1;
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
                // VK_IMAGE_USAGE_TRANSFER_DST_BIT for Blit or Revolve (see presentToSwapChain())
                .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT |
                VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
                .preTransform = swapChainSupport.capabilities.currentTransform,
                .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
                .presentMode = presentMode,
                .clipped = VK_TRUE
            };
            const VKPhysicalDevice::QueueFamilyIndices indices = physicalDevice.findQueueFamilies(physicalDevice.getPhysicalDevice());
            const uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};
            if (indices.graphicsFamily != indices.presentFamily) {
                createInfo.imageSharingMode      = VK_SHARING_MODE_CONCURRENT;
                createInfo.queueFamilyIndexCount = 2;
                createInfo.pQueueFamilyIndices   = queueFamilyIndices;
            } else {
                createInfo.imageSharingMode      = VK_SHARING_MODE_EXCLUSIVE;
                createInfo.queueFamilyIndexCount = 0; // Optional
                createInfo.pQueueFamilyIndices   = nullptr; // Optional
            }
            // Need VK_KHR_SWAPCHAIN extension, or it will crash (no validation error)
            if (vkCreateSwapchainKHR(device.getDevice(), &createInfo, nullptr, &swapChain) != VK_SUCCESS) {
                die("Failed to create Vulkan swap chain!");
            }
        }

        vkGetSwapchainImagesKHR(device.getDevice(), swapChain, &imageCount, nullptr);
        swapChainImages.resize(imageCount);
        vkGetSwapchainImagesKHR(device.getDevice(), swapChain, &imageCount, swapChainImages.data());
        swapChainImageFormat = surfaceFormat.format;
        SwapChain::extent      = Extent{ swapChainExtent.width, swapChainExtent.height };
        swapChainRatio       = static_cast<float>(swapChainExtent.width) / static_cast<float>(swapChainExtent.height);

        swapChainImageViews.resize(swapChainImages.size());
        for (uint32_t i = 0; i < swapChainImages.size(); i++) {
            swapChainImageViews[i] = device.createImageView(swapChainImages[i],
                                                     swapChainImageFormat,
                                                     VK_IMAGE_ASPECT_COLOR_BIT,
                                                     1);
        }

        // For bliting image to swapchain
        constexpr VkOffset3D vkOffset0{0, 0, 0};
        const VkOffset3D     vkOffset1{
            static_cast<int32_t>(swapChainExtent.width),
            static_cast<int32_t>(swapChainExtent.height),
            1,
        };
        colorImageBlit.srcOffsets[0]                 = vkOffset0;
        colorImageBlit.srcOffsets[1]                 = vkOffset1;
        colorImageBlit.srcSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
        colorImageBlit.srcSubresource.mipLevel       = 0;
        colorImageBlit.srcSubresource.baseArrayLayer = 0;
        colorImageBlit.srcSubresource.layerCount     = 1;
        colorImageBlit.dstOffsets[0]                 = vkOffset0;
        colorImageBlit.dstOffsets[1]                 = vkOffset1;
        colorImageBlit.dstSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
        colorImageBlit.dstSubresource.mipLevel       = 0;
        colorImageBlit.dstSubresource.baseArrayLayer = 0;
        colorImageBlit.dstSubresource.layerCount     = 1;
    }

    VKSwapChain::SwapChainSupportDetails VKSwapChain::querySwapChainSupport(
        const VkPhysicalDevice vkPhysicalDevice,
        VkSurfaceKHR surface) const {
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
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
                availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) { return availableFormat; }
        }
        return availableFormats[0];
    }

    VkPresentModeKHR VKSwapChain::chooseSwapPresentMode(const vector<VkPresentModeKHR> &availablePresentModes) {
        // https://vulkan-tutorial.com/Drawing_a_triangle/Presentation/Swap_chain#page_Presentation-mode
        const auto mode = VK_PRESENT_MODE_FIFO_KHR; //static_cast<VkPresentModeKHR>(app().getConfig().vSyncMode);
        for (const auto &availablePresentMode : availablePresentModes) {
            if (availablePresentMode == mode) {
                return availablePresentMode;
            }
        }
        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VkExtent2D VKSwapChain::chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities, uint32_t width, uint32_t height) const {
        // https://vulkan-tutorial.com/Drawing_a_triangle/Presentation/Swap_chain#page_Swap-extent
        // if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) { return capabilities.currentExtent; }
        VkExtent2D actualExtent{
            .width = width,
            .height = height
        };
        // actualExtent.width = std::max(
                // capabilities.minImageExtent.width,
                // std::min(capabilities.maxImageExtent.width, actualExtent.width));
        // actualExtent.height = std::max(
                // capabilities.minImageExtent.height,
                // std::min(capabilities.maxImageExtent.height, actualExtent.height));
        return actualExtent;
    }

    void VKSwapChain::nextSwapChain() {
        currentFrameIndex = (currentFrameIndex + 1) % SwapChain::FRAMES_IN_FLIGHT;
    }

    void VKSwapChain::present(const FrameData& frameData) {
        const auto& data = static_cast<const VKFrameData&>(frameData);
        const VkSwapchainKHR   swapChains[] = { swapChain };
        const VkPresentInfoKHR presentInfo{
            .sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
            .waitSemaphoreCount = 1,
            .pWaitSemaphores    = &data.renderFinishedSemaphore,
            .swapchainCount     = 1,
            .pSwapchains        = swapChains,
            .pImageIndices      = &data.imageIndex,
            .pResults           = nullptr // Optional
        };
        if (vkQueuePresentKHR(presentQueue, &presentInfo) != VK_SUCCESS) {
            die("failed to present swap chain image!");
        }
    }

    void VKSwapChain::prepare(FrameData& frameData) {
        auto& data = static_cast<VKFrameData&>(frameData);
        // get the next available swap chain image
        const auto result = vkAcquireNextImageKHR(
             device,
             swapChain,
             UINT64_MAX,
             data.imageAvailableSemaphore,
             VK_NULL_HANDLE,
             &data.imageIndex);
        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            die("not implemented");
            // recreateSwapChain();
            // for (const auto &renderer : renderers) { renderer->recreateImagesResources(); }
            // return;
        }
        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            die("failed to acquire swap chain image :", to_string(result));
        }
    }

    VKSwapChain::~VKSwapChain() {
        vkDeviceWaitIdle(device);
        // https://vulkan-tutorial.com/Drawing_a_triangle/Swap_chain_recreation#page_Recreating-the-swap-chain
        for (auto &swapChainImageView : swapChainImageViews) {
            vkDestroyImageView(device, swapChainImageView, nullptr);
        }
        vkDestroySwapchainKHR(device, swapChain, nullptr);
    }

}