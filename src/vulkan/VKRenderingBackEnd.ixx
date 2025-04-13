/*
* Copyright (c) 2025-present Henri Michelon
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/
module;
#include "vireo/backend/vulkan/Tools.h"
export module vireo.vulkan;

import vireo;
import vireo.framedata;
import vireo.descriptors;
import vireo.device;
import vireo.resources;

import vireo.vulkan.device;
import vireo.vulkan.descriptors;
import vireo.vulkan.resources;

export namespace vireo {

    class VKSubmitQueue : public SubmitQueue {
    public:
        VKSubmitQueue(CommandList::Type type, const VKDevice& device, const string& name);

        auto getCommandQueue() const { return commandQueue; }

        void submit(const shared_ptr<FrameData>& frameData, vector<shared_ptr<CommandList>> commandLists) override;

        void submit(vector<shared_ptr<CommandList>> commandLists) override;

        void waitIdle() override;

    private:
        VkQueue commandQueue;
    };

    class VKCommandAllocator : public CommandAllocator {
    public:
        VKCommandAllocator(CommandList::Type type, const VKDevice& device);
        ~VKCommandAllocator() override;

        shared_ptr<CommandList> createCommandList(shared_ptr<Pipeline>& pipeline) const override;

        shared_ptr<CommandList> createCommandList() const override;

    private:
        const VKDevice& device;
        VkCommandPool   commandPool;
    };

    class VKCommandList : public CommandList {
    public:
        VKCommandList(const VKDevice& device, VkCommandPool commandPool);

        ~VKCommandList() override;

        void reset() override;

        void begin(shared_ptr<Pipeline>& pipeline) override;

        void begin() override;

        void end() override;

        void cleanup() override;

        void bindVertexBuffer(shared_ptr<Buffer>& buffer) override;

        void drawInstanced(uint32_t vertexCountPerInstance, uint32_t instanceCount = 1) override;

        void upload(shared_ptr<Buffer>& destination, const void* source) override;

        void upload(shared_ptr<Image>& destination, const void* source) override;

        auto getCommandBuffer() const { return commandBuffer; }

        void pipelineBarrier(
            VkPipelineStageFlags srcStageMask,
            VkPipelineStageFlags dstStageMask,
            const vector<VkImageMemoryBarrier>& barriers) const;

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
        vector<VkBuffer> stagingBuffers{};
        vector<VkDeviceMemory> stagingBuffersMemory{};
    };

    class VKSwapChain : public SwapChain {
    public:
        VKSwapChain(const VKPhysicalDevice& physicalDevice, const VKDevice& device, void* windowHandle);

        ~VKSwapChain() override;

        auto getSwapChain() const { return swapChain; }

        auto getFormat() const { return swapChainImageFormat; }

        const auto& getImageViews() const { return swapChainImageViews; }

        const auto& getImages() const { return swapChainImages; }

        void nextSwapChain() override;

        bool acquire(shared_ptr<FrameData>& frameData) override;

        void begin(shared_ptr<FrameData>& frameData, shared_ptr<CommandList>& commandList) override;

        void end(shared_ptr<FrameData>& frameData, shared_ptr<CommandList>& commandList) override;

        void present(shared_ptr<FrameData>& framesData) override;

    private:
        // For Device::querySwapChainSupport()
        struct SwapChainSupportDetails {
            VkSurfaceCapabilitiesKHR        capabilities;
            vector<VkSurfaceFormatKHR> formats;
            vector<VkPresentModeKHR>   presentModes;
        };

        const VKDevice&             device;
        const VKPhysicalDevice&     physicalDevice;
        VkSurfaceKHR                surface;
        VkSwapchainKHR              swapChain;
        vector<VkImage>        swapChainImages;
        VkFormat                    swapChainImageFormat;
        VkExtent2D                  swapChainExtent;
        vector<VkImageView>    swapChainImageViews;
        VkQueue                     presentQueue;

#ifdef _WIN32
        HWND hWnd;
#endif

        void recreate();

        void create();

        void cleanup() const;

        // Get the swap chain capabilities
        SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice vkPhysicalDevice, VkSurfaceKHR surface) const;

        // Get the swap chain format, default for sRGB/NON-LINEAR
        static VkSurfaceFormatKHR chooseSwapSurfaceFormat(
                const vector<VkSurfaceFormatKHR> &availableFormats);

        // Get the swap chain present mode, default to MAILBOX, if not available FIFO (V-SYNC)
        static VkPresentModeKHR chooseSwapPresentMode(
                const vector<VkPresentModeKHR> &availablePresentModes);

        // Get the swap chain images sizes
        VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities) const;
    };

    class VKVertexInputLayout : public VertexInputLayout {
    public:
        static constexpr VkFormat VKFormat[] {
            VK_FORMAT_R32G32_SFLOAT,
            VK_FORMAT_R32G32B32_SFLOAT,
            VK_FORMAT_R32G32B32A32_SFLOAT
        };

        VKVertexInputLayout(size_t size, const vector<AttributeDescription>& attributesDescriptions);

        const auto& getVertexBindingDescription() const { return vertexBindingDescription; }

        const auto& getVertexAttributeDescription() const { return vertexAttributeDescriptions; }

    private:
        VkVertexInputBindingDescription                vertexBindingDescription;
        vector<VkVertexInputAttributeDescription> vertexAttributeDescriptions;
    };

    class VKShaderModule: public ShaderModule {
    public:
        VKShaderModule(VkDevice device, const string& fileName);

        ~VKShaderModule() override;

        auto getShaderModule() const { return shaderModule; }

    private:
        VkDevice       device;
        VkShaderModule shaderModule;
    };

    class VKPipelineResources : public PipelineResources {
    public:
        VKPipelineResources(
            VkDevice device,
            const vector<shared_ptr<DescriptorLayout>>& descriptorLayouts,
            const wstring& name);

        ~VKPipelineResources() override;

        auto getPipelineLayout() const { return pipelineLayout; }

        const auto& getSetLayouts() const { return setLayouts; }

    private:
        VkDevice device;
        VkPipelineLayout pipelineLayout;
        vector<VkDescriptorSetLayout> setLayouts;
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
           const wstring& name);

        auto getPipeline() const { return pipeline; }

        ~VKPipeline() override;

    private:
        VkDevice     device;
        VkPipeline   pipeline;
    };

    class VKRenderingBackEnd : public RenderingBackEnd {
    public:
        VKRenderingBackEnd(void *windowHandle);

        void waitIdle() override;

        shared_ptr<CommandAllocator> createCommandAllocator(CommandList::Type type) const override;

        shared_ptr<FrameData> createFrameData(
            uint32_t frameIndex,
            const vector<shared_ptr<DescriptorSet>>& descriptorSet) override;

        void destroyFrameData(shared_ptr<FrameData>& frameData) override;

        shared_ptr<VertexInputLayout> createVertexLayout(
            size_t size,
            const vector<VertexInputLayout::AttributeDescription>& attributesDescriptions) const override;

        shared_ptr<ShaderModule> createShaderModule(const string& fileName) const override;

        shared_ptr<PipelineResources> createPipelineResources(
            const vector<shared_ptr<DescriptorLayout>>& descriptorLayouts,
            const wstring& name = L"PipelineResource") const override;

        shared_ptr<Pipeline> createPipeline(
            PipelineResources& pipelineResources,
            VertexInputLayout& vertexInputLayout,
            ShaderModule& vertexShader,
            ShaderModule& fragmentShader,
            const wstring& name = L"Pipeline") const override;

        shared_ptr<Buffer> createBuffer(
            Buffer::Type type,
            size_t size,
            size_t count = 1,
            size_t alignment = 1,
            const wstring& name = L"Buffer") const override;

        shared_ptr<Image> createImage(
            ImageFormat format,
            uint32_t width,
            uint32_t height,
            const wstring& name = L"Image") const override;

        shared_ptr<DescriptorLayout> createDescriptorLayout(
            const wstring& name) override;

        shared_ptr<DescriptorLayout> createSamplerDescriptorLayout(
            const wstring& name = L"createSamplerDescriptorLayout") override;

        shared_ptr<DescriptorSet> createDescriptorSet(
            shared_ptr<DescriptorLayout>& layout,
            const wstring& name) override;

        shared_ptr<Sampler> createSampler(
           Filter minFilter,
           Filter magFilter,
           AddressMode addressModeU,
           AddressMode addressModeV,
           AddressMode addressModeW,
           float minLod = 0.0f,
           float maxLod = 1.0f,
           bool anisotropyEnable = true,
           MipMapMode mipMapMode = MipMapMode::LINEAR) const override;

        void beginRendering(
            shared_ptr<FrameData>& frameData,
            shared_ptr<PipelineResources>& pipelineResources,
            shared_ptr<Pipeline>& pipeline,
            shared_ptr<CommandList>& commandList) override;

        void endRendering(shared_ptr<CommandList>& commandList) override;

        auto getVKInstance() const { return reinterpret_pointer_cast<VKInstance>(instance); }

        auto getVKPhysicalDevice() const { return reinterpret_pointer_cast<VKPhysicalDevice>(physicalDevice); }

        auto getVKDevice() const { return reinterpret_pointer_cast<VKDevice>(device); }

        auto getVKSwapChain() const { return reinterpret_pointer_cast<VKSwapChain>(swapChain); }

        auto getVKGraphicCommandQueue() const { return reinterpret_pointer_cast<VKSubmitQueue>(graphicCommandQueue); }

    private:
#ifdef _WIN32
        HWND hWnd;
#endif
        const VkClearValue depthClearValue{.depthStencil = {1.0f, 0}};
    };

}