/*
* Copyright (c) 2025-present Henri Michelon
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/
module;
#include "Tools.h"
export module vireo.backend;

export import vireo.backend.descriptors;
export import vireo.backend.device;
export import vireo.backend.framedata;
export import vireo.backend.resources;

export namespace vireo::backend {

    enum class RenderingBackends {
        DIRECTX,
        VULKAN,
    };

    class VertexInputLayout {
    public:
        enum AttributeFormat {
            R32G32_FLOAT,
            R32G32B32_FLOAT,
            R32G32B32A32_FLOAT,
        };
        struct AttributeDescription {
            std::string     binding;
            AttributeFormat format;
            uint32_t        offset;
        };
        virtual ~VertexInputLayout() = default;
    };

    class ShaderModule {
    public:
        virtual ~ShaderModule() = default;
    };

    class PipelineResources {
    public:
        virtual ~PipelineResources() = default;
    };

    class Pipeline {
    public:
        virtual ~Pipeline() = default;
    };

    class CommandList {
    public:
        enum Type {
            GRAPHIC,
            TRANSFER,
            COMPUTE,
        };

        virtual void begin(Pipeline& pipeline) = 0;

        virtual void reset() = 0;

        virtual void begin() = 0;

        virtual void end() = 0;

        virtual void bindVertexBuffer(Buffer& buffer) = 0;

        virtual void drawInstanced(uint32_t vertexCountPerInstance, uint32_t instanceCount = 1) = 0;

        virtual void upload(Buffer& destination, const void* source) = 0;

        virtual void upload(Image& destination, const void* source) = 0;

        virtual void cleanup() = 0;

        virtual ~CommandList() = default;
    };

    class CommandAllocator {
    public:
        CommandAllocator(const CommandList::Type type) : commandListType{type} {}

        virtual std::shared_ptr<CommandList> createCommandList(Pipeline& pipeline) const  = 0;

        virtual std::shared_ptr<CommandList> createCommandList() const  = 0;

        virtual ~CommandAllocator() = default;

        auto getCommandListType() const { return commandListType; }

    private:
        CommandList::Type commandListType;
    };

    class SubmitQueue {
    public:
        virtual void submit(const FrameData& frameData, std::vector<std::shared_ptr<CommandList>> commandLists) = 0;

        virtual void submit(std::vector<std::shared_ptr<CommandList>> commandLists) = 0;

        virtual void waitIdle() = 0;

        virtual ~SubmitQueue() = default;
    };

    class SwapChain {
    public:
        static constexpr uint32_t FRAMES_IN_FLIGHT = 2;

        struct Extent {
            uint32_t width;
            uint32_t height;
        };

        virtual ~SwapChain() = default;

        const auto& getExtent() const { return extent; }

        auto getAspectRatio() const { return aspectRatio; }

        auto getCurrentFrameIndex() const { return currentFrameIndex; }

        virtual void nextSwapChain() = 0;

        virtual bool acquire(FrameData& frameData) = 0;

        virtual void begin(FrameData& frameData, CommandList& commandList) {}

        virtual void end(FrameData& frameData, CommandList& commandList) {}

        virtual void present(FrameData& frameData) = 0;

    protected:
        Extent      extent{};
        float       aspectRatio{};
        uint32_t    currentFrameIndex{0};
    };

    class RenderingBackEnd {
    public:
        virtual ~RenderingBackEnd() = default;

        virtual std::shared_ptr<FrameData> createFrameData(
            uint32_t frameIndex,
            const std::vector<std::shared_ptr<DescriptorSet>>& descriptorSet) = 0;

        virtual void destroyFrameData(FrameData& frameData) {}

        virtual void waitIdle() = 0;

        virtual std::shared_ptr<CommandAllocator> createCommandAllocator(
            CommandList::Type type) const = 0;

        virtual std::shared_ptr<VertexInputLayout> createVertexLayout(
            size_t size,
            const std::vector<VertexInputLayout::AttributeDescription>& attributesDescriptions) const = 0;

        virtual std::shared_ptr<ShaderModule> createShaderModule(
            const std::string& fileName) const = 0;

        virtual std::shared_ptr<PipelineResources> createPipelineResources(
            const std::vector<std::shared_ptr<DescriptorLayout>>& descriptorLayouts,
            const std::vector<std::shared_ptr<Sampler>>& staticSamplers,
            const std::wstring& name = L"PipelineResource") const = 0;

        virtual std::shared_ptr<Pipeline> createPipeline(
            PipelineResources& pipelineResources,
            VertexInputLayout& vertexInputLayout,
            ShaderModule& vertexShader,
            ShaderModule& fragmentShader,
            const std::wstring& name = L"Pipeline") const = 0;

        virtual std::shared_ptr<Buffer> createBuffer(
            Buffer::Type type,
            size_t size,
            size_t count = 1,
            size_t alignment = 1,
            const std::wstring& name = L"Buffer") const = 0;

        virtual std::shared_ptr<Image> createImage(
            ImageFormat format,
            uint32_t width,
            uint32_t height,
            const std::wstring& name = L"Image") const = 0;

        virtual std::shared_ptr<DescriptorLayout> createDescriptorLayout(
            DescriptorType type,
            uint32_t capacity,
            const std::wstring& name = L"DescriptorLayout") = 0;

        virtual std::shared_ptr<DescriptorLayout> createDescriptorLayout(
            DescriptorType type,
            uint32_t capacity,
            const std::vector<std::shared_ptr<Sampler>>& staticSamplers,
            const std::wstring& name = L"DescriptorLayout") = 0;

        virtual std::shared_ptr<DescriptorSet> createDescriptorSet(
            DescriptorLayout& layout,
            const std::wstring& name = L"DescriptorSet") = 0;

        virtual std::shared_ptr<Sampler> createSampler(
            Filter minFilter,
            Filter magFilter,
            AddressMode addressModeU,
            AddressMode addressModeV,
            AddressMode addressModeW,
            float minLod = 0.0f,
            float maxLod = 1.0f,
            bool anisotropyEnable = true,
            MipMapMode mipMapMode = MipMapMode::LINEAR) const = 0;

        virtual void beginRendering(
            FrameData& frameData,
            PipelineResources& pipelineResources,
            Pipeline& pipeline,
            CommandList& commandList) = 0;

        virtual void endRendering(CommandList& commandList) = 0;

        void setClearColor(const glm::vec4& color) { clearColor = color; }

        auto& getInstance() const { return instance; }

        auto& getPhysicalDevice() const { return physicalDevice; }

        auto& getDevice() const { return device; }

        auto& getGraphicCommandQueue() const { return graphicCommandQueue; }

        auto& getTransferCommandQueue() const { return transferCommandQueue; }

        auto& getSwapChain() const { return swapChain; }

    protected:
        glm::vec4                        clearColor{0.0f};
        std::shared_ptr<Instance>        instance;
        std::shared_ptr<PhysicalDevice>  physicalDevice;
        std::shared_ptr<Device>          device;
        std::shared_ptr<SubmitQueue>     graphicCommandQueue;
        std::shared_ptr<SubmitQueue>     transferCommandQueue;
        std::shared_ptr<SwapChain>       swapChain;

    };
}