/*
* Copyright (c) 2025-present Henri Michelon
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/
module;
#include "vireo/Libraries.h"
export module vireo;

export import vireo.tools;

export namespace vireo {

    /**
     * Type of supported backends
     */
    enum class Backend : uint8_t {
        //! Not used
        UNDEFINED,
        //! Direct X 12
        DIRECTX,
        //! Vulkan 1.3
        VULKAN,
    };

    /**
     * Possible values of the Vireo::createSampler magFilter and minFilter parameters, specifying filters used for texture lookups
     */
    enum class Filter {
        //! Nearest filtering
        NEAREST,
        //! Linear filtering
        LINEAR,
    };

    /**
     * Possible values of the Vireo::createSampler addressMode* parameters,
     * specifying the behavior of sampling with coordinates outside the range [0,1] for the respective u, v, or w
     * coordinate
     */
    enum class AddressMode {
        //! Repeat wrap mode
        REPEAT,
        //! Mirror repeat wrap mode
        MIRRORED_REPEAT,
        //! Clamp to edge wrap mode
        CLAMP_TO_EDGE,
        //! Clamp to border wrap mode
        CLAMP_TO_BORDER,
    };

    /**
     * Possible values of the Vireo::createSampler mipmapMode parameter, specifying the mipmap mode used for texture
     * lookups
     */
    enum class MipMapMode {
        //! Nearest filtering
        NEAREST,
        //! Linear filtering
        LINEAR,
    };

    /**
     * Available image formats. Only the common formats between the supported back ends are available.
     */
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

    /**
     * VRAM Buffer use type
     */
    enum class BufferType {
        //! Used to store vertices (in device local memory)
        VERTEX,
        //! Used to store indices (in device local memory)
        INDEX,
        //! Used for shader uniform (in host visible memory)
        UNIFORM,
        //! Used for copy operations (in host visible memory)
        TRANSFER,
    };

    /**
     * Index type for vertex indices
     */
    enum class IndexType {
        //! Two bytes unsigned int
        UINT16,
        //! Four bytes unsigned int
        UINT32,
    };

    /**
     * Type of data for shaders
     */
    enum class DescriptorType {
        //! Uniform buffer
        BUFFER,
        //! Sampled texture (image only, no sampler)
        SAMPLED_IMAGE,
        //! Sampler for SAMPLED_IMAGE
        SAMPLER,
        //! Read/Write image for compute shaders
        READWRITE_IMAGE,
    };

    /**
     * Type of command or submit queue
     */
    enum class CommandType {
        //! Command/Queue for a graphic pipeline
        GRAPHIC,
        //! Command/Queue for copy operations
        TRANSFER,
        //! Command/Queue for a compute pipeline
        COMPUTE,
    };

    /**
     * Size and type of a vertex attribute data
     */
    enum class AttributeFormat {
        //! One signed float
        R32_FLOAT,
        //! Two signed floats
        R32G32_FLOAT,
        //! Three signed floats
        R32G32B32_FLOAT,
        //! Four signed floats
        R32G32B32A32_FLOAT,
        //! One signed int
        R32_SINT,
        //! Two signed ints
        R32G32_SINT,
        //! Three signed ints
        R32G32B32_SINT,
        //! Four signed ints
        R32G32B32A32_SINT,
        //! One unsigned int
        R32_UINT,
        //! Two unsigned ints
        R32G32_UINT,
        //! Three unsigned ints
        R32G32B32_UINT,
        //! Four unsigned ints
        R32G32B32A32_UINT,
    };

    /**
     * Use of a render target image
     */
    enum class RenderTargetType {
        //! Used as a color attachment
        COLOR,
        //! Used as a depth attachment
        DEPTH,
    };

    /**
     * Values controlling triangle culling. Triangle orientation is specified in the pipeline configuration.
     */
    enum class CullMode {
        //! No triangles are discarded
        NONE,
        //! Front-facing triangles are discarded
        FRONT,
        //! Back-facing triangles are discarded
        BACK,
    };

    /**
     * Supported primitive topologies
     */
    enum class PrimitiveTopology {
        //! A series of separate point primitives.
        POINT_LIST,
        //! A series of separate line primitives.
        LINE_LIST,
        //! A series of connected line primitives with consecutive lines sharing a vertex.
        LINE_STRIP,
        //! A series of separate triangle primitives.
        TRIANGLE_LIST,
        //! A series of connected triangle primitives with consecutive triangles sharing an edge.
        TRIANGLE_STRIP,
    };

    /**
    * Control polygon rasterization mode
    */
    enum class PolygonMode {
        //! Polygon is filled
        FILL,
        //! Polygon edges are drawn as line segments
        WIREFRAME,
    };

    /**
     * Comparison operator for depth, stencil, and sampler operations
     */
    enum class CompareOp {
        //! Comparison always evaluates false.
        NEVER,
        //! Comparison evaluates reference < test.
        LESS,
        //! Comparison evaluates reference = test.
        EQUAL,
        //! Comparison evaluates reference ≤ test.
        LESS_OR_EQUAL,
        //! Comparison evaluates reference > test.
        GREATER,
        //! Comparison evaluates reference ≠ test.
        NOT_EQUAL,
        //! Comparison evaluates reference ≥ test.
        GREATER_OR_EQUAL,
        //! Comparison always evaluates true.
        ALWAYS,
    };

    enum class BlendFactor {
        ZERO,
        ONE,
        SRC_COLOR,
        ONE_MINUS_SRC_COLOR,
        DST_COLOR,
        ONE_MINUS_DST_COLOR,
        SRC_ALPHA,
        ONE_MINUS_SRC_ALPHA,
        DST_ALPHA,
        ONE_MINUS_DST_ALPHA,
        CONSTANT_COLOR,
        ONE_MINUS_CONSTANT_COLOR,
        CONSTANT_ALPHA,
        ONE_MINUS_CONSTANT_ALPHA,
        SRC_ALPHA_SATURATE,
        SRC1_COLOR,
        ONE_MINUS_SRC1_COLOR,
        SRC1_ALPHA,
        ONE_MINUS_SRC1_ALPHA
    };

    enum class BlendOp {
        ADD,
        SUBTRACT,
        REVERSE_SUBTRACT,
        MIN,
        MAX
    };

    enum class LogicOp {
        CLEAR,
        SET,
        COPY,
        COPY_INVERTED,
        NOOP,
        INVERT,
        AND,
        NAND,
        OR,
        NOR,
        XOR,
        EQUIV,
        AND_REVERSE,
        AND_INVERTED,
        OR_REVERSE,
        OR_INVERTED
    };

    enum class ColorWriteMask : uint8_t {
        RED  = 0x00000001,
        G    = 0x00000002,
        B    = 0x00000004,
        A    = 0x00000008,
        ALL  = 0x0000000f,
    };

    struct ColorBlendDesc {
        bool            blendEnable{false};
        BlendFactor     srcColorBlendFactor{BlendFactor::SRC_ALPHA};
        BlendFactor     dstColorBlendFactor{BlendFactor::ONE_MINUS_SRC_ALPHA};
        BlendOp         colorBlendOp{BlendOp::ADD};
        BlendFactor     srcAlphaBlendFactor{BlendFactor::ONE};
        BlendFactor     dstAlphaBlendFactor{BlendFactor::ZERO};
        BlendOp         alphaBlendOp{BlendOp::ADD};
        ColorWriteMask  colorWriteMask{ColorWriteMask::ALL};
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
        COMPUTE_READ,
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
        vector<ImageFormat>    colorRenderFormats{};
        vector<ColorBlendDesc> colorBlendDesc{};
        PrimitiveTopology      primitiveTopology{PrimitiveTopology::TRIANGLE_LIST};
        MSAA                   msaa{MSAA::NONE};
        CullMode               cullMode{CullMode::NONE};
        PolygonMode            polygonMode{PolygonMode::FILL};
        bool                   frontFaceCounterClockwise{true};
        ImageFormat            depthImageFormat{ImageFormat::D32_SFLOAT};
        bool                   depthTestEnable{false};
        bool                   depthWriteEnable{false};
        bool                   depthBiasEnable{false};
        CompareOp              depthCompareOp{CompareOp::LESS_OR_EQUAL};
        float                  depthBiasConstantFactor{0.0f};
        float                  depthBiasClamp{0.0f};
        float                  depthBiasSlopeFactor{0.0f};
        bool                   logicOpEnable{false};
        LogicOp                logicOp{LogicOp::NOOP};
        bool                   alphaToCoverageEnable{false};
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

        auto getMappedAddress() const { return mappedAddress; }

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

        auto getMipLevels() const { return mipLevels; }

        auto getArraySize() const { return arraySize; }

        auto getImageSize() const { return width * height * pixelSize[static_cast<int>(format)]; }

        auto getRowPitch() const { return width * pixelSize[static_cast<int>(format)]; }

        auto isReadWrite() const { return readWrite; }

    protected:
        Image(
            const ImageFormat format,
            const uint32_t width,
            const uint32_t height,
            const uint32_t mipLevels,
            const uint32_t arraySize,
            const bool isReadWrite) :
            format{format},
            width{width},
            height{height},
            mipLevels{mipLevels},
            arraySize{arraySize},
            readWrite{isReadWrite} {}

    private:
        const ImageFormat format;
        const uint32_t    width;
        const uint32_t    height;
        const uint32_t    mipLevels;
        const uint32_t    arraySize;
        const bool        readWrite;
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

    struct RenderTargetDesc {
        shared_ptr<SwapChain>    swapChain{nullptr};
        shared_ptr<RenderTarget> renderTarget{nullptr};
        shared_ptr<RenderTarget> multisampledRenderTarget{nullptr};
        bool                     clearColor{true};
        ClearValue               clearColorValue{ .color = {0.0f, 0.0f, 0.0f, 0.0f} };
    };

    struct RenderingConfiguration {
        vector<RenderTargetDesc>  colorRenderTargets{};
        shared_ptr<RenderTarget>  depthRenderTarget{nullptr};
        shared_ptr<RenderTarget>  multisampledDepthRenderTarget{nullptr};
        bool                      clearDepth{true};
        ClearValue                depthClearValue{ .depthStencil = {1.0f, 0} };
    };

    class CommandList {
    public:
        virtual void begin() const = 0;

        virtual void end() const = 0;

        virtual void upload(const shared_ptr<const Buffer>& destination, const void* source) = 0;

        virtual void upload(
            const shared_ptr<const Image>& destination,
            const void* source,
            uint32_t firstMipLevel = 0) = 0;

        virtual void copy(
            const shared_ptr<const Buffer>& source,
            const shared_ptr<const Image>& destination,
            uint32_t sourceOffset = 0,
            uint32_t firstMipLevel = 0) = 0;

        virtual void uploadArray(
            const shared_ptr<const Image>& destination,
            const vector<void*>& sources,
            uint32_t firstMipLevel = 0) = 0;

        virtual void copy(
            const shared_ptr<const Image>& source,
            const shared_ptr<const SwapChain>& swapChain) const = 0;

        inline void copy(
            const shared_ptr<const RenderTarget>& source,
            const shared_ptr<const SwapChain>& swapChain) const {
            copy(source->getImage(), swapChain);
        }

        virtual void blit(
            const shared_ptr<const Image>& source,
            const shared_ptr<const SwapChain>& swapChain,
            Filter filter = Filter::NEAREST) const = 0;

        void blit(
            const shared_ptr<const RenderTarget>& source,
            const shared_ptr<const SwapChain>& swapChain,
            Filter filter = Filter::NEAREST) const {
            blit(source->getImage(), swapChain, filter);
        }

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
            const vector<shared_ptr<const RenderTarget>>& renderTargets,
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

    /**
     * Main Vireo abstraction class
     */
    class Vireo {
    public:
        /**
         * Creates a new Vireo class using the given backend
         */
        static unique_ptr<Vireo> create(Backend backend);

        virtual ~Vireo() = default;

        virtual void waitIdle() {}

        /**
         * Creates a swap chain.
         * @param format            Color format of the swap chain images
         * @param presentQueue      Queue used to present the swap chain images in the window surface
         * @param presentMode       Presentation mode
         * @param windowHandle      Opaque window handle to present the swap chain images
         * @param framesInFlight    Number of concurrent swap chain images
         */
        virtual shared_ptr<SwapChain> createSwapChain(
            ImageFormat format,
            const shared_ptr<const SubmitQueue>& presentQueue,
            void* windowHandle,
            PresentMode presentMode = PresentMode::VSYNC,
            uint32_t framesInFlight = 2) const = 0;

        /**
         * Creates a submission queue
         * @param commandType Type of commands that will be used with this queue.
         * @param name Objet name for debug
         */
        virtual shared_ptr<SubmitQueue> createSubmitQueue(
            CommandType commandType,
            const wstring& name = L"SubmitQueue") const = 0;

        /**
         * Creates a fence for CPU/GPU synchronization
         * @param name Object name for debug
         */
        virtual shared_ptr<Fence> createFence(const wstring& name = L"Fence") const = 0;

        /**
         * Creates a command allocator (command pool) for a given command type
         * @param type Type of commands that will be used with command lists created from this allocator
         */
        virtual shared_ptr<CommandAllocator> createCommandAllocator(CommandType type) const = 0;

        /**
         * Creates an input vertex layout from a description
         * @param size Size in bytes of a vertex
         * @param attributesDescriptions Description of all the vertex attributes
         */
        virtual shared_ptr<VertexInputLayout> createVertexLayout(
            size_t size,
            const vector<VertexAttributeDesc>& attributesDescriptions) const = 0;

        /**
         * Load a compiled shader and creates a shader module
         * @param fileName File name without the extension. The file name extension must be `.spv`for Vulkan et `.dxil`
         * for DirectX
         */
        virtual shared_ptr<ShaderModule> createShaderModule(
            const string& fileName) const = 0;

        /**
         * Creates a pipeline resources description. Describe resources that can be accessed by
         * the shaders associated with the future pipelines.
         * @param descriptorLayouts Descriptions of the resources (can be empty)
         * @param pushConstant Description of an optional push constant
         * @param name Object name for debug
         */
        virtual shared_ptr<PipelineResources> createPipelineResources(
            const vector<shared_ptr<DescriptorLayout>>& descriptorLayouts,
            const PushConstantsDesc& pushConstant = {},
            const wstring& name = L"PipelineResource") const = 0;

        /**
         * Creates a compute pipeline
         * @param pipelineResources Resources for the shader
         * @param shader The shader
         * @param name Object name for debug
         */
        virtual shared_ptr<ComputePipeline> createComputePipeline(
            const shared_ptr<PipelineResources>& pipelineResources,
            const shared_ptr<const ShaderModule>& shader,
            const wstring& name = L"ComputePipeline") const = 0;

        /**
         * Creates a graphic pipeline. At least one shader must be used.
         * @param pipelineResources Resources for the shaders
         * @param vertexInputLayout Description of the input vertices
         * @param vertexShader Shader for the vertex shader
         * @param fragmentShader Shader for the fragment/pixel shader
         * @param configuration Pipeline configuration
         * @param name Object name for debug
         */
        virtual shared_ptr<GraphicPipeline> createGraphicPipeline(
            const shared_ptr<PipelineResources>& pipelineResources,
            const shared_ptr<const VertexInputLayout>& vertexInputLayout,
            const shared_ptr<const ShaderModule>& vertexShader,
            const shared_ptr<const ShaderModule>& fragmentShader,
            const GraphicPipelineConfiguration& configuration,
            const wstring& name = L"GraphicPipeline") const = 0;

        /**
         * Creates a data buffer in VRAM.
         * For types UNIFORM & TRANSFER the buffer will be created in host visible memory/upload heap type.
         * For types VERTEX & INDEX the buffer will be created in device local memory/default heap type
         * @param type Type of buffer to create.
         * @param size Size of one element in bytes
         * @param count Number of elements
         * @param name Object name for debug
         */
        virtual shared_ptr<Buffer> createBuffer(
            BufferType type,
            size_t size,
            size_t count = 1,
            const wstring& name = L"Buffer") const = 0;

        /**
         * Creates a read-only image in VRAM
         * @param format Pixel format
         * @param width With in pixels
         * @param height Height in pixels
         * @param mipLevels Number of mips levels
         * @param arraySize Number of layers/array size
         * @param name Object name for debug
         */
        virtual shared_ptr<Image> createImage(
            ImageFormat format,
            uint32_t width,
            uint32_t height,
            uint32_t mipLevels = 1,
            uint32_t arraySize = 1,
            const wstring& name = L"Image") const = 0;

        /**
         * Creates a read/write image in VRAM
         * @param format Pixel format
         * @param width With in pixels
         * @param height Height in pixels
         * @param mipLevels Number of mips levels
         * @param arraySize Number of layers/array size
         * @param name Object name for debug
         */
        virtual shared_ptr<Image> createReadWriteImage(
            ImageFormat format,
            uint32_t width,
            uint32_t height,
            uint32_t mipLevels = 1,
            uint32_t arraySize = 1,
            const wstring& name = L"RWImage") const = 0;

        /**
         * Creates a read/write image in VRAM for use as a render target
         * @param format Pixel format
         * @param width With in pixels
         * @param height Height in pixels
         * @param type Type of render target use
         * @param clearValue A clear value used for optimized clearing. Must be the same as the clear value used when rendering.
         * @param msaa Number of samples for MSAA. A value of 1 disables MSAA.
         * @param name Object name for debug
         */
        virtual shared_ptr<RenderTarget> createRenderTarget(
            ImageFormat format,
            uint32_t width,
            uint32_t height,
            RenderTargetType type = RenderTargetType::COLOR,
            ClearValue clearValue = {},
            MSAA msaa = MSAA::NONE,
            const wstring& name = L"RenderTarget") const = 0;

        /**
         * Creates a read/write image in VRAM for use as a render target with a similar format as a swap chain.
         * @param swapChain Swap chain to copy format & size from
         * @param clearValue A clear value used for optimized clearing. Must be the same as the clear value used when
         * rendering.
         * @param msaa Number of samples for MSAA. A value of 1 disables MSAA.
         * @param name Object name for debug
         */
        virtual shared_ptr<RenderTarget> createRenderTarget(
            const shared_ptr<const SwapChain>& swapChain,
            ClearValue clearValue = {},
            MSAA msaa = MSAA::NONE,
            const wstring& name = L"RenderTarget") const = 0;

        /**
         * Creates an empty description layout.
         * @param name Object name for debug
         */
        virtual shared_ptr<DescriptorLayout> createDescriptorLayout(
            const wstring& name = L"DescriptorLayout") = 0;

        /**
         * Creates an empty description layout for Sampler resources types
         * @param name Object name for debug
         */
        virtual shared_ptr<DescriptorLayout> createSamplerDescriptorLayout(
            const wstring& name = L"createSamplerDescriptorLayout") = 0;

        /**
         * Creates an empty descriptor set
         * @param layout Layout of the set
         * @param name Object name for debug
         */
        virtual shared_ptr<DescriptorSet> createDescriptorSet(
            const shared_ptr<const DescriptorLayout>& layout,
            const wstring& name = L"DescriptorSet") = 0;

        /**
         * Creates a texture sampler.
         */
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