/*
* Copyright (c) 2025-present Henri Michelon
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/
module;
#include "vireo/backend/vulkan/Libraries.h"
#ifdef _WIN32
#include <Windows.h>
#include <dxgi1_6.h>
#include <d3d12.h>
#endif
module vireo.vulkan.devices;

import vireo.tools;

import vireo.vulkan.tools;

namespace vireo {

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT,
                                                       VkDebugUtilsMessageTypeFlagsEXT,
                                                       const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                                                       void *) {
        const std::string message("validation layer: " + std::string(pCallbackData->pMessage));
        std::cerr << message << std::endl;
#ifdef _WIN32
        if (IsDebuggerPresent()) {
            OutputDebugStringA(message.c_str());
        }
#endif
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
            if (!layerFound) { throw Exception("A requested Vulkan layer is not supported"); }
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
            .apiVersion = VK_API_VERSION_1_3};
        const VkInstanceCreateInfo createInfo = {VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
                                                 nullptr,
                                                 0,
                                                 &applicationInfo,
                                                 static_cast<uint32_t>(requestedLayers.size()),
                                                 requestedLayers.data(),
                                                 static_cast<uint32_t>(instanceExtensions.size()),
                                                 instanceExtensions.data()};
        vkCheck(vkCreateInstance(&createInfo, nullptr, &instance));
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
        vkCheck(CreateDebugUtilsMessengerEXT(instance, &debugCreateInfo, nullptr, &debugMessenger));
#endif
    }

    VKInstance::~VKInstance() {
#ifdef _DEBUG
        DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
#endif
        vkDestroyInstance(instance, nullptr);
        vulkanFinalize();
    }

    VKPhysicalDevice::VKPhysicalDevice(const VkInstance instance):
        instance(instance),
        // Requested device extensions
        deviceExtensions {
            // Mandatory to create a swap chain
            VK_KHR_SWAPCHAIN_EXTENSION_NAME,
            // https://docs.vulkan.org/samples/latest/samples/extensions/dynamic_rendering/README.html
            VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME,
            // for Vulkan Memory Allocator
            VK_KHR_GET_MEMORY_REQUIREMENTS_2_EXTENSION_NAME,
            // Pipelines dynamic states for IA & viewports
            VK_EXT_EXTENDED_DYNAMIC_STATE_3_EXTENSION_NAME,
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
            throw Exception("Failed to find GPUs with Vulkan support");
        }

        // Use the better Vulkan physical device found
        // https://vulkan-tutorial.com/Drawing_a_triangle/Setup/Physical_devices_and_queue_families#page_Base-device-suitability-checks
        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());
        // Use an ordered map to automatically sort candidates by increasing score
        std::multimap<uint32_t, VkPhysicalDevice> candidates;
        for (const auto &dev : devices) {
            uint32_t score = rateDeviceSuitability(dev, deviceExtensions);
            candidates.insert(std::make_pair(score, dev));
        }
        // Check if the best candidate is suitable at all
        if (candidates.rbegin()->first > 0) {
            // Select the better suitable device found
            physicalDevice = candidates.rbegin()->second;
            deviceProperties.pNext = &physDeviceIDProps;
            vkGetPhysicalDeviceProperties2(physicalDevice, &deviceProperties);
            // Get the GPU description and total memory
            // getAdapterDescFromOS();
            vkGetPhysicalDeviceFeatures(physicalDevice, &deviceFeatures);
        } else {
            throw Exception("Failed to find a suitable GPU!");
        }
    }

     VKPhysicalDevice::QueueFamilyIndices VKPhysicalDevice::findQueueFamilies(const VkPhysicalDevice vkPhysicalDevice) {
        // https://vulkan-tutorial.com/Drawing_a_triangle/Setup/Physical_devices_and_queue_families#page_Queue-families
        QueueFamilyIndices indices;
        uint32_t           queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice, &queueFamilyCount, nullptr);
        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
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
            if (indices.isComplete()) {
                break;
            }
            i++;
        }
        return indices;
    }

    int32_t VKPhysicalDevice::findTransferQueueFamily() const {
        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
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
        return -1;
    }

    int32_t VKPhysicalDevice::findComputeQueueFamily() const {
        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
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
        return -1;
    }

    uint32_t VKPhysicalDevice::findMemoryType(const uint32_t typeFilter, const VkMemoryPropertyFlags properties) const {
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);
        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
            if ((typeFilter & (1 << i)) &&
                (memProperties.memoryTypes[i].propertyFlags & properties) == properties) { return i; }
        }
        throw Exception("failed to find suitable memory type!");
    }

    VkSampleCountFlagBits VKPhysicalDevice::getMaxUsableMSAASampleCount() const {
        // https://vulkan-tutorial.com/Multisampling#page_Getting-available-sample-count
        VkPhysicalDeviceProperties physicalDeviceProperties;
        vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);
        const VkSampleCountFlags counts =
            physicalDeviceProperties.limits.framebufferColorSampleCounts &
            physicalDeviceProperties.limits.framebufferDepthSampleCounts;
        if (counts & VK_SAMPLE_COUNT_64_BIT) { return VK_SAMPLE_COUNT_64_BIT; }
        if (counts & VK_SAMPLE_COUNT_32_BIT) { return VK_SAMPLE_COUNT_32_BIT; }
        if (counts & VK_SAMPLE_COUNT_16_BIT) { return VK_SAMPLE_COUNT_16_BIT; }
        if (counts & VK_SAMPLE_COUNT_8_BIT) { return VK_SAMPLE_COUNT_8_BIT; }
        if (counts & VK_SAMPLE_COUNT_4_BIT) { return VK_SAMPLE_COUNT_4_BIT; }
        if (counts & VK_SAMPLE_COUNT_2_BIT) { return VK_SAMPLE_COUNT_2_BIT; }
        return VK_SAMPLE_COUNT_1_BIT;
    }

    uint32_t VKPhysicalDevice::rateDeviceSuitability(
        const VkPhysicalDevice      vkPhysicalDevice,
        const std::vector<const char *> &deviceExtensions) {
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
        // The maximum possible size of textures affects graphics quality
        score += _deviceProperties.limits.maxImageDimension2D;
        // Application can't function without geometry shaders
        if (!deviceFeatures.geometryShader) {
            return 0;
        }

        const bool extensionsSupported = checkDeviceExtensionSupport(vkPhysicalDevice, deviceExtensions);
        // bool swapChainAdequate   = false;
        // if (extensionsSupported) {
            // SwapChainSupportDetails swapChainSupport = querySwapChainSupport(vkPhysicalDevice);
            // swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
        // }
        const QueueFamilyIndices indices = findQueueFamilies(vkPhysicalDevice);
        // if ((!extensionsSupported) || (!indices.isComplete()) || (!swapChainAdequate)) {
        if ((!extensionsSupported) || (!indices.isComplete())) {
            return 0;
        }
        return score;
    }

    bool VKPhysicalDevice::checkDeviceExtensionSupport(
        const VkPhysicalDevice            vkPhysicalDevice,
        const std::vector<const char *> &deviceExtensions) {
        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(vkPhysicalDevice, nullptr, &extensionCount, nullptr);
        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(vkPhysicalDevice,
                                             nullptr,
                                             &extensionCount,
                                             availableExtensions.data());
        std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());
        for (const auto &extension : availableExtensions) { requiredExtensions.erase(extension.extensionName); }
        return requiredExtensions.empty();
    }

    // https://dev.to/reg__/there-is-a-way-to-query-gpu-memory-usage-in-vulkan---use-dxgi-1f0d
    const PhysicalDeviceDesc VKPhysicalDevice::getDescription() const {
        PhysicalDeviceDesc result;
        IDXGIFactory4 *dxgiFactory{nullptr};
        if (FAILED(CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory)))) { return result; }
        IDXGIAdapter1 *tmpDxgiAdapter{nullptr};
        UINT           adapterIndex{0};
        IDXGIAdapter3 *dxgiAdapter{nullptr};
        while (dxgiFactory->EnumAdapters1(adapterIndex, &tmpDxgiAdapter) != DXGI_ERROR_NOT_FOUND) {
            DXGI_ADAPTER_DESC1 desc;
            if (FAILED(tmpDxgiAdapter->GetDesc1(&desc))) { return result; }
            if (memcmp(&desc.AdapterLuid, physDeviceIDProps.deviceLUID, VK_LUID_SIZE) == 0) {
                if (FAILED(tmpDxgiAdapter->QueryInterface(IID_PPV_ARGS(&dxgiAdapter)))) { return result; }
                result.name = std::to_string(desc.Description);
                result.dedicatedVideoMemory = desc.DedicatedVideoMemory;
                result.dedicatedSystemMemory = desc.DedicatedSystemMemory;
                result.sharedSystemMemory = desc.SharedSystemMemory;
            }
            tmpDxgiAdapter->Release();
            ++adapterIndex;
        }
        dxgiFactory->Release();
        return result;
    }

    VKDevice::VKDevice(const VKPhysicalDevice& physicalDevice, const std::vector<const char *>& requestedLayers):
        physicalDevice{physicalDevice} {
         /// Select command queues
        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        constexpr auto queuePriority = std::array{1.0f};
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
        // Use a compute command queue if different from the graphical queue
        computeQueueFamilyIndex = physicalDevice.findComputeQueueFamily();
        if (computeQueueFamilyIndex != -1) {
            if (computeQueueFamilyIndex != graphicsQueueFamilyIndex) {
                const VkDeviceQueueCreateInfo queueCreateInfo{
                    .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                    .queueFamilyIndex = computeQueueFamilyIndex,
                    .queueCount = 1,
                    .pQueuePriorities = queuePriority.data(),
                };
                queueCreateInfos.push_back(queueCreateInfo);
            }
        } else {
            computeQueueFamilyIndex = graphicsQueueFamilyIndex;
        }
        // Use a dedicated transfer queue for DMA transfers
        transferQueueFamilyIndex = physicalDevice.findTransferQueueFamily();
        if (transferQueueFamilyIndex != -1) {
            const VkDeviceQueueCreateInfo queueCreateInfo{
                .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                .queueFamilyIndex = transferQueueFamilyIndex,
                .queueCount = 1,
                .pQueuePriorities = queuePriority.data(),
            };
            queueCreateInfos.push_back(queueCreateInfo);
        } else {
            transferQueueFamilyIndex = graphicsQueueFamilyIndex;
        }

        // Initialize device extensions and create a logical device
        {
            VkPhysicalDeviceSynchronization2FeaturesKHR sync2Features{
                .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES,
                .pNext = nullptr,
                .synchronization2 = VK_TRUE
            };
            VkPhysicalDeviceFeatures2 deviceFeatures2 {
                .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2,
                .pNext = &sync2Features,
                .features = {
                    .independentBlend = VK_TRUE,
                    .multiDrawIndirect = VK_TRUE,
                    .fillModeNonSolid = VK_TRUE,
                    .samplerAnisotropy = VK_TRUE,
                    .vertexPipelineStoresAndAtomics = VK_TRUE,
                }
            };
            VkPhysicalDeviceVulkan11Features deviceVulkan11Features {
                .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES,
                .pNext = &deviceFeatures2,
                .shaderDrawParameters = VK_TRUE,
            };
            VkPhysicalDeviceVulkan12Features deviceVulkan12Features {
                .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES,
                .pNext = &deviceVulkan11Features,
                .drawIndirectCount = VK_TRUE,
                .runtimeDescriptorArray = VK_TRUE,
                .timelineSemaphore = VK_TRUE,
            };
            const VkPhysicalDeviceDynamicRenderingFeaturesKHR dynamicRenderingFeature{
                .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR,
                .pNext = &deviceVulkan12Features,
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
            vkCheck(vkCreateDevice(physicalDevice.getPhysicalDevice(), &createInfo, nullptr, &device));
#ifdef _DEBUG
            vkSetObjectName(device, reinterpret_cast<uint64_t>(device), VK_OBJECT_TYPE_DEVICE, "VKDevice");
#endif
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
        vkCheck(vkCreateImageView(device, &viewInfo, nullptr, &imageView));
        return imageView;
    }

    VKDevice::~VKDevice() {
        vkDestroyDevice(device, nullptr);
    }


}