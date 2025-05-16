/*
* Copyright (c) 2025-present Henri Michelon
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/
export module vireo;
#undef DOMAIN
export import vireo.tools;

export namespace vireo {

    /**
     * Type of supported backends
     *
     * Manual page : \ref manual_010_vireo_class
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
     *
     * Manual page : \ref manual_030_03_resources
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
     * coordinate.
     *
     * Manual page : \ref manual_030_03_resources
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
     * lookups.
     *
     * Manual page : \ref manual_030_03_resources
     */
    enum class MipMapMode {
        //! Nearest filtering
        NEAREST,
        //! Linear filtering
        LINEAR,
    };

    /**
     * Available image formats. Only the common formats between the supported back ends are available.
     *
     * Manual page : \ref manual_030_02_resources
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
     *
     * Manual page : \ref manual_030_01_resources
     */
    enum class BufferType {
        //! Used to store vertices (in GPU memory)
        VERTEX,
        //! Used to store indices (in GPU memory)
        INDEX,
        //! Used for shader uniform (in host visible memory)
        UNIFORM,
        //! Used for vertex and index buffers copy operations (from host visible memory to GPU memory)
        BUFFER_UPLOAD,
        //! Used for image copy operations (from host visible memory to GPU memory)
        IMAGE_UPLOAD,
        //! Used for image copy operations (from GPU memory to host visible)
        IMAGE_DOWNLOAD,
};

    /**
     * Index type for vertex indices
     *
     * Manual page : \ref manual_080_01_graphic_pipelines
     */
    enum class IndexType {
        //! Two bytes unsigned int
        UINT16,
        //! Four bytes unsigned int
        UINT32,
    };

    /**
     * Type of data for shaders
     *
     * Manual page : \ref manual_040_00_descriptors
     */
    enum class DescriptorType {
        //! Uniform buffer
        UNIFORM,
        //! Dynamic binding uniform buffer
        UNIFORM_DYNAMIC,
        //! Sampled texture (image only, no sampler)
        SAMPLED_IMAGE,
        //! Sampler for SAMPLED_IMAGE
        SAMPLER,
        //! Read/Write image for compute shaders
        READWRITE_IMAGE,
    };

    /**
     * Type of command or submit queue
     *
     * Manual pages : \ref manual_050_00_commands, \ref manual_060_00_queues
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
     *
     * Manual page : \ref manual_080_01_graphic_pipelines
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
     *
     * Manual page : \ref manual_080_01_graphic_pipelines
     */
    enum class RenderTargetType {
        //! Used as a color attachment
        COLOR,
        //! Used as a depth attachment
        DEPTH,
        //! Used as a depth and stencil attachment
        DEPTH_STENCIL,
    };

    /**
     * Values controlling triangle culling. Triangle orientation is specified in the pipeline configuration.
     *
     * Manual page : \ref manual_080_01_graphic_pipelines
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
     *
     * Manual page : \ref manual_080_01_graphic_pipelines
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
    *
    * Manual page : \ref manual_080_01_graphic_pipelines
    */
    enum class PolygonMode {
        //! Polygon is filled
        FILL,
        //! Polygon edges are drawn as line segments
        WIREFRAME,
    };

    /**
     * Comparison operator for depth, stencil, and sampler operations
     *
     * Manual page : \ref manual_080_01_graphic_pipelines
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

    /*
     * Stencil comparison function
     *
     * Manual page : \ref manual_080_01_graphic_pipelines
     */
    enum class StencilOp {
        //! Keeps the current value.
        KEEP = 0,
        //! Sets the value to 0.
        ZERO = 1,
        //! Sets the value to the reference value
        REPLACE = 2,
        //! Increments the current value and clamps to the maximum representable unsigned value.
        INCREMENT_AND_CLAMP = 3,
        //! Decrements the current value and clamps to 0.
        DECREMENT_AND_CLAMP = 4,
        //! Bitwise-inverts the current value.
        INVERT = 5,
        //! Increments the current value and wraps to 0 when the maximum value would have been exceeded.
        INCREMENT_AND_WRAP = 6,
        //! Decrements the current value and wraps to the maximum possible value when the value would go below 0.
        DECREMENT_AND_WRAP = 7,
    };

    /**
     * Framebuffer blending factors.
     * cf. https://registry.khronos.org/vulkan/specs/latest/man/html/VkBlendFactor.html
     *
     * Manual page : \ref manual_080_01_graphic_pipelines
     */
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

    /**
     * Framebuffer blending operations.
     * cf. https://registry.khronos.org/vulkan/specs/latest/man/html/VkBlendOp.html
     *
     * Manual page : \ref manual_080_01_graphic_pipelines
     */
    enum class BlendOp {
        ADD,
        SUBTRACT,
        REVERSE_SUBTRACT,
        MIN,
        MAX
    };

    /**
     * Framebuffer logical operations.
     * cf. https://registry.khronos.org/vulkan/specs/latest/man/html/VkLogicOp.html
     *
     * Manual page : \ref manual_080_01_graphic_pipelines
     */
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

    /*
     * Bitmask controlling which components are written to the framebuffer
     *
     * Manual page : \ref manual_080_01_graphic_pipelines
    */
    enum class ColorWriteMask : uint8_t {
        RED    = 0x00000001,
        GREEN  = 0x00000002,
        BLUE   = 0x00000004,
        ALPHA  = 0x00000008,
        ALL    = 0x0000000f,
    };

    /**
     * Graphic pipeline color blend attachment description
     *
     * Manual page : \ref manual_080_01_graphic_pipelines
     */
    struct ColorBlendDesc {
        //! Controls whether blending is enabled for the corresponding color attachment. If blending is not enabled, the source fragment’s color for that attachment is passed through unmodified.
        bool            blendEnable{false};
        //! Selects which blend factor is used to determine the source factors (Sr,Sg,Sb).
        BlendFactor     srcColorBlendFactor{BlendFactor::SRC_ALPHA};
        //! Selects which blend factor is used to determine the destination factors (Dr,Dg,Db).
        BlendFactor     dstColorBlendFactor{BlendFactor::ONE_MINUS_SRC_ALPHA};
        //! Selects which blend operation is used to calculate the RGB values to write to the color attachment.
        BlendOp         colorBlendOp{BlendOp::ADD};
        //! Selects which blend factor is used to determine the source factor Sa.
        BlendFactor     srcAlphaBlendFactor{BlendFactor::ONE};
        //! Selects which blend factor is used to determine the destination factor Da.
        BlendFactor     dstAlphaBlendFactor{BlendFactor::ZERO};
        //! Selects which blend operation is used to calculate the alpha values to write to the color attachment.
        BlendOp         alphaBlendOp{BlendOp::ADD};
        //! Is a bitmask specifying which of the R, G, B, and/or A components are enabled for writing
        ColorWriteMask  colorWriteMask{ColorWriteMask::ALL};
    };

    /**
     * Structure specifying stencil operation state
     *
     * Manual page : \ref manual_080_01_graphic_pipelines
     */
    struct StencilOpState {
        //! Value specifying the action performed on samples that fail the stencil test.
        StencilOp failOp{StencilOp::KEEP};
        //! Value specifying the action performed on samples that pass both the depth and stencil tests.
        StencilOp passOp{StencilOp::KEEP};
        //! Value specifying the action performed on samples that pass the stencil test and fail the depth test.
        StencilOp depthFailOp{StencilOp::KEEP};
        //! Value specifying the comparison operator used in the stencil test.
        CompareOp compareOp{CompareOp::ALWAYS};
        //! Selects the bits of the unsigned integer stencil values participating in the stencil test.
        uint32_t  compareMask{0xFFFFFFFF};
        //! Selects the bits of the unsigned integer stencil values updated by the stencil test in the stencil attachment
        uint32_t  writeMask{0xFFFFFFFF};
    };

    /**
     * Pipeline stage
     *
     * Manual page : \ref manual_070_00_shaders
     */
    enum class ShaderStage {
        //! All shader stages supported by the device
        ALL,
        //! Vertex shader stage
        VERTEX,
        //! Fragment/pixel stage
        FRAGMENT,
        //! Tessellation control/Hull stage
        HULL,
        //! Tessellation evaluation/Domain stage
        DOMAIN,
        //! Geometry stage
        GEOMETRY,
        //! Compute stage
        COMPUTE,
    };

    /**
     * Semaphores wait stages
     *
     * Manual page : \ref manual_090_02_semaphores
     */
    enum class WaitStage {
        NONE,
        PIPELINE_TOP,
        VERTEX_INPUT,
        VERTEX_SHADER,
        DEPTH_STENCIL_TEST_BEFORE_FRAGMENT_SHADER,
        FRAGMENT_SHADER,
        DEPTH_STENCIL_TEST_AFTER_FRAGMENT_SHADER,
        COLOR_OUTPUT,
        COMPUTE_SHADER,
        TRANSFER,
        PIPELINE_BOTTOM,
        ALL_GRAPHICS,
        ALL_COMMANDS,
        COPY,
        RESOLV,
        BLIT,
        CLEAR,
    };

    /**
     *  State/Layout of images for pipeline barriers
     *
     * Manual page : \ref manual_090_03_barriers
     */
    enum class ResourceState {
        //! Unknown/general/common state
        UNDEFINED,
        //! All types of device access
        GENERAL,
        //! Used as a color attachment in the graphics pipeline
        RENDER_TARGET_COLOR,
        //! Used as a depth attachment in the graphics pipeline
        RENDER_TARGET_DEPTH,
        //! Used as a depth attachment in the graphics pipeline
        RENDER_TARGET_DEPTH_READ,
        //! Used as a depth and stencil attachment in the graphics pipeline
        RENDER_TARGET_DEPTH_STENCIL,
        //! Used as a depth and stencil attachment in the graphics pipeline
        RENDER_TARGET_DEPTH_STENCIL_READ,
        //! Used as a read/write image with a compute pipeline
        DISPATCH_TARGET,
        //! Used for presenting a presentable image for display.
        PRESENT,
        //! Used as a source image of a transfer/copy/blit command
        COPY_SRC,
        //! Used as a destination image of a transfer/copy/blit command
        COPY_DST,
        //! Read-only access as an attachment or in shaders as a sampled image
        SHADER_READ,
        //! Read-only access in a compute  shader
        COMPUTE_READ,
    };

    /**
     * Sample counts supported for an image used for storage operations
     *
     * Manual pages : \ref manual_080_01_graphic_pipelines and \ref manual_100_00_renderpass
     */
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

    /**
     * Presentation mode for a surface
     *
     * Manual page : \ref manual_110_00_swapchain
     */
    enum class PresentMode {
        //! The presentation engine does not wait for a vertical blanking period to update the current image, meaning this mode may result in visible tearing.
        IMMEDIATE = 0,
        //! The presentation engine waits for the next vertical blanking period to update the current image. Tearing cannot be observed.
        VSYNC     = 1,
    };

    /**
     * Pipeline type
     *
     * Manual page : \ref manual_080_00_pipelines
     */
    enum class PipelineType {
        GRAPHIC,
        COMPUTE
    };

    /**
     * Semaphores types.
     *
     * Manual page : \ref manual_090_02_semaphores
     */
    enum class SemaphoreType {
        //! Binary semaphores have two states - signaled and unsignaled.
        BINARY,
        //! Timeline semaphores have a strictly increasing 64-bit unsigned integer payload and are signaled with respect to a particular reference value.
        TIMELINE,
    };

    /**
     * Index type used for binding resources with a descriptor set
     *
     * Manual page : \ref manual_040_00_descriptors
     */
    using DescriptorIndex = uint32_t;

    /**
     * Description of a video adapter
     *
     * Manual page : \ref manual_020_devices
     */
    struct PhysicalDeviceDesc {
        //! Adapter name
        std::wstring name{L"Unknown"};
        //! The number of bytes of dedicated video memory that are not shared with the CPU.
        size_t  dedicatedVideoMemory{0};
        //! The number of bytes of dedicated system memory that are not shared with the CPU
        size_t  dedicatedSystemMemory{0};
        //! The number of bytes of shared system memory
        size_t  sharedSystemMemory{0};
    };

    /**
     * A two-dimensional extent
     */
    struct Extent {
        //! width of the extent
        uint32_t width;
        //! height of the extent
        uint32_t height;
    };

    /**
     *  Structure specifying two-dimensional subregion
    */
    struct Rect {
        //! Rect’s upper left corner (x,y).
        int32_t    x{0};
        //! Rect’s upper left corner (x,y).
        int32_t    y{0};
        //! Rect’s width
        uint32_t   width{0};
        //! Rect’s height
        uint32_t   height{0};
    };

    /**
     *  Structure specifying a viewport
    */
    struct Viewport {
        //! Viewport’s upper left corner (x,y).
        float    x{0.0f};
        //! Viewport’s upper left corner (x,y).
        float    y{0.0f};
        //! Viewport’s width
        float   width{0.0f};
        //! Viewport’s height
        float   height{0.0f};
        //! min depth range for the viewport.
        float    minDepth{0.0f};
        //! max depth range for the viewport.
        float    maxDepth{1.0f};
    };

    /**
     * A push constant range
     *
     * Manual page : \ref manual_040_03_push_constants
     */
    struct PushConstantsDesc {
        //! Shader stages that will access a range of push constants
        ShaderStage stage{ShaderStage::ALL};
        //! Size of push constant data in bytes
        uint32_t    size{0};
        //! Offset of push constant data in bytes
        uint32_t    offset{0};
    };

    /**
     * Vertex input attribute description
     *
     * Manual page : \ref manual_080_01_graphic_pipelines
     */
    struct VertexAttributeDesc {
        //! Binding name which this attribute takes its data from.
        std::string          binding;
        //! Size and type of the vertex attribute data
        AttributeFormat format;
        //! Byte offset of this attribute relative to the start of an element in the vertex input binding.
        uint32_t        offset;
    };

    /**
     * A clear depth stencil value
     *
     * Manual page : \ref manual_100_00_renderpass
     */
    struct DepthClearValue {
        //! Clear value for the depth aspect of the depth/stencil attachment. It is a floating-point value which is automatically converted to the attachment’s forma
        float    depth{1.0f};
        //! Clear value for the stencil aspect of the depth/stencil attachment. It is a 32-bit integer value which is converted to the attachment’s format by taking the appropriate number of LSBs.
        uint32_t stencil{0};
    };

    /**
     * Structure specifying a clear value
     *
     * Manual page : \ref manual_100_00_renderpass
     */
    typedef union ClearValue {
        //! The color image clear values to use when clearing a color image or attachment.
        float           color[4]{0.0f, 0.0f, 0.0f, 1.0f};
        //! The depth and stencil clear values to use when clearing a depth/stencil image or attachment.
        DepthClearValue depthStencil;
    } ClearValue;

    /**
     * A fence object. Fences are a synchronization primitive that can be used to insert a dependency from a queue to
     * the host (CPU/GPU synchronization).
     *
     * Manual page : \ref manual_090_01_fences
     */
    class Fence {
    public:
        //! Wait for the fences to become signaled
        virtual void wait() const = 0;

        //! Reset the fence state
        virtual void reset() = 0;

        virtual ~Fence() = default;
    };

    /**
     * A Semaphore object.
     * Semaphores are a synchronization primitive that can be used to insert a dependency between queue operations
     * (GPU/GPU synchronization)
     *
     * Manual page : \ref manual_090_02_semaphores
     */
    class Semaphore {
    public:
        Semaphore(const SemaphoreType type) : type{type} {}

        /**
         * Return the type of the semaphore
         */
        auto getType() const { return type; }

        /**
         * Returns the current integer value of the semaphore (the value that will be used in the next
         * wait or signal command)
        */
        auto getValue() const { return value; }

        /**
         * Sets the current integer value of the semaphore (the value that will be used in the next
         * wait or signal command)
        */
        void setValue(const uint64_t value) { this->value = value; }

        /**
         * Increments the current integer value of the semaphore (the value that will be used in the next
         * wait or signal command)
        */
        void incrementValue() { value++; }

        /**
         * Decrements the current integer value of the semaphore (the value that will be used in the next
         * wait or signal command)
        */
        void decrementValue() { value--; }

        virtual ~Semaphore() = default;

    protected:
        const SemaphoreType type;
        uint64_t value{0};
    };

    /**
     * Backend instance object
     *
     * Manual page : \ref manual_020_devices
     */
    class Instance {
    public:
        virtual ~Instance() = default;

    protected:
        Instance() = default;
    };

    /**
     * A physical device object (adapter)
     *
     * Manual page : \ref manual_020_devices
     */
    class PhysicalDevice {
    public:
        virtual ~PhysicalDevice() = default;

        /**
         * Returns the graphic adapter description
         */
        virtual const PhysicalDeviceDesc getDescription() const = 0;

    protected:
        PhysicalDevice() = default;
    };

    /**
     * A logical device object
     *
     * Manual page : \ref manual_020_devices
     */
    class Device {
    public:
        virtual ~Device() = default;

    protected:
        Device() = default;
    };

    /**
     * A buffer object.
     * Buffers represent linear arrays of data which are used for various purposes by binding them to a
     * graphics or compute pipeline via descriptor sets or certain commands,
     * or by directly specifying them as parameters to certain commands.
     *
     * Manual page : \ref manual_030_01_resources
     */
    class Buffer {
    public:
        static constexpr size_t WHOLE_SIZE = ~0ULL;

        virtual ~Buffer() = default;

        /**
         * Returns the total buffer size in bytes
         */
        auto getSize() const { return bufferSize; }

        /**
         * Returns the type of the buffer
         */
        auto getType() const { return type; }

        /**
         * Returns the size of a data instance, in bytes
         */
        auto getInstanceSize() const { return instanceSize; }

        /**
         * Returns the aligned size of a data instance, in bytes
         */
        auto getInstanceSizeAligned() const { return instanceSizeAligned; }

        /**
         * Returns the number of data instances
         */
        auto getInstanceCount() const { return instanceCount; }

        /**
         * Returns a host virtual memory address. Valid only after a `map()` call.
         */
        auto getMappedAddress() const { return mappedAddress; }

        /**
         * Maps the device memory associated with the buffer into a host adress sapce
         */
        virtual void map() = 0;

        /**
         *  Unmap a previously mapped buffer
         */
        virtual void unmap() = 0;

        /**
         * Writes data into the host mapped memory associated. Buffer must be mapped before.
         * @param data Source data address in the host address space
         * @param size Size of the data in bytes
         * @param offset Destination offset in bytes
         */
        void write(const void* data, size_t size = WHOLE_SIZE, size_t offset = 0) const;

    protected:
        size_t bufferSize{0};
        uint32_t instanceSize{0};
        uint32_t instanceCount{0};
        uint32_t instanceSizeAligned{0};
        void*  mappedAddress{nullptr};

        Buffer(const BufferType type): type{type} {}

    private:
        const BufferType type;
    };

    /**
     * A texture sampler object
     *
     * Manual page : \ref manual_030_03_resources
     */
    class Sampler {
    public:
        static constexpr float LOD_CLAMP_NONE = 3.402823466e+38f;

        virtual ~Sampler() = default;

    protected:
        Sampler() = default;
    };

    /**
     * An image object
     *
     * Manual page : \ref manual_030_02_resources
     */
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

        /**
         * Returns the pixel format
         */
        auto getFormat() const { return format; }

        /**
         * Returns the width of the first layer in pixels
         */
        auto getWidth() const { return width; }

        /**
         * Returns the height of the first layer in pixels
         */
        auto getHeight() const { return height; }

        /**
         * Returns the number of mips levels
         */
        auto getMipLevels() const { return mipLevels; }

        /**
         * Returns the number of layers
         */
        auto getArraySize() const { return arraySize; }

        /**
         * Return the size in bytes of each row
         */
        auto getRowPitch() const { return width * pixelSize[static_cast<int>(format)]; }

        /**
         * Return the size in bytes of the first layer
         */
        auto getImageSize() const { return getRowPitch() * height; }

        /**
         * Return the size in bytes of the first layer, with aligned rows
         */
        auto getAlignedImageSize() const { return getAlignedRowPitch() * height; }

        /**
         * Return the size in bytes of aligned rows
         */
        virtual uint32_t getAlignedRowPitch() const { return getRowPitch(); }

        /**
         * Return `true` if the image have read/write access
         */
        auto isReadWrite() const { return readWrite; }

        /**
         * Returns the number of bytes for one pixel
         */
        static auto getPixelSize(const ImageFormat format) { return pixelSize[static_cast<int>(format)]; }

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

    /**
     * A color or depth render attachment
     *
     * Manual page : \ref manual_100_00_renderpass
     */
    class RenderTarget {
    public:
        virtual ~RenderTarget() = default;

        /**
         * Return the associated image
         */
        auto getImage() const { return image; }

        /**
         * Return the type of the attachment
         */
        auto getType() const { return type; }

        RenderTarget(const RenderTargetType type, const std::shared_ptr<Image>& image) :
            type{type},
            image{image} {}

    private:
        const RenderTargetType  type;
        const std::shared_ptr<Image> image;
    };

    /**
     * A descriptor set layout object.
     * Describes resources that shaders can use.
     * Add resources with `add()` then call `build()` to build the layout.
     * Note that samplers must be bounds to a specific sampler-only layout.
     *
     * Manual page : \ref manual_040_01_descriptor_layout
     */
    class DescriptorLayout {
    public:
        virtual ~DescriptorLayout() = default;

        /**
         * Add a resource to the layout
         * @param index Binding index
         * @param type Type of resource
         * @param count Number of instances
         */
        virtual DescriptorLayout& add(DescriptorIndex index, DescriptorType type, size_t count = 1) = 0;

        /**
         * Build the layout after adding resource descriptions with `add()`
         */
        virtual void build() {}

        /**
         * Returns the total number of resources instances
         */
        auto getCapacity() const { return capacity; }

        /**
         * Returns `true` if the layout describes one dynamic uniform buffer
         */
        auto isDynamicUniform() const { return dynamic; }

        /**
         * Returns `true` if the layout is for samplers only
         */
        auto isSamplers() const { return samplers; }

    protected:
        size_t capacity{0};
        bool   samplers{false};
        bool   dynamic{false};

        DescriptorLayout(bool samplers, bool dynamic): samplers{samplers}, dynamic{dynamic} {}
    };

    /**
     * A descriptor set object.
     * Contains resources for the shaders.
     *
     * Manual page : \ref manual_040_02_descriptor_set
     */
    class DescriptorSet {
    public:
        virtual ~DescriptorSet() = default;

        /**
         * Bind an uniform buffer
         * @param index Binding index
         * @param buffer The buffezr
         */
        virtual void update(DescriptorIndex index, const std::shared_ptr<const Buffer>& buffer) = 0;

        /**
         * Bind a dynamic uniform buffer
         * @param buffer The buffer
         */
        void update(const std::shared_ptr<const Buffer>& buffer) {
            update(0, buffer);
        }

        /**
         * Bind a texture
         * @param index Binding index
         * @param image The texture
         */
        virtual void update(DescriptorIndex index, const std::shared_ptr<const Image>& image) const = 0;

        /**
         * Bind a sampler
         * @param index Binding index
         * @param sampler The sampler
         */
        virtual void update(DescriptorIndex index, const std::shared_ptr<const Sampler>& sampler) const = 0;

        /**
         * Bind an array of textures
         * @param index Binding index
         * @param images The images
         */
        virtual void update(DescriptorIndex index, const std::vector<std::shared_ptr<Image>>& images) const = 0;

        /**
         * Bind an array of buffers
         * @param index Binding index
         * @param buffers The buffers
         */
        virtual void update(DescriptorIndex index, const std::vector<std::shared_ptr<Buffer>>& buffers) = 0;

        /**
         * Bind an array of samplers
         * @param index Binding index
         * @param samplers The samplers
         */
        virtual void update(DescriptorIndex index, const std::vector<std::shared_ptr<Sampler>>& samplers) const = 0;

        const auto& getLayout() const { return layout; }

    protected:
        const std::shared_ptr<const DescriptorLayout> layout;

        DescriptorSet(const std::shared_ptr<const DescriptorLayout>& layout) : layout{layout} {}
    };

    /**
     * An input vertex layout
     *
     * Manual page : \ref manual_080_01_graphic_pipelines
     */
    class VertexInputLayout {
    public:
        virtual ~VertexInputLayout() = default;

    protected:
        VertexInputLayout() = default;
    };

    /**
     * A shader module object
     *
     * Manual page : \ref manual_070_00_shaders
     */
    class ShaderModule {
    public:
        virtual ~ShaderModule() = default;

    protected:
        ShaderModule() = default;
    };

    /**
     * All resources used by the shaders of a pipeline : descriptor layouts & push constants
     *
     * Manual page : \ref manual_040_04_pipeline_resources
     */
    class PipelineResources {
    public:
        virtual ~PipelineResources() = default;

    protected:
        PipelineResources() = default;
    };

    /**
     * Base class for all pipeline types
     *
     * Manual page : \ref manual_080_00_pipelines
     */
    class Pipeline {
    public:
        virtual ~Pipeline() = default;

        /**
         * Returns the pipeline resources
         */
        auto getResources() const { return pipelineResources; }

        /**
         * Return the type of the pipeline
         */
        auto getType() const { return type; }

    protected:
        Pipeline(const PipelineType type, const std::shared_ptr<PipelineResources>& pipelineResources) :
            type{type},
            pipelineResources{pipelineResources} {}

    private:
        const PipelineType type;
        std::shared_ptr<PipelineResources> pipelineResources;
    };

    /**
     * A compute pipeline
     *
     * Manual page : \ref manual_080_02_compute_pipelines
     */
    class ComputePipeline : public Pipeline {
    protected:
        ComputePipeline(const std::shared_ptr<PipelineResources>& pipelineResources) :
            Pipeline{PipelineType::COMPUTE, pipelineResources} {}
    };

    /**
     * A graphic pipeline
     *
     * Manual page : \ref manual_080_01_graphic_pipelines
     */
    class GraphicPipeline : public Pipeline {
    protected:
        GraphicPipeline(const std::shared_ptr<PipelineResources>& pipelineResources) :
            Pipeline{PipelineType::GRAPHIC, pipelineResources} {}
    };

    class SwapChain;

    /**
     * Color attachments description
     *
     * Manual page : \ref manual_100_00_renderpass
     */
    struct RenderTargetDesc {
        //! Use the current swap chain image as a color attachment. Must be `nullptr` if `renderTarget` is set
        std::shared_ptr<SwapChain>    swapChain{nullptr};
        //! Color attachment. Must be `nullptr` if `swapChain` is set
        std::shared_ptr<RenderTarget> renderTarget{nullptr};
        //! Multisampled color attachment. `nullptr` if MSAA is disabled for the current pipeline.
        std::shared_ptr<RenderTarget> multisampledRenderTarget{nullptr};
        //! Clear the color attachment if `true`
        bool                     clear{false};
        //! Color clear value
        ClearValue               clearValue{ .color = {0.0f, 0.0f, 0.0f, 0.0f} };
        //! Discard the content after rendering
        bool                     discardAfterRender{false};
    };

    /**
     * Color & depth attachments descriptions
     *
     * Manual page : \ref manual_100_00_renderpass
     */
    struct RenderingConfiguration {
        //! Color attachments. Can be empty
        std::vector<RenderTargetDesc>  colorRenderTargets{};
        //! Depth and stencil attachment. Can be empty
        std::shared_ptr<RenderTarget>  depthStencilRenderTarget{nullptr};
        //! Multisampled depth attachment. `nullptr` if MSAA is disabled for the current pipeline.
        std::shared_ptr<RenderTarget>  multisampledDepthStencilRenderTarget{nullptr};
        //! `true` is the depth and stencil attachment is used for depth testing
        bool                           depthTestEnable{false};
        //! `true` is the depth and stencil attachment is used for stencil testing
        bool                           stencilTestEnable{false};
        //! Clear the depth and stencil attachment if `true`
        bool                           clearDepthStencil{false};
        //! Depth and stencil clear value
        ClearValue                     depthStencilClearValue{ .depthStencil = {1.0f, 0} };
        //! Discard the content of the depth and stencil attachment after rendering
        bool                           discardDepthStencilAfterRender{false};
    };

    /**
     * Batch buffer upload infos
     */
    struct BufferUploadInfo {
        const std::shared_ptr<Buffer> buffer;
        const void* data;
    };


    /**
     * Batch image upload infos
     */
    struct ImageUploadInfo {
        const std::shared_ptr<Image> image;
        const void* data;
    };


    /**
     * A command list (buffer) object
     *
     * Manual page : \ref manual_050_00_commands
     */
    class CommandList {
    public:
        static constexpr uint32_t indexTypeSize[] = {
            2,
            4
        };

        /**
         * Start recording a command list
         */
        virtual void begin() const = 0;

        /**
         * Stop recording a command list
         */
        virtual void end() const = 0;

        /**
         * Uploads data into a buffer using a temporary (staging) buffer.
         */
        virtual void upload(
            const std::shared_ptr<const Buffer>& destination,
            const void* source) = 0;

        /**
         * Uploads data into buffers using temporary (staging) buffers.
         */
        virtual void upload(const std::vector<BufferUploadInfo>& infos);

        /**
         * Upload data into an image using a temporary (staging) buffer.
         */
        virtual void upload(
            const std::shared_ptr<const Image>& destination,
            const void* source,
            uint32_t firstMipLevel = 0) = 0;

        /**
         * Uploads data into images using temporary (staging) buffers.
         */
        virtual void upload(const std::vector<ImageUploadInfo>& infos);

        /**
        * Copy a buffer into an image
         */
        virtual void copy(
            const std::shared_ptr<Buffer>& source,
            const std::shared_ptr<const Image>& destination,
            uint32_t sourceOffset = 0,
            uint32_t firstMipLevel = 0) = 0;

        /**
        * Copy an image into a buffer
        */
        virtual void copy(
           const std::shared_ptr<const Image>& destination,
           const std::shared_ptr<Buffer>& source,
           uint32_t destinationOffset = 0,
           uint32_t firstMipLevel = 0) = 0;

        /**
         * Copy a buffer into another buffer
         */
        virtual void copy(
            const std::shared_ptr<Buffer>& source,
            const std::shared_ptr<Buffer>& destination,
            size_t size = Buffer::WHOLE_SIZE,
            uint32_t sourceOffset = 0,
            uint32_t destinationOffset = 0) = 0;

        /**
         * Upload images into an image array
         */
        virtual void uploadArray(
            const std::shared_ptr<const Image>& destination,
            const std::vector<void*>& sources,
            uint32_t firstMipLevel = 0) = 0;

        /**
         * Copy an image into the current swap chain image
         */
        virtual void copy(
            const std::shared_ptr<const Image>& source,
            const std::shared_ptr<const SwapChain>& swapChain) const = 0;

        /**
         * Copy an image into the current swap chain image
         */
        inline void copy(
            const std::shared_ptr<const RenderTarget>& source,
            const std::shared_ptr<const SwapChain>& swapChain) const {
            copy(source->getImage(), swapChain);
        }

        /**
         * Begin a render pass
         */
        virtual void beginRendering(const RenderingConfiguration& configuration) = 0;

        /**
         * End a render pass
         */
        virtual void endRendering() {}

        /**
         * Dispatch compute work items
         * @param x The number of local workgroups to dispatch in the X dimension.
         * @param y The number of local workgroups to dispatch in the Y dimension.
         * @param z The number of local workgroups to dispatch in the Z dimension.
         */
        virtual void dispatch(uint32_t x, uint32_t y, uint32_t z) const = 0;

        /**
         * Bind a vertex buffer to a command list
         * @param buffer A vertex buffer
         * @param offset Offset in bytes
         */
        virtual void bindVertexBuffer(
            const std::shared_ptr<const Buffer>& buffer,
            size_t offset = 0) const = 0;

        /**
         * Binds vertex buffers to a command list
         * @param buffers Buffers to bind
         * @param offsets Offsets for each buffer in bytes
         */
        virtual void bindVertexBuffers(
            const std::vector<std::shared_ptr<const Buffer>>& buffers,
            std::vector<size_t> offsets = {}) const = 0;

        /**
         * Binds an index buffer to a command list
         * @param buffer An index buffer
         * @param indexType Value specifying the size of the indices
         * @param firstIndex First index in the buffer
         */
        virtual void bindIndexBuffer(
            const std::shared_ptr<const Buffer>& buffer,
            IndexType indexType = IndexType::UINT32,
            uint32_t firstIndex = 0) const = 0;

        /**
         * Binds a pipeline object to a command list
         */
        virtual void bindPipeline(const std::shared_ptr<const Pipeline>& pipeline) = 0;

        /**
         * Sets descriptor sets to bind a command list. All used descriptor sets must be sets.
         * @param descriptors The descriptor sets to attach
         */
        virtual void setDescriptors(const std::vector<std::shared_ptr<const DescriptorSet>>& descriptors) const {}

        /**
         * Binds descriptor sets to a command list
         * @param pipeline The pipeline that will use the descriptors
         * @param descriptors The descriptor sets to bind
         * @param firstSet The set number of the first descriptor set to be bound
         */
        virtual void bindDescriptors(
            const std::shared_ptr<const Pipeline>& pipeline,
            const std::vector<std::shared_ptr<const DescriptorSet>>& descriptors,
            uint32_t firstSet = 0) const = 0;

        /**
         * Binds descriptor set to a command list
         * @param pipeline The pipeline that will use the descriptors
         * @param descriptor The descriptor set to bind
         * @param set The set number of the descriptor set to be bound
        */
        virtual void bindDescriptor(
            const std::shared_ptr<const Pipeline>& pipeline,
            const std::shared_ptr<const DescriptorSet>& descriptor,
            uint32_t set) const = 0;

        /**
         * Binds a dynamic uniform descriptor set to a command list
         * @param pipeline The pipeline that will use the descriptors
         * @param descriptor The descriptor set to bind
         * @param set The set number of the descriptor set to be bound
         * @param offset Values specifying dynamic offsets for the UNIFORM_DYNAMIC resource.
        */
        virtual void bindDescriptor(
            const std::shared_ptr<const Pipeline>& pipeline,
            const std::shared_ptr<const DescriptorSet>& descriptor,
            uint32_t set,
            uint32_t offset) const = 0;

        /**
         * Draw primitives
         * @param vertexCountPerInstance The number of vertices per instance to draw
         * @param instanceCount The number of instances
         * @param firstVertex The index of the first vertex to draw
         * @param firstInstance The index of the first instance to draw.
         */
        virtual void draw(
            uint32_t vertexCountPerInstance,
            uint32_t instanceCount = 1,
            uint32_t firstVertex = 0,
            uint32_t firstInstance = 0) const = 0;

        /**
         * Draw primitives with indexed vertices
         * @param indexCountPerInstance The number of vertices per instance to draw
         * @param instanceCount The number of instances
         * @param firstIndex The base index within the index buffer.
         * @param vertexOffset The value added to the vertex index before indexing into the vertex buffer.
         * @param firstInstance The index of the first instance to draw.
         */
        virtual void drawIndexed(
            uint32_t indexCountPerInstance,
            uint32_t instanceCount = 1,
            uint32_t firstIndex = 0,
            uint32_t vertexOffset = 0,
            uint32_t firstInstance = 0) const = 0;

        /**
         * Sets the viewports for a command list
         * @param viewports An array of `Viewport` structures specifying viewport parameters
         */
        virtual void setViewports(const std::vector<Viewport>& viewports) const = 0;

        /**
         * Sets the scissors for a command list
         * @param rects An array of `Rect` structures specifying viewport parameters.
         */
        virtual void setScissors(const std::vector<Rect>& rects) const = 0;

        /**
        * Sets the viewport for a command list
        * @param viewport An array of `Extent` structures specifying viewport parameters.
        */
        virtual void setViewport(const Viewport& viewport) const = 0;

        /**
         * Sets the scissors for a command list
         * @param rect An array of `Extent` structures specifying viewport parameters.
         */
        virtual void setScissors(const Rect& rect) const = 0;

        /**
         * Set the reference value for stencil tests and operations
         */
        virtual void setStencilReference(uint32_t reference) const = 0;

        /**
         * Insert a memory dependency
         * @param image The image affected by this barrier.
         * @param oldState Old state in an image state transition.
         * @param newState New state in an image state transition.
         * @param firstMipLevel  The first mip level to include is this barrier
         * @param levelCount Number of level to include
         */
        virtual void barrier(
            const std::shared_ptr<const Image>& image,
            ResourceState oldState,
            ResourceState newState,
            uint32_t firstMipLevel = 0,
            uint32_t levelCount = 1) const = 0;

        /**
         * Insert a memory dependency
         * @param renderTarget The image affected by this barrier.
         * @param oldState Old state in an image state transition.
         * @param newState New state in an image state transition.
         */
        virtual void barrier(
            const std::shared_ptr<const RenderTarget>& renderTarget,
            ResourceState oldState,
            ResourceState newState) const = 0;

        /**
         * Insert a memory dependency
         * @param renderTargets The images affected by this barrier.
         * @param oldState Old state in an image state transition.
         * @param newState New state in an image state transition.
         */
        virtual void barrier(
            const std::vector<std::shared_ptr<const RenderTarget>>& renderTargets,
            ResourceState oldState,
            ResourceState newState) const = 0;

        /**
         * Insert a memory dependency
         * @param swapChain The image affected by this barrier.
         * @param oldState Old state in an image state transition.
         * @param newState New state in an image state transition.
         */
        virtual void barrier(
            const std::shared_ptr<const SwapChain>& swapChain,
            ResourceState oldState,
            ResourceState newState) const = 0;

        /**
         * Update the values of push constants
         * @param pipelineResources The pipeline layout used to program the push constant updates.
         * @param pushConstants The push constant description
         * @param data The new push constant values.
         */
        virtual void pushConstants(
            const std::shared_ptr<const PipelineResources>& pipelineResources,
            const PushConstantsDesc& pushConstants,
            const void* data) const = 0;

        /**
         * Cleanup staging buffers used by `upload` functions
         */
        virtual void cleanup() = 0;

        virtual ~CommandList() = default;

    protected:
        CommandList() = default;
    };

    /**
     * a command allocator (pool) object
     *
     * Manual page : \ref manual_050_00_commands
     */
    class CommandAllocator {
    public:
        /**
         * Resets the command allocator and of the associated command lists
         */
        virtual void reset() const = 0;

        /**
         * Returns a new graphic command list
         * @param pipeline Associate pipeline
         */
        virtual std::shared_ptr<CommandList> createCommandList(const std::shared_ptr<const Pipeline>& pipeline) const  = 0;

        /**
         * Returns a new command lists
         */
        virtual std::shared_ptr<CommandList> createCommandList() const  = 0;

        virtual ~CommandAllocator() = default;

        /**
         * Returns the type of command list created by this allocator
         */
        auto getCommandListType() const { return commandListType; }

    protected:
        CommandAllocator(const CommandType type) : commandListType{type} {}

    private:
        const CommandType commandListType;
    };

    /**
     * Command submission queue
     *
     * Manual page : \ref manual_060_00_queues
     */
    class SubmitQueue {
    public:
        /**
         * Submit graphics commands and synchronize the host & the device with a fence
         * @param fence Host/device synchronization fence
         * @param swapChain  Associated swap chain
         * @param commandLists Commands to execute
         */
        virtual void submit(
            const std::shared_ptr<Fence>& fence,
            const std::shared_ptr<const SwapChain>& swapChain,
            const std::vector<std::shared_ptr<const CommandList>>& commandLists) const = 0;

        /**
         * Submit graphics commands and synchronize the host & the device with a fence
         * and the GPU operations with a semaphore.
         * @param waitSemaphore GPU semaphore to wait
         * @param waitStage Stage to wait (Vulkan only)
         * @param fence Host/device synchronization fence
         * @param swapChain  Associated swap chain
         * @param commandLists Commands to execute
         */
        virtual void submit(
            const std::shared_ptr<Semaphore>& waitSemaphore,
            WaitStage waitStage,
            const std::shared_ptr<Fence>& fence,
            const std::shared_ptr<const SwapChain>& swapChain,
            const std::vector<std::shared_ptr<const CommandList>>& commandLists) const = 0;

        /**
         * Submit graphics commands and synchronize the host & the device with a fence
         * and the GPU operations with a semaphore.
         * @param waitSemaphore GPU semaphore to wait  (must be a timeline semaphore)
         * @param waitStages Stages to wait.  The queue will wait for two incremental values of the semaphore.
         * @param fence Host/device synchronization fence
         * @param swapChain  Associated swap chain
         * @param commandLists Commands to execute
         */
        virtual void submit(
            const std::shared_ptr<Semaphore>& waitSemaphore,
            const std::vector<WaitStage>& waitStages,
            const std::shared_ptr<Fence>& fence,
            const std::shared_ptr<const SwapChain>& swapChain,
            const std::vector<std::shared_ptr<const CommandList>>& commandLists) const = 0;

        /**
         * Submit commands without synchronization
         * @param commandLists Commands to execute
         */
        virtual void submit(const std::vector<std::shared_ptr<const CommandList>>& commandLists) const = 0;

        /**
         * Submit commands with CPU/GPU synchronization
         * @param fence Host/device synchronization fence
         * @param commandLists Commands to execute
         */
        virtual void submit(
            const std::shared_ptr<Fence>& fence,
            const std::vector<std::shared_ptr<const CommandList>>& commandLists) const = 0;

        /**
         * Submit commands with GPU/GPU synchronization
         * @param waitSemaphore GPU semaphore to wait
         * @param waitStage Stage to wait (Vulkan only)
         * @param signalStage Stage to wait for signal (Vulkan only)
         * @param signalSemaphore GPU semaphore to signal
         * @param commandLists Commands to execute
         */
        virtual void submit(
            const std::shared_ptr<Semaphore>& waitSemaphore,
            WaitStage waitStage,
            WaitStage signalStage,
            const std::shared_ptr<Semaphore>& signalSemaphore,
            const std::vector<std::shared_ptr<const CommandList>>& commandLists) const = 0;

        /**
          * Submit commands with GPU/GPU synchronization
          * @param waitSemaphore GPU semaphore to wait (must be a timeline semaphore)
          * @param waitStages Stages to wait. The queue will wait for two incremental values of the semaphore.
          * @param signalStage Stage to wait for signal (Vulkan only)
          * @param signalSemaphore GPU semaphore to signal
          * @param commandLists Commands to execute
          */
        virtual void submit(
            const std::shared_ptr<Semaphore>& waitSemaphore,
            const std::vector<WaitStage>& waitStages,
            WaitStage signalStage,
            const std::shared_ptr<Semaphore>& signalSemaphore,
            const std::vector<std::shared_ptr<const CommandList>>& commandLists) const = 0;

        /**
         * Submit commands with GPU/GPU synchronization
         * @param waitSemaphore GPU semaphore to wait
         * @param waitStage Stage to wait (Vulkan only)
         * @param commandLists Commands to execute
         */
        virtual void submit(
            const std::shared_ptr<Semaphore>& waitSemaphore,
            const WaitStage waitStage,
            const std::vector<std::shared_ptr<const CommandList>>& commandLists) const {
            submit(waitSemaphore, waitStage, WaitStage::NONE, nullptr, commandLists);
        }

        /**
         * Submit commands with GPU/GPU synchronization
         * @param waitSemaphore GPU semaphore to wait   (must be a timeline semaphore)
         * @param waitStages Stage to wait. The queue will wait for two incremental values of the semaphore.
         * @param commandLists Commands to execute
         */
        virtual void submit(
            const std::shared_ptr<Semaphore>& waitSemaphore,
            const std::vector<WaitStage>& waitStages,
            const std::vector<std::shared_ptr<const CommandList>>& commandLists) const {
            submit(waitSemaphore, waitStages, WaitStage::NONE, nullptr, commandLists);
        }

        /**
         * Submit commands with GPU/GPU synchronization
         * @param signalSemaphore GPU semaphore to signal
         * @param signalStage Stage to wait for signal (Vulkan only).
         * @param commandLists Commands to execute
         */
        virtual void submit(
            const WaitStage signalStage,
            const std::shared_ptr<Semaphore>& signalSemaphore,
            const std::vector<std::shared_ptr<const CommandList>>& commandLists) const {
            submit(nullptr, WaitStage::NONE, signalStage, signalSemaphore, commandLists);
        }

        /**
         * Wait for all commands to be executed
         */
        virtual void waitIdle() const = 0;

        virtual ~SubmitQueue() = default;

    protected:
        SubmitQueue() = default;
    };

    /**
     * A swapchain object
     *
     * Manual page : \ref manual_110_00_swapchain
     */
    class SwapChain {
    public:
        virtual ~SwapChain() = default;

        /**
         * Returns the swap chain extent
         */
        const Extent& getExtent() const { return extent; }

        /**
         * Returns the swap chain with/height ratio
         */
        auto getAspectRatio() const { return aspectRatio; }

        /**
         * Returns the current frame index. Note that frame indexes are not necessarily incremental
         */
        auto getCurrentFrameIndex() const { return currentFrameIndex; }

        /**
         * Returns the number of frame/back buffers in the swap chain
         */
        auto getFramesInFlight() const { return framesInFlight; }

        /**
         * Returns the pixel format of the frame buffers
         */
        auto getFormat() const { return format; }

        /**
         * Get the next frame index
         */
        virtual void nextFrameIndex() = 0;

        /**
         * Acquires the next frame buffer.
         * @return `false` if the operation failed.
         */
        virtual bool acquire(const std::shared_ptr<Fence>& fence) = 0;

        /**
         * Presents the current frame buffer into the surface
         */
        virtual void present() = 0;

        /**
         * Recreates the swap chain if the extent changed (in case of a window resizing)
         */
        virtual void recreate() = 0;

        /**
         * Waits for the last present operation to end
         */
        virtual void waitIdle() = 0;

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
     * Parameters for creating a graphics pipeline
     *
     * Manual page : \ref manual_080_01_graphic_pipelines
     */
    struct GraphicPipelineConfiguration {
        //! Description of descriptor sets and push constants
        std::shared_ptr<PipelineResources> resources;
        //! Values defining the format of color attachments used in this pipeline.
        std::vector<ImageFormat>           colorRenderFormats{};
        //! Blend state for each color attachment
        std::vector<ColorBlendDesc>        colorBlendDesc{};
        //! Description of the input vertices formats and order
        std::shared_ptr<VertexInputLayout> vertexInputLayout{nullptr};
        //! Vertex shader
        std::shared_ptr<ShaderModule>      vertexShader{nullptr};
        //! Fragment/Pixel shader
        std::shared_ptr<ShaderModule>      fragmentShader{nullptr};
        //! Tessellation control/Hull shader
        std::shared_ptr<ShaderModule>      hullShader{nullptr};
        //! Tessellation evaluation/Domain shader
        std::shared_ptr<ShaderModule>      domainShader{nullptr};
        //! Geometry shader
        std::shared_ptr<ShaderModule>      geometryShader{nullptr};

        //! The primitive topology
        PrimitiveTopology primitiveTopology{PrimitiveTopology::TRIANGLE_LIST};
        //! The number of samples used in rasterization
        MSAA              msaa{MSAA::NONE};
        //! The triangle facing direction used for primitive culling
        CullMode          cullMode{CullMode::NONE};
        //! The triangle rendering mode
        PolygonMode       polygonMode{PolygonMode::FILL};
        //! The front-facing triangle orientation to be used for culling
        bool              frontFaceCounterClockwise{true};

        //! The format of the depth attachment used in this pipeline.
        ImageFormat       depthStencilImageFormat{ImageFormat::D32_SFLOAT};
        //! Controls whether depth testing is enabled.
        bool              depthTestEnable{false};
        //! Controls whether depth writes are enabled when `depthTestEnable` is `true`. Depth writes are always disabled when `depthTestEnable` is `false`.
        bool              depthWriteEnable{false};
        //! Value specifying the comparison operator to use in the Depth Comparison step of the depth test.
        CompareOp         depthCompareOp{CompareOp::LESS_OR_EQUAL};
        //! Controls whether to bias fragment depth values.
        bool              depthBiasEnable{false};
        //! A scalar factor controlling the constant depth value added to each fragment.
        float             depthBiasConstantFactor{0.0f};
        //! The maximum (or minimum) depth bias of a fragment.
        float             depthBiasClamp{0.0f};
        //! A scalar factor applied to a fragment’s slope in depth bias calculations.
        float             depthBiasSlopeFactor{0.0f};

        //! Controls whether stencil testing is enabled
        bool              stencilTestEnable{false};
        //! value controlling the corresponding parameters of the stencil test.
        StencilOpState    frontStencilOpState{};
        //! value controlling the corresponding parameters of the stencil test.
        StencilOpState    backStencilOpState{};

        //! Controls whether to apply Logical Operations.
        bool              logicOpEnable{false};
        //! Which logical operation to apply.
        LogicOp           logicOp{LogicOp::NOOP};

        //! Controls whether a temporary coverage value is generated based on the alpha component of the fragment’s first color output
        bool              alphaToCoverageEnable{false};
    };

    /**
     * Main abstraction class.
     *
     * Manual page : \ref manual_010_vireo_class
     */
    class Vireo {
    public:
        /**
         * Creates a new Vireo class using the given backend.
         */
        static std::shared_ptr<Vireo> create(Backend backend);

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
        virtual std::shared_ptr<SwapChain> createSwapChain(
            ImageFormat format,
            const std::shared_ptr<const SubmitQueue>& presentQueue,
            void* windowHandle,
            PresentMode presentMode = PresentMode::VSYNC,
            uint32_t framesInFlight = 2) const = 0;

        /**
         * Creates a submission queue
         * @param commandType Type of commands that will be used with this queue.
         * @param name Objet name for debug
         */
        virtual std::shared_ptr<SubmitQueue> createSubmitQueue(
            CommandType commandType,
            const std::wstring& name = L"SubmitQueue") const = 0;

        /**
         * Creates a fence for CPU/GPU synchronization
         * @param createSignaled Create the fence in signaled state
         * @param name Object name for debug
         */
        virtual std::shared_ptr<Fence> createFence(
            bool createSignaled = false,
            const std::wstring& name = L"Fence") const = 0;

        /**
         * Creates a semaphore for GPU/GPU synchronization
         * @param type Semaphore type
         * @param name Object name for debug
         */
        virtual std::shared_ptr<Semaphore> createSemaphore(
            SemaphoreType type,
            const std::wstring& name = L"Semaphore") const = 0;

        /**
         * Creates a command allocator (command pool) for a given command type
         * @param type Type of commands that will be used with command lists created from this allocator
         */
        virtual std::shared_ptr<CommandAllocator> createCommandAllocator(CommandType type) const = 0;

        /**
         * Creates an input vertex layout from a description
         * @param size Size in bytes of a vertex
         * @param attributesDescriptions Description of all the vertex attributes
         */
        virtual std::shared_ptr<VertexInputLayout> createVertexLayout(
            size_t size,
            const std::vector<VertexAttributeDesc>& attributesDescriptions) const = 0;

        /**
         * Load a compiled shader and creates a shader module
         * @param fileName File name without the extension. The file name extension must be `.spv`for Vulkan et `.dxil`
         * for DirectX
         */
        virtual std::shared_ptr<ShaderModule> createShaderModule(
            const std::string& fileName) const = 0;

        /**
         * Creates a pipeline resources description. Describe resources that can be accessed by
         * the shaders associated with the future pipelines.
         * @param descriptorLayouts Descriptions of the resources (can be empty)
         * @param pushConstant Description of an optional push constant
         * @param name Object name for debug
         */
        virtual std::shared_ptr<PipelineResources> createPipelineResources(
            const std::vector<std::shared_ptr<DescriptorLayout>>& descriptorLayouts = {},
            const PushConstantsDesc& pushConstant = {},
            const std::wstring& name = L"PipelineResource") const = 0;

        /**
         * Creates a compute pipeline
         * @param pipelineResources Resources for the shader
         * @param shader The shader
         * @param name Object name for debug
         */
        virtual std::shared_ptr<ComputePipeline> createComputePipeline(
            const std::shared_ptr<PipelineResources>& pipelineResources,
            const std::shared_ptr<const ShaderModule>& shader,
            const std::wstring& name = L"ComputePipeline") const = 0;

        /**
         * Creates a graphic pipeline. At least one shader must be used.
         * @param configuration Pipeline configuration
         * @param name Object name for debug
         */
        virtual std::shared_ptr<GraphicPipeline> createGraphicPipeline(
            const GraphicPipelineConfiguration& configuration,
            const std::wstring& name = L"GraphicPipeline") const = 0;

        /**
         * Creates a data buffer in VRAM.
         * For types UNIFORM & TRANSFER the buffer will be created in host visible memory/upload heap type.
         * For types VERTEX & INDEX the buffer will be created in device local memory/default heap type
         * @param type Type of buffer to create.
         * @param size Size of one element in bytes
         * @param count Number of elements
         * @param name Object name for debug
         */
        virtual std::shared_ptr<Buffer> createBuffer(
            BufferType type,
            size_t size,
            size_t count = 1,
            const std::wstring& name = L"Buffer") const = 0;

        /**
         * Creates a read-only image in VRAM
         * @param format Pixel format
         * @param width With in pixels
         * @param height Height in pixels
         * @param mipLevels Number of mips levels
         * @param arraySize Number of layers/array size
         * @param name Object name for debug
         */
        virtual std::shared_ptr<Image> createImage(
            ImageFormat format,
            uint32_t width,
            uint32_t height,
            uint32_t mipLevels = 1,
            uint32_t arraySize = 1,
            const std::wstring& name = L"Image") const = 0;

        /**
         * Creates a read/write image in VRAM
         * @param format Pixel format
         * @param width With in pixels
         * @param height Height in pixels
         * @param mipLevels Number of mips levels
         * @param arraySize Number of layers/array size
         * @param name Object name for debug
         */
        virtual std::shared_ptr<Image> createReadWriteImage(
            ImageFormat format,
            uint32_t width,
            uint32_t height,
            uint32_t mipLevels = 1,
            uint32_t arraySize = 1,
            const std::wstring& name = L"RWImage") const = 0;

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
        virtual std::shared_ptr<RenderTarget> createRenderTarget(
            ImageFormat format,
            uint32_t width,
            uint32_t height,
            RenderTargetType type = RenderTargetType::COLOR,
            ClearValue clearValue = {},
            MSAA msaa = MSAA::NONE,
            const std::wstring& name = L"RenderTarget") const = 0;

        /**
         * Creates a read/write image in VRAM for use as a render target with a similar format as a swap chain.
         * @param swapChain Swap chain to copy format & size from
         * @param clearValue A clear value used for optimized clearing. Must be the same as the clear value used when
         * rendering.
         * @param msaa Number of samples for MSAA. A value of 1 disables MSAA.
         * @param name Object name for debug
         */
        virtual std::shared_ptr<RenderTarget> createRenderTarget(
            const std::shared_ptr<const SwapChain>& swapChain,
            ClearValue clearValue = {},
            MSAA msaa = MSAA::NONE,
            const std::wstring& name = L"RenderTarget") const = 0;

        /**
         * Creates an empty description layout.
         * @param name Object name for debug
         */
        virtual std::shared_ptr<DescriptorLayout> createDescriptorLayout(
            const std::wstring& name = L"DescriptorLayout") = 0;

        /**
         * Creates an empty description layout for SAMPLER resources types
         * @param name Object name for debug
         */
        virtual std::shared_ptr<DescriptorLayout> createSamplerDescriptorLayout(
            const std::wstring& name = L"createSamplerDescriptorLayout") = 0;

        /**
         * Creates an empty description layout for UNIFORM_DYNAMIX resources types
         * @param name Object name for debug
         */
        std::shared_ptr<DescriptorLayout> createDynamicUniformDescriptorLayout(
            const std::wstring& name = L"createDynamicUniformDescriptorLayout");

        /**
         * Creates an empty descriptor set
         * @param layout Layout of the set
         * @param name Object name for debug
         */
        virtual std::shared_ptr<DescriptorSet> createDescriptorSet(
            const std::shared_ptr<const DescriptorLayout>& layout,
            const std::wstring& name = L"DescriptorSet") = 0;

        /**
         * Creates a texture sampler.
         */
        virtual std::shared_ptr<Sampler> createSampler(Filter minFilter,
                                                  Filter magFilter,
                                                  AddressMode addressModeU,
                                                  AddressMode addressModeV,
                                                  AddressMode addressModeW,
                                                  float minLod = 0.0f,
                                                  float maxLod = Sampler::LOD_CLAMP_NONE,
                                                  bool anisotropyEnable = true,
                                                  MipMapMode mipMapMode = MipMapMode::LINEAR) const = 0;

        /**
         * Returns `true` if the backend API is supported
         */
        static bool isBackendSupported(Backend backend);

        /**
         * Returns the physical device/adapter object
         */
        auto getPhysicalDevice() const { return physicalDevice; }

        auto getDevice() const { return device; }

        auto getInstance() const { return instance; }

    protected:
        std::shared_ptr<Instance>        instance;
        std::shared_ptr<PhysicalDevice>  physicalDevice;
        std::shared_ptr<Device>          device;

        virtual std::shared_ptr<DescriptorLayout> _createDynamicUniformDescriptorLayout(
            const std::wstring& name = L"DynamicUniformDescriptorLayout") = 0;

    };

}