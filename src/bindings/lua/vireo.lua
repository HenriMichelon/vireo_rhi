---@diagnostic disable: missing-return
--EmmyLua annotations and documentation for Vireo

return {
           ------------------------------------------------------------------------
           -- Enums
           ------------------------------------------------------------------------

           ---@class vireo.Backend
           ---@field UNDEFINED integer
           ---@field DIRECTX integer
           ---@field VULKAN integer
           Backend = vireo.Backend,

           ---@class vireo.Filter
           ---@field NEAREST integer
           ---@field LINEAR integer
           Filter = vireo.Filter,

           ---@class vireo.AddressMode
           ---@field REPEAT integer
           ---@field MIRRORED_REPEAT integer
           ---@field CLAMP_TO_EDGE integer
           ---@field CLAMP_TO_BORDER integer
           AddressMode = vireo.AddressMode,

           ---@class vireo.MipMapMode
           ---@field NEAREST integer
           ---@field LINEAR integer
           MipMapMode = vireo.MipMapMode,

           ---@class vireo.ImageFormat
           ---@field R8_UNORM integer
           ---@field R8_SNORM integer
           ---@field R8_UINT integer
           ---@field R8_SINT integer
           ---@field R8G8_UNORM integer
           ---@field R8G8_SNORM integer
           ---@field R8G8_UINT integer
           ---@field R8G8_SINT integer
           ---@field R8G8B8A8_UNORM integer
           ---@field R8G8B8A8_SNORM integer
           ---@field R8G8B8A8_UINT integer
           ---@field R8G8B8A8_SINT integer
           ---@field R8G8B8A8_SRGB integer
           ---@field B8G8R8A8_UNORM integer
           ---@field B8G8R8A8_SRGB integer
           ---@field B8G8R8X8_UNORM integer
           ---@field B8G8R8X8_SRGB integer
           ---@field A2B10G10R10_UNORM integer
           ---@field A2B10G10R10_UINT integer
           ---@field R16_UNORM integer
           ---@field R16_SNORM integer
           ---@field R16_UINT integer
           ---@field R16_SINT integer
           ---@field R16_SFLOAT integer
           ---@field R16G16_UNORM integer
           ---@field R16G16_SNORM integer
           ---@field R16G16_UINT integer
           ---@field R16G16_SINT integer
           ---@field R16G16_SFLOAT integer
           ---@field R16G16B16A16_UNORM integer
           ---@field R16G16B16A16_SNORM integer
           ---@field R16G16B16A16_UINT integer
           ---@field R16G16B16A16_SINT integer
           ---@field R16G16B16A16_SFLOAT integer
           ---@field R32_UINT integer
           ---@field R32_SINT integer
           ---@field R32_SFLOAT integer
           ---@field R32G32_UINT integer
           ---@field R32G32_SINT integer
           ---@field R32G32_SFLOAT integer
           ---@field R32G32B32_UINT integer
           ---@field R32G32B32_SINT integer
           ---@field R32G32B32_SFLOAT integer
           ---@field R32G32B32A32_UINT integer
           ---@field R32G32B32A32_SINT integer
           ---@field R32G32B32A32_SFLOAT integer
           ---@field D16_UNORM integer
           ---@field D24_UNORM_S8_UINT integer
           ---@field D32_SFLOAT integer
           ---@field D32_SFLOAT_S8_UINT integer
           ---@field BC1_UNORM integer
           ---@field BC1_UNORM_SRGB integer
           ---@field BC2_UNORM integer
           ---@field BC2_UNORM_SRGB integer
           ---@field BC3_UNORM integer
           ---@field BC3_UNORM_SRGB integer
           ---@field BC4_UNORM integer
           ---@field BC4_SNORM integer
           ---@field BC5_UNORM integer
           ---@field BC5_SNORM integer
           ---@field BC6H_UFLOAT integer
           ---@field BC6H_SFLOAT integer
           ---@field BC7_UNORM integer
           ---@field BC7_UNORM_SRGB integer
           ImageFormat = vireo.ImageFormat,

           ---@class vireo.BufferType
           ---@field VERTEX integer
           ---@field INDEX integer
           ---@field INDIRECT integer
           ---@field UNIFORM integer
           ---@field STORAGE integer
           ---@field DEVICE_STORAGE integer
           ---@field READWRITE_STORAGE integer
           ---@field BUFFER_UPLOAD integer
           ---@field BUFFER_DOWNLOAD integer
           ---@field IMAGE_UPLOAD integer
           ---@field IMAGE_DOWNLOAD integer
           BufferType = vireo.BufferType,

           ---@class vireo.IndexType
           ---@field UINT16 integer
           ---@field UINT32 integer
           IndexType = vireo.IndexType,

           ---@class vireo.DescriptorType
           ---@field UNIFORM integer
           ---@field UNIFORM_DYNAMIC integer
           ---@field STORAGE integer
           ---@field DEVICE_STORAGE integer
           ---@field READWRITE_STORAGE integer
           ---@field SAMPLED_IMAGE integer
           ---@field SAMPLER integer
           ---@field READWRITE_IMAGE integer
           DescriptorType = vireo.DescriptorType,

           ---@class vireo.CommandType
           ---@field GRAPHIC integer
           ---@field TRANSFER integer
           ---@field COMPUTE integer
           CommandType = vireo.CommandType,

           ---@class vireo.AttributeFormat
           ---@field R32_FLOAT integer
           ---@field R32G32_FLOAT integer
           ---@field R32G32B32_FLOAT integer
           ---@field R32G32B32A32_FLOAT integer
           ---@field R32_SINT integer
           ---@field R32G32_SINT integer
           ---@field R32G32B32_SINT integer
           ---@field R32G32B32A32_SINT integer
           ---@field R32_UINT integer
           ---@field R32G32_UINT integer
           ---@field R32G32B32_UINT integer
           ---@field R32G32B32A32_UINT integer
           AttributeFormat = vireo.AttributeFormat,

           ---@class vireo.RenderTargetType
           ---@field COLOR integer
           ---@field DEPTH integer
           ---@field DEPTH_STENCIL integer
           RenderTargetType = vireo.RenderTargetType,

           ---@class vireo.CullMode
           ---@field NONE integer
           ---@field FRONT integer
           ---@field BACK integer
           CullMode = vireo.CullMode,

           ---@class vireo.PrimitiveTopology
           ---@field POINT_LIST integer
           ---@field LINE_LIST integer
           ---@field LINE_STRIP integer
           ---@field TRIANGLE_LIST integer
           ---@field TRIANGLE_STRIP integer
           PrimitiveTopology = vireo.PrimitiveTopology,

           ---@class vireo.PolygonMode
           ---@field FILL integer
           ---@field WIREFRAME integer
           PolygonMode = vireo.PolygonMode,

           ---@class vireo.CompareOp
           ---@field NEVER integer
           ---@field LESS integer
           ---@field EQUAL integer
           ---@field LESS_OR_EQUAL integer
           ---@field GREATER integer
           ---@field NOT_EQUAL integer
           ---@field GREATER_OR_EQUAL integer
           ---@field ALWAYS integer
           CompareOp = vireo.CompareOp,

           ---@class vireo.StencilOp
           ---@field KEEP integer
           ---@field ZERO integer
           ---@field REPLACE integer
           ---@field INCREMENT_AND_CLAMP integer
           ---@field DECREMENT_AND_CLAMP integer
           ---@field INVERT integer
           ---@field INCREMENT_AND_WRAP integer
           ---@field DECREMENT_AND_WRAP integer
           StencilOp = vireo.StencilOp,

           ---@class vireo.BlendFactor
           ---@field ZERO integer
           ---@field ONE integer
           ---@field SRC_COLOR integer
           ---@field ONE_MINUS_SRC_COLOR integer
           ---@field DST_COLOR integer
           ---@field ONE_MINUS_DST_COLOR integer
           ---@field SRC_ALPHA integer
           ---@field ONE_MINUS_SRC_ALPHA integer
           ---@field DST_ALPHA integer
           ---@field ONE_MINUS_DST_ALPHA integer
           ---@field CONSTANT_COLOR integer
           ---@field ONE_MINUS_CONSTANT_COLOR integer
           ---@field CONSTANT_ALPHA integer
           ---@field ONE_MINUS_CONSTANT_ALPHA integer
           ---@field SRC_ALPHA_SATURATE integer
           ---@field SRC1_COLOR integer
           ---@field ONE_MINUS_SRC1_COLOR integer
           ---@field SRC1_ALPHA integer
           ---@field ONE_MINUS_SRC1_ALPHA integer
           BlendFactor = vireo.BlendFactor,

           ---@class vireo.BlendOp
           ---@field ADD integer
           ---@field SUBTRACT integer
           ---@field REVERSE_SUBTRACT integer
           ---@field MIN integer
           ---@field MAX integer
           BlendOp = vireo.BlendOp,

           ---@class vireo.LogicOp
           ---@field CLEAR integer
           ---@field SET integer
           ---@field COPY integer
           ---@field COPY_INVERTED integer
           ---@field NOOP integer
           ---@field INVERT integer
           ---@field AND integer
           ---@field NAND integer
           ---@field OR integer
           ---@field NOR integer
           ---@field XOR integer
           ---@field EQUIV integer
           ---@field AND_REVERSE integer
           ---@field AND_INVERTED integer
           ---@field OR_REVERSE integer
           ---@field OR_INVERTED integer
           LogicOp = vireo.LogicOp,

           ---@class vireo.ColorWriteMask
           ---@field RED integer
           ---@field GREEN integer
           ---@field BLUE integer
           ---@field ALPHA integer
           ---@field ALL integer
           ColorWriteMask = vireo.ColorWriteMask,

           ---@class vireo.ShaderStage
           ---@field ALL integer
           ---@field VERTEX integer
           ---@field FRAGMENT integer
           ---@field HULL integer
           ---@field DOMAIN integer
           ---@field GEOMETRY integer
           ---@field COMPUTE integer
           ShaderStage = vireo.ShaderStage,

           ---@class vireo.WaitStage
           ---@field NONE integer
           ---@field PIPELINE_TOP integer
           ---@field VERTEX_INPUT integer
           ---@field VERTEX_SHADER integer
           ---@field DEPTH_STENCIL_TEST_BEFORE_FRAGMENT_SHADER integer
           ---@field FRAGMENT_SHADER integer
           ---@field DEPTH_STENCIL_TEST_AFTER_FRAGMENT_SHADER integer
           ---@field COLOR_OUTPUT integer
           ---@field COMPUTE_SHADER integer
           ---@field TRANSFER integer
           ---@field PIPELINE_BOTTOM integer
           ---@field ALL_GRAPHICS integer
           ---@field ALL_COMMANDS integer
           ---@field COPY integer
           ---@field RESOLV integer
           ---@field BLIT integer
           ---@field CLEAR integer
           WaitStage = vireo.WaitStage,

           ---@class vireo.ResourceState
           ---@field UNDEFINED integer
           ---@field GENERAL integer
           ---@field RENDER_TARGET_COLOR integer
           ---@field RENDER_TARGET_DEPTH integer
           ---@field RENDER_TARGET_DEPTH_READ integer
           ---@field RENDER_TARGET_DEPTH_STENCIL integer
           ---@field RENDER_TARGET_DEPTH_STENCIL_READ integer
           ---@field DISPATCH_TARGET integer
           ---@field PRESENT integer
           ---@field COPY_SRC integer
           ---@field COPY_DST integer
           ---@field SHADER_READ integer
           ---@field COMPUTE_READ integer
           ---@field COMPUTE_WRITE integer
           ---@field INDIRECT_DRAW integer
           ---@field VERTEX_INPUT integer
           ---@field UNIFORM integer
           ResourceState = vireo.ResourceState,

           ---@class vireo.MSAA
           ---@field NONE integer
           ---@field X2 integer
           ---@field X4 integer
           ---@field X8 integer
           ---@field X16 integer
           ---@field X32 integer
           ---@field X64 integer
           MSAA = vireo.MSAA,

           ---@class vireo.PresentMode
           ---@field IMMEDIATE integer
           ---@field VSYNC integer
           PresentMode = vireo.PresentMode,

           ---@class vireo.PipelineType
           ---@field GRAPHIC integer
           ---@field COMPUTE integer
           PipelineType = vireo.PipelineType,

           ---@class vireo.SemaphoreType
           ---@field BINARY integer
           ---@field TIMELINE integer
           SemaphoreType = vireo.SemaphoreType,

           ---@class vireo.VideoMemoryAllocationUsage
           ---@field BUFFER integer
           ---@field IMAGE integer
           VideoMemoryAllocationUsage = vireo.VideoMemoryAllocationUsage,

           ------------------------------------------------------------------------
           -- Aliases / typedefs
           ------------------------------------------------------------------------

           ---@alias vireo.DescriptorIndex integer
           DescriptorIndex = nil, -- alias only, no runtime value

           ------------------------------------------------------------------------
           -- Structs
           ------------------------------------------------------------------------

           ---@class vireo.ColorBlendDesc
           ---@field blendEnable boolean
           ---@field srcColorBlendFactor vireo.BlendFactor
           ---@field dstColorBlendFactor vireo.BlendFactor
           ---@field colorBlendOp vireo.BlendOp
           ---@field srcAlphaBlendFactor vireo.BlendFactor
           ---@field dstAlphaBlendFactor vireo.BlendFactor
           ---@field alphaBlendOp vireo.BlendOp
           ---@field colorWriteMask vireo.ColorWriteMask
           ColorBlendDesc = vireo.ColorBlendDesc,

           ---@class vireo.StencilOpState
           ---@field failOp vireo.StencilOp
           ---@field passOp vireo.StencilOp
           ---@field depthFailOp vireo.StencilOp
           ---@field compareOp vireo.CompareOp
           ---@field compareMask integer
           ---@field writeMask integer
           StencilOpState = vireo.StencilOpState,

           ---@class vireo.PhysicalDeviceDesc
           ---@field name string
           ---@field dedicatedVideoMemory integer
           ---@field dedicatedSystemMemory integer
           ---@field sharedSystemMemory integer
           PhysicalDeviceDesc = vireo.PhysicalDeviceDesc,

           ---@class vireo.Extent
           ---@field width integer
           ---@field height integer
           Extent = vireo.Extent,

           ---@class vireo.Rect
           ---@field x integer
           ---@field y integer
           ---@field width integer
           ---@field height integer
           Rect = vireo.Rect,

           ---@class vireo.Viewport
           ---@field x number
           ---@field y number
           ---@field width number
           ---@field height number
           ---@field minDepth number
           ---@field maxDepth number
           Viewport = vireo.Viewport,

           ---@class vireo.PushConstantsDesc
           ---@field stage vireo.ShaderStage
           ---@field size integer
           ---@field offset integer
           PushConstantsDesc = vireo.PushConstantsDesc,

           ---@class vireo.VertexAttributeDesc
           ---@field binding string
           ---@field format vireo.AttributeFormat
           ---@field offset integer
           VertexAttributeDesc = vireo.VertexAttributeDesc,

           ---@class vireo.DepthClearValue
           ---@field depth number
           ---@field stencil integer
           DepthClearValue = vireo.DepthClearValue,

           ---@class vireo.ClearValue
           ---@field color number[]            @length 4
           ---@field depthStencil vireo.DepthClearValue
           ClearValue = vireo.ClearValue,

           ---@class vireo.VideoMemoryAllocationDesc
           ---@field usage vireo.VideoMemoryAllocationUsage
           ---@field name string
           ---@field size integer
           ---@field ref any
           VideoMemoryAllocationDesc = vireo.VideoMemoryAllocationDesc,

           ---@class vireo.RenderTargetDesc
           ---@field swapChain vireo.SwapChain|nil
           ---@field renderTarget vireo.RenderTarget|nil
           ---@field multisampledRenderTarget vireo.RenderTarget|nil
           ---@field clear boolean
           ---@field clearValue vireo.ClearValue
           ---@field discardAfterRender boolean
           RenderTargetDesc = vireo.RenderTargetDesc,

           ---@class vireo.RenderingConfiguration
           ---@field colorRenderTargets vireo.RenderTargetDesc[]
           ---@field depthStencilRenderTarget vireo.RenderTarget|nil
           ---@field multisampledDepthStencilRenderTarget vireo.RenderTarget|nil
           ---@field depthTestEnable boolean
           ---@field stencilTestEnable boolean
           ---@field clearDepthStencil boolean
           ---@field depthStencilClearValue vireo.ClearValue
           ---@field discardDepthStencilAfterRender boolean
           RenderingConfiguration = vireo.RenderingConfiguration,

           ---@class vireo.BufferUploadInfo
           ---@field buffer vireo.Buffer
           ---@field data lightuserdata|any
           BufferUploadInfo = vireo.BufferUploadInfo,

           ---@class vireo.ImageUploadInfo
           ---@field image vireo.Image
           ---@field data lightuserdata|any
           ImageUploadInfo = vireo.ImageUploadInfo,

           ---@class vireo.DrawIndirectCommand
           ---@field vertexCount integer
           ---@field instanceCount integer
           ---@field firstVertex integer
           ---@field firstInstance integer
           DrawIndirectCommand = vireo.DrawIndirectCommand,

           ---@class vireo.DrawIndexedIndirectCommand
           ---@field indexCount integer
           ---@field instanceCount integer
           ---@field firstIndex integer
           ---@field vertexOffset integer
           ---@field firstInstance integer
           DrawIndexedIndirectCommand = vireo.DrawIndexedIndirectCommand,

           ---@class vireo.BufferCopyRegion
           ---@field srcOffset integer
           ---@field dstOffset integer
           ---@field size integer
           BufferCopyRegion = vireo.BufferCopyRegion,

           ---@class vireo.GraphicPipelineConfiguration
           ---@field resources vireo.PipelineResources
           ---@field colorRenderFormats vireo.ImageFormat[]
           ---@field colorBlendDesc vireo.ColorBlendDesc[]
           ---@field vertexInputLayout vireo.VertexInputLayout|nil
           ---@field vertexShader vireo.ShaderModule|nil
           ---@field fragmentShader vireo.ShaderModule|nil
           ---@field hullShader vireo.ShaderModule|nil
           ---@field domainShader vireo.ShaderModule|nil
           ---@field geometryShader vireo.ShaderModule|nil
           ---@field primitiveTopology vireo.PrimitiveTopology
           ---@field msaa vireo.MSAA
           ---@field cullMode vireo.CullMode
           ---@field polygonMode vireo.PolygonMode
           ---@field frontFaceCounterClockwise boolean
           ---@field depthStencilImageFormat vireo.ImageFormat
           ---@field depthTestEnable boolean
           ---@field depthWriteEnable boolean
           ---@field depthCompareOp vireo.CompareOp
           ---@field depthBiasEnable boolean
           ---@field depthBiasConstantFactor number
           ---@field depthBiasClamp number
           ---@field depthBiasSlopeFactor number
           ---@field stencilTestEnable boolean
           ---@field frontStencilOpState vireo.StencilOpState
           ---@field backStencilOpState vireo.StencilOpState
           ---@field logicOpEnable boolean
           ---@field logicOp vireo.LogicOp
           ---@field alphaToCoverageEnable boolean
           GraphicPipelineConfiguration = vireo.GraphicPipelineConfiguration,

           ------------------------------------------------------------------------
           -- Classes / objets
           ------------------------------------------------------------------------

           ---@class vireo.Fence
           ---@field wait fun(self:vireo.Fence):nil
           ---@field reset fun(self:vireo.Fence):nil
           Fence = vireo.Fence,

           ---@class vireo.Semaphore
           ---@field getType fun(self:vireo.Semaphore):vireo.SemaphoreType
           ---@field getValue fun(self:vireo.Semaphore):integer
           ---@field setValue fun(self:vireo.Semaphore, value:integer):nil
           ---@field incrementValue fun(self:vireo.Semaphore):nil
           ---@field decrementValue fun(self:vireo.Semaphore):nil
           Semaphore = vireo.Semaphore,

           ---@class vireo.Instance
           Instance = vireo.Instance,

           ---@class vireo.PhysicalDevice
           ---@field getDescription fun(self:vireo.PhysicalDevice):vireo.PhysicalDeviceDesc
           PhysicalDevice = vireo.PhysicalDevice,

           ---@class vireo.Device
           ---@field haveDedicatedTransferQueue fun(self:vireo.Device):boolean
           Device = vireo.Device,

           ---@class vireo.Buffer
           ---@field WHOLE_SIZE integer
           ---@field getSize fun(self:vireo.Buffer):integer
           ---@field getType fun(self:vireo.Buffer):vireo.BufferType
           ---@field getInstanceSize fun(self:vireo.Buffer):integer
           ---@field getInstanceSizeAligned fun(self:vireo.Buffer):integer
           ---@field getInstanceCount fun(self:vireo.Buffer):integer
           ---@field getMappedAddress fun(self:vireo.Buffer):lightuserdata|any
           ---@field map fun(self:vireo.Buffer):nil
           ---@field unmap fun(self:vireo.Buffer):nil
           ---@field write fun(self:vireo.Buffer, data:lightuserdata|any, size:integer|nil, offset:integer|nil):nil
           ---@field getMemoryAllocations fun():vireo.VideoMemoryAllocationDesc[]
           Buffer = vireo.Buffer,

           ---@class vireo.Sampler
           Sampler = vireo.Sampler,

           ---@class vireo.Image
           ---@field IMAGE_ROW_PITCH_ALIGNMENT integer
           ---@field getFormat fun(self:vireo.Image):vireo.ImageFormat
           ---@field getWidth fun(self:vireo.Image):integer
           ---@field getHeight fun(self:vireo.Image):integer
           ---@field getMipLevels fun(self:vireo.Image):integer
           ---@field getArraySize fun(self:vireo.Image):integer
           ---@field getRowPitch fun(self:vireo.Image, mipLevel:integer|nil):integer
           ---@field getRowLength fun(self:vireo.Image, mipLevel:integer|nil):integer
           ---@field getImageSize fun(self:vireo.Image, mipLevel:integer|nil):integer
           ---@field getAlignedImageSize fun(self:vireo.Image, mipLevel:integer|nil):integer
           ---@field getAlignedRowPitch fun(self:vireo.Image, mipLevel:integer|nil):integer
           ---@field getAlignedRowLength fun(self:vireo.Image, mipLevel:integer|nil):integer
           ---@field isReadWrite fun(self:vireo.Image):boolean
           ---@field isDepthFormat fun(self:vireo.Image):boolean
           ---@field isDepthStencilFormat fun(self:vireo.Image):boolean
           ---@field getPixelSize fun(format:vireo.ImageFormat):integer
           ---@field getMemoryAllocations fun():vireo.VideoMemoryAllocationDesc[]
           Image = vireo.Image,

           ---@class vireo.RenderTarget
           ---@field getImage fun(self:vireo.RenderTarget):vireo.Image
           ---@field getType fun(self:vireo.RenderTarget):vireo.RenderTargetType
           RenderTarget = vireo.RenderTarget,

           ---@class vireo.DescriptorLayout
           ---@field add fun(self:vireo.DescriptorLayout, index:vireo.DescriptorIndex, type:vireo.DescriptorType, count:integer|nil):vireo.DescriptorLayout
           ---@field build fun(self:vireo.DescriptorLayout):nil
           ---@field getCapacity fun(self:vireo.DescriptorLayout):integer
           ---@field isDynamicUniform fun(self:vireo.DescriptorLayout):boolean
           ---@field isSamplers fun(self:vireo.DescriptorLayout):boolean
           DescriptorLayout = vireo.DescriptorLayout,

           ---@class vireo.DescriptorSet
           ---@field update fun(self:vireo.DescriptorSet, index:vireo.DescriptorIndex, bufferOrImageOrSampler:any, counterBuffer:any|nil):nil
           ---@field getLayout fun(self:vireo.DescriptorSet):vireo.DescriptorLayout
           DescriptorSet = vireo.DescriptorSet,

           ---@class vireo.VertexInputLayout
           VertexInputLayout = vireo.VertexInputLayout,

           ---@class vireo.ShaderModule
           ShaderModule = vireo.ShaderModule,

           ---@class vireo.PipelineResources
           PipelineResources = vireo.PipelineResources,

           ---@class vireo.Pipeline
           ---@field getResources fun(self:vireo.Pipeline):vireo.PipelineResources
           ---@field getType fun(self:vireo.Pipeline):vireo.PipelineType
           Pipeline = vireo.Pipeline,

           ---@class vireo.ComputePipeline : vireo.Pipeline
           ComputePipeline = vireo.ComputePipeline,

           ---@class vireo.GraphicPipeline : vireo.Pipeline
           GraphicPipeline = vireo.GraphicPipeline,

           ---@class vireo.SwapChain
           ---@field getExtent fun(self:vireo.SwapChain):vireo.Extent
           ---@field getAspectRatio fun(self:vireo.SwapChain):number
           ---@field getCurrentFrameIndex fun(self:vireo.SwapChain):integer
           ---@field getFramesInFlight fun(self:vireo.SwapChain):integer
           ---@field getFormat fun(self:vireo.SwapChain):vireo.ImageFormat
           ---@field nextFrameIndex fun(self:vireo.SwapChain):nil
           ---@field acquire fun(self:vireo.SwapChain, fence:vireo.Fence):boolean
           ---@field present fun(self:vireo.SwapChain):nil
           ---@field recreate fun(self:vireo.SwapChain):nil
           ---@field waitIdle fun(self:vireo.SwapChain):nil
           SwapChain = vireo.SwapChain,

           ---@class vireo.CommandList
           ---@field begin fun(self:vireo.CommandList):nil
           ---@field end fun(self:vireo.CommandList):nil
           ---@field upload fun(self:vireo.CommandList, destination:any, source:any, firstMipLevel:integer|nil):nil
           ---@field uploadArray fun(self:vireo.CommandList, destination:vireo.Image, sources:any[], firstMipLevel:integer|nil):nil
           ---@field copy fun(self:vireo.CommandList, ...):nil
           ---@field beginRendering fun(self:vireo.CommandList, config:vireo.RenderingConfiguration):nil
           ---@field endRendering fun(self:vireo.CommandList):nil
           ---@field dispatch fun(self:vireo.CommandList, x:integer, y:integer, z:integer):nil
           ---@field bindVertexBuffer fun(self:vireo.CommandList, buffer:vireo.Buffer, offset:integer|nil):nil
           ---@field bindVertexBuffers fun(self:vireo.CommandList, buffers:vireo.Buffer[], offsets:integer[]|nil):nil
           ---@field bindIndexBuffer fun(self:vireo.CommandList, buffer:vireo.Buffer, indexType:vireo.IndexType|nil, firstIndex:integer|nil):nil
           ---@field bindPipeline fun(self:vireo.CommandList, pipeline:vireo.Pipeline):nil
           ---@field bindDescriptors fun(self:vireo.CommandList, descriptors:vireo.DescriptorSet[], firstSet:integer|nil):nil
           ---@field bindDescriptor fun(self:vireo.CommandList, descriptor:vireo.DescriptorSet, set:integer, offset:integer|nil):nil
           ---@field draw fun(self:vireo.CommandList, vertexCountPerInstance:integer, instanceCount:integer|nil, firstVertex:integer|nil, firstInstance:integer|nil):nil
           ---@field drawIndexed fun(self:vireo.CommandList, indexCountPerInstance:integer, instanceCount:integer|nil, firstIndex:integer|nil, firstVertex:integer|nil, firstInstance:integer|nil):nil
           ---@field drawIndirect fun(self:vireo.CommandList, buffer:vireo.Buffer, offset:integer, drawCount:integer, stride:integer, firstCommandOffset:integer|nil):nil
           ---@field drawIndexedIndirect fun(self:vireo.CommandList, buffer:vireo.Buffer, offset:integer, maxDrawCount:integer, stride:integer, firstCommandOffset:integer|nil):nil
           ---@field drawIndexedIndirectCount fun(self:vireo.CommandList, buffer:vireo.Buffer, offset:integer, countBuffer:vireo.Buffer, countOffset:integer, maxDrawCount:integer, stride:integer, firstCommandOffset:integer|nil):nil
           ---@field setViewports fun(self:vireo.CommandList, viewports:vireo.Viewport[]):nil
           ---@field setScissors fun(self:vireo.CommandList, rects:vireo.Rect[]):nil
           ---@field setViewport fun(self:vireo.CommandList, viewport:vireo.Viewport):nil
           ---@field setScissors fun(self:vireo.CommandList, rect:vireo.Rect):nil
           ---@field setStencilReference fun(self:vireo.CommandList, reference:integer):nil
           ---@field barrier fun(self:vireo.CommandList, ...):nil
           ---@field pushConstants fun(self:vireo.CommandList, resources:vireo.PipelineResources, desc:vireo.PushConstantsDesc, data:any):nil
           ---@field cleanup fun(self:vireo.CommandList):nil
           CommandList = vireo.CommandList,

           ---@class vireo.CommandAllocator
           ---@field reset fun(self:vireo.CommandAllocator):nil
           ---@field createCommandList fun(self:vireo.CommandAllocator, pipelineOrNil:vireo.Pipeline|nil):vireo.CommandList
           ---@field getCommandListType fun(self:vireo.CommandAllocator):vireo.CommandType
           CommandAllocator = vireo.CommandAllocator,

           ---@class vireo.SubmitQueue
           ---@field submit fun(self:vireo.SubmitQueue, ...):nil
           ---@field waitIdle fun(self:vireo.SubmitQueue):nil
           SubmitQueue = vireo.SubmitQueue,

           ---@class vireo.Vireo
           ---@field waitIdle fun(self:vireo.Vireo):nil
           ---@field createSwapChain fun(self:vireo.Vireo, format:vireo.ImageFormat, presentQueue:vireo.SubmitQueue, windowHandle:any, presentMode:vireo.PresentMode|nil, framesInFlight:integer|nil):vireo.SwapChain
           ---@field createSubmitQueue fun(self:vireo.Vireo, commandType:vireo.CommandType, name:string|nil):vireo.SubmitQueue
           ---@field createFence fun(self:vireo.Vireo, createSignaled:boolean|nil, name:string|nil):vireo.Fence
           ---@field createSemaphore fun(self:vireo.Vireo, type:vireo.SemaphoreType, name:string|nil):vireo.Semaphore
           ---@field createCommandAllocator fun(self:vireo.Vireo, type:vireo.CommandType):vireo.CommandAllocator
           ---@field createVertexLayout fun(self:vireo.Vireo, size:integer, attributes:vireo.VertexAttributeDesc[]):vireo.VertexInputLayout
           ---@field createShaderModule fun(self:vireo.Vireo, dataOrFile:any):vireo.ShaderModule
           ---@field createPipelineResources fun(self:vireo.Vireo, layouts:vireo.DescriptorLayout[]|nil, pushConstant:vireo.PushConstantsDesc|nil, name:string|nil):vireo.PipelineResources
           ---@field createComputePipeline fun(self:vireo.Vireo, resources:vireo.PipelineResources, shader:vireo.ShaderModule, name:string|nil):vireo.ComputePipeline
           ---@field createGraphicPipeline fun(self:vireo.Vireo, config:vireo.GraphicPipelineConfiguration, name:string|nil):vireo.GraphicPipeline
           ---@field createBuffer fun(self:vireo.Vireo, type:vireo.BufferType, size:integer, count:integer|nil, name:string|nil):vireo.Buffer
           ---@field createImage fun(self:vireo.Vireo, format:vireo.ImageFormat, width:integer, height:integer, mipLevels:integer|nil, arraySize:integer|nil, name:string|nil):vireo.Image
           ---@field createReadWriteImage fun(self:vireo.Vireo, format:vireo.ImageFormat, width:integer, height:integer, mipLevels:integer|nil, arraySize:integer|nil, name:string|nil):vireo.Image
           ---@field createRenderTarget fun(self:vireo.Vireo, ...):vireo.RenderTarget
           ---@field createDescriptorLayout fun(self:vireo.Vireo, name:string|nil):vireo.DescriptorLayout
           ---@field createSamplerDescriptorLayout fun(self:vireo.Vireo, name:string|nil):vireo.DescriptorLayout
           ---@field createDynamicUniformDescriptorLayout fun(self:vireo.Vireo, name:string|nil):vireo.DescriptorLayout
           ---@field createDescriptorSet fun(self:vireo.Vireo, layout:vireo.DescriptorLayout, name:string|nil):vireo.DescriptorSet
           ---@field createSampler fun(self:vireo.Vireo, minFilter:vireo.Filter, magFilter:vireo.Filter, addressModeU:vireo.AddressMode, addressModeV:vireo.AddressMode, addressModeW:vireo.AddressMode, minLod:number|nil, maxLod:number|nil, anisotropyEnable:boolean|nil, mipMapMode:vireo.MipMapMode|nil, compareOp:vireo.CompareOp|nil):vireo.Sampler
           ---@field getShaderFileExtension fun(self:vireo.Vireo):string
           ---@field getPhysicalDevice fun(self:vireo.Vireo):vireo.PhysicalDevice
           ---@field getDevice fun(self:vireo.Vireo):vireo.Device
           ---@field getInstance fun(self:vireo.Vireo):vireo.Instance
           Vireo = vireo.Vireo,
       }

