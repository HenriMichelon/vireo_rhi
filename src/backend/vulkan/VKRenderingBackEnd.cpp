/*
* Copyright (c) 2025-present Henri Michelon
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/
module;
#include "backend/vulkan/Tools.h"

module vireo.backend.vulkan;

import vireo.app.win32;
import vireo.backend.vulkan.framedata;

namespace vireo::backend {

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

    VKRenderingBackEnd::VKRenderingBackEnd() {
        instance = std::make_shared<VKInstance>();
        physicalDevice = std::make_shared<VKPhysicalDevice>(getVKInstance()->getInstance());
        device = std::make_shared<VKDevice>(*getVKPhysicalDevice(), getVKInstance()->getRequestedLayers());
        graphicCommandQueue = std::make_shared<VKSubmitQueue>(CommandList::GRAPHIC, *getVKDevice());
        transferCommandQueue = std::make_shared<VKSubmitQueue>(CommandList::TRANSFER, *getVKDevice());
        swapChain = std::make_shared<VKSwapChain>(*getVKPhysicalDevice(), *getVKDevice());
    }

    void VKRenderingBackEnd::destroyFrameData(FrameData& frameData) {
        const auto& data = static_cast<VKFrameData&>(frameData);
        vkDestroySemaphore(getVKDevice()->getDevice(), data.imageAvailableSemaphore, nullptr);
        vkDestroySemaphore(getVKDevice()->getDevice(), data.renderFinishedSemaphore, nullptr);
        vkDestroyFence(getVKDevice()->getDevice(), data.inFlightFence, nullptr);
    }

    std::shared_ptr<FrameData> VKRenderingBackEnd::createFrameData(const uint32_t frameIndex) {
        auto data = std::make_shared<VKFrameData>();
        constexpr VkSemaphoreCreateInfo semaphoreInfo{
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO
        };
        constexpr VkFenceCreateInfo fenceInfo{
            .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
            .flags = VK_FENCE_CREATE_SIGNALED_BIT
        };
        if (vkCreateSemaphore(getVKDevice()->getDevice(), &semaphoreInfo, nullptr, &data->imageAvailableSemaphore) != VK_SUCCESS
            || vkCreateSemaphore(getVKDevice()->getDevice(), &semaphoreInfo, nullptr, &data->renderFinishedSemaphore) != VK_SUCCESS
            || vkCreateFence(getVKDevice()->getDevice(), &fenceInfo, nullptr, &data->inFlightFence) != VK_SUCCESS) {
            die("failed to create semaphores!");
        }
        data->imageAvailableSemaphoreSubmitInfo = {
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
            .semaphore = data->imageAvailableSemaphore,
            .value = 1,
            .stageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR,
            .deviceIndex = 0
        };
        data->renderFinishedSemaphoreSubmitInfo = {
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
            .semaphore = data->renderFinishedSemaphore,
            .value = 1,
            .stageMask = VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT,
            .deviceIndex = 0
        };
        return data;
    }

    std::shared_ptr<VertexInputLayout> VKRenderingBackEnd::createVertexLayout(
           size_t size,
           const std::vector<VertexInputLayout::AttributeDescription>& attributesDescriptions) const {
        return make_shared<VKVertexInputLayout>(size, attributesDescriptions);
    }

    std::shared_ptr<CommandAllocator> VKRenderingBackEnd::createCommandAllocator(CommandList::Type type) const {
        return std::make_shared<VKCommandAllocator>(type, *getVKDevice());
    }

    std::shared_ptr<ShaderModule> VKRenderingBackEnd::createShaderModule(const std::string& fileName) const {
        return std::make_shared<VKShaderModule>(getVKDevice()->getDevice(), fileName);
    }

    std::shared_ptr<PipelineResources> VKRenderingBackEnd::createPipelineResources() const {
        return std::make_shared<VKPipelineResources>(getVKDevice()->getDevice());
    }

    std::shared_ptr<Pipeline> VKRenderingBackEnd::createPipeline(
        PipelineResources& pipelineResources,
        VertexInputLayout& vertexInputLayout,
        ShaderModule& vertexShader,
        ShaderModule& fragmentShader,
        const std::wstring& name) const {
        return make_shared<VKPipeline>(
            getVKDevice()->getDevice(),
            *getVKSwapChain(),
            pipelineResources,
            vertexInputLayout,
            vertexShader,
            fragmentShader,
            name
        );
    }

    std::shared_ptr<Buffer> VKRenderingBackEnd::createBuffer(Buffer::Type type, size_t size, size_t count, const size_t alignment, const std::wstring& name) const  {
        return make_shared<VKBuffer>(
           *getVKDevice(),Buffer::VERTEX,
           size, count, alignment,
           name);
    }

    void VKRenderingBackEnd::beginRendering(FrameData& frameData, PipelineResources& pipelineResources, Pipeline& pipeline, CommandList& commandList) {
        const auto& data = static_cast<VKFrameData&>(frameData);
        const auto& vkCommandList = static_cast<VKCommandList&>(commandList);
        const auto& swapChain = *getVKSwapChain();

        vkCommandList.pipelineBarrier(
            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            {VKCommandList::imageMemoryBarrier(swapChain.getImages()[data.imageIndex],
                    0, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                    VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)});

        const auto vkClearColor = VkClearValue{{
            clearColor.r, clearColor.g, clearColor.b, clearColor.a
        }};
        const VkExtent2D extent = {swapChain.getExtent().width, swapChain.getExtent().height};

        const auto colorAttachmentInfo = VkRenderingAttachmentInfo {
            .sType              = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR,
            .imageView          = swapChain.getImageViews()[data.imageIndex],
            .imageLayout        = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            .resolveMode        = VK_RESOLVE_MODE_NONE,
            .loadOp             = VK_ATTACHMENT_LOAD_OP_CLEAR,
            .storeOp            = VK_ATTACHMENT_STORE_OP_STORE,
            .clearValue         = vkClearColor,
        };
        const auto renderingInfo = VkRenderingInfo {
            .sType               = VK_STRUCTURE_TYPE_RENDERING_INFO_KHR,
            .pNext                = nullptr,
            .renderArea           = {
                {0, 0},
                extent
            },
            .layerCount           = 1,
            .colorAttachmentCount = 1,
            .pColorAttachments    = &colorAttachmentInfo,
            .pDepthAttachment     = nullptr,
            .pStencilAttachment   = nullptr
        };
        const auto& commandBuffer = vkCommandList.getCommandBuffer();
        vkCmdBeginRendering(commandBuffer, &renderingInfo);

        const VkViewport viewport{
            .x = 0.0f,
            .y = static_cast<float>(extent.height),
            .width = static_cast<float>(extent.width),
            .height = -static_cast<float>(extent.height),
            .minDepth = 0.0f,
            .maxDepth = 1.0f
        };
        const VkRect2D scissor{
            .offset = {0, 0},
            .extent = extent
        };
        vkCmdSetViewportWithCount(commandBuffer, 1, &viewport);
        vkCmdSetScissorWithCount(commandBuffer, 1, &scissor);
        vkCmdSetCullMode(commandBuffer, VK_CULL_MODE_NONE);
    }

    void VKRenderingBackEnd::endRendering(CommandList& commandList) {
        const auto& vkCommandList = static_cast<VKCommandList&>(commandList);
        vkCmdEndRendering(vkCommandList.getCommandBuffer());
    }

    void VKRenderingBackEnd::waitIdle() {
        vkDeviceWaitIdle(getVKDevice()->getDevice());
    }

    VKBuffer::VKBuffer(
            const VKDevice& device,
            const Type type,
            const size_t size,
            const size_t count,
            const size_t minOffsetAlignment,
            const std::wstring& name) : Buffer{type},device{device} {
        alignmentSize = minOffsetAlignment > 0
               ? (size + minOffsetAlignment - 1) & ~(minOffsetAlignment - 1)
               : size;
        bufferSize = alignmentSize * count;

        const VkBufferCreateFlags usage =
            type == VERTEX ? VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT :
            type == INDEX ? VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT:
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        const auto memTypeIndex = (type == VERTEX || type == INDEX) ?
            device.getPhysicalDevice().getMemoryTypeDeviceLocalIndex() :
            device.getPhysicalDevice().getMemoryTypeHostVisibleIndex();
        createBuffer(device, bufferSize, usage, memTypeIndex, buffer, bufferMemory);
        vkSetObjectName(device.getDevice(), reinterpret_cast<uint64_t>(buffer), VK_OBJECT_TYPE_BUFFER, wstring_to_string(name));
    }

    void VKBuffer::map() {
        vkMapMemory(device.getDevice(), bufferMemory, 0, bufferSize, 0, &mappedAddress);
    }

    void VKBuffer::unmap() {
        vkUnmapMemory(device.getDevice(), bufferMemory);
        mappedAddress = nullptr;
    }

    void VKBuffer::write(const void* data, const size_t size, const size_t offset) {
        if (size == WHOLE_SIZE) {
            memcpy(mappedAddress, data, bufferSize);
        } else {
            memcpy(static_cast<unsigned char*>(mappedAddress) + offset, data, size);
        }
    }

    void VKBuffer::createBuffer(
            const VKDevice& device,
            const VkDeviceSize size,
            const VkBufferUsageFlags usage,
            const uint32_t memoryTypeIndex,
            VkBuffer& buffer,
            VkDeviceMemory& memory) {
        const auto bufferInfo = VkBufferCreateInfo {
            .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            .size = size,
            .usage = usage,
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        };
        DieIfFailed (vkCreateBuffer(device.getDevice(), &bufferInfo, nullptr, &buffer));
        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(device.getDevice(), buffer, &memRequirements);
        const auto allocInfo = VkMemoryAllocateInfo {
            .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
            .allocationSize = memRequirements.size,
            .memoryTypeIndex = memoryTypeIndex
        };
        DieIfFailed(vkAllocateMemory(device.getDevice(), &allocInfo, nullptr, &memory));
        vkBindBufferMemory(device.getDevice(), buffer, memory, 0);
    }

    VKBuffer::~VKBuffer() {
        vkDestroyBuffer(device.getDevice(), buffer, nullptr);
        vkFreeMemory(device.getDevice(), bufferMemory, nullptr);
    }

    VKVertexInputLayout::VKVertexInputLayout(size_t size, const std::vector<AttributeDescription>& attributesDescriptions) {
        vertexBindingDescription.binding = 0;
        vertexBindingDescription.stride = size;
        vertexBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        for (int i = 0; i < attributesDescriptions.size(); i++) {
            vertexAttributeDescriptions.push_back({
                .location = static_cast<uint32_t>(i),
                .binding = 0,
                .format = VKFormat[attributesDescriptions[i].format],
                .offset = attributesDescriptions[i].offset,
            });
        }
    }

    VKShaderModule::VKShaderModule(const VkDevice device, const std::string& fileName):
        device{device} {
        std::ifstream file(fileName + ".spv", std::ios::ate | std::ios::binary);
        if (!file.is_open()) {
            die("failed to open shader file!");
        }
        const auto fileSize = static_cast<size_t>(file.tellg());
        std::vector<char> buffer(fileSize);
        file.seekg(0);
        file.read(buffer.data(), fileSize);
        file.close();
        const auto createInfo = VkShaderModuleCreateInfo {
            .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
            .codeSize = buffer.size(),
            .pCode = reinterpret_cast<const uint32_t*>(buffer.data())
        };
        DieIfFailed(vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule));
    }

    VKShaderModule::~VKShaderModule() {
        vkDestroyShaderModule(device, shaderModule, nullptr);
    }

    VKPipelineResources::VKPipelineResources(VkDevice device):
        device{device} {
        auto pipelineLayoutInfo = VkPipelineLayoutCreateInfo {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            .setLayoutCount = 0,
            .pSetLayouts = nullptr,
            .pushConstantRangeCount = 0,
            .pPushConstantRanges = nullptr,
        };
        DieIfFailed(vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout));
    }

    VKPipelineResources::~VKPipelineResources() {
        vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
    }

    VKPipeline::VKPipeline(
           VkDevice device,
           VKSwapChain& swapChain,
           PipelineResources& pipelineResources,
           VertexInputLayout& vertexInputLayout,
           ShaderModule& vertexShader,
           ShaderModule& fragmentShader,
           const std::wstring& name):
        device{device} {
        const auto vertexShaderModule = static_cast<VKShaderModule&>(vertexShader).getShaderModule();
        const auto fragmentShaderModule = static_cast<VKShaderModule&>(fragmentShader).getShaderModule();
        const auto& vkVertexInputLayout = static_cast<VKVertexInputLayout&>(vertexInputLayout);
        const auto& vkPipelineLayout = static_cast<VKPipelineResources&>(pipelineResources);

        const auto shaderStages = std::vector<VkPipelineShaderStageCreateInfo> {
            {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                .stage = VK_SHADER_STAGE_VERTEX_BIT,
                .module = vertexShaderModule,
                .pName = "VSMain"
            },
            {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
                .module = fragmentShaderModule,
                .pName = "PSMain"
            }
        };
        const auto vertexInputInfo = VkPipelineVertexInputStateCreateInfo {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
            .vertexBindingDescriptionCount = 1,
            .pVertexBindingDescriptions = &vkVertexInputLayout.getVertexBindingDescription(),
            .vertexAttributeDescriptionCount = static_cast<uint32_t>(vkVertexInputLayout.getVertexAttributeDescription().size()),
            .pVertexAttributeDescriptions = vkVertexInputLayout.getVertexAttributeDescription().data()
        };
        constexpr auto inputAssembly = VkPipelineInputAssemblyStateCreateInfo {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
            .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
            .primitiveRestartEnable = VK_FALSE,
        };

        const std::vector dynamicStates = {
            VK_DYNAMIC_STATE_VIEWPORT_WITH_COUNT,
            VK_DYNAMIC_STATE_SCISSOR_WITH_COUNT,
            VK_DYNAMIC_STATE_CULL_MODE
        };
        const auto dynamicState = VkPipelineDynamicStateCreateInfo {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
            .dynamicStateCount = static_cast<uint32_t>(dynamicStates.size()),
            .pDynamicStates = dynamicStates.data()
        };
        constexpr  auto viewportState = VkPipelineViewportStateCreateInfo {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        };
        constexpr auto rasterizer = VkPipelineRasterizationStateCreateInfo {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
            .depthClampEnable = VK_FALSE,
            .rasterizerDiscardEnable = VK_FALSE,
            .polygonMode = VK_POLYGON_MODE_FILL,
            .frontFace = VK_FRONT_FACE_CLOCKWISE,
            .depthBiasEnable = VK_FALSE,
            .depthBiasConstantFactor = 0.0f,
            .depthBiasClamp = 0.0f,
            .depthBiasSlopeFactor = 0.0f,
            .lineWidth = 1.0f,
        };
        constexpr auto multisampling = VkPipelineMultisampleStateCreateInfo {
            .sType                  = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
            .rasterizationSamples   = VK_SAMPLE_COUNT_1_BIT,
            .sampleShadingEnable    = VK_FALSE,
            .minSampleShading       = 1.0f,
            .pSampleMask            = nullptr,
            .alphaToCoverageEnable  = VK_FALSE,
            .alphaToOneEnable       = VK_FALSE
        };
        constexpr auto colorBlendAttachment = VkPipelineColorBlendAttachmentState {
            .blendEnable = VK_FALSE,
            // .srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
            // .dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
            // .colorBlendOp = VK_BLEND_OP_ADD,
            // .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
            // .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
            // .alphaBlendOp = VK_BLEND_OP_ADD,
            .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
        };
        const auto colorBlending = VkPipelineColorBlendStateCreateInfo {
            .sType          = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
            .logicOpEnable  = VK_FALSE,
            .logicOp        = VK_LOGIC_OP_COPY,
            .attachmentCount= 1,
            .pAttachments   = &colorBlendAttachment,
            .blendConstants = { 0.0f, 0.0f, 0.0f, 0.0f }
        };

        const auto swapChainImageFormat = swapChain.getFormat();
        const auto dynamicRenderingCreateInfo = VkPipelineRenderingCreateInfoKHR{
            .sType                   = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR,
            .pNext                   = VK_NULL_HANDLE,
            .colorAttachmentCount    = 1,
            .pColorAttachmentFormats = &swapChainImageFormat,
        };

        const auto pipelineInfo = VkGraphicsPipelineCreateInfo {
            .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
            .pNext = &dynamicRenderingCreateInfo,
            .stageCount = static_cast<uint32_t>(shaderStages.size()),
            .pStages = shaderStages.data(),
            .pVertexInputState = &vertexInputInfo,
            .pInputAssemblyState = &inputAssembly,
            .pViewportState = &viewportState,
            .pRasterizationState = &rasterizer,
            .pMultisampleState = &multisampling,
            .pDepthStencilState = nullptr,
            .pColorBlendState = &colorBlending,
            .pDynamicState = &dynamicState,
            .layout = vkPipelineLayout.getPipelineLayout(),
            .renderPass = VK_NULL_HANDLE,
            .subpass = 0,
            .basePipelineHandle = VK_NULL_HANDLE,
            .basePipelineIndex = -1,
        };
        DieIfFailed(vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline));
        vkSetObjectName(device, reinterpret_cast<uint64_t>(pipeline), VK_OBJECT_TYPE_PIPELINE, wstring_to_string(name));
    }

    VKPipeline::~VKPipeline() {
        vkDestroyPipeline(device, pipeline, nullptr);
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
        DieIfFailed(vkCreateInstance(&createInfo, nullptr, &instance));
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
        DieIfFailed(CreateDebugUtilsMessengerEXT(instance, &debugCreateInfo, nullptr, &debugMessenger));
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
        DieIfFailed(vkCreateWin32SurfaceKHR(instance, &surfaceCreateInfo, nullptr, &surface));
#endif


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

        {
            VkPhysicalDeviceMemoryProperties memoryProperties{};
            vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);
            for (int i = 0; i < memoryProperties.memoryTypeCount; i++) {
                if ((memoryProperties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) ==
                    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) {
                    memoryTypeDeviceLocalIndex = i;
                } else if ((memoryProperties.memoryTypes[i].propertyFlags & (VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)) ==
                    (VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)) {
                    memoryTypeHostVisibleIndex = i;
                }
            }
        }
    }

     VKPhysicalDevice::QueueFamilyIndices VKPhysicalDevice::findQueueFamilies(const VkPhysicalDevice vkPhysicalDevice) const {
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
        die("Could not find dedicated transfer queue family");
        return i;
    }

    uint32_t VKPhysicalDevice::findComputeQueueFamily() const {
        uint32_t           queueFamilyCount = 0;
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
        die("Could not find dedicated compute queue family");
        return i;
    }

    VKPhysicalDevice::~VKPhysicalDevice() {
        vkDestroySurfaceKHR(instance, surface, nullptr);
    }

    uint32_t VKPhysicalDevice::rateDeviceSuitability(
        VkPhysicalDevice            vkPhysicalDevice,
        const std::vector<const char *> &deviceExtensions) const {
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
        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(vkPhysicalDevice,
                                             nullptr,
                                             &extensionCount,
                                             availableExtensions.data());
        std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());
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

            // https://lesleylai.info/en/vk-khr-dynamic-rendering/
            const VkPhysicalDeviceDynamicRenderingFeaturesKHR dynamicRenderingFeature{
                .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR,
                .pNext = &deviceFeatures2,
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
            DieIfFailed(vkCreateDevice(physicalDevice.getPhysicalDevice(), &createInfo, nullptr, &device));
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
        DieIfFailed(vkCreateImageView(device, &viewInfo, nullptr, &imageView));
        return imageView;
    }

    VkImageMemoryBarrier VKDevice::imageMemoryBarrier(
          const VkImage image,
          const VkAccessFlags srcAccessMask,
          const VkAccessFlags dstAccessMask,
          const VkImageLayout oldLayout,
          const VkImageLayout newLayout,
          const uint32_t baseMipLevel,
          const uint32_t levelCount
      ) {
        return VkImageMemoryBarrier {
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
            .srcAccessMask =  srcAccessMask,
            .dstAccessMask = dstAccessMask,
            .oldLayout = oldLayout,
            .newLayout = newLayout,
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .image = image,
            .subresourceRange = {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel = baseMipLevel,
                .levelCount = levelCount,
                .baseArrayLayer = 0,
                .layerCount = VK_REMAINING_ARRAY_LAYERS,
            }
        };
    }

    VKDevice::~VKDevice() {
        vkDestroyDevice(device, nullptr);
    }

    VKSubmitQueue::VKSubmitQueue(const CommandList::Type type, const VKDevice& device) {
        vkGetDeviceQueue(
            device.getDevice(),
            type == CommandList::COMPUTE ? device.getComputeQueueFamilyIndex() :
            type == CommandList::TRANSFER ?  device.getTransferQueueFamilyIndex() :
            device.getGraphicsQueueFamilyIndex(),
            0,
            &commandQueue);
    }

    void VKSubmitQueue::waitIdle() {
        vkQueueWaitIdle(commandQueue);
    }

    void VKSubmitQueue::submit(const FrameData& frameData, std::vector<std::shared_ptr<CommandList>> commandLists) {
        auto& data = static_cast<const VKFrameData&>(frameData);
        std::vector<VkCommandBufferSubmitInfo> submitInfos(commandLists.size());
        for (int i = 0; i < commandLists.size(); i++) {
            submitInfos[i] = {
                .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO,
                .commandBuffer = static_pointer_cast<VKCommandList>(commandLists[i])->getCommandBuffer(),
            };
        }
        const VkSubmitInfo2 submitInfo {
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2,
            .waitSemaphoreInfoCount = 1,
            .pWaitSemaphoreInfos = &data.imageAvailableSemaphoreSubmitInfo,
            .commandBufferInfoCount = static_cast<uint32_t>(submitInfos.size()),
            .pCommandBufferInfos = submitInfos.data(),
            .signalSemaphoreInfoCount = 1,
            .pSignalSemaphoreInfos = &data.renderFinishedSemaphoreSubmitInfo
        };
        DieIfFailed(vkQueueSubmit2(commandQueue, 1, &submitInfo, data.inFlightFence));
    }

    void VKSubmitQueue::submit(std::vector<std::shared_ptr<CommandList>> commandLists) {
        std::vector<VkCommandBufferSubmitInfo> submitInfos(commandLists.size());
        for (int i = 0; i < commandLists.size(); i++) {
            submitInfos[i] = {
                .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO,
                .commandBuffer = static_pointer_cast<VKCommandList>(commandLists[i])->getCommandBuffer(),
            };
        }
        const VkSubmitInfo2 submitInfo {
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2,
            .waitSemaphoreInfoCount = 0,
            .commandBufferInfoCount = static_cast<uint32_t>(submitInfos.size()),
            .pCommandBufferInfos = submitInfos.data(),
            .signalSemaphoreInfoCount = 0,
        };
        DieIfFailed(vkQueueSubmit2(commandQueue, 1, &submitInfo, VK_NULL_HANDLE));
    }

    VKCommandAllocator::VKCommandAllocator(const CommandList::Type type, const VKDevice& device):
        CommandAllocator{type},
        device{device} {
        const VkCommandPoolCreateInfo poolInfo           = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT, // TODO optional
            .queueFamilyIndex = type == CommandList::COMPUTE ? device.getComputeQueueFamilyIndex() :
                type == CommandList::TRANSFER ?  device.getTransferQueueFamilyIndex() :
                 device.getGraphicsQueueFamilyIndex()
        };
        DieIfFailed(vkCreateCommandPool(device.getDevice(), &poolInfo, nullptr, &commandPool));
    }

    VKCommandAllocator::~VKCommandAllocator() {
        vkDestroyCommandPool(device.getDevice(), commandPool, nullptr);
    }

    std::shared_ptr<CommandList> VKCommandAllocator::createCommandList(Pipeline& pipeline) const {
        return createCommandList();
    }

    std::shared_ptr<CommandList> VKCommandAllocator::createCommandList() const {
        return std::make_shared<VKCommandList>(device, commandPool);
    }

    VKCommandList::VKCommandList(const VKDevice& device, const VkCommandPool commandPool) : device{device} {
        const VkCommandBufferAllocateInfo allocInfo{
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .commandPool = commandPool,
            .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = 1
        };
        DieIfFailed(vkAllocateCommandBuffers(device.getDevice(), &allocInfo, &commandBuffer));
    }

    void VKCommandList::bindVertexBuffer(Buffer& buffer) {
        const auto& vkBuffer = static_cast<VKBuffer&>(buffer);
        const VkBuffer         buffers[] = {vkBuffer.getBuffer()};
        constexpr VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
    }

    void VKCommandList::drawInstanced(const uint32_t vertexCountPerInstance, const uint32_t instanceCount) {
        vkCmdDraw(commandBuffer, vertexCountPerInstance, instanceCount, 0, 0);
    }

    VKCommandList::~VKCommandList() {
    }

    void VKCommandList::reset() {
        vkResetCommandBuffer(commandBuffer, 0);
    }

    void VKCommandList::begin(Pipeline& pipeline) {
        constexpr VkCommandBufferBeginInfo beginInfo{
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        };
        DieIfFailed(vkBeginCommandBuffer(commandBuffer, &beginInfo));
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS , static_cast<VKPipeline&>(pipeline).getPipeline());
    }

    void VKCommandList::begin() {
        constexpr VkCommandBufferBeginInfo beginInfo{
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        };
        vkResetCommandBuffer(commandBuffer, 0);
        DieIfFailed(vkBeginCommandBuffer(commandBuffer, &beginInfo));
    }

    void VKCommandList::end() {
        DieIfFailed(vkEndCommandBuffer(commandBuffer));
    }


    void VKCommandList::cleanup() {
        for (int i = 0; i < stagingBuffers.size(); i++) {
            vkDestroyBuffer(device.getDevice(), stagingBuffers[i], nullptr);
            vkFreeMemory(device.getDevice(), stagingBuffersMemory[i], nullptr);
        }
        stagingBuffers.clear();
        stagingBuffersMemory.clear();
    }

    void VKCommandList::upload(Buffer& destination, const void* source) {
        const auto& buffer = static_cast<VKBuffer&>(destination);
        VkBuffer       stagingBuffer{VK_NULL_HANDLE};
        VkDeviceMemory stagingBufferMemory{VK_NULL_HANDLE};
        VKBuffer::createBuffer(
            device,
            buffer.getSize(),
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            device.getPhysicalDevice().getMemoryTypeHostVisibleIndex(),
            stagingBuffer, stagingBufferMemory);

        void* stagingData;
        vkMapMemory(device.getDevice(), stagingBufferMemory, 0, buffer.getSize(), 0, &stagingData);
        memcpy(stagingData, source, buffer.getSize());
        vkUnmapMemory(device.getDevice(), stagingBufferMemory);

        const auto copyRegion = VkBufferCopy{
            .srcOffset = 0,
            .dstOffset = 0,
            .size = buffer.getSize(),
        };
        vkCmdCopyBuffer(
            commandBuffer,
            stagingBuffer,
            buffer.getBuffer(),
            1,
            &copyRegion);

        stagingBuffers.push_back(stagingBuffer);
        stagingBuffersMemory.push_back(stagingBufferMemory);
    }

    void VKCommandList::pipelineBarrier(
       const VkPipelineStageFlags srcStageMask,
       const VkPipelineStageFlags dstStageMask,
       const std::vector<VkImageMemoryBarrier>& barriers) const {
        vkCmdPipelineBarrier(commandBuffer,
            srcStageMask,
            dstStageMask,
            0,
            0,
            nullptr,
            0,
            nullptr,
            static_cast<uint32_t>(barriers.size()),
            barriers.data());
    }

    VkImageMemoryBarrier VKCommandList::imageMemoryBarrier(
           const VkImage image,
           const VkAccessFlags srcAccessMask, const VkAccessFlags dstAccessMask,
           const VkImageLayout oldLayout, const VkImageLayout newLayout,
           const uint32_t baseMipLevel,
           const uint32_t levelCount
       ) {
        return VkImageMemoryBarrier {
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
            .srcAccessMask =  srcAccessMask,
            .dstAccessMask = dstAccessMask,
            .oldLayout = oldLayout,
            .newLayout = newLayout,
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .image = image,
            .subresourceRange = {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel = baseMipLevel,
                .levelCount = levelCount,
                .baseArrayLayer = 0,
                .layerCount = VK_REMAINING_ARRAY_LAYERS,
            }
        };
    }

    VKSwapChain::VKSwapChain(const VKPhysicalDevice& physicalDevice, const VKDevice& device):
        device{device},
        physicalDevice{physicalDevice},
        surface{physicalDevice.getSurface()} {
        vkGetDeviceQueue(
            device.getDevice(),
            device.getPresentQueueFamilyIndex(),
            0,
            &presentQueue);
        create();
    }

    void VKSwapChain::create() {
        // https://vulkan-tutorial.com/Drawing_a_triangle/Presentation/Swap_chain
        const SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice.getPhysicalDevice(), physicalDevice.getSurface());
        const VkSurfaceFormatKHR surfaceFormat= chooseSwapSurfaceFormat(swapChainSupport.formats);
        const VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
        swapChainExtent = chooseSwapExtent(swapChainSupport.capabilities);

        uint32_t imageCount = FRAMES_IN_FLIGHT + 1;
        if (swapChainSupport.capabilities.maxImageCount > 0 &&
            imageCount > swapChainSupport.capabilities.maxImageCount) {
            imageCount = swapChainSupport.capabilities.maxImageCount;
        }

        {
            VkSwapchainCreateInfoKHR createInfo = {
                .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
                .surface = surface,
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
            if (device.getPresentQueueFamilyIndex() != device.getGraphicsQueueFamilyIndex()) {
                const uint32_t queueFamilyIndices[] = {device.getPresentQueueFamilyIndex(), device.getGraphicsQueueFamilyIndex()};
                createInfo.imageSharingMode      = VK_SHARING_MODE_CONCURRENT;
                createInfo.queueFamilyIndexCount = 2;
                createInfo.pQueueFamilyIndices   = queueFamilyIndices;
            } else {
                createInfo.imageSharingMode      = VK_SHARING_MODE_EXCLUSIVE;
                createInfo.queueFamilyIndexCount = 0;
                createInfo.pQueueFamilyIndices   = nullptr;
            }
            // Need VK_KHR_SWAPCHAIN extension, or it will crash (no validation error)
            DieIfFailed(vkCreateSwapchainKHR(device.getDevice(), &createInfo, nullptr, &swapChain));
        }
        vkGetSwapchainImagesKHR(device.getDevice(), swapChain, &imageCount, nullptr);
        swapChainImages.resize(imageCount);
        swapChainImageViews.resize(swapChainImages.size());
        swapChainImageFormat = surfaceFormat.format;

        vkGetSwapchainImagesKHR(device.getDevice(), swapChain, &imageCount, swapChainImages.data());
        extent      = Extent{ swapChainExtent.width, swapChainExtent.height };
        aspectRatio = static_cast<float>(swapChainExtent.width) / static_cast<float>(swapChainExtent.height);

        for (uint32_t i = 0; i < swapChainImages.size(); i++) {
            swapChainImageViews[i] = device.createImageView(swapChainImages[i],
                                                     swapChainImageFormat,
                                                     VK_IMAGE_ASPECT_COLOR_BIT,
                                                     1);
        }

        // For bliting image to swapchain
        // constexpr VkOffset3D vkOffset0{0, 0, 0};
        // const VkOffset3D     vkOffset1{
        //     static_cast<int32_t>(swapChainExtent.width),
        //     static_cast<int32_t>(swapChainExtent.height),
        //     1,
        // };
        // colorImageBlit.srcOffsets[0]                 = vkOffset0;
        // colorImageBlit.srcOffsets[1]                 = vkOffset1;
        // colorImageBlit.srcSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
        // colorImageBlit.srcSubresource.mipLevel       = 0;
        // colorImageBlit.srcSubresource.baseArrayLayer = 0;
        // colorImageBlit.srcSubresource.layerCount     = 1;
        // colorImageBlit.dstOffsets[0]                 = vkOffset0;
        // colorImageBlit.dstOffsets[1]                 = vkOffset1;
        // colorImageBlit.dstSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
        // colorImageBlit.dstSubresource.mipLevel       = 0;
        // colorImageBlit.dstSubresource.baseArrayLayer = 0;
        // colorImageBlit.dstSubresource.layerCount     = 1;
    }

    void VKSwapChain::recreate() {
        vkDeviceWaitIdle(device.getDevice());
        cleanup();
        create();
    }

    void VKSwapChain::cleanup() const {
        // https://vulkan-tutorial.com/Drawing_a_triangle/Swap_chain_recreation#page_Recreating-the-swap-chain
        for (const auto &swapChainImageView : swapChainImageViews) {
            vkDestroyImageView(device.getDevice(), swapChainImageView, nullptr);
        }
        vkDestroySwapchainKHR(device.getDevice(), swapChain, nullptr);
    }

    VKSwapChain::SwapChainSupportDetails VKSwapChain::querySwapChainSupport(
        const VkPhysicalDevice vkPhysicalDevice,
        const VkSurfaceKHR surface) const {
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

    VkSurfaceFormatKHR VKSwapChain::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats) {
        // https://vulkan-tutorial.com/Drawing_a_triangle/Presentation/Swap_chain#page_Choosing-the-right-settings-for-the-swap-chain
        for (const auto &availableFormat : availableFormats) {
            // Using sRGB no-linear color space
            // https://learnopengl.com/Advanced-Lighting/Gamma-Correction
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
                availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) { return availableFormat; }
        }
        return availableFormats[0];
    }

    VkPresentModeKHR VKSwapChain::chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes) {
        // https://vulkan-tutorial.com/Drawing_a_triangle/Presentation/Swap_chain#page_Presentation-mode
        constexpr  auto mode = VK_PRESENT_MODE_MAILBOX_KHR; //static_cast<VkPresentModeKHR>(app().getConfig().vSyncMode);
        for (const auto &availablePresentMode : availablePresentModes) {
            if (availablePresentMode == mode) {
                return availablePresentMode;
            }
        }
        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VkExtent2D VKSwapChain::chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities) const {
        // https://vulkan-tutorial.com/Drawing_a_triangle/Presentation/Swap_chain#page_Swap-extent
        RECT windowRect{};
        if (GetClientRect(Win32Application::getHwnd(), &windowRect) == 0) {
            die("Error getting window rect");
        }
        VkExtent2D actualExtent{
            .width = static_cast<uint32_t>(windowRect.right - windowRect.left),
            .height = static_cast<uint32_t>(windowRect.bottom - windowRect.top)
        };
        actualExtent.width = max(
                capabilities.minImageExtent.width,
                min(capabilities.maxImageExtent.width, actualExtent.width));
        actualExtent.height = max(
                capabilities.minImageExtent.height,
                min(capabilities.maxImageExtent.height, actualExtent.height));
        return actualExtent;
    }

    void VKSwapChain::nextSwapChain() {
        currentFrameIndex = (currentFrameIndex + 1) % FRAMES_IN_FLIGHT;
    }

    void VKSwapChain::present(FrameData& frameData) {
        auto& data = static_cast<VKFrameData&>(frameData);

        {
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
            const auto result = vkQueuePresentKHR(presentQueue, &presentInfo);
            if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
                recreate();
            } else if (result != VK_SUCCESS) {
                die("failed to present swap chain image!");
            }
        }
    }

    void VKSwapChain::begin(FrameData& frameData, CommandList& commandList) {
    }

    void VKSwapChain::end(FrameData& frameData, CommandList& commandList) {
        const auto& data = static_cast<VKFrameData&>(frameData);
        static_cast<VKCommandList&>(commandList).pipelineBarrier(
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
            {
                VKCommandList::imageMemoryBarrier(swapChainImages[data.imageIndex],
                    VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, 0,
                    VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,VK_IMAGE_LAYOUT_PRESENT_SRC_KHR)
            });
    }

    bool VKSwapChain::acquire(FrameData& frameData) {
        auto& data = static_cast<VKFrameData&>(frameData);
        // wait until the GPU has finished rendering the frame.
        if (vkWaitForFences(device.getDevice(), 1, &data.inFlightFence, VK_TRUE, UINT64_MAX) == VK_TIMEOUT) {
            die("timeout waiting for inFlightFence");
            return false;
        }
        // get the next available swap chain image
        {
            const auto result = vkAcquireNextImageKHR(
                 device.getDevice(),
                 swapChain,
                 UINT64_MAX,
                 data.imageAvailableSemaphore,
                 VK_NULL_HANDLE,
                 &data.imageIndex);
            if (result == VK_ERROR_OUT_OF_DATE_KHR) {
                recreate();
                // for (const auto &renderer : renderers) { renderer->recreateImagesResources(); }
                return false;
            }
            if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
                die("failed to acquire swap chain image :", std::to_string(result));
            }
        }
        vkResetFences(device.getDevice(), 1, &data.inFlightFence);
        return true;
    }

    VKSwapChain::~VKSwapChain() {
        cleanup();
    }

}