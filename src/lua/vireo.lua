-- Vireo API for ZeroBrane Studio autocompletion
-- Put this file in: api/lua/vireo.lua and add "vireo" to your project api list.

local vireo = {
  type = 'library',
  description = 'Vireo rendering abstraction layer',
  childs = {

    ---------------------------------------------------------------------------
    -- Enums
    ---------------------------------------------------------------------------

    Backend = {
      type = 'table',
      description = 'Type of supported backends.',
      childs = {
        UNDEFINED = { type = 'value', description = 'Not used.' },
        DIRECTX   = { type = 'value', description = 'DirectX 12 backend.' },
        VULKAN    = { type = 'value', description = 'Vulkan 1.3 backend.' },
      },
    },

    Filter = {
      type = 'table',
      description = 'Texture filtering mode.',
      childs = {
        NEAREST = { type = 'value' },
        LINEAR  = { type = 'value' },
      },
    },

    AddressMode = {
      type = 'table',
      description = 'Sampler address modes.',
      childs = {
        REPEAT          = { type = 'value' },
        MIRRORED_REPEAT = { type = 'value' },
        CLAMP_TO_EDGE   = { type = 'value' },
        CLAMP_TO_BORDER = { type = 'value' },
      },
    },

    MipMapMode = {
      type = 'table',
      description = 'Sampler mipmap mode.',
      childs = {
        NEAREST = { type = 'value' },
        LINEAR  = { type = 'value' },
      },
    },

    ImageFormat = {
      type = 'table',
      description = 'Common image formats supported by Vireo.',
      childs = {
        R8_UNORM = { type = 'value' },
        R8_SNORM = { type = 'value' },
        R8_UINT  = { type = 'value' },
        R8_SINT  = { type = 'value' },

        R8G8_UNORM = { type = 'value' },
        R8G8_SNORM = { type = 'value' },
        R8G8_UINT  = { type = 'value' },
        R8G8_SINT  = { type = 'value' },

        R8G8B8A8_UNORM = { type = 'value' },
        R8G8B8A8_SNORM = { type = 'value' },
        R8G8B8A8_UINT  = { type = 'value' },
        R8G8B8A8_SINT  = { type = 'value' },
        R8G8B8A8_SRGB  = { type = 'value' },

        B8G8R8A8_UNORM = { type = 'value' },
        B8G8R8A8_SRGB  = { type = 'value' },
        B8G8R8X8_UNORM = { type = 'value' },
        B8G8R8X8_SRGB  = { type = 'value' },

        A2B10G10R10_UNORM = { type = 'value' },
        A2B10G10R10_UINT  = { type = 'value' },

        R16_UNORM  = { type = 'value' },
        R16_SNORM  = { type = 'value' },
        R16_UINT   = { type = 'value' },
        R16_SINT   = { type = 'value' },
        R16_SFLOAT = { type = 'value' },

        R16G16_UNORM  = { type = 'value' },
        R16G16_SNORM  = { type = 'value' },
        R16G16_UINT   = { type = 'value' },
        R16G16_SINT   = { type = 'value' },
        R16G16_SFLOAT = { type = 'value' },

        R16G16B16A16_UNORM  = { type = 'value' },
        R16G16B16A16_SNORM  = { type = 'value' },
        R16G16B16A16_UINT   = { type = 'value' },
        R16G16B16A16_SINT   = { type = 'value' },
        R16G16B16A16_SFLOAT = { type = 'value' },

        R32_UINT   = { type = 'value' },
        R32_SINT   = { type = 'value' },
        R32_SFLOAT = { type = 'value' },

        R32G32_UINT   = { type = 'value' },
        R32G32_SINT   = { type = 'value' },
        R32G32_SFLOAT = { type = 'value' },

        R32G32B32_UINT   = { type = 'value' },
        R32G32B32_SINT   = { type = 'value' },
        R32G32B32_SFLOAT = { type = 'value' },

        R32G32B32A32_UINT   = { type = 'value' },
        R32G32B32A32_SINT   = { type = 'value' },
        R32G32B32A32_SFLOAT = { type = 'value' },

        D16_UNORM          = { type = 'value' },
        D24_UNORM_S8_UINT  = { type = 'value' },
        D32_SFLOAT         = { type = 'value' },
        D32_SFLOAT_S8_UINT = { type = 'value' },

        BC1_UNORM       = { type = 'value' },
        BC1_UNORM_SRGB  = { type = 'value' },
        BC2_UNORM       = { type = 'value' },
        BC2_UNORM_SRGB  = { type = 'value' },
        BC3_UNORM       = { type = 'value' },
        BC3_UNORM_SRGB  = { type = 'value' },
        BC4_UNORM       = { type = 'value' },
        BC4_SNORM       = { type = 'value' },
        BC5_UNORM       = { type = 'value' },
        BC5_SNORM       = { type = 'value' },
        BC6H_UFLOAT     = { type = 'value' },
        BC6H_SFLOAT     = { type = 'value' },
        BC7_UNORM       = { type = 'value' },
        BC7_UNORM_SRGB  = { type = 'value' },
      },
    },

    BufferType = {
      type = 'table',
      description = 'VRAM buffer usage type.',
      childs = {
        VERTEX           = { type = 'value' },
        INDEX            = { type = 'value' },
        INDIRECT         = { type = 'value' },
        UNIFORM          = { type = 'value' },
        STORAGE          = { type = 'value' },
        DEVICE_STORAGE   = { type = 'value' },
        READWRITE_STORAGE= { type = 'value' },
        BUFFER_UPLOAD    = { type = 'value' },
        BUFFER_DOWNLOAD  = { type = 'value' },
        IMAGE_UPLOAD     = { type = 'value' },
        IMAGE_DOWNLOAD   = { type = 'value' },
      },
    },

    IndexType = {
      type = 'table',
      description = 'Index type for index buffers.',
      childs = {
        UINT16 = { type = 'value' },
        UINT32 = { type = 'value' },
      },
    },

    DescriptorType = {
      type = 'table',
      description = 'Descriptor types used in descriptor sets.',
      childs = {
        UNIFORM         = { type = 'value' },
        UNIFORM_DYNAMIC = { type = 'value' },
        STORAGE         = { type = 'value' },
        DEVICE_STORAGE  = { type = 'value' },
        READWRITE_STORAGE = { type = 'value' },
        SAMPLED_IMAGE   = { type = 'value' },
        SAMPLER         = { type = 'value' },
        READWRITE_IMAGE = { type = 'value' },
      },
    },

    CommandType = {
      type = 'table',
      description = 'Command list / queue type.',
      childs = {
        GRAPHIC = { type = 'value' },
        TRANSFER= { type = 'value' },
        COMPUTE = { type = 'value' },
      },
    },

    AttributeFormat = {
      type = 'table',
      description = 'Vertex attribute formats.',
      childs = {
        R32_FLOAT        = { type = 'value' },
        R32G32_FLOAT     = { type = 'value' },
        R32G32B32_FLOAT  = { type = 'value' },
        R32G32B32A32_FLOAT = { type = 'value' },

        R32_SINT         = { type = 'value' },
        R32G32_SINT      = { type = 'value' },
        R32G32B32_SINT   = { type = 'value' },
        R32G32B32A32_SINT= { type = 'value' },

        R32_UINT         = { type = 'value' },
        R32G32_UINT      = { type = 'value' },
        R32G32B32_UINT   = { type = 'value' },
        R32G32B32A32_UINT= { type = 'value' },
      },
    },

    RenderTargetType = {
      type = 'table',
      description = 'Render target attachment type.',
      childs = {
        COLOR         = { type = 'value' },
        DEPTH         = { type = 'value' },
        DEPTH_STENCIL = { type = 'value' },
      },
    },

    CullMode = {
      type = 'table',
      description = 'Triangle culling mode.',
      childs = {
        NONE  = { type = 'value' },
        FRONT = { type = 'value' },
        BACK  = { type = 'value' },
      },
    },

    PrimitiveTopology = {
      type = 'table',
      description = 'Primitive topology.',
      childs = {
        POINT_LIST    = { type = 'value' },
        LINE_LIST     = { type = 'value' },
        LINE_STRIP    = { type = 'value' },
        TRIANGLE_LIST = { type = 'value' },
        TRIANGLE_STRIP= { type = 'value' },
      },
    },

    PolygonMode = {
      type = 'table',
      description = 'Polygon rasterization mode.',
      childs = {
        FILL      = { type = 'value' },
        WIREFRAME = { type = 'value' },
      },
    },

    CompareOp = {
      type = 'table',
      description = 'Depth/stencil comparison operation.',
      childs = {
        NEVER           = { type = 'value' },
        LESS            = { type = 'value' },
        EQUAL           = { type = 'value' },
        LESS_OR_EQUAL   = { type = 'value' },
        GREATER         = { type = 'value' },
        NOT_EQUAL       = { type = 'value' },
        GREATER_OR_EQUAL= { type = 'value' },
        ALWAYS          = { type = 'value' },
      },
    },

    StencilOp = {
      type = 'table',
      description = 'Stencil operations.',
      childs = {
        KEEP                 = { type = 'value' },
        ZERO                 = { type = 'value' },
        REPLACE              = { type = 'value' },
        INCREMENT_AND_CLAMP  = { type = 'value' },
        DECREMENT_AND_CLAMP  = { type = 'value' },
        INVERT               = { type = 'value' },
        INCREMENT_AND_WRAP   = { type = 'value' },
        DECREMENT_AND_WRAP   = { type = 'value' },
      },
    },

    BlendFactor = {
      type = 'table',
      description = 'Framebuffer blend factors.',
      childs = {
        ZERO                    = { type = 'value' },
        ONE                     = { type = 'value' },
        SRC_COLOR               = { type = 'value' },
        ONE_MINUS_SRC_COLOR     = { type = 'value' },
        DST_COLOR               = { type = 'value' },
        ONE_MINUS_DST_COLOR     = { type = 'value' },
        SRC_ALPHA               = { type = 'value' },
        ONE_MINUS_SRC_ALPHA     = { type = 'value' },
        DST_ALPHA               = { type = 'value' },
        ONE_MINUS_DST_ALPHA     = { type = 'value' },
        CONSTANT_COLOR          = { type = 'value' },
        ONE_MINUS_CONSTANT_COLOR= { type = 'value' },
        CONSTANT_ALPHA          = { type = 'value' },
        ONE_MINUS_CONSTANT_ALPHA= { type = 'value' },
        SRC_ALPHA_SATURATE      = { type = 'value' },
        SRC1_COLOR              = { type = 'value' },
        ONE_MINUS_SRC1_COLOR    = { type = 'value' },
        SRC1_ALPHA              = { type = 'value' },
        ONE_MINUS_SRC1_ALPHA    = { type = 'value' },
      },
    },

    BlendOp = {
      type = 'table',
      description = 'Framebuffer blending operations.',
      childs = {
        ADD             = { type = 'value' },
        SUBTRACT        = { type = 'value' },
        REVERSE_SUBTRACT= { type = 'value' },
        MIN             = { type = 'value' },
        MAX             = { type = 'value' },
      },
    },

    LogicOp = {
      type = 'table',
      description = 'Framebuffer logical operations.',
      childs = {
        CLEAR         = { type = 'value' },
        SET           = { type = 'value' },
        COPY          = { type = 'value' },
        COPY_INVERTED = { type = 'value' },
        NOOP          = { type = 'value' },
        INVERT        = { type = 'value' },
        AND           = { type = 'value' },
        NAND          = { type = 'value' },
        OR            = { type = 'value' },
        NOR           = { type = 'value' },
        XOR           = { type = 'value' },
        EQUIV         = { type = 'value' },
        AND_REVERSE   = { type = 'value' },
        AND_INVERTED  = { type = 'value' },
        OR_REVERSE    = { type = 'value' },
        OR_INVERTED   = { type = 'value' },
      },
    },

    ColorWriteMask = {
      type = 'table',
      description = 'Color write mask bit flags.',
      childs = {
        RED   = { type = 'value' },
        GREEN = { type = 'value' },
        BLUE  = { type = 'value' },
        ALPHA = { type = 'value' },
        ALL   = { type = 'value' },
      },
    },

    ShaderStage = {
      type = 'table',
      description = 'Shader stages.',
      childs = {
        ALL      = { type = 'value' },
        VERTEX   = { type = 'value' },
        FRAGMENT = { type = 'value' },
        HULL     = { type = 'value' },
        DOMAIN   = { type = 'value' },
        GEOMETRY = { type = 'value' },
        COMPUTE  = { type = 'value' },
      },
    },

    WaitStage = {
      type = 'table',
      description = 'Semaphore wait pipeline stages.',
      childs = {
        NONE                               = { type = 'value' },
        PIPELINE_TOP                       = { type = 'value' },
        VERTEX_INPUT                       = { type = 'value' },
        VERTEX_SHADER                      = { type = 'value' },
        DEPTH_STENCIL_TEST_BEFORE_FRAGMENT_SHADER = { type = 'value' },
        FRAGMENT_SHADER                    = { type = 'value' },
        DEPTH_STENCIL_TEST_AFTER_FRAGMENT_SHADER  = { type = 'value' },
        COLOR_OUTPUT                       = { type = 'value' },
        COMPUTE_SHADER                     = { type = 'value' },
        TRANSFER                           = { type = 'value' },
        PIPELINE_BOTTOM                    = { type = 'value' },
        ALL_GRAPHICS                       = { type = 'value' },
        ALL_COMMANDS                       = { type = 'value' },
        COPY                               = { type = 'value' },
        RESOLV                             = { type = 'value' },
        BLIT                               = { type = 'value' },
        CLEAR                              = { type = 'value' },
      },
    },

    ResourceState = {
      type = 'table',
      description = 'Image/buffer resource states for barriers.',
      childs = {
        UNDEFINED                     = { type = 'value' },
        GENERAL                       = { type = 'value' },
        RENDER_TARGET_COLOR           = { type = 'value' },
        RENDER_TARGET_DEPTH           = { type = 'value' },
        RENDER_TARGET_DEPTH_READ      = { type = 'value' },
        RENDER_TARGET_DEPTH_STENCIL   = { type = 'value' },
        RENDER_TARGET_DEPTH_STENCIL_READ = { type = 'value' },
        DISPATCH_TARGET               = { type = 'value' },
        PRESENT                       = { type = 'value' },
        COPY_SRC                      = { type = 'value' },
        COPY_DST                      = { type = 'value' },
        SHADER_READ                   = { type = 'value' },
        COMPUTE_READ                  = { type = 'value' },
        COMPUTE_WRITE                 = { type = 'value' },
        INDIRECT_DRAW                 = { type = 'value' },
        VERTEX_INPUT                  = { type = 'value' },
        UNIFORM                       = { type = 'value' },
      },
    },

    MSAA = {
      type = 'table',
      description = 'MSAA sample count.',
      childs = {
        NONE = { type = 'value' },
        X2   = { type = 'value' },
        X4   = { type = 'value' },
        X8   = { type = 'value' },
        X16  = { type = 'value' },
        X32  = { type = 'value' },
        X64  = { type = 'value' },
      },
    },

    PresentMode = {
      type = 'table',
      description = 'Swapchain present mode.',
      childs = {
        IMMEDIATE = { type = 'value' },
        VSYNC     = { type = 'value' },
      },
    },

    PipelineType = {
      type = 'table',
      description = 'Pipeline type.',
      childs = {
        GRAPHIC = { type = 'value' },
        COMPUTE = { type = 'value' },
      },
    },

    SemaphoreType = {
      type = 'table',
      description = 'Semaphore type.',
      childs = {
        BINARY   = { type = 'value' },
        TIMELINE = { type = 'value' },
      },
    },

    VideoMemoryAllocationUsage = {
      type = 'table',
      description = 'Video memory allocation usage.',
      childs = {
        BUFFER = { type = 'value' },
        IMAGE  = { type = 'value' },
      },
    },

    ---------------------------------------------------------------------------
    -- Simple structs / POD
    ---------------------------------------------------------------------------

    PhysicalDeviceDesc = {
      type = 'class',
      description = 'Video adapter description.',
      childs = {
        name                 = { type = 'string', description = 'Adapter name.' },
        dedicatedVideoMemory = { type = 'number' },
        dedicatedSystemMemory= { type = 'number' },
        sharedSystemMemory   = { type = 'number' },
      },
    },

    Extent = {
      type = 'class',
      description = 'Two-dimensional extent.',
      childs = {
        width  = { type = 'number' },
        height = { type = 'number' },
      },
    },

    Rect = {
      type = 'class',
      description = 'Rectangular region.',
      childs = {
        x      = { type = 'number' },
        y      = { type = 'number' },
        width  = { type = 'number' },
        height = { type = 'number' },
      },
    },

    Viewport = {
      type = 'class',
      description = 'Viewport parameters.',
      childs = {
        x        = { type = 'number' },
        y        = { type = 'number' },
        width    = { type = 'number' },
        height   = { type = 'number' },
        minDepth = { type = 'number' },
        maxDepth = { type = 'number' },
      },
    },

    PushConstantsDesc = {
      type = 'class',
      description = 'Push constant range description.',
      childs = {
        stage  = { type = 'value', description = 'ShaderStage.' },
        size   = { type = 'number' },
        offset = { type = 'number' },
      },
    },

    VertexAttributeDesc = {
      type = 'class',
      description = 'Vertex input attribute description.',
      childs = {
        binding = { type = 'string' },
        format  = { type = 'value', description = 'AttributeFormat.' },
        offset  = { type = 'number' },
      },
    },

    DepthClearValue = {
      type = 'class',
      description = 'Depth/stencil clear value.',
      childs = {
        depth   = { type = 'number' },
        stencil = { type = 'number' },
      },
    },

    ClearValue = {
      type = 'class',
      description = 'Clear color or depth/stencil value.',
      childs = {
        color        = { type = 'table', description = 'float[4]' },
        depthStencil = { type = 'class', description = 'DepthClearValue' },
      },
    },

    VideoMemoryAllocationDesc = {
      type = 'class',
      description = 'Video memory allocation description.',
      childs = {
        usage = { type = 'value', description = 'VideoMemoryAllocationUsage.' },
        name  = { type = 'string' },
        size  = { type = 'number' },
        ref   = { type = 'userdata' },
      },
    },

    ColorBlendDesc = {
      type = 'class',
      description = 'Color blend attachment description.',
      childs = {
        blendEnable          = { type = 'boolean' },
        srcColorBlendFactor  = { type = 'value', description = 'BlendFactor.' },
        dstColorBlendFactor  = { type = 'value', description = 'BlendFactor.' },
        colorBlendOp         = { type = 'value', description = 'BlendOp.' },
        srcAlphaBlendFactor  = { type = 'value', description = 'BlendFactor.' },
        dstAlphaBlendFactor  = { type = 'value', description = 'BlendFactor.' },
        alphaBlendOp         = { type = 'value', description = 'BlendOp.' },
        colorWriteMask       = { type = 'value', description = 'ColorWriteMask.' },
      },
    },

    StencilOpState = {
      type = 'class',
      description = 'Stencil operation state.',
      childs = {
        failOp      = { type = 'value', description = 'StencilOp.' },
        passOp      = { type = 'value', description = 'StencilOp.' },
        depthFailOp = { type = 'value', description = 'StencilOp.' },
        compareOp   = { type = 'value', description = 'CompareOp.' },
        compareMask = { type = 'number' },
        writeMask   = { type = 'number' },
      },
    },

    RenderTargetDesc = {
      type = 'class',
      description = 'Color attachment description.',
      childs = {
        swapChain                = { type = 'class', description = 'SwapChain.' },
        renderTarget             = { type = 'class', description = 'RenderTarget.' },
        multisampledRenderTarget = { type = 'class', description = 'RenderTarget.' },
        clear                    = { type = 'boolean' },
        clearValue               = { type = 'class', description = 'ClearValue.' },
        discardAfterRender       = { type = 'boolean' },
      },
    },

    RenderingConfiguration = {
      type = 'class',
      description = 'Rendering configuration (color + depth).',
      childs = {
        colorRenderTargets               = { type = 'table' },
        depthStencilRenderTarget         = { type = 'class', description = 'RenderTarget.' },
        multisampledDepthStencilRenderTarget = { type = 'class', description = 'RenderTarget.' },
        depthTestEnable                  = { type = 'boolean' },
        stencilTestEnable                = { type = 'boolean' },
        clearDepthStencil                = { type = 'boolean' },
        depthStencilClearValue           = { type = 'class', description = 'ClearValue.' },
        discardDepthStencilAfterRender   = { type = 'boolean' },
      },
    },

    BufferUploadInfo = {
      type = 'class',
      description = 'Batch buffer upload info.',
      childs = {
        buffer = { type = 'class', description = 'Buffer.' },
        data   = { type = 'userdata' },
      },
    },

    ImageUploadInfo = {
      type = 'class',
      description = 'Batch image upload info.',
      childs = {
        image = { type = 'class', description = 'Image.' },
        data  = { type = 'userdata' },
      },
    },

    DrawIndirectCommand = {
      type = 'class',
      description = 'Non-indexed indirect draw parameters.',
      childs = {
        vertexCount   = { type = 'number' },
        instanceCount = { type = 'number' },
        firstVertex   = { type = 'number' },
        firstInstance = { type = 'number' },
      },
    },

    DrawIndexedIndirectCommand = {
      type = 'class',
      description = 'Indexed indirect draw parameters.',
      childs = {
        indexCount    = { type = 'number' },
        instanceCount = { type = 'number' },
        firstIndex    = { type = 'number' },
        vertexOffset  = { type = 'number' },
        firstInstance = { type = 'number' },
      },
    },

    BufferCopyRegion = {
      type = 'class',
      description = 'Buffer copy region.',
      childs = {
        srcOffset = { type = 'number' },
        dstOffset = { type = 'number' },
        size      = { type = 'number' },
      },
    },

    GraphicPipelineConfiguration = {
      type = 'class',
      description = 'Configuration for creating a graphic pipeline.',
      childs = {
        resources                    = { type = 'class', description = 'PipelineResources.' },
        colorRenderFormats           = { type = 'table' },
        colorBlendDesc               = { type = 'table' },
        vertexInputLayout            = { type = 'class', description = 'VertexInputLayout.' },
        vertexShader                 = { type = 'class', description = 'ShaderModule.' },
        fragmentShader               = { type = 'class', description = 'ShaderModule.' },
        hullShader                   = { type = 'class', description = 'ShaderModule.' },
        domainShader                 = { type = 'class', description = 'ShaderModule.' },
        geometryShader               = { type = 'class', description = 'ShaderModule.' },
        primitiveTopology            = { type = 'value', description = 'PrimitiveTopology.' },
        msaa                         = { type = 'value', description = 'MSAA.' },
        cullMode                     = { type = 'value', description = 'CullMode.' },
        polygonMode                  = { type = 'value', description = 'PolygonMode.' },
        frontFaceCounterClockwise    = { type = 'boolean' },
        depthStencilImageFormat      = { type = 'value', description = 'ImageFormat.' },
        depthTestEnable              = { type = 'boolean' },
        depthWriteEnable             = { type = 'boolean' },
        depthCompareOp               = { type = 'value', description = 'CompareOp.' },
        depthBiasEnable              = { type = 'boolean' },
        depthBiasConstantFactor      = { type = 'number' },
        depthBiasClamp               = { type = 'number' },
        depthBiasSlopeFactor         = { type = 'number' },
        stencilTestEnable            = { type = 'boolean' },
        frontStencilOpState          = { type = 'class', description = 'StencilOpState.' },
        backStencilOpState           = { type = 'class', description = 'StencilOpState.' },
        logicOpEnable                = { type = 'boolean' },
        logicOp                      = { type = 'value', description = 'LogicOp.' },
        alphaToCoverageEnable        = { type = 'boolean' },
      },
    },

    ---------------------------------------------------------------------------
    -- Core classes / objects
    ---------------------------------------------------------------------------

    Fence = {
      type = 'class',
      description = 'CPU/GPU synchronization fence.',
      childs = {
        wait  = {
          type = 'function',
          description = 'Waits for the fence to become signaled.',
          args = '()',
        },
        reset = {
          type = 'function',
          description = 'Resets the fence to the unsignaled state.',
          args = '()',
        },
      },
    },

    Semaphore = {
      type = 'class',
      description = 'GPU/GPU synchronization semaphore (binary or timeline).',
      childs = {
        getType = {
          type = 'function',
          description = 'Returns the semaphore type.',
          args = '()',
          returns = 'vireo.SemaphoreType',
        },
        getValue = {
          type = 'function',
          description = 'Returns current integer value (timeline).',
          args = '()',
          returns = 'number',
        },
        setValue = {
          type = 'function',
          description = 'Sets current integer value (timeline).',
          args = '(number value)',
        },
        incrementValue = {
          type = 'function',
          description = 'Increments current integer value.',
          args = '()',
        },
        decrementValue = {
          type = 'function',
          description = 'Decrements current integer value.',
          args = '()',
        },
      },
    },

    Instance = {
      type = 'class',
      description = 'Backend instance object (internal).',
    },

    PhysicalDevice = {
      type = 'class',
      description = 'Physical device (GPU adapter).',
      childs = {
        getDescription = {
          type = 'function',
          description = 'Returns a PhysicalDeviceDesc.',
          args = '()',
          returns = 'vireo.PhysicalDeviceDesc',
        },
      },
    },

    Device = {
      type = 'class',
      description = 'Logical device.',
      childs = {
        haveDedicatedTransferQueue = {
          type = 'function',
          description = 'Returns true if a dedicated transfer queue is available.',
          args = '()',
          returns = 'boolean',
        },
      },
    },

    Buffer = {
      type = 'class',
      description = 'VRAM buffer.',
      childs = {
        WHOLE_SIZE = { type = 'value', description = 'Constant: use entire buffer size.' },

        getSize = {
          type = 'function',
          args = '()',
          returns = 'number',
        },
        getType = {
          type = 'function',
          args = '()',
          returns = 'vireo.BufferType',
        },
        getInstanceSize = {
          type = 'function',
          args = '()',
          returns = 'number',
        },
        getInstanceSizeAligned = {
          type = 'function',
          args = '()',
          returns = 'number',
        },
        getInstanceCount = {
          type = 'function',
          args = '()',
          returns = 'number',
        },
        getMappedAddress = {
          type = 'function',
          args = '()',
          returns = 'userdata',
        },
        map = {
          type = 'function',
          description = 'Maps the buffer memory.',
          args = '()',
        },
        unmap = {
          type = 'function',
          description = 'Unmaps the buffer memory.',
          args = '()',
        },
        write = {
          type = 'function',
          description = 'Writes data to the mapped buffer.',
          args = '(userdata data[, number size[, number offset]])',
        },
        getMemoryAllocations = {
          type = 'function',
          description = 'Returns the list of current VideoMemoryAllocationDesc entries.',
          args = '()',
          returns = 'table',
        },
      },
    },

    Sampler = {
      type = 'class',
      description = 'Texture sampler object.',
      childs = {
        LOD_CLAMP_NONE = { type = 'value', description = 'No LOD clamp.' },
      },
    },

    Image = {
      type = 'class',
      description = 'Image/texture object.',
      childs = {
        IMAGE_ROW_PITCH_ALIGNMENT = {
          type = 'value',
          description = 'Row pitch alignment (bytes).',
        },

        getFormat = {
          type = 'function',
          args = '()',
          returns = 'vireo.ImageFormat',
        },
        getWidth = {
          type = 'function',
          args = '()',
          returns = 'number',
        },
        getHeight = {
          type = 'function',
          args = '()',
          returns = 'number',
        },
        getMipLevels = {
          type = 'function',
          args = '()',
          returns = 'number',
        },
        getArraySize = {
          type = 'function',
          args = '()',
          returns = 'number',
        },
        getRowPitch = {
          type = 'function',
          args = '([number mipLevel])',
          returns = 'number',
        },
        getRowLength = {
          type = 'function',
          args = '([number mipLevel])',
          returns = 'number',
        },
        getImageSize = {
          type = 'function',
          args = '([number mipLevel])',
          returns = 'number',
        },
        getAlignedImageSize = {
          type = 'function',
          args = '([number mipLevel])',
          returns = 'number',
        },
        getAlignedRowPitch = {
          type = 'function',
          args = '([number mipLevel])',
          returns = 'number',
        },
        getAlignedRowLength = {
          type = 'function',
          args = '([number mipLevel])',
          returns = 'number',
        },
        isReadWrite = {
          type = 'function',
          args = '()',
          returns = 'boolean',
        },
        getPixelSize = {
          type = 'function',
          description = 'Returns pixel or block size for a given format.',
          args = '(vireo.ImageFormat format)',
          returns = 'number',
        },
        getMemoryAllocations = {
          type = 'function',
          description = 'Returns current image memory allocations.',
          args = '()',
          returns = 'table',
        },
        isDepthFormat = {
          type = 'function',
          args = '(vireo.ImageFormat format)',
          returns = 'boolean',
        },
        isDepthStencilFormat = {
          type = 'function',
          args = '(vireo.ImageFormat format)',
          returns = 'boolean',
        },
        isDepthFormatInstance = {
          type = 'function',
          description = 'Instance variant of isDepthFormat().',
          args = '()',
          returns = 'boolean',
        },
        isDepthStencilFormatInstance = {
          type = 'function',
          description = 'Instance variant of isDepthStencilFormat().',
          args = '()',
          returns = 'boolean',
        },
      },
    },

    RenderTarget = {
      type = 'class',
      description = 'Color or depth render attachment.',
      childs = {
        getImage = {
          type = 'function',
          args = '()',
          returns = 'vireo.Image',
        },
        getType = {
          type = 'function',
          args = '()',
          returns = 'vireo.RenderTargetType',
        },
      },
    },

    DescriptorLayout = {
      type = 'class',
      description = 'Descriptor set layout.',
      childs = {
        add = {
          type = 'function',
          description = 'Adds a descriptor binding.',
          args = '(number index, vireo.DescriptorType type[, number count])',
          returns = 'vireo.DescriptorLayout',
        },
        build = {
          type = 'function',
          description = 'Builds the layout after add().',
          args = '()',
        },
        getCapacity = {
          type = 'function',
          args = '()',
          returns = 'number',
        },
        isDynamicUniform = {
          type = 'function',
          args = '()',
          returns = 'boolean',
        },
        isSamplers = {
          type = 'function',
          args = '()',
          returns = 'boolean',
        },
      },
    },

    DescriptorSet = {
      type = 'class',
      description = 'Descriptor set (resources for shaders).',
      childs = {
        updateBuffer = {
          type = 'function',
          description = 'Binds a buffer to a binding index.',
          args = '(number index, vireo.Buffer buffer)',
        },
        updateImage = {
          type = 'function',
          description = 'Binds an image to a binding index.',
          args = '(number index, vireo.Image image)',
        },
        updateSampler = {
          type = 'function',
          description = 'Binds a sampler to a binding index.',
          args = '(number index, vireo.Sampler sampler)',
        },
        updateImageArray = {
          type = 'function',
          description = 'Binds an array of images.',
          args = '(number index, table images)',
        },
        updateBufferArray = {
          type = 'function',
          description = 'Binds an array of buffers.',
          args = '(number index, table buffers)',
        },
        updateSamplerArray = {
          type = 'function',
          description = 'Binds an array of samplers.',
          args = '(number index, table samplers)',
        },
        getLayout = {
          type = 'function',
          args = '()',
          returns = 'vireo.DescriptorLayout',
        },
      },
    },

    VertexInputLayout = {
      type = 'class',
      description = 'Vertex input layout.',
    },

    ShaderModule = {
      type = 'class',
      description = 'Shader module.',
    },

    PipelineResources = {
      type = 'class',
      description = 'Pipeline resources (descriptor layouts + push constants).',
    },

    Pipeline = {
      type = 'class',
      description = 'Base class for pipelines.',
      childs = {
        getResources = {
          type = 'function',
          args = '()',
          returns = 'vireo.PipelineResources',
        },
        getType = {
          type = 'function',
          args = '()',
          returns = 'vireo.PipelineType',
        },
      },
    },

    ComputePipeline = {
      type = 'class',
      description = 'Compute pipeline.',
    },

    GraphicPipeline = {
      type = 'class',
      description = 'Graphic pipeline.',
    },

    CommandList = {
      type = 'class',
      description = 'Command list (recorded GPU commands).',
      childs = {
        begin_ = {
          type = 'function',
          description = 'Begin recording commands.',
          args = '()',
        },
        end_ = {
          type = 'function',
          description = 'End recording commands.',
          args = '()',
        },
        uploadBuffer = {
          type = 'function',
          description = 'Upload data into a buffer using staging.',
          args = '(vireo.Buffer destination, userdata source)',
        },
        uploadImage = {
          type = 'function',
          description = 'Upload data into an image using staging.',
          args = '(vireo.Image destination, userdata source[, number firstMipLevel])',
        },
        upload = {
          type = 'function',
          description = 'Batch upload for buffers or images (implementation-dependent).',
          args = '(table infos)',
        },

        copyBufferToImage = {
          type = 'function',
          description = 'Copy buffer data to an image level.',
          args = '(vireo.Buffer source, vireo.Image destination[, number sourceOffset[, number mipLevel[, boolean rowPitchAlignment]]])',
        },
        copyImageToBuffer = {
          type = 'function',
          description = 'Copy image level to buffer.',
          args = '(vireo.Image source, vireo.Buffer destination[, number destinationOffset[, number mipLevel]])',
        },
        copyBufferToBuffer = {
          type = 'function',
          description = 'Copy data between buffers.',
          args = '(vireo.Buffer source, vireo.Buffer destination[, number size[, number sourceOffset[, number destinationOffset]]])',
        },
        copyBufferRegions = {
          type = 'function',
          description = 'Copy multiple buffer regions.',
          args = '(vireo.Buffer source, vireo.Buffer destination, table regions)',
        },
        uploadArray = {
          type = 'function',
          description = 'Upload multiple images into an image array.',
          args = '(vireo.Image destination, table sources[, number firstMipLevel])',
        },
        copyToSwapChain = {
          type = 'function',
          description = 'Copy an image to the current swapchain image.',
          args = '(vireo.Image source, vireo.SwapChain swapChain)',
        },

        beginRendering = {
          type = 'function',
          description = 'Begin rendering with a RenderingConfiguration.',
          args = '(vireo.RenderingConfiguration config)',
        },
        endRendering = {
          type = 'function',
          description = 'End rendering.',
          args = '()',
        },

        dispatch = {
          type = 'function',
          description = 'Dispatch compute work.',
          args = '(number x, number y, number z)',
        },

        bindVertexBuffer = {
          type = 'function',
          description = 'Bind a single vertex buffer.',
          args = '(vireo.Buffer buffer[, number offset])',
        },
        bindVertexBuffers = {
          type = 'function',
          description = 'Bind multiple vertex buffers.',
          args = '(table buffers[, table offsets])',
        },
        bindIndexBuffer = {
          type = 'function',
          description = 'Bind an index buffer.',
          args = '(vireo.Buffer buffer[, vireo.IndexType indexType[, number firstIndex]])',
        },

        bindPipeline = {
          type = 'function',
          description = 'Bind a pipeline.',
          args = '(vireo.Pipeline pipeline)',
        },

        bindDescriptors = {
          type = 'function',
          description = 'Bind multiple descriptor sets.',
          args = '(table descriptors[, number firstSet])',
        },
        bindDescriptor = {
          type = 'function',
          description = 'Bind a descriptor set with optional dynamic offset.',
          args = '(vireo.DescriptorSet descriptor, number set[, number offset])',
        },

        draw = {
          type = 'function',
          description = 'Draw non-indexed primitives.',
          args = '(number vertexCountPerInstance[, number instanceCount[, number firstVertex[, number firstInstance]]])',
        },
        drawIndexed = {
          type = 'function',
          description = 'Draw indexed primitives.',
          args = '(number indexCountPerInstance[, number instanceCount[, number firstIndex[, number firstVertex[, number firstInstance]]]])',
        },
        drawIndirect = {
          type = 'function',
          description = 'Draw from buffer with indirect parameters.',
          args = '(vireo.Buffer buffer, number offset, number drawCount, number stride[, number firstCommandOffset])',
        },
        drawIndexedIndirect = {
          type = 'function',
          description = 'Draw indexed from buffer with indirect parameters.',
          args = '(vireo.Buffer buffer, number offset, number maxDrawCount, number stride[, number firstCommandOffset])',
        },
        drawIndexedIndirectCount = {
          type = 'function',
          description = 'Draw indexed from buffer with count in another buffer.',
          args = '(vireo.Buffer buffer, number offset, vireo.Buffer countBuffer, number countOffset, number maxDrawCount, number stride[, number firstCommandOffset])',
        },

        setViewports = {
          type = 'function',
          description = 'Set multiple viewports.',
          args = '(table viewports)',
        },
        setScissors = {
          type = 'function',
          description = 'Set multiple scissors.',
          args = '(table rects)',
        },
        setViewport = {
          type = 'function',
          description = 'Set a single viewport.',
          args = '(vireo.Viewport viewport)',
        },
        setScissor = {
          type = 'function',
          description = 'Set a single scissor rect.',
          args = '(vireo.Rect rect)',
        },
        setStencilReference = {
          type = 'function',
          description = 'Set stencil reference value.',
          args = '(number reference)',
        },

        barrierImage = {
          type = 'function',
          description = 'Insert barrier for image state transition.',
          args = '(vireo.Image image, vireo.ResourceState oldState, vireo.ResourceState newState[, number firstMipLevel[, number levelCount]])',
        },
        barrierRenderTarget = {
          type = 'function',
          description = 'Insert barrier for a single render target.',
          args = '(vireo.RenderTarget rt, vireo.ResourceState oldState, vireo.ResourceState newState)',
        },
        barrierRenderTargets = {
          type = 'function',
          description = 'Insert barrier for multiple render targets.',
          args = '(table renderTargets, vireo.ResourceState oldState, vireo.ResourceState newState)',
        },
        barrierSwapChain = {
          type = 'function',
          description = 'Insert barrier for swapchain.',
          args = '(vireo.SwapChain swapChain, vireo.ResourceState oldState, vireo.ResourceState newState)',
        },
        barrierBuffer = {
          type = 'function',
          description = 'Insert barrier for buffer state transition.',
          args = '(vireo.Buffer buffer, vireo.ResourceState oldState, vireo.ResourceState newState)',
        },

        pushConstants = {
          type = 'function',
          description = 'Update push constants.',
          args = '(vireo.PipelineResources resources, vireo.PushConstantsDesc desc, userdata data)',
        },

        cleanup = {
          type = 'function',
          description = 'Cleanup staging resources used by upload().',
          args = '()',
        },
      },
    },

    CommandAllocator = {
      type = 'class',
      description = 'Command allocator (pool) for command lists.',
      childs = {
        reset = {
          type = 'function',
          description = 'Reset allocator and associated command lists.',
          args = '()',
        },
        createCommandList = {
          type = 'function',
          description = 'Create a command list (with or without pipeline).',
          args = '([vireo.Pipeline pipeline])',
          returns = 'vireo.CommandList',
        },
        getCommandListType = {
          type = 'function',
          args = '()',
          returns = 'vireo.CommandType',
        },
      },
    },

    SubmitQueue = {
      type = 'class',
      description = 'Command submission queue.',
      childs = {
        submit = {
          type = 'function',
          description = 'Submits command lists (several overloads in C++; here represented generically).',
          args = '(... commandLists)',
        },
        waitIdle = {
          type = 'function',
          description = 'Wait for all submitted work to finish.',
          args = '()',
        },
      },
    },

    SwapChain = {
      type = 'class',
      description = 'Swapchain for presenting images.',
      childs = {
        getExtent = {
          type = 'function',
          args = '()',
          returns = 'vireo.Extent',
        },
        getAspectRatio = {
          type = 'function',
          args = '()',
          returns = 'number',
        },
        getCurrentFrameIndex = {
          type = 'function',
          args = '()',
          returns = 'number',
        },
        getFramesInFlight = {
          type = 'function',
          args = '()',
          returns = 'number',
        },
        getFormat = {
          type = 'function',
          args = '()',
          returns = 'vireo.ImageFormat',
        },
        nextFrameIndex = {
          type = 'function',
          args = '()',
        },
        acquire = {
          type = 'function',
          description = 'Acquire next frame buffer.',
          args = '(vireo.Fence fence)',
          returns = 'boolean',
        },
        present = {
          type = 'function',
          description = 'Present the current frame.',
          args = '()',
        },
        recreate = {
          type = 'function',
          description = 'Recreate swapchain on resize.',
          args = '()',
        },
        waitIdle = {
          type = 'function',
          description = 'Wait for last present to finish.',
          args = '()',
        },
      },
    },

    ---------------------------------------------------------------------------
    -- Main Vireo class
    ---------------------------------------------------------------------------

    Vireo = {
      type = 'class',
      description = 'Main Vireo abstraction class.',
      childs = {
        create = {
          type = 'function',
          description = 'Creates a new Vireo instance.',
          args = '(vireo.Backend backend[, number maxDirectX12Descriptors[, number maxDirectX12Samplers]])',
          returns = 'vireo.Vireo',
        },

        waitIdle = {
          type = 'function',
          description = 'Waits for all device work to finish.',
          args = '()',
        },

        createSwapChain = {
          type = 'function',
          description = 'Creates a swapchain.',
          args = '(vireo.ImageFormat format, vireo.SubmitQueue presentQueue, userdata windowHandle[, vireo.PresentMode presentMode[, number framesInFlight]])',
          returns = 'vireo.SwapChain',
        },

        createSubmitQueue = {
          type = 'function',
          description = 'Creates a submit queue.',
          args = '(vireo.CommandType commandType[, string name])',
          returns = 'vireo.SubmitQueue',
        },

        createFence = {
          type = 'function',
          description = 'Creates a CPU/GPU fence.',
          args = '([boolean createSignaled[, string name]])',
          returns = 'vireo.Fence',
        },

        createSemaphore = {
          type = 'function',
          description = 'Creates a GPU/GPU semaphore.',
          args = '(vireo.SemaphoreType type[, string name])',
          returns = 'vireo.Semaphore',
        },

        createCommandAllocator = {
          type = 'function',
          description = 'Creates a command allocator for a given command type.',
          args = '(vireo.CommandType type)',
          returns = 'vireo.CommandAllocator',
        },

        createVertexLayout = {
          type = 'function',
          description = 'Creates a vertex input layout.',
          args = '(number size, table attributesDescriptions)',
          returns = 'vireo.VertexInputLayout',
        },

        createShaderModuleFromFile = {
          type = 'function',
          description = 'Creates a shader module from a compiled file.',
          args = '(string fileName)',
          returns = 'vireo.ShaderModule',
        },

        createShaderModuleFromData = {
          type = 'function',
          description = 'Creates a shader module from a byte array.',
          args = '(table data)',
          returns = 'vireo.ShaderModule',
        },

        createPipelineResources = {
          type = 'function',
          description = 'Creates pipeline resources description.',
          args = '([table descriptorLayouts[, vireo.PushConstantsDesc pushConstant[, string name]]])',
          returns = 'vireo.PipelineResources',
        },

        createComputePipeline = {
          type = 'function',
          description = 'Creates a compute pipeline.',
          args = '(vireo.PipelineResources resources, vireo.ShaderModule shader[, string name])',
          returns = 'vireo.ComputePipeline',
        },

        createGraphicPipeline = {
          type = 'function',
          description = 'Creates a graphic pipeline.',
          args = '(vireo.GraphicPipelineConfiguration configuration[, string name])',
          returns = 'vireo.GraphicPipeline',
        },

        createBuffer = {
          type = 'function',
          description = 'Creates a buffer in VRAM.',
          args = '(vireo.BufferType type, number size[, number count[, string name]])',
          returns = 'vireo.Buffer',
        },

        createImage = {
          type = 'function',
          description = 'Creates a read-only image in VRAM.',
          args = '(vireo.ImageFormat format, number width, number height[, number mipLevels[, number arraySize[, string name]]])',
          returns = 'vireo.Image',
        },

        createReadWriteImage = {
          type = 'function',
          description = 'Creates a read/write image in VRAM.',
          args = '(vireo.ImageFormat format, number width, number height[, number mipLevels[, number arraySize[, string name]]])',
          returns = 'vireo.Image',
        },

        createRenderTarget = {
          type = 'function',
          description = 'Creates a render target from explicit parameters or from a swapchain.',
          args = '(... )',
          returns = 'vireo.RenderTarget',
        },

        createDescriptorLayout = {
          type = 'function',
          description = 'Creates an empty descriptor layout.',
          args = '([string name])',
          returns = 'vireo.DescriptorLayout',
        },

        createSamplerDescriptorLayout = {
          type = 'function',
          description = 'Creates a sampler-only descriptor layout.',
          args = '([string name])',
          returns = 'vireo.DescriptorLayout',
        },

        createDynamicUniformDescriptorLayout = {
          type = 'function',
          description = 'Creates a dynamic uniform descriptor layout.',
          args = '([string name])',
          returns = 'vireo.DescriptorLayout',
        },

        createDescriptorSet = {
          type = 'function',
          description = 'Creates a descriptor set for a given layout.',
          args = '(vireo.DescriptorLayout layout[, string name])',
          returns = 'vireo.DescriptorSet',
        },

        createSampler = {
          type = 'function',
          description = 'Creates a texture sampler.',
          args = '(vireo.Filter minFilter, vireo.Filter magFilter, vireo.AddressMode addressModeU, vireo.AddressMode addressModeV, vireo.AddressMode addressModeW[, number minLod[, number maxLod[, boolean anisotropyEnable[, vireo.MipMapMode mipMapMode[, vireo.CompareOp compareOp]]]]])',
          returns = 'vireo.Sampler',
        },

        isBackendSupported = {
          type = 'function',
          description = 'Returns true if the backend is supported.',
          args = '(vireo.Backend backend)',
          returns = 'boolean',
        },

        getShaderFileExtension = {
          type = 'function',
          description = 'Returns the compiled shader file extension for this backend.',
          args = '()',
          returns = 'string',
        },

        getPhysicalDevice = {
          type = 'function',
          description = 'Returns underlying PhysicalDevice.',
          args = '()',
          returns = 'vireo.PhysicalDevice',
        },

        getDevice = {
          type = 'function',
          description = 'Returns underlying Device.',
          args = '()',
          returns = 'vireo.Device',
        },

        getInstance = {
          type = 'function',
          description = 'Returns underlying Instance.',
          args = '()',
          returns = 'vireo.Instance',
        },
      },
    },
  },
}

return vireo
