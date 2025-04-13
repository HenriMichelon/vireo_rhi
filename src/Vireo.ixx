/*
* Copyright (c) 2025-present Henri Michelon
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/
module;
#include "vireo/Libraries.h"
export module vireo;

import vireo.config;

export namespace vireo {

    class Instance {
    public:
        virtual ~Instance() = default;
    };

    class PhysicalDevice {
    public:
        virtual ~PhysicalDevice() = default;
    };

    class Device {
    public:
        virtual ~Device() = default;
    };

    class Buffer {
    public:
        enum Type {
            VERTEX,
            INDEX,
            UNIFORM
        };

        static constexpr size_t WHOLE_SIZE = ~0ULL;

        Buffer(const Type type): type{type} {}

        virtual ~Buffer() = default;

        auto getSize() const { return bufferSize; }

        auto getType() const { return type; }

        virtual void map() = 0;

        virtual void unmap() = 0;

        virtual void write(const void* data, size_t size = WHOLE_SIZE, size_t offset = 0) = 0;

    protected:
        Type    type;
        size_t  bufferSize{0};
        size_t  alignmentSize{0};
        void*   mappedAddress{nullptr};
    };


    enum class Filter : uint8_t {
        NEAREST = 0,
        LINEAR  = 1,
    };

    enum class AddressMode : uint8_t {
        REPEAT          = 0,
        MIRRORED_REPEAT = 1,
        CLAMP_TO_EDGE   = 2,
        CLAMP_TO_BORDER = 3,
    };

    enum class MipMapMode : uint8_t {
        NEAREST = 0,
        LINEAR  = 1,
    };

    class Sampler {
    public:
        virtual ~Sampler() = default;
    };

    enum class ImageFormat : uint8_t {
        R8G8B8A8_SRGB   = 0,
    };

    class Image {
    public:
        static constexpr uint8_t pixelSize[] {
            4
        };

        Image(const ImageFormat format, const uint32_t width, const uint32_t height) :
            format{format},
            width{width},
            height{height} {}

        virtual ~Image() = default;

        auto getFormat() const { return format; }

        auto getWidth() const { return width; }

        auto getHeight() const { return height; }

        auto getSize() const { return width * height * pixelSize[static_cast<int>(format)]; }

        auto getRowPitch() const { return width * pixelSize[static_cast<int>(format)]; }

    private:
        ImageFormat format;
        uint32_t width;
        uint32_t height;
    };

    enum class DescriptorType : uint8_t {
        BUFFER  = 0,
        IMAGE   = 1,
        SAMPLER = 2,
    };

    using DescriptorIndex = uint32_t;

    class DescriptorLayout {
    public:
        virtual ~DescriptorLayout() = default;

        virtual DescriptorLayout& add(DescriptorIndex index, DescriptorType type, size_t count = 1) = 0;

        virtual void build() {}

        auto getCapacity() const { return capacity; }

    protected:
        size_t capacity{0};
    };

    class DescriptorSet {
    public:
        virtual ~DescriptorSet() = default;

        virtual void update(DescriptorIndex index, const shared_ptr<Buffer>& buffer) = 0;

        virtual void update(DescriptorIndex index, const shared_ptr<Image>& image) = 0;

        virtual void update(DescriptorIndex index, const shared_ptr<Sampler>& sampler) = 0;

        virtual void update(DescriptorIndex index, const vector<shared_ptr<Image>>& images) = 0;

        virtual void update(DescriptorIndex index, const vector<shared_ptr<Buffer>>& buffer) = 0;

        virtual void update(DescriptorIndex index, const vector<shared_ptr<Sampler>>& samplers) = 0;

    protected:
        const shared_ptr<DescriptorLayout> layout;
        DescriptorSet(const shared_ptr<DescriptorLayout>& layout) : layout{layout} {}
    };

    class VertexInputLayout {
    public:
        enum AttributeFormat {
            R32G32_FLOAT,
            R32G32B32_FLOAT,
            R32G32B32A32_FLOAT,
        };
        struct AttributeDescription {
            string     binding;
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

    struct FrameData {
        FrameData(const vector<shared_ptr<DescriptorSet>>& descriptorSets): descriptorSets{descriptorSets} {}

        virtual ~FrameData() = default;

        vector<shared_ptr<DescriptorSet>> descriptorSets;
    };


    class CommandList {
    public:
        enum Type {
            GRAPHIC,
            TRANSFER,
            COMPUTE,
        };

        virtual void begin(shared_ptr<Pipeline>& pipeline) = 0;

        virtual void reset() = 0;

        virtual void begin() = 0;

        virtual void end() = 0;

        virtual void bindVertexBuffer(shared_ptr<Buffer>& buffer) = 0;

        virtual void drawInstanced(uint32_t vertexCountPerInstance, uint32_t instanceCount = 1) = 0;

        virtual void upload(shared_ptr<Buffer>& destination, const void* source) = 0;

        virtual void upload(shared_ptr<Image>& destination, const void* source) = 0;

        virtual void cleanup() = 0;

        virtual ~CommandList() = default;
    };

    class CommandAllocator {
    public:
        CommandAllocator(const CommandList::Type type) : commandListType{type} {}

        virtual shared_ptr<CommandList> createCommandList(shared_ptr<Pipeline>& pipeline) const  = 0;

        virtual shared_ptr<CommandList> createCommandList() const  = 0;

        virtual ~CommandAllocator() = default;

        auto getCommandListType() const { return commandListType; }

    private:
        CommandList::Type commandListType;
    };

    class SubmitQueue {
    public:
        virtual void submit(const shared_ptr<FrameData>& frameData, vector<shared_ptr<CommandList>> commandLists) = 0;

        virtual void submit(vector<shared_ptr<CommandList>> commandLists) = 0;

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

        virtual bool acquire(shared_ptr<FrameData>& frameData) = 0;

        virtual void begin(shared_ptr<FrameData>& frameData, shared_ptr<CommandList>& commandList) {}

        virtual void end(shared_ptr<FrameData>& frameData, shared_ptr<CommandList>& commandList) {}

        virtual void present(shared_ptr<FrameData>& frameData) = 0;

    protected:
        Extent      extent{};
        float       aspectRatio{};
        uint32_t    currentFrameIndex{0};
    };

    class RenderingBackEnd {
    public:
        static unique_ptr<RenderingBackEnd> create(const Configuration& configuration);

        virtual ~RenderingBackEnd() = default;

        virtual shared_ptr<FrameData> createFrameData(
            uint32_t frameIndex,
            const vector<shared_ptr<DescriptorSet>>& descriptorSet) = 0;

        virtual void destroyFrameData(
            shared_ptr<FrameData>& frameData) {}

        virtual void waitIdle() = 0;

        virtual shared_ptr<CommandAllocator> createCommandAllocator(
            CommandList::Type type) const = 0;

        virtual shared_ptr<VertexInputLayout> createVertexLayout(
            size_t size,
            const vector<VertexInputLayout::AttributeDescription>& attributesDescriptions) const = 0;

        virtual shared_ptr<ShaderModule> createShaderModule(
            const string& fileName) const = 0;

        virtual shared_ptr<PipelineResources> createPipelineResources(
            const vector<shared_ptr<DescriptorLayout>>& descriptorLayouts,
            const wstring& name = L"PipelineResource") const = 0;

        virtual shared_ptr<Pipeline> createPipeline(
            PipelineResources& pipelineResources,
            VertexInputLayout& vertexInputLayout,
            ShaderModule& vertexShader,
            ShaderModule& fragmentShader,
            const wstring& name = L"Pipeline") const = 0;

        virtual shared_ptr<Buffer> createBuffer(
            Buffer::Type type,
            size_t size,
            size_t count = 1,
            size_t alignment = 1,
            const wstring& name = L"Buffer") const = 0;

        virtual shared_ptr<Image> createImage(
            ImageFormat format,
            uint32_t width,
            uint32_t height,
            const wstring& name = L"Image") const = 0;

        virtual shared_ptr<DescriptorLayout> createDescriptorLayout(
            const wstring& name = L"DescriptorLayout") = 0;

        virtual shared_ptr<DescriptorLayout> createSamplerDescriptorLayout(
            const wstring& name = L"createSamplerDescriptorLayout") = 0;

        virtual shared_ptr<DescriptorSet> createDescriptorSet(
            shared_ptr<DescriptorLayout>& layout,
            const wstring& name = L"DescriptorSet") = 0;

        virtual shared_ptr<Sampler> createSampler(
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
            shared_ptr<FrameData>& frameData,
            shared_ptr<PipelineResources>& pipelineResources,
            shared_ptr<Pipeline>& pipeline,
            shared_ptr<CommandList>& commandList) = 0;

        virtual void endRendering(shared_ptr<CommandList>& commandList) = 0;

        void setClearColor(const float r, const float g, const float b) {
            clearColor[0] = r;
            clearColor[1] = g;
            clearColor[2] = b;
        }

        auto& getInstance() const { return instance; }

        auto& getPhysicalDevice() const { return physicalDevice; }

        auto& getDevice() const { return device; }

        auto& getGraphicCommandQueue() const { return graphicCommandQueue; }

        auto& getTransferCommandQueue() const { return transferCommandQueue; }

        auto& getSwapChain() const { return swapChain; }

    protected:
        float                       clearColor[3] = {};
        shared_ptr<Instance>        instance;
        shared_ptr<PhysicalDevice>  physicalDevice;
        shared_ptr<Device>          device;
        shared_ptr<SubmitQueue>     graphicCommandQueue;
        shared_ptr<SubmitQueue>     transferCommandQueue;
        shared_ptr<SwapChain>       swapChain;

    };
}