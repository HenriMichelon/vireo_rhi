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

    enum class ImageFormat : uint8_t {
        R8G8B8A8_SRGB   = 0,
    };

    enum class BufferType : uint8_t {
        VERTEX  = 0,
        INDEX   = 1,
        UNIFORM = 2,
    };

    enum class DescriptorType : uint8_t {
        BUFFER  = 0,
        IMAGE   = 1,
        SAMPLER = 2,
    };

    enum class CommandType : uint8_t {
        GRAPHIC     = 0,
        TRANSFER    = 1,
        COMPUTE     = 2,
    };

    enum class AttributeFormat : uint8_t {
        R32G32_FLOAT        = 0,
        R32G32B32_FLOAT     = 1,
        R32G32B32A32_FLOAT  = 2,
    };

    enum class CullMode : uint8_t {
        NONE    = 0,
        FRONT   = 1,
        BACK    = 2,
    };

    enum class PrimitiveTopology : uint8_t {
        POINT_LIST     = 0,
        LINE_LIST      = 1,
        LINE_STRIP     = 2,
        TRIANGLE_LIST  = 3,
        TRIANGLE_STRIP = 4,
    };

    using DescriptorIndex = uint32_t;

    struct Extent {
        uint32_t width;
        uint32_t height;
    };

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
        static constexpr size_t WHOLE_SIZE = ~0ULL;

        Buffer(const BufferType type): type{type} {}

        virtual ~Buffer() = default;

        auto getSize() const { return bufferSize; }

        auto getType() const { return type; }

        virtual void map() = 0;

        virtual void unmap() = 0;

        virtual void write(const void* data, size_t size = WHOLE_SIZE, size_t offset = 0) = 0;

    protected:
        const BufferType type;
        size_t           bufferSize{0};
        size_t           alignmentSize{0};
        void*            mappedAddress{nullptr};
    };

    class Sampler {
    public:
        virtual ~Sampler() = default;
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
        const ImageFormat format;
        const uint32_t    width;
        const uint32_t    height;
    };

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

        virtual void update(DescriptorIndex index, const shared_ptr<const Buffer>& buffer) const = 0;

        virtual void update(DescriptorIndex index, const shared_ptr<const Image>& image) const = 0;

        virtual void update(DescriptorIndex index, const shared_ptr<const Sampler>& sampler) const = 0;

        virtual void update(DescriptorIndex index, const vector<shared_ptr<Image>>& images) const = 0;

        virtual void update(DescriptorIndex index, const vector<shared_ptr<Buffer>>& buffer) const = 0;

        virtual void update(DescriptorIndex index, const vector<shared_ptr<Sampler>>& samplers) const = 0;

    protected:
        const shared_ptr<const DescriptorLayout> layout;
        DescriptorSet(const shared_ptr<const DescriptorLayout>& layout) : layout{layout} {}
    };

    class VertexInputLayout {
    public:
        struct AttributeDescription {
            string          binding;
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
        struct Configuration {
            CullMode cullMode{CullMode::NONE};
            bool     colorBlendEnable{false};
        };

        Pipeline(const shared_ptr<PipelineResources>& pipelineResources) :pipelineResources{pipelineResources} {}

        virtual ~Pipeline() = default;

        auto getResources() const { return pipelineResources; }

    private:
        shared_ptr<PipelineResources> pipelineResources;
    };

    struct FrameData {
        virtual ~FrameData() = default;
    };

    class CommandList {
    public:
        virtual void begin() const = 0;

        virtual void end() const = 0;

        virtual void upload(const shared_ptr<const Buffer>& destination, const void* source) = 0;

        virtual void upload(const shared_ptr<const Image>& destination, const void* source) = 0;

        virtual void bindVertexBuffer(const shared_ptr<const Buffer>& buffer) const = 0;

        virtual void bindPipeline(const shared_ptr<const Pipeline>& pipeline) = 0;

        virtual void bindDescriptors(const vector<shared_ptr<const DescriptorSet>>& descriptors) const = 0;

        virtual void drawInstanced(uint32_t vertexCountPerInstance, uint32_t instanceCount = 1) const = 0;

        virtual void setViewports(uint32_t count, const vector<Extent>& extent) const = 0;

        virtual void setScissors(uint32_t count, const vector<Extent>& extent) const = 0;

        virtual void setPrimitiveTopology(PrimitiveTopology primitiveTopology) const = 0;

        virtual void cleanup() = 0;

        virtual ~CommandList() = default;
    };

    class CommandAllocator {
    public:
        CommandAllocator(const CommandType type) : commandListType{type} {}

        virtual void reset() const = 0;

        virtual shared_ptr<CommandList> createCommandList(const shared_ptr<const Pipeline>& pipeline) const  = 0;

        virtual shared_ptr<CommandList> createCommandList() const  = 0;

        virtual ~CommandAllocator() = default;

        auto getCommandListType() const { return commandListType; }

    private:
        const CommandType commandListType;
    };

    class SubmitQueue {
    public:
        virtual void submit(const shared_ptr<const FrameData>& frameData, const vector<shared_ptr<const CommandList>>& commandLists) const = 0;

        virtual void submit(const vector<shared_ptr<const CommandList>>& commandLists) const = 0;

        virtual void waitIdle() const = 0;

        virtual ~SubmitQueue() = default;
    };

    class SwapChain {
    public:
        static constexpr uint32_t FRAMES_IN_FLIGHT = 2;

        virtual ~SwapChain() = default;

        const auto& getExtent() const { return extent; }

        auto getAspectRatio() const { return aspectRatio; }

        auto getCurrentFrameIndex() const { return currentFrameIndex; }

        virtual void nextSwapChain() = 0;

        virtual bool begin(const shared_ptr<FrameData>& frameData) = 0;

        virtual void end(const shared_ptr<const FrameData>& frameData, const shared_ptr<const CommandList>& commandList) const {}

        virtual void present(const shared_ptr<FrameData>& frameData) = 0;

    protected:
        Extent      extent{};
        float       aspectRatio{};
        uint32_t    currentFrameIndex{0};
    };

    class RenderingBackEnd {
    public:
        static unique_ptr<RenderingBackEnd> create(const Configuration& configuration);

        RenderingBackEnd(const Configuration& configuration) : configuration{configuration} {}

        virtual ~RenderingBackEnd() = default;

        virtual shared_ptr<FrameData> createFrameData(uint32_t frameIndex) = 0;

        virtual void destroyFrameData(const shared_ptr<FrameData>& frameData) {}

        virtual void waitIdle() = 0;

        virtual shared_ptr<CommandAllocator> createCommandAllocator(CommandType type) const = 0;

        virtual shared_ptr<VertexInputLayout> createVertexLayout(
            size_t size,
            const vector<VertexInputLayout::AttributeDescription>& attributesDescriptions) const = 0;

        virtual shared_ptr<ShaderModule> createShaderModule(
            const string& fileName) const = 0;

        virtual shared_ptr<PipelineResources> createPipelineResources(
            const vector<shared_ptr<DescriptorLayout>>& descriptorLayouts,
            const wstring& name = L"PipelineResource") const = 0;

        virtual shared_ptr<Pipeline> createPipeline(
            const shared_ptr<PipelineResources>& pipelineResources,
            const shared_ptr<const VertexInputLayout>& vertexInputLayout,
            const shared_ptr<const ShaderModule>& vertexShader,
            const shared_ptr<const ShaderModule>& fragmentShader,
            const Pipeline::Configuration& configuration,
            const wstring& name = L"Pipeline") const = 0;

        virtual shared_ptr<Buffer> createBuffer(
            BufferType type,
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
            const shared_ptr<const DescriptorLayout>& layout,
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
            const shared_ptr<FrameData>& frameData,
            const shared_ptr<const CommandList>& commandList) = 0;

        virtual void endRendering(const shared_ptr<const CommandList>& commandList) = 0;

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
        const Configuration&        configuration;
        float                       clearColor[3] = {};
        shared_ptr<Instance>        instance;
        shared_ptr<PhysicalDevice>  physicalDevice;
        shared_ptr<Device>          device;
        shared_ptr<SubmitQueue>     graphicCommandQueue;
        shared_ptr<SubmitQueue>     transferCommandQueue;
        shared_ptr<SwapChain>       swapChain;

    };
}