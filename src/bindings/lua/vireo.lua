---@meta
---@diagnostic disable: missing-return, lowercase-global

------------------------------------------------------------------------
-- Enums (exposed as namespaces)
------------------------------------------------------------------------

---@class vireo.Backend
---@field UNDEFINED integer
---@field DIRECTX integer
---@field VULKAN integer

---@class vireo.Filter
---@field NEAREST integer
---@field LINEAR integer

---@class vireo.AddressMode
---@field REPEAT integer
---@field MIRRORED_REPEAT integer
---@field CLAMP_TO_EDGE integer
---@field CLAMP_TO_BORDER integer

---@class vireo.FilterMode
---@field NEAREST integer
---@field LINEAR integer

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

---@class vireo.IndexType
---@field UINT16 integer
---@field UINT32 integer

---@class vireo.DescriptorType
---@field UNIFORM integer
---@field UNIFORM_DYNAMIC integer
---@field STORAGE integer
---@field DEVICE_STORAGE integer
---@field READWRITE_STORAGE integer
---@field SAMPLED_IMAGE integer
---@field SAMPLER integer
---@field READWRITE_IMAGE integer

---@class vireo.CommandType
---@field GRAPHIC integer
---@field TRANSFER integer
---@field COMPUTE integer

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

---@class vireo.RenderTargetType
---@field COLOR integer
---@field DEPTH integer
---@field DEPTH_STENCIL integer

---@class vireo.CullMode
---@field NONE integer
---@field FRONT integer
---@field BACK integer

---@class vireo.PrimitiveTopology
---@field POINT_LIST integer
---@field LINE_LIST integer
---@field LINE_STRIP integer
---@field TRIANGLE_LIST integer
---@field TRIANGLE_STRIP integer

---@class vireo.PolygonMode
---@field FILL integer
---@field WIREFRAME integer

---@class vireo.CompareOp
---@field NEVER integer
---@field LESS integer
---@field EQUAL integer
---@field LESS_OR_EQUAL integer
---@field GREATER integer
---@field NOT_EQUAL integer
---@field GREATER_OR_EQUAL integer
---@field ALWAYS integer

---@class vireo.StencilOp
---@field KEEP integer
---@field ZERO integer
---@field REPLACE integer
---@field INCREMENT_AND_CLAMP integer
---@field DECREMENT_AND_CLAMP integer
---@field INVERT integer
---@field INCREMENT_AND_WRAP integer
---@field DECREMENT_AND_WRAP integer

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

---@class vireo.BlendOp
---@field ADD integer
---@field SUBTRACT integer
---@field REVERSE_SUBTRACT integer
---@field MIN integer
---@field MAX integer

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

---@class vireo.ColorWriteMask
---@field RED integer
---@field GREEN integer
---@field BLUE integer
---@field ALPHA integer
---@field ALL integer

---@class vireo.ShaderStage
---@field ALL integer
---@field VERTEX integer
---@field FRAGMENT integer
---@field HULL integer
---@field DOMAIN integer
---@field GEOMETRY integer
---@field COMPUTE integer

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

---@class vireo.ResourceState
---@field UNDEFINED integer
---@field GENERAL integer
---@field RENDER_TARGET_COLOR integer
---@field RENDER_TARGET_DEPTH integer
---@field RENDER_TARGET_DEPTH_READ integer
---@field RENDER_TARGET_DEPTH_STENCIL integer
---@field RENDER_TARGET_DEPTH_STENCIL_READ integer
---@field PRESENT integer
---@field COPY_SRC integer
---@field COPY_DST integer
---@field SHADER_READ integer
---@field COMPUTE_READ integer
---@field COMPUTE_WRITE integer
---@field INDIRECT_DRAW integer
---@field VERTEX_INPUT integer
---@field UNIFORM integer

---@class vireo.MSAA
---@field NONE integer
---@field X2 integer
---@field X4 integer
---@field X8 integer
---@field X16 integer
---@field X32 integer
---@field X64 integer

---@class vireo.PresentMode
---@field IMMEDIATE integer
---@field VSYNC integer

---@class vireo.PipelineType
---@field GRAPHIC integer
---@field COMPUTE integer

---@class vireo.SemaphoreType
---@field BINARY integer
---@field TIMELINE integer

------------------------------------------------------------------------
-- Aliases / typedefs
------------------------------------------------------------------------

---@alias vireo.DescriptorIndex integer

------------------------------------------------------------------------
-- Structs
-- Note: properties use snake_case as registered in LuaBindings
------------------------------------------------------------------------

---@class vireo.ColorBlendDesc
---@field blend_enable boolean
---@field src_color_blend_factor vireo.BlendFactor
---@field dst_color_blend_factor vireo.BlendFactor
---@field color_blend_op vireo.BlendOp
---@field src_alpha_blend_factor vireo.BlendFactor
---@field dst_alpha_blend_factor vireo.BlendFactor
---@field alpha_blend_op vireo.BlendOp
---@field color_write_mask vireo.ColorWriteMask

---@class vireo.StencilOpState
---@field fail_op vireo.StencilOp
---@field pass_op vireo.StencilOp
---@field depth_fail_op vireo.StencilOp
---@field compare_op vireo.CompareOp
---@field compare_mask integer
---@field write_mask integer

---@class vireo.PhysicalDeviceDesc
---@field name string
---@field dedicated_video_memory integer
---@field dedicated_system_memory integer
---@field shared_system_memory integer

---@class vireo.Extent
---@field width integer
---@field height integer

---@class vireo.Rect
---Constructor overloads:
---  Rect(width, height)          -- x=0, y=0
---  Rect(x, y, width, height)
---@field x integer
---@field y integer
---@field width integer
---@field height integer

---@class vireo.Viewport
---Constructor overloads:
---  Viewport(width, height)
---  Viewport(x, y, width, height)
---@field x number
---@field y number
---@field width number
---@field height number
---@field min_depth number
---@field max_depth number

---@class vireo.PushConstantsDesc
---@field stage vireo.ShaderStage
---@field size integer
---@field offset integer

---@class vireo.VertexAttributeDesc
---@field binding string
---@field format vireo.AttributeFormat
---@field offset integer

---@class vireo.DepthClearValue
---@field depth number
---@field stencil integer

--- Note: color[] and depthStencil sub-fields are NOT currently exposed
--- via properties in LuaBindings (the helper functions are commented out).
---@class vireo.ClearValue

---@class vireo.VideoMemoryAllocationDesc
---@field usage vireo.VideoMemoryAllocationUsage
---@field name string
---@field size integer
---@field ref any

---@class vireo.RenderTargetDesc
---@field swap_chain vireo.SwapChain|nil
---@field render_target vireo.RenderTarget|nil
---@field multisampled_render_target vireo.RenderTarget|nil
---@field clear boolean
---@field clear_value vireo.ClearValue
---@field discard_after_render boolean

---@class vireo.RenderingConfiguration
---@field color_render_targets vireo.RenderTargetDesc[]
---@field depth_stencil_render_target vireo.RenderTarget|nil
---@field multisampled_depth_stencil_render_target vireo.RenderTarget|nil
---@field depth_test_enable boolean
---@field stencil_test_enable boolean
---@field clear_depth_stencil boolean
---@field depth_stencil_clear_value vireo.ClearValue
---@field discard_depth_stencil_after_render boolean

---@class vireo.DrawIndirectCommand
---@field vertex_count integer
---@field instance_count integer
---@field first_vertex integer
---@field first_instance integer

---@class vireo.DrawIndexedIndirectCommand
---@field index_count integer
---@field instance_count integer
---@field first_index integer
---@field vertex_offset integer
---@field first_instance integer

---@class vireo.BufferCopyRegion
---@field src_offset integer
---@field dst_offset integer
---@field size integer

---@class vireo.GraphicPipelineConfiguration
---@field resources vireo.PipelineResources
---@field color_render_formats vireo.ImageFormat[]
---@field color_blend_desc vireo.ColorBlendDesc[]
---@field vertex_input_layout vireo.VertexInputLayout|nil
---@field vertex_shader vireo.ShaderModule|nil
---@field fragment_shader vireo.ShaderModule|nil
---@field hull_shader vireo.ShaderModule|nil
---@field domain_shader vireo.ShaderModule|nil
---@field geometry_shader vireo.ShaderModule|nil
---@field primitive_topology vireo.PrimitiveTopology
---@field msaa vireo.MSAA
---@field cull_mode vireo.CullMode
---@field polygon_mode vireo.PolygonMode
---@field front_face_counter_clockwise boolean
---@field depth_stencil_image_format vireo.ImageFormat
---@field depth_test_enable boolean
---@field depth_write_enable boolean
---@field depth_compare_op vireo.CompareOp
---@field depth_bias_enable boolean
---@field depth_bias_constant_factor number
---@field depth_bias_clamp number
---@field depth_bias_slope_factor number
---@field stencil_test_enable boolean
---@field front_stencil_op_state vireo.StencilOpState
---@field back_stencil_op_state vireo.StencilOpState
---@field logic_op_enable boolean
---@field logic_op vireo.LogicOp
---@field alpha_to_coverage_enable boolean

------------------------------------------------------------------------
-- Classes / objects
------------------------------------------------------------------------

---@class vireo.Fence
---@field wait fun(self:vireo.Fence):nil
---@field reset fun(self:vireo.Fence):nil

---@class vireo.Semaphore
---@field get_type fun(self:vireo.Semaphore):vireo.SemaphoreType
---@field get_value fun(self:vireo.Semaphore):integer
---@field set_value fun(self:vireo.Semaphore, value:integer):nil
---@field increment_value fun(self:vireo.Semaphore):nil
---@field decrement_value fun(self:vireo.Semaphore):nil

---@class vireo.Instance

--- Note: get_description() is commented out in LuaBindings
---@class vireo.PhysicalDevice

---@class vireo.Device
---@field have_dedicated_transfer_queue fun(self:vireo.Device):boolean

---@class vireo.Buffer
---@field get_size fun(self:vireo.Buffer):integer
---@field get_type fun(self:vireo.Buffer):vireo.BufferType
---@field get_instance_size fun(self:vireo.Buffer):integer
---@field get_instance_size_aligned fun(self:vireo.Buffer):integer
---@field get_instance_count fun(self:vireo.Buffer):integer
---@field get_mapped_address fun(self:vireo.Buffer):lightuserdata|any
---@field map fun(self:vireo.Buffer):nil
---@field unmap fun(self:vireo.Buffer):nil
---@field write fun(self:vireo.Buffer, data:lightuserdata|any, size:integer|nil, offset:integer|nil):nil
---@field get_memory_allocations fun():vireo.VideoMemoryAllocationDesc[]   @static

---@class vireo.Sampler

---@class vireo.Image
---@field get_format fun(self:vireo.Image):vireo.ImageFormat
---@field get_width fun(self:vireo.Image):integer
---@field get_height fun(self:vireo.Image):integer
---@field get_mip_levels fun(self:vireo.Image):integer
---@field get_array_size fun(self:vireo.Image):integer
---@field get_row_pitch fun(self:vireo.Image, mipLevel:integer|nil):integer
---@field get_row_length fun(self:vireo.Image, mipLevel:integer|nil):integer
---@field get_image_size fun(self:vireo.Image, mipLevel:integer|nil):integer
---@field get_aligned_image_size fun(self:vireo.Image, mipLevel:integer|nil):integer
---@field get_aligned_row_pitch fun(self:vireo.Image, mipLevel:integer|nil):integer
---@field get_aligned_row_length fun(self:vireo.Image, mipLevel:integer|nil):integer
---@field is_read_write fun(self:vireo.Image):boolean
---@field get_pixel_size fun(format:vireo.ImageFormat):integer   @static
---@field get_memory_allocations fun():vireo.VideoMemoryAllocationDesc[]   @static

---@class vireo.RenderTarget
---@field get_image fun(self:vireo.RenderTarget):vireo.Image
---@field get_type fun(self:vireo.RenderTarget):vireo.RenderTargetType

---@class vireo.DescriptorLayout
---@field add fun(self:vireo.DescriptorLayout, index:vireo.DescriptorIndex, type:vireo.DescriptorType, count:integer|nil):vireo.DescriptorLayout
---@field build fun(self:vireo.DescriptorLayout):nil
---@field get_capacity fun(self:vireo.DescriptorLayout):integer
---@field is_dynamic_uniform fun(self:vireo.DescriptorLayout):boolean
---@field is_samplers fun(self:vireo.DescriptorLayout):boolean

--- Note: update() overloads are commented out in LuaBindings
---@class vireo.DescriptorSet
---@field get_layout fun(self:vireo.DescriptorSet):vireo.DescriptorLayout

---@class vireo.VertexInputLayout

---@class vireo.ShaderModule

---@class vireo.PipelineResources

---@class vireo.Pipeline
---@field get_resources fun(self:vireo.Pipeline):vireo.PipelineResources
---@field get_type fun(self:vireo.Pipeline):vireo.PipelineType

---@class vireo.ComputePipeline : vireo.Pipeline

---@class vireo.GraphicPipeline : vireo.Pipeline

---@class vireo.SwapChain
---@field get_extent fun(self:vireo.SwapChain):vireo.Extent
---@field get_aspect_ratio fun(self:vireo.SwapChain):number
---@field get_current_frame_index fun(self:vireo.SwapChain):integer
---@field get_frames_in_flight fun(self:vireo.SwapChain):integer
---@field get_format fun(self:vireo.SwapChain):vireo.ImageFormat
---@field next_frame_index fun(self:vireo.SwapChain):nil
---@field acquire fun(self:vireo.SwapChain, fence:vireo.Fence):boolean
---@field present fun(self:vireo.SwapChain):nil
---@field recreate fun(self:vireo.SwapChain):nil
---@field wait_idle fun(self:vireo.SwapChain):nil

---@class vireo.CommandList
---@field begin fun(self:vireo.CommandList):nil
---@field end fun(self:vireo.CommandList):nil
--- Upload a buffer to GPU memory
---@field upload_buffer fun(self:vireo.CommandList, destination:vireo.Buffer, data:lightuserdata|any):nil
--- Upload an image to GPU memory
---@field upload_image fun(self:vireo.CommandList, destination:vireo.Image, data:lightuserdata|any, firstMipLevel:integer):nil
--- Copy a buffer region into an image (single mip level)
---@field copy_buffer_to_image fun(self:vireo.CommandList, src:vireo.Buffer, dst:vireo.Image, mipLevel:integer, arrayLayer:integer, generateMips:boolean):nil
--- Copy a buffer into an image providing explicit byte offsets per mip level
---@field copy_buffer_to_image_levels fun(self:vireo.CommandList, src:vireo.Buffer, dst:vireo.Image, offsets:integer[], generateMips:boolean):nil
--- Copy a buffer into another buffer
---@field copy_buffer_to_buffer fun(self:vireo.CommandList, src:vireo.Buffer, dst:vireo.Buffer, size:integer, srcOffset:integer, dstOffset:integer):nil
--- Copy buffer sub-regions
---@field copy_buffer_regions fun(self:vireo.CommandList, src:vireo.Buffer, dst:vireo.Buffer, regions:vireo.BufferCopyRegion[]):nil
--- Blit / resolve an Image into the SwapChain backbuffer
---@field copy_image_to_swap_chain fun(self:vireo.CommandList, src:vireo.Image, swapChain:vireo.SwapChain):nil
---@field begin_rendering fun(self:vireo.CommandList, config:vireo.RenderingConfiguration):nil
---@field end_rendering fun(self:vireo.CommandList):nil
---@field dispatch fun(self:vireo.CommandList, x:integer, y:integer, z:integer):nil
--- Bind a single vertex buffer at optional byte offset
---@field bind_vertex_buffer fun(self:vireo.CommandList, buffer:vireo.Buffer, offset:integer|nil):nil
--- Bind multiple vertex buffers
---@field bind_vertex_buffers fun(self:vireo.CommandList, buffers:vireo.Buffer[], offsets:integer[]|nil):nil
--- Bind a descriptor set at the given set index
---@field bind_descriptor fun(self:vireo.CommandList, descriptor:vireo.DescriptorSet, set:integer):nil
--- Bind a dynamic-uniform descriptor set with a dynamic offset
---@field bind_descriptor_dynamic fun(self:vireo.CommandList, descriptor:vireo.DescriptorSet, set:integer, offset:integer):nil
---@field draw fun(self:vireo.CommandList, vertexCountPerInstance:integer, instanceCount:integer|nil, firstVertex:integer|nil, firstInstance:integer|nil):nil
---@field draw_indexed fun(self:vireo.CommandList, indexCountPerInstance:integer, instanceCount:integer|nil, firstIndex:integer|nil, firstVertex:integer|nil, firstInstance:integer|nil):nil
--- Indirect draw (non-indexed)
---@field draw_indirect fun(self:vireo.CommandList, buffer:vireo.Buffer, offset:integer, drawCount:integer, stride:integer, firstCommandOffset:integer):nil
--- Indirect indexed draw with a GPU-side count buffer
---@field draw_indexed_indirect_count fun(self:vireo.CommandList, buffer:vireo.Buffer, offset:integer, countBuffer:vireo.Buffer, countOffset:integer, maxDrawCount:integer, stride:integer, firstCommandOffset:integer):nil
--- Indirect indexed draw with a CPU-side draw count
---@field draw_indexed_indirect fun(self:vireo.CommandList, buffer:vireo.Buffer, offset:integer, maxDrawCount:integer, stride:integer, firstCommandOffset:integer):nil
---@field set_viewports fun(self:vireo.CommandList, viewports:vireo.Viewport[]):nil
---@field set_scissors fun(self:vireo.CommandList, rects:vireo.Rect[]):nil
---@field set_viewport fun(self:vireo.CommandList, viewport:vireo.Viewport):nil
--- Set a single scissor rect
---@field set_scissor fun(self:vireo.CommandList, rect:vireo.Rect):nil
---@field set_stencil_reference fun(self:vireo.CommandList, reference:integer):nil
---@field push_constants fun(self:vireo.CommandList, resources:vireo.PipelineResources, desc:vireo.PushConstantsDesc, data:any):nil
---@field cleanup fun(self:vireo.CommandList):nil

---@class vireo.CommandAllocator
---@field reset fun(self:vireo.CommandAllocator):nil
---@field get_command_list_type fun(self:vireo.CommandAllocator):vireo.CommandType

--- Note: submit() overloads are commented out; only wait_idle() is currently bound
---@class vireo.SubmitQueue
---@field wait_idle fun(self:vireo.SubmitQueue):nil

---@class vireo.Vireo
---@field backend vireo.Backend                @read-only property
---@field wait_idle fun(self:vireo.Vireo):nil
---@field create_swap_chain fun(self:vireo.Vireo, format:vireo.ImageFormat, presentQueue:vireo.SubmitQueue, windowHandle:any, presentMode:vireo.PresentMode|nil, framesInFlight:integer|nil):vireo.SwapChain
---@field create_submit_queue fun(self:vireo.Vireo, commandType:vireo.CommandType, name:string|nil):vireo.SubmitQueue
---@field create_fence fun(self:vireo.Vireo, createSignaled:boolean|nil, name:string|nil):vireo.Fence
---@field create_semaphore fun(self:vireo.Vireo, type:vireo.SemaphoreType, name:string|nil):vireo.Semaphore
---@field create_command_allocator fun(self:vireo.Vireo, type:vireo.CommandType):vireo.CommandAllocator
---@field create_vertex_layout fun(self:vireo.Vireo, size:integer, attributes:vireo.VertexAttributeDesc[]):vireo.VertexInputLayout
--- Load a shader module from a file path (extension provided by get_shader_file_extension)
---@field create_shader_module_from_file fun(self:vireo.Vireo, path:string):vireo.ShaderModule
--- Load a shader module from raw byte data with an optional debug name
---@field create_shader_module_from_data fun(self:vireo.Vireo, data:any, name:string):vireo.ShaderModule
---@field create_pipeline_resources fun(self:vireo.Vireo, layouts:vireo.DescriptorLayout[]|nil, pushConstant:vireo.PushConstantsDesc|nil, name:string|nil):vireo.PipelineResources
---@field create_compute_pipeline fun(self:vireo.Vireo, resources:vireo.PipelineResources, shader:vireo.ShaderModule, name:string|nil):vireo.ComputePipeline
---@field create_graphic_pipeline fun(self:vireo.Vireo, config:vireo.GraphicPipelineConfiguration, name:string|nil):vireo.GraphicPipeline
---@field create_buffer fun(self:vireo.Vireo, type:vireo.BufferType, size:integer, count:integer|nil, name:string|nil):vireo.Buffer
---@field create_image fun(self:vireo.Vireo, format:vireo.ImageFormat, width:integer, height:integer, mipLevels:integer|nil, arraySize:integer|nil, name:string|nil):vireo.Image
---@field create_read_write_image fun(self:vireo.Vireo, format:vireo.ImageFormat, width:integer, height:integer, mipLevels:integer|nil, arraySize:integer|nil, name:string|nil):vireo.Image
---@field create_descriptor_layout fun(self:vireo.Vireo, name:string|nil):vireo.DescriptorLayout
---@field create_sampler_descriptor_layout fun(self:vireo.Vireo, name:string|nil):vireo.DescriptorLayout
---@field create_dynamic_uniform_descriptor_layout fun(self:vireo.Vireo, name:string|nil):vireo.DescriptorLayout
---@field create_descriptor_set fun(self:vireo.Vireo, layout:vireo.DescriptorLayout, name:string|nil):vireo.DescriptorSet
---@field create_sampler fun(self:vireo.Vireo, minFilter:vireo.Filter, magFilter:vireo.Filter, addressModeU:vireo.AddressMode, addressModeV:vireo.AddressMode, addressModeW:vireo.AddressMode, minLod:number|nil, maxLod:number|nil, anisotropyEnable:boolean|nil, mipMapMode:vireo.FilterMode|nil, compareOp:vireo.CompareOp|nil):vireo.Sampler
---@field is_backend_supported fun(backend:vireo.Backend):boolean   @static
---@field get_shader_file_extension fun(self:vireo.Vireo):string
---@field get_physical_device fun(self:vireo.Vireo):vireo.PhysicalDevice
---@field get_device fun(self:vireo.Vireo):vireo.Device
---@field get_instance fun(self:vireo.Vireo):vireo.Instance

------------------------------------------------------------------------
-- Global namespace declaration
-- All vireo.* classes above are accessible as fields of this global.
------------------------------------------------------------------------

---@class vireo
---@field Backend vireo.Backend
---@field Filter vireo.Filter
---@field AddressMode vireo.AddressMode
---@field FilterMode vireo.FilterMode
---@field ImageFormat vireo.ImageFormat
---@field BufferType vireo.BufferType
---@field IndexType vireo.IndexType
---@field DescriptorType vireo.DescriptorType
---@field CommandType vireo.CommandType
---@field AttributeFormat vireo.AttributeFormat
---@field RenderTargetType vireo.RenderTargetType
---@field CullMode vireo.CullMode
---@field PrimitiveTopology vireo.PrimitiveTopology
---@field PolygonMode vireo.PolygonMode
---@field CompareOp vireo.CompareOp
---@field StencilOp vireo.StencilOp
---@field BlendFactor vireo.BlendFactor
---@field BlendOp vireo.BlendOp
---@field LogicOp vireo.LogicOp
---@field ColorWriteMask vireo.ColorWriteMask
---@field ShaderStage vireo.ShaderStage
---@field WaitStage vireo.WaitStage
---@field ResourceState vireo.ResourceState
---@field MSAA vireo.MSAA
---@field PresentMode vireo.PresentMode
---@field PipelineType vireo.PipelineType
---@field SemaphoreType vireo.SemaphoreType
---@field ColorBlendDesc vireo.ColorBlendDesc
---@field StencilOpState vireo.StencilOpState
---@field PhysicalDeviceDesc vireo.PhysicalDeviceDesc
---@field Extent vireo.Extent
---@field Rect vireo.Rect
---@field Viewport vireo.Viewport
---@field PushConstantsDesc vireo.PushConstantsDesc
---@field VertexAttributeDesc vireo.VertexAttributeDesc
---@field DepthClearValue vireo.DepthClearValue
---@field ClearValue vireo.ClearValue
---@field VideoMemoryAllocationDesc vireo.VideoMemoryAllocationDesc
---@field RenderTargetDesc vireo.RenderTargetDesc
---@field RenderingConfiguration vireo.RenderingConfiguration
---@field DrawIndirectCommand vireo.DrawIndirectCommand
---@field DrawIndexedIndirectCommand vireo.DrawIndexedIndirectCommand
---@field BufferCopyRegion vireo.BufferCopyRegion
---@field GraphicPipelineConfiguration vireo.GraphicPipelineConfiguration
---@field Fence vireo.Fence
---@field Semaphore vireo.Semaphore
---@field Instance vireo.Instance
---@field PhysicalDevice vireo.PhysicalDevice
---@field Device vireo.Device
---@field Buffer vireo.Buffer
---@field Sampler vireo.Sampler
---@field Image vireo.Image
---@field RenderTarget vireo.RenderTarget
---@field DescriptorLayout vireo.DescriptorLayout
---@field DescriptorSet vireo.DescriptorSet
---@field VertexInputLayout vireo.VertexInputLayout
---@field ShaderModule vireo.ShaderModule
---@field PipelineResources vireo.PipelineResources
---@field Pipeline vireo.Pipeline
---@field ComputePipeline vireo.ComputePipeline
---@field GraphicPipeline vireo.GraphicPipeline
---@field SwapChain vireo.SwapChain
---@field CommandList vireo.CommandList
---@field CommandAllocator vireo.CommandAllocator
---@field SubmitQueue vireo.SubmitQueue
---@field Vireo vireo.Vireo

---@type vireo
vireo = {}