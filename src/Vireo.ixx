/*
* Copyright (c) 2025-present Henri Michelon
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/
module;
#include "vireo/Libraries.h"
export module vireo;

export namespace vireo {

    enum class Backend {
        UNDEFINED,
        DIRECTX,
        VULKAN,
    };

    enum class Filter {
        NEAREST,
        LINEAR,
    };

    enum class AddressMode {
        REPEAT,
        MIRRORED_REPEAT,
        CLAMP_TO_EDGE,
        CLAMP_TO_BORDER,
    };

    enum class MipMapMode {
        NEAREST,
        LINEAR,
    };

    enum class ImageFormat {
        R8_UNORM,
        R8_SNORM,
        R8_UINT,
        R8_SINT,

        R8G8_UNORM,
        R8G8_SNORM,
        R8G8_UINT,
        R8G8_SINT,

        R8G8B8A8_UNORM,
        R8G8B8A8_SNORM,
        R8G8B8A8_UINT,
        R8G8B8A8_SINT,
        R8G8B8A8_SRGB,

        B8G8R8A8_UNORM,
        B8G8R8A8_SRGB,
        B8G8R8X8_UNORM,
        B8G8R8X8_SRGB,

        A2B10G10R10_UNORM,
        A2B10G10R10_UINT,

        R16_UNORM,
        R16_SNORM,
        R16_UINT,
        R16_SINT,
        R16_SFLOAT,

        R16G16_UNORM,
        R16G16_SNORM,
        R16G16_UINT,
        R16G16_SINT,
        R16G16_SFLOAT,

        R16G16B16A16_UNORM,
        R16G16B16A16_SNORM,
        R16G16B16A16_UINT,
        R16G16B16A16_SINT,
        R16G16B16A16_SFLOAT,

        R32_UINT,
        R32_SINT,
        R32_SFLOAT,

        R32G32_UINT,
        R32G32_SINT,
        R32G32_SFLOAT,

        R32G32B32_UINT,
        R32G32B32_SINT,
        R32G32B32_SFLOAT,

        R32G32B32A32_UINT,
        R32G32B32A32_SINT,
        R32G32B32A32_SFLOAT,

        D16_UNORM,
        D24_UNORM_S8_UINT,
        D32_SFLOAT,
        D32_SFLOAT_S8_UINT,

        BC1_UNORM,
        BC1_UNORM_SRGB,
        BC2_UNORM,
        BC2_UNORM_SRGB,
        BC3_UNORM,
        BC3_UNORM_SRGB,
        BC4_UNORM,
        BC4_SNORM,
        BC5_UNORM,
        BC5_SNORM,
        BC6H_UFLOAT,
        BC6H_SFLOAT,
        BC7_UNORM,
        BC7_UNORM_SRGB,
    };

    enum class BufferType {
        VERTEX,
        INDEX,
        UNIFORM,
        TRANSFER,
    };

    enum class IndexType {
        UINT16,
        UINT32,
    };

    enum class DescriptorType {
        BUFFER,
        SAMPLED_IMAGE,
        SAMPLER,
        READWRITE_IMAGE,
    };

    enum class CommandType {
        GRAPHIC,
        TRANSFER,
        COMPUTE,
        PRESENT,
    };

    enum class AttributeFormat {
        R32G32_FLOAT,
        R32G32B32_FLOAT,
        R32G32B32A32_FLOAT,
    };

    enum class RenderTargetType {
        COLOR,
        DEPTH,
    };

    enum class CullMode {
        NONE,
        FRONT,
        BACK,
    };

    enum class PrimitiveTopology {
        POINT_LIST,
        LINE_LIST,
        LINE_STRIP,
        TRIANGLE_LIST,
        TRIANGLE_STRIP,
    };

    enum class PolygonMode {
        FILL,
        WIREFRAME,
    };

    enum class CompareOp {
        NEVER,
        LESS,
        EQUAL,
        LESS_OR_EQUAL,
        GREATER,
        NOT_EQUAL,
        GREATER_OR_EQUAL,
        ALWAYS,
    };

    enum class ShaderStage{
        ALL,
        VERTEX,
        FRAGMENT,
    };

    enum class ResourceState {
        UNDEFINED,
        GENERAL,
        RENDER_TARGET_COLOR,
        RENDER_TARGET_DEPTH_STENCIL,
        DISPATCH_TARGET,
        PRESENT,
        COPY_SRC,
        COPY_DST,
        SHADER_READ,
    };

    enum class MSAA {
        NONE = 0,
        //! 2x MSAA
        X2   = 1,
        //! 4x MSAA
        X4   = 2,
        //! 8x MSAA
        X8   = 3,
        //! 16x MSAA
        X16  = 4,
        //! 32x MSAA
        X32  = 5,
        //! 64x MSAA
        X64  = 6
    };

    enum class PresentMode {
        IMMEDIATE = 0,
        VSYNC     = 1,
    };

    using DescriptorIndex = uint32_t;

    struct Extent {
        uint32_t width;
        uint32_t height;
    };

    struct PushConstantsDesc {
        ShaderStage stage{ShaderStage::ALL};
        uint32_t    size{0};
        uint32_t    offset{0};
    };

    struct VertexAttributeDesc {
        string          binding;
        AttributeFormat format;
        uint32_t        offset;
    };

    struct DepthClearValue {
        float    depth{1.0f};
        uint32_t stencil{0};
    };

    struct GraphicPipelineConfiguration {
        ImageFormat       colorRenderFormat{ImageFormat::R8G8B8A8_UNORM};
        PrimitiveTopology primitiveTopology{PrimitiveTopology::TRIANGLE_LIST};
        MSAA              msaa{MSAA::NONE};
        CullMode          cullMode{CullMode::NONE};
        PolygonMode       polygonMode{PolygonMode::FILL};
        bool              frontFaceCounterClockwise{true};
        bool              colorBlendEnable{false};
        ImageFormat       depthImageFormat{ImageFormat::D32_SFLOAT};
        bool              depthTestEnable{false};
        bool              depthWriteEnable{false};
        bool              depthBiasEnable{false};
        CompareOp         depthCompareOp{CompareOp::LESS_OR_EQUAL};
        float             depthBiasConstantFactor{0.0f};
        float             depthBiasClamp{0.0f};
        float             depthBiasSlopeFactor{0.0f};
        bool              alphaToCoverageEnable{false};
    };

    typedef union ClearValue {
        float           color[4]{0.0f, 0.0f, 0.0f, 1.0f};
        DepthClearValue depthStencil;
    } ClearValue;

    class Fence {
    public:
        virtual ~Fence() = default;
    };

    class Instance {
    public:
        virtual ~Instance() = default;

    protected:
        Instance() = default;
    };

    class PhysicalDevice {
    public:
        virtual ~PhysicalDevice() = default;

    protected:
        PhysicalDevice() = default;
    };

    class Device {
    public:
        virtual ~Device() = default;

    protected:
        Device() = default;
    };

    class Buffer {
    public:
        static constexpr size_t WHOLE_SIZE = ~0ULL;

        virtual ~Buffer() = default;

        auto getSize() const { return bufferSize; }


        auto getType() const { return type; }

        auto getInstanceSize() const { return instanceSize; }

        auto getInstanceCount() const { return instanceCount; }

        virtual void map() = 0;

        virtual void unmap() = 0;

        virtual void write(const void* data, size_t size = WHOLE_SIZE, size_t offset = 0) = 0;

    protected:
        size_t bufferSize{0};
        size_t instanceSize{0};
        size_t instanceCount{0};
        void*  mappedAddress{nullptr};

        Buffer(const BufferType type): type{type} {}

    private:
        const BufferType type;
    };

    class Sampler {
    public:
        virtual ~Sampler() = default;

    protected:
        Sampler() = default;
    };

    class Image {
    public:
        static constexpr uint8_t pixelSize[] = {
            1,  // R8_UNORM
            1,  // R8_SNORM
            1,  // R8_UINT
            1,  // R8_SINT

            2,  // R8G8_UNORM
            2,  // R8G8_SNORM
            2,  // R8G8_UINT
            2,  // R8G8_SINT

            4,  // R8G8B8A8_UNORM
            4,  // R8G8B8A8_SNORM
            4,  // R8G8B8A8_UINT
            4,  // R8G8B8A8_SINT
            4,  // R8G8B8A8_SRGB

            4,  // B8G8R8A8_UNORM
            4,  // B8G8R8A8_SRGB
            4,  // B8G8R8X8_UNORM (approx. as 4 bytes per texel)
            4,  // B8G8R8X8_SRGB   (approx. idem)

            4,  // A2B10G10R10_UNORM
            4,  // A2B10G10R10_UINT

            2,  // R16_UNORM
            2,  // R16_SNORM
            2,  // R16_UINT
            2,  // R16_SINT
            2,  // R16_SFLOAT

            4,  // R16G16_UNORM
            4,  // R16G16_SNORM
            4,  // R16G16_UINT
            4,  // R16G16_SINT
            4,  // R16G16_SFLOAT

            8,  // R16G16B16A16_UNORM
            8,  // R16G16B16A16_SNORM
            8,  // R16G16B16A16_UINT
            8,  // R16G16B16A16_SINT
            8,  // R16G16B16A16_SFLOAT

            4,  // R32_UINT
            4,  // R32_SINT
            4,  // R32_SFLOAT

            8,  // R32G32_UINT
            8,  // R32G32_SINT
            8,  // R32G32_SFLOAT

            12, // R32G32B32_UINT
            12, // R32G32B32_SINT
            12, // R32G32B32_SFLOAT

            16, // R32G32B32A32_UINT
            16, // R32G32B32A32_SINT
            16, // R32G32B32A32_SFLOAT

            2,  // D16_UNORM
            4,  // D24_UNORM_S8_UINT
            4,  // D32_SFLOAT
            8,  // D32_SFLOAT_S8_UINT
            1,  // S8_UINT

            8,  // BC1_UNORM (compressed: 4x4 block = 8 bytes → 0.5 byte per pixel)
            8,  // BC1_UNORM_SRGB
            16, // BC2_UNORM (4x4 block = 16 bytes)
            16, // BC2_UNORM_SRGB
            16, // BC3_UNORM
            16, // BC3_UNORM_SRGB
            8,  // BC4_UNORM
            8,  // BC4_SNORM
            16, // BC5_UNORM
            16, // BC5_SNORM
            16, // BC6H_UFLOAT
            16, // BC6H_SFLOAT
            16, // BC7_UNORM
            16, // BC7_UNORM_SRGB
        };

        virtual ~Image() = default;

        auto getFormat() const { return format; }

        auto getWidth() const { return width; }

        auto getHeight() const { return height; }

        auto getArraySize() const { return arraySize; }

        auto getImageSize() const { return width * height * pixelSize[static_cast<int>(format)]; }

        auto getRowPitch() const { return width * pixelSize[static_cast<int>(format)]; }

    protected:
        Image(const ImageFormat format, const uint32_t width, const uint32_t height, const uint32_t arraySize) :
            format{format},
            width{width},
            height{height},
            arraySize{arraySize} {}

    private:
        const ImageFormat format;
        const uint32_t    width;
        const uint32_t    height;
        const uint32_t    arraySize;
    };

    class RenderTarget {
    public:
        RenderTarget(const RenderTargetType type, const shared_ptr<Image>& image) :
            type{type},
            image{image} {}

        virtual ~RenderTarget() = default;

        auto getImage() const { return image; }

        auto getType() const { return type; }

    private:
        const RenderTargetType  type;
        const shared_ptr<Image> image;
    };

    class DescriptorLayout {
    public:
        virtual ~DescriptorLayout() = default;

        virtual DescriptorLayout& add(DescriptorIndex index, DescriptorType type, size_t count = 1) = 0;

        virtual void build() {}

        auto getCapacity() const { return capacity; }

    protected:
        size_t capacity{0};

        DescriptorLayout() = default;
    };

    class DescriptorSet {
    public:
        virtual ~DescriptorSet() = default;

        virtual void update(DescriptorIndex index, const shared_ptr<const Buffer>& buffer) const = 0;

        virtual void update(DescriptorIndex index, const shared_ptr<const Image>& image, bool useByComputeShader = false) const = 0;

        virtual void update(DescriptorIndex index, const shared_ptr<const Sampler>& sampler) const = 0;

        virtual void update(DescriptorIndex index, const vector<shared_ptr<Image>>& images, bool useByComputeShader = false) const = 0;

        virtual void update(DescriptorIndex index, const vector<shared_ptr<Buffer>>& buffer) const = 0;

        virtual void update(DescriptorIndex index, const vector<shared_ptr<Sampler>>& samplers) const = 0;

    protected:
        const shared_ptr<const DescriptorLayout> layout;

        DescriptorSet(const shared_ptr<const DescriptorLayout>& layout) : layout{layout} {}
    };

    class VertexInputLayout {
    public:
        virtual ~VertexInputLayout() = default;

    protected:
        VertexInputLayout() = default;
    };

    class ShaderModule {
    public:
        virtual ~ShaderModule() = default;

    protected:
        ShaderModule() = default;
    };

    class PipelineResources {
    public:
        virtual ~PipelineResources() = default;

    protected:
        PipelineResources() = default;
    };

    class Pipeline {
    public:
        enum Type {
            GRAPHIC,
            COMPUTE
        };

        virtual ~Pipeline() = default;

        auto getResources() const { return pipelineResources; }

        auto getType() const { return type; }

    protected:
        Pipeline(const Type type, const shared_ptr<PipelineResources>& pipelineResources) :
            pipelineResources{pipelineResources},
            type{type} {}

    private:
        const Type type;
        shared_ptr<PipelineResources> pipelineResources;
    };

    class ComputePipeline : public Pipeline {
    protected:
        ComputePipeline(const shared_ptr<PipelineResources>& pipelineResources) :
            Pipeline{COMPUTE, pipelineResources} {}
    };

    class GraphicPipeline : public Pipeline {
    protected:
        GraphicPipeline(const shared_ptr<PipelineResources>& pipelineResources) :
            Pipeline{GRAPHIC, pipelineResources} {}
    };

    class SwapChain;

    struct RenderingConfiguration {
        shared_ptr<SwapChain>    swapChain{nullptr};
        shared_ptr<RenderTarget> colorRenderTarget{nullptr};
        shared_ptr<RenderTarget> depthRenderTarget{nullptr};
        shared_ptr<RenderTarget> multisampledColorRenderTarget{nullptr};
        shared_ptr<RenderTarget> multisampledDepthRenderTarget{nullptr};
        ClearValue               clearColorValue{ .color = {0.0f, 0.0f, 0.0f, 0.0f} };
        ClearValue               depthClearValue{ .depthStencil = {1.0f, 0} };
        bool                     clearColor{true};
        bool                     clearDepth{true};
    };

    class CommandList {
    public:
        virtual void begin() const = 0;

        virtual void end() const = 0;

        virtual void upload(const shared_ptr<const Buffer>& destination, const void* source) = 0;

        virtual void upload(const shared_ptr<const Image>& destination, const void* source) = 0;

        virtual void copy(
            const shared_ptr<const Image>& source,
            const shared_ptr<const SwapChain>& swapChain) const = 0;

        virtual void blit(
            const shared_ptr<const Image>& source,
            const shared_ptr<const SwapChain>& swapChain,
            Filter filter = Filter::NEAREST) const = 0;

        virtual void beginRendering(const RenderingConfiguration& configuration) = 0;

        virtual void endRendering() {}

        virtual void dispatch(uint32_t x, uint32_t y, uint32_t z) const = 0;

        virtual void bindVertexBuffer(
            const shared_ptr<const Buffer>& buffer,
            size_t offset = 0) const = 0;

        virtual void bindVertexBuffers(
            const vector<shared_ptr<const Buffer>>& buffers,
            vector<size_t> offsets = {}) const = 0;

        virtual void bindIndexBuffer(
            const shared_ptr<const Buffer>& buffer,
            IndexType indexType = IndexType::UINT32,
            size_t offset = 0) const = 0;

        virtual void bindPipeline(const shared_ptr<const Pipeline>& pipeline) = 0;

        virtual void bindDescriptors(
            const shared_ptr<const Pipeline>& pipeline,
            const vector<shared_ptr<const DescriptorSet>>& descriptors) const = 0;

        virtual void draw(
            uint32_t vertexCountPerInstance,
            uint32_t instanceCount = 1,
            uint32_t firstVertex = 0,
            uint32_t firstInstance = 0) const = 0;

        virtual void drawIndexed(
            uint32_t indexCountPerInstance,
            uint32_t instanceCount = 1,
            uint32_t firstIndex = 0,
            uint32_t vertexOffset = 0,
            uint32_t firstInstance = 0) const = 0;

        virtual void setViewports(uint32_t count, const vector<Extent>& extent) const = 0;

        virtual void setScissors(uint32_t count, const vector<Extent>& extent) const = 0;

        virtual void barrier(
            const shared_ptr<const Image>& image,
            ResourceState oldState,
            ResourceState newState) const = 0;

        virtual void barrier(
            const shared_ptr<const RenderTarget>& renderTarget,
            ResourceState oldState,
            ResourceState newState) const = 0;

        virtual void barrier(
            const shared_ptr<const SwapChain>& swapChain,
            ResourceState oldState,
            ResourceState newState) const = 0;

        virtual void pushConstants(
            const shared_ptr<const PipelineResources>& pipelineResources,
            const PushConstantsDesc& pushConstants,
            const void* data) const = 0;

        virtual void cleanup() = 0;

        virtual ~CommandList() = default;

    protected:
        CommandList() = default;
    };

    class CommandAllocator {
    public:

        virtual void reset() const = 0;

        virtual shared_ptr<CommandList> createCommandList(const shared_ptr<const Pipeline>& pipeline) const  = 0;

        virtual shared_ptr<CommandList> createCommandList() const  = 0;

        virtual ~CommandAllocator() = default;

        auto getCommandListType() const { return commandListType; }

    protected:
        CommandAllocator(const CommandType type) : commandListType{type} {}

    private:
        const CommandType commandListType;
    };

    class SubmitQueue {
    public:
        virtual void submit(
            const shared_ptr<Fence>& fence,
            const shared_ptr<const SwapChain>& swap,
            const vector<shared_ptr<const CommandList>>& commandLists) const = 0;

        virtual void submit(const vector<shared_ptr<const CommandList>>& commandLists) const = 0;

        virtual void waitIdle() const = 0;

        virtual ~SubmitQueue() = default;

    protected:
        SubmitQueue() = default;
    };

    class SwapChain {
    public:
        virtual ~SwapChain() = default;

        const auto& getExtent() const { return extent; }

        auto getAspectRatio() const { return aspectRatio; }

        auto getCurrentFrameIndex() const { return currentFrameIndex; }

        auto getFramesInFlight() const { return framesInFlight; }

        auto getFormat() const { return format; }

        virtual void nextSwapChain() = 0;

        virtual bool acquire(const shared_ptr<Fence>& fence) = 0;

        virtual void present() = 0;

        virtual void recreate() = 0;

        virtual void waitIdle() const = 0;

    protected:
        const PresentMode presentMode;
        const ImageFormat format;
        uint32_t    framesInFlight;
        Extent      extent{};
        float       aspectRatio{};
        uint32_t    currentFrameIndex{0};

        SwapChain(const ImageFormat format, const PresentMode presentMode, const uint32_t framesInFlight) :
            format{format},
            presentMode{presentMode},
            framesInFlight{framesInFlight} {}
    };

    class Vireo {
    public:
        static unique_ptr<Vireo> create(Backend backend, void* windowHandle);

        virtual ~Vireo() = default;

        virtual void waitIdle() {}

        virtual shared_ptr<SwapChain> createSwapChain(
            ImageFormat format,
            const shared_ptr<const SubmitQueue>& submitQueue,
            PresentMode presentMode = PresentMode::VSYNC,
            uint32_t framesInFlight = 2) const = 0;

        virtual shared_ptr<SubmitQueue> createSubmitQueue(
            CommandType commandType,
            const wstring& name = L"SubmitQueue") const = 0;

        virtual shared_ptr<Fence> createFence(const wstring& name = L"Fence") const = 0;

        virtual shared_ptr<CommandAllocator> createCommandAllocator(CommandType type) const = 0;

        virtual shared_ptr<VertexInputLayout> createVertexLayout(
            size_t size,
            const vector<VertexAttributeDesc>& attributesDescriptions) const = 0;

        virtual shared_ptr<ShaderModule> createShaderModule(
            const string& fileName) const = 0;

        virtual shared_ptr<PipelineResources> createPipelineResources(
            const vector<shared_ptr<DescriptorLayout>>& descriptorLayouts,
            const PushConstantsDesc& pushConstant = {},
            const wstring& name = L"PipelineResource") const = 0;

        virtual shared_ptr<ComputePipeline> createComputePipeline(
            const shared_ptr<PipelineResources>& pipelineResources,
            const shared_ptr<const ShaderModule>& shader,
            const wstring& name = L"ComputePipeline") const = 0;

        virtual shared_ptr<GraphicPipeline> createGraphicPipeline(
            const shared_ptr<PipelineResources>& pipelineResources,
            const shared_ptr<const VertexInputLayout>& vertexInputLayout,
            const shared_ptr<const ShaderModule>& vertexShader,
            const shared_ptr<const ShaderModule>& fragmentShader,
            const GraphicPipelineConfiguration& configuration,
            const wstring& name = L"GraphicPipeline") const = 0;

        virtual shared_ptr<Buffer> createBuffer(
            BufferType type,
            size_t size,
            size_t count = 1,
            const wstring& name = L"Buffer") const = 0;

        virtual shared_ptr<Image> createImage(
            ImageFormat format,
            uint32_t width,
            uint32_t height,
            uint32_t arraySize = 1,
            const wstring& name = L"Image") const = 0;

        virtual shared_ptr<Image> createReadWriteImage(
            ImageFormat format,
            uint32_t width,
            uint32_t height,
            uint32_t arraySize = 1,
            const wstring& name = L"RWImage") const = 0;

        virtual shared_ptr<RenderTarget> createRenderTarget(
            ImageFormat format,
            uint32_t width,
            uint32_t height,
            RenderTargetType type = RenderTargetType::COLOR,
            ClearValue clearValue = {},
            MSAA msaa = MSAA::NONE,
            const wstring& name = L"RenderTarget") const = 0;

        virtual shared_ptr<RenderTarget> createRenderTarget(
            const shared_ptr<const SwapChain>& swapChain,
            ClearValue clearValue = {},
            MSAA msaa = MSAA::NONE,
            const wstring& name = L"RenderTarget") const = 0;

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

    protected:
        shared_ptr<Instance>        instance;
        shared_ptr<PhysicalDevice>  physicalDevice;
        shared_ptr<Device>          device;
    };

}