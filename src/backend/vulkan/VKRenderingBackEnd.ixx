/*
* Copyright (c) 2025-present Henri Michelon
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/
module;
#include "backend/vulkan/Tools.h"

export module vireo.backend.vulkan;

import vireo.backend;
import vireo.backend.framedata;

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

    class VKSubmitQueue : public SubmitQueue {
    public:
        VKSubmitQueue(CommandList::Type type, const VKDevice& device);

        auto getCommandQueue() const { return commandQueue; }

        void submit(const FrameData& frameData, std::vector<std::shared_ptr<CommandList>> commandLists) override;

        void submit(std::vector<std::shared_ptr<CommandList>> commandLists) override;

        void waitIdle() override;

    private:
        VkQueue commandQueue;
    };

    class VKCommandAllocator : public CommandAllocator {
    public:
        VKCommandAllocator(CommandList::Type type, const VKDevice& device);
        ~VKCommandAllocator() override;

        std::shared_ptr<CommandList> createCommandList(Pipeline& pipeline) const override;

        std::shared_ptr<CommandList> createCommandList() const override;

    private:
        const VKDevice& device;
        VkCommandPool   commandPool;
    };

    class VKCommandList : public CommandList {
    public:
        VKCommandList(const VKDevice& device, VkCommandPool commandPool);

        ~VKCommandList() override;

        void reset() override;

        void begin(Pipeline& pipeline) override;

        void begin() override;

        void end() override;

        void cleanup() override;

        void bindVertexBuffer(Buffer& buffer) override;

        void drawInstanced(uint32_t vertexCountPerInstance, uint32_t instanceCount = 1) override;

        void upload(Buffer& destination, const void* source) override;

        auto getCommandBuffer() const { return commandBuffer; }

        void pipelineBarrier(
            VkPipelineStageFlags srcStageMask,
            VkPipelineStageFlags dstStageMask,
            const std::vector<VkImageMemoryBarrier>& barriers) const;

        static VkImageMemoryBarrier imageMemoryBarrier(
           VkImage image,
           VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask,
           VkImageLayout oldLayout, VkImageLayout newLayout,
           uint32_t baseMipLevel = 0,
           uint32_t levelCount = VK_REMAINING_MIP_LEVELS
       );

    private:
        const VKDevice& device;
        VkCommandBuffer commandBuffer;
        std::vector<VkBuffer> stagingBuffers{};
        std::vector<VkDeviceMemory> stagingBuffersMemory{};
    };

    class VKSwapChain : public SwapChain {
    public:
        VKSwapChain(const VKPhysicalDevice& physicalDevice, const VKDevice& device);

        ~VKSwapChain() override;

        auto getSwapChain() const { return swapChain; }

        auto getFormat() const { return swapChainImageFormat; }

        const auto& getImageViews() const { return swapChainImageViews; }

        const auto& getImages() const { return swapChainImages; }

        void nextSwapChain() override;

        bool acquire(FrameData& frameData) override;

        void begin(FrameData& frameData, CommandList& commandList) override;

        void end(FrameData& frameData, CommandList& commandList) override;

        void present(FrameData& framesData) override;

    private:
        // For Device::querySwapChainSupport()
        struct SwapChainSupportDetails {
            VkSurfaceCapabilitiesKHR        capabilities;
            std::vector<VkSurfaceFormatKHR> formats;
            std::vector<VkPresentModeKHR>   presentModes;
        };

        const VKDevice&             device;
        const VKPhysicalDevice&     physicalDevice;
        VkSurfaceKHR                surface;
        VkSwapchainKHR              swapChain;
        std::vector<VkImage>        swapChainImages;
        VkFormat                    swapChainImageFormat;
        VkExtent2D                  swapChainExtent;
        std::vector<VkImageView>    swapChainImageViews;
        VkQueue                     presentQueue;

        void recreate();

        void create();

        void cleanup() const;

        // Get the swap chain capabilities
        SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice vkPhysicalDevice, VkSurfaceKHR surface) const;

        // Get the swap chain format, default for sRGB/NON-LINEAR
        static VkSurfaceFormatKHR chooseSwapSurfaceFormat(
                const std::vector<VkSurfaceFormatKHR> &availableFormats);

        // Get the swap chain present mode, default to MAILBOX, if not available FIFO (V-SYNC)
        static VkPresentModeKHR chooseSwapPresentMode(
                const std::vector<VkPresentModeKHR> &availablePresentModes);

        // Get the swap chain images sizes
        VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities) const;
    };

    class VKBuffer : public Buffer {
    public:
        VKBuffer(
            const VKDevice& device,
            Type type,
            size_t size,
            size_t count,
            size_t minOffsetAlignment,
            const std::wstring& name);

        ~VKBuffer() override;

        void map() override;

        void unmap() override;

        void write(const void* data, size_t size = WHOLE_SIZE, size_t offset = 0) override;

        inline auto getBuffer() const { return buffer; }

        static void createBuffer(
            const VKDevice& device,
            VkDeviceSize size,
            VkBufferUsageFlags usage,
            uint32_t memoryTypeIndex,
            VkBuffer& buffer,
            VkDeviceMemory& memory);

    private:
        const VKDevice& device;
        VkBuffer       buffer;
        VkDeviceMemory bufferMemory;
    };

    class VKVertexInputLayout : public VertexInputLayout {
    public:
        static constexpr VkFormat VKFormat[] {
            VK_FORMAT_R32G32_SFLOAT,
            VK_FORMAT_R32G32B32_SFLOAT,
            VK_FORMAT_R32G32B32A32_SFLOAT
        };

        VKVertexInputLayout(size_t size, const std::vector<AttributeDescription>& attributesDescriptions);

        const auto& getVertexBindingDescription() const { return vertexBindingDescription; }

        const auto& getVertexAttributeDescription() const { return vertexAttributeDescriptions; }

    private:
        VkVertexInputBindingDescription                vertexBindingDescription;
        std::vector<VkVertexInputAttributeDescription> vertexAttributeDescriptions;
    };

    class VKShaderModule: public ShaderModule {
    public:
        VKShaderModule(VkDevice device, const std::string& fileName);

        ~VKShaderModule() override;

        auto getShaderModule() { return shaderModule; }

    private:
        VkDevice       device;
        VkShaderModule shaderModule;
    };

    class VKPipelineResources : public PipelineResources {
    public:
        VKPipelineResources(VkDevice device);

        ~VKPipelineResources() override;

        auto getPipelineLayout() const { return pipelineLayout; }

    private:
        VkDevice         device;
        VkPipelineLayout pipelineLayout;
    };

    class VKPipeline : public Pipeline {
    public:
        VKPipeline(
           VkDevice device,
           VKSwapChain& swapChain,
           PipelineResources& pipelineResources,
           VertexInputLayout& vertexInputLayout,
           ShaderModule& vertexShader,
           ShaderModule& fragmentShader,
           const std::wstring& name);

        auto getPipeline() const { return pipeline; }

        ~VKPipeline() override;

    private:
        VkDevice     device;
        VkPipeline   pipeline;
    };

    class VKRenderingBackEnd : public RenderingBackEnd {
    public:
        VKRenderingBackEnd();

        void waitIdle() override;

        std::shared_ptr<CommandAllocator> createCommandAllocator(CommandList::Type type) const override;

        std::shared_ptr<FrameData> createFrameData(uint32_t frameIndex) override;

        void destroyFrameData(FrameData& frameData) override;

        std::shared_ptr<VertexInputLayout> createVertexLayout(
            size_t size,
            const std::vector<VertexInputLayout::AttributeDescription>& attributesDescriptions) const override;

        std::shared_ptr<ShaderModule> createShaderModule(const std::string& fileName) const override;

        std::shared_ptr<PipelineResources> createPipelineResources() const override;

        std::shared_ptr<Pipeline> createPipeline(
            PipelineResources& pipelineResources,
            VertexInputLayout& vertexInputLayout,
            ShaderModule& vertexShader,
            ShaderModule& fragmentShader,
            const std::wstring& name = L"Pipeline") const override;

        std::shared_ptr<Buffer> createBuffer(Buffer::Type type, size_t size, size_t count = 1, size_t alignment = 1, const std::wstring& name = L"Buffer") const override;

        void beginRendering(FrameData& frameData, PipelineResources& pipelineResources, Pipeline& pipeline, CommandList& commandList) override;

        void endRendering(CommandList& commandList) override;

        auto getVKInstance() const { return std::reinterpret_pointer_cast<VKInstance>(instance); }

        auto getVKPhysicalDevice() const { return std::reinterpret_pointer_cast<VKPhysicalDevice>(physicalDevice); }

        auto getVKDevice() const { return std::reinterpret_pointer_cast<VKDevice>(device); }

        auto getVKSwapChain() const { return std::reinterpret_pointer_cast<VKSwapChain>(swapChain); }

        auto getVKGraphicCommandQueue() const { return std::reinterpret_pointer_cast<VKSubmitQueue>(graphicCommandQueue); }

    private:
        const VkClearValue depthClearValue{.depthStencil = {1.0f, 0}};
    };

}