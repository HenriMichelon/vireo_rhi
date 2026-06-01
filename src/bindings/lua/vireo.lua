---@meta
---@diagnostic disable: missing-return, lowercase-global

------------------------------------------------------------------------
-- Enums (exposed as namespaces with addVariable)
------------------------------------------------------------------------

---@class vireo.Backend Type of supported backends
---@field UNDEFINED integer Unknown or uninitialized backend.
---@field DIRECTX integer DirectX 12 backend.
---@field VULKAN integer Vulkan backend.

---@class vireo.Filter
---@field NEAREST integer Nearest-neighbor filtering (no interpolation).
---@field LINEAR integer Bilinear filtering.

---@class vireo.AddressMode
---@field REPEAT integer Texture coordinates wrap around (tile).
---@field MIRRORED_REPEAT integer Texture coordinates mirror on each wrap.
---@field CLAMP_TO_EDGE integer Texture coordinates clamp to [0,1]; edge texels are repeated.
---@field CLAMP_TO_BORDER integer Texture coordinates outside [0,1] sample the border color.

---@class vireo.FilterMode
---@field NEAREST integer Nearest mip-level selection.
---@field LINEAR integer Linear interpolation between mip levels.

---@class vireo.ImageFormat
---@field R8_UNORM integer 8-bit single-channel, normalized unsigned.
---@field R8_SNORM integer 8-bit single-channel, normalized signed.
---@field R8_UINT integer 8-bit single-channel, unsigned integer.
---@field R8_SINT integer 8-bit single-channel, signed integer.
---@field R8G8_UNORM integer 8-bit two-channel, normalized unsigned.
---@field R8G8_SNORM integer 8-bit two-channel, normalized signed.
---@field R8G8_UINT integer 8-bit two-channel, unsigned integer.
---@field R8G8_SINT integer 8-bit two-channel, signed integer.
---@field R8G8B8A8_UNORM integer 8-bit RGBA, normalized unsigned.
---@field R8G8B8A8_SNORM integer 8-bit RGBA, normalized signed.
---@field R8G8B8A8_UINT integer 8-bit RGBA, unsigned integer.
---@field R8G8B8A8_SINT integer 8-bit RGBA, signed integer.
---@field R8G8B8A8_SRGB integer 8-bit RGBA, sRGB color space.
---@field B8G8R8A8_UNORM integer 8-bit BGRA, normalized unsigned (common swapchain format).
---@field B8G8R8A8_SRGB integer 8-bit BGRA, sRGB color space.
---@field B8G8R8X8_UNORM integer 8-bit BGRx, normalized unsigned (X channel unused).
---@field B8G8R8X8_SRGB integer 8-bit BGRx, sRGB color space (X channel unused).
---@field A2B10G10R10_UNORM integer 10-bit RGB + 2-bit alpha, normalized unsigned.
---@field A2B10G10R10_UINT integer 10-bit RGB + 2-bit alpha, unsigned integer.
---@field R16_UNORM integer 16-bit single-channel, normalized unsigned.
---@field R16_SNORM integer 16-bit single-channel, normalized signed.
---@field R16_UINT integer 16-bit single-channel, unsigned integer.
---@field R16_SINT integer 16-bit single-channel, signed integer.
---@field R16_SFLOAT integer 16-bit single-channel, float.
---@field R16G16_UNORM integer 16-bit two-channel, normalized unsigned.
---@field R16G16_SNORM integer 16-bit two-channel, normalized signed.
---@field R16G16_UINT integer 16-bit two-channel, unsigned integer.
---@field R16G16_SINT integer 16-bit two-channel, signed integer.
---@field R16G16_SFLOAT integer 16-bit two-channel, float.
---@field R16G16B16A16_UNORM integer 16-bit RGBA, normalized unsigned.
---@field R16G16B16A16_SNORM integer 16-bit RGBA, normalized signed.
---@field R16G16B16A16_UINT integer 16-bit RGBA, unsigned integer.
---@field R16G16B16A16_SINT integer 16-bit RGBA, signed integer.
---@field R16G16B16A16_SFLOAT integer 16-bit RGBA, float.
---@field R32_UINT integer 32-bit single-channel, unsigned integer.
---@field R32_SINT integer 32-bit single-channel, signed integer.
---@field R32_SFLOAT integer 32-bit single-channel, float.
---@field R32G32_UINT integer 32-bit two-channel, unsigned integer.
---@field R32G32_SINT integer 32-bit two-channel, signed integer.
---@field R32G32_SFLOAT integer 32-bit two-channel, float.
---@field R32G32B32_UINT integer 32-bit three-channel, unsigned integer.
---@field R32G32B32_SINT integer 32-bit three-channel, signed integer.
---@field R32G32B32_SFLOAT integer 32-bit three-channel, float.
---@field R32G32B32A32_UINT integer 32-bit RGBA, unsigned integer.
---@field R32G32B32A32_SINT integer 32-bit RGBA, signed integer.
---@field R32G32B32A32_SFLOAT integer 32-bit RGBA, float.
---@field D16_UNORM integer 16-bit depth, normalized unsigned.
---@field D24_UNORM_S8_UINT integer 24-bit depth + 8-bit stencil.
---@field D32_SFLOAT integer 32-bit depth, float.
---@field D32_SFLOAT_S8_UINT integer 32-bit depth (float) + 8-bit stencil.
---@field BC1_UNORM integer BC1 block-compressed, normalized unsigned (RGB + 1-bit alpha).
---@field BC1_UNORM_SRGB integer BC1 block-compressed, sRGB.
---@field BC2_UNORM integer BC2 block-compressed, normalized unsigned (RGBA, 4-bit alpha).
---@field BC2_UNORM_SRGB integer BC2 block-compressed, sRGB.
---@field BC3_UNORM integer BC3 block-compressed, normalized unsigned (RGBA, interpolated alpha).
---@field BC3_UNORM_SRGB integer BC3 block-compressed, sRGB.
---@field BC4_UNORM integer BC4 block-compressed, single-channel normalized unsigned.
---@field BC4_SNORM integer BC4 block-compressed, single-channel normalized signed.
---@field BC5_UNORM integer BC5 block-compressed, two-channel normalized unsigned.
---@field BC5_SNORM integer BC5 block-compressed, two-channel normalized signed.
---@field BC6H_UFLOAT integer BC6H block-compressed, HDR unsigned float.
---@field BC6H_SFLOAT integer BC6H block-compressed, HDR signed float.
---@field BC7_UNORM integer BC7 block-compressed, high-quality RGBA normalized unsigned.
---@field BC7_UNORM_SRGB integer BC7 block-compressed, high-quality RGBA sRGB.

---@class vireo.BufferType
---@field VERTEX integer Vertex buffer, read on the GPU during draw calls.
---@field INDEX integer Index buffer, read on the GPU during indexed draw calls.
---@field INDIRECT integer Indirect draw/dispatch argument buffer.
---@field UNIFORM integer Uniform (constant) buffer, CPU-writable, shader-readable.
---@field STORAGE integer Storage buffer, shader-readable.
---@field DEVICE_STORAGE integer Device-local storage buffer, GPU-only.
---@field READWRITE_STORAGE integer Read/write storage buffer (UAV/SSBO).
---@field BUFFER_UPLOAD integer CPU-side staging buffer for uploading data to a VERTEX/INDEX/UNIFORM/STORAGE buffer.
---@field BUFFER_DOWNLOAD integer CPU-side readback buffer for downloading data from a GPU buffer.
---@field IMAGE_UPLOAD integer CPU-side staging buffer for uploading image data.
---@field IMAGE_DOWNLOAD integer CPU-side readback buffer for downloading image data.

---@class vireo.IndexType
---@field UINT16 integer 16-bit unsigned integer indices.
---@field UINT32 integer 32-bit unsigned integer indices.

---@class vireo.DescriptorType
---@field UNIFORM integer Uniform (constant) buffer binding.
---@field UNIFORM_DYNAMIC integer Dynamic uniform buffer binding (offset supplied at bind time).
---@field STORAGE integer Read-only storage buffer binding.
---@field DEVICE_STORAGE integer Device-local storage buffer binding.
---@field READWRITE_STORAGE integer Read/write storage buffer binding (UAV/SSBO).
---@field SAMPLED_IMAGE integer Sampled image (texture) binding.
---@field SAMPLER integer Sampler binding.
---@field READWRITE_IMAGE integer Read/write image binding (storage image / UAV).

---@class vireo.CommandType
---@field GRAPHIC integer Graphics command queue / allocator.
---@field TRANSFER integer Transfer (copy) command queue / allocator.
---@field COMPUTE integer Compute command queue / allocator.

---@class vireo.AttributeFormat
---@field R32_FLOAT integer Single 32-bit float (e.g. a scalar weight).
---@field R32G32_FLOAT integer Two 32-bit floats (e.g. UV coordinates).
---@field R32G32B32_FLOAT integer Three 32-bit floats (e.g. positions or normals).
---@field R32G32B32A32_FLOAT integer Four 32-bit floats (e.g. tangents or colors).
---@field R32_SINT integer Single 32-bit signed integer.
---@field R32G32_SINT integer Two 32-bit signed integers.
---@field R32G32B32_SINT integer Three 32-bit signed integers.
---@field R32G32B32A32_SINT integer Four 32-bit signed integers.
---@field R32_UINT integer Single 32-bit unsigned integer.
---@field R32G32_UINT integer Two 32-bit unsigned integers.
---@field R32G32B32_UINT integer Three 32-bit unsigned integers.
---@field R32G32B32A32_UINT integer Four 32-bit unsigned integers.

---@class vireo.RenderTargetType
---@field COLOR integer Color render target.
---@field DEPTH integer Depth-only render target.
---@field DEPTH_STENCIL integer Combined depth/stencil render target.

---@class vireo.CullMode
---@field NONE integer No face culling.
---@field FRONT integer Cull front-facing polygons.
---@field BACK integer Cull back-facing polygons (default).

---@class vireo.PrimitiveTopology
---@field POINT_LIST integer Each vertex is a separate point.
---@field LINE_LIST integer Every two vertices form an independent line segment.
---@field LINE_STRIP integer Vertices form a connected polyline.
---@field TRIANGLE_LIST integer Every three vertices form an independent triangle (default).
---@field TRIANGLE_STRIP integer Vertices form a strip of connected triangles.

---@class vireo.PolygonMode
---@field FILL integer Polygons are filled (default).
---@field WIREFRAME integer Polygons are drawn as wireframe outlines.

---@class vireo.CompareOp
---@field NEVER integer The test never passes.
---@field LESS integer Passes if source < destination.
---@field EQUAL integer Passes if source == destination.
---@field LESS_OR_EQUAL integer Passes if source <= destination.
---@field GREATER integer Passes if source > destination.
---@field NOT_EQUAL integer Passes if source != destination.
---@field GREATER_OR_EQUAL integer Passes if source >= destination.
---@field ALWAYS integer The test always passes.

---@class vireo.StencilOp
---@field KEEP integer Keep the current stencil value.
---@field ZERO integer Set the stencil value to 0.
---@field REPLACE integer Replace with the reference value.
---@field INCREMENT_AND_CLAMP integer Increment, clamped to the maximum representable value.
---@field DECREMENT_AND_CLAMP integer Decrement, clamped to 0.
---@field INVERT integer Bitwise-invert the stencil value.
---@field INCREMENT_AND_WRAP integer Increment, wrapping to 0 on overflow.
---@field DECREMENT_AND_WRAP integer Decrement, wrapping to max on underflow.

---@class vireo.BlendFactor
---@field ZERO integer Factor is (0, 0, 0, 0).
---@field ONE integer Factor is (1, 1, 1, 1).
---@field SRC_COLOR integer Factor is the source color (Rs, Gs, Bs, As).
---@field ONE_MINUS_SRC_COLOR integer Factor is (1-Rs, 1-Gs, 1-Bs, 1-As).
---@field DST_COLOR integer Factor is the destination color (Rd, Gd, Bd, Ad).
---@field ONE_MINUS_DST_COLOR integer Factor is (1-Rd, 1-Gd, 1-Bd, 1-Ad).
---@field SRC_ALPHA integer Factor is the source alpha (As, As, As, As).
---@field ONE_MINUS_SRC_ALPHA integer Factor is (1-As, 1-As, 1-As, 1-As). Common for alpha blending.
---@field DST_ALPHA integer Factor is the destination alpha (Ad, Ad, Ad, Ad).
---@field ONE_MINUS_DST_ALPHA integer Factor is (1-Ad, 1-Ad, 1-Ad, 1-Ad).
---@field CONSTANT_COLOR integer Factor is the blend constant color.
---@field ONE_MINUS_CONSTANT_COLOR integer Factor is one minus the blend constant color.
---@field CONSTANT_ALPHA integer Factor is the blend constant alpha.
---@field ONE_MINUS_CONSTANT_ALPHA integer Factor is one minus the blend constant alpha.
---@field SRC_ALPHA_SATURATE integer Factor is (min(As,1-Ad), min(As,1-Ad), min(As,1-Ad), 1).
---@field SRC1_COLOR integer Factor is the second source color (dual-source blending).
---@field ONE_MINUS_SRC1_COLOR integer Factor is one minus the second source color.
---@field SRC1_ALPHA integer Factor is the second source alpha.
---@field ONE_MINUS_SRC1_ALPHA integer Factor is one minus the second source alpha.

---@class vireo.BlendOp
---@field ADD integer Result = Src * SrcFactor + Dst * DstFactor.
---@field SUBTRACT integer Result = Src * SrcFactor - Dst * DstFactor.
---@field REVERSE_SUBTRACT integer Result = Dst * DstFactor - Src * SrcFactor.
---@field MIN integer Result = min(Src, Dst).
---@field MAX integer Result = max(Src, Dst).

---@class vireo.LogicOp
---@field CLEAR integer Result = 0.
---@field SET integer Result = 1.
---@field COPY integer Result = Src.
---@field COPY_INVERTED integer Result = ~Src.
---@field NOOP integer Result = Dst.
---@field INVERT integer Result = ~Dst.
---@field AND integer Result = Src & Dst.
---@field NAND integer Result = ~(Src & Dst).
---@field OR integer Result = Src | Dst.
---@field NOR integer Result = ~(Src | Dst).
---@field XOR integer Result = Src ^ Dst.
---@field EQUIV integer Result = ~(Src ^ Dst).
---@field AND_REVERSE integer Result = Src & ~Dst.
---@field AND_INVERTED integer Result = ~Src & Dst.
---@field OR_REVERSE integer Result = Src | ~Dst.
---@field OR_INVERTED integer Result = ~Src | Dst.

---@class vireo.ColorWriteMask
---@field RED integer Enable writes to the red channel.
---@field GREEN integer Enable writes to the green channel.
---@field BLUE integer Enable writes to the blue channel.
---@field ALPHA integer Enable writes to the alpha channel.
---@field ALL integer Enable writes to all four channels (R+G+B+A).

---@class vireo.ShaderStage
---@field ALL integer All shader stages.
---@field VERTEX integer Vertex shader stage.
---@field FRAGMENT integer Fragment (pixel) shader stage.
---@field HULL integer Hull (tessellation control) shader stage.
---@field DOMAIN integer Domain (tessellation evaluation) shader stage.
---@field GEOMETRY integer Geometry shader stage.
---@field COMPUTE integer Compute shader stage.

---@class vireo.WaitStage
---@field NONE integer No stage / no wait.
---@field PIPELINE_TOP integer Top of the pipeline (before any stage begins).
---@field VERTEX_INPUT integer Vertex input stage (buffer reads).
---@field VERTEX_SHADER integer Vertex shader stage.
---@field DEPTH_STENCIL_TEST_BEFORE_FRAGMENT_SHADER integer Early depth/stencil test (before fragment shading).
---@field FRAGMENT_SHADER integer Fragment shader stage.
---@field DEPTH_STENCIL_TEST_AFTER_FRAGMENT_SHADER integer Late depth/stencil test (after fragment shading).
---@field COLOR_OUTPUT integer Color attachment output stage.
---@field COMPUTE_SHADER integer Compute shader stage.
---@field TRANSFER integer Transfer (copy/blit) stage.
---@field PIPELINE_BOTTOM integer Bottom of the pipeline (all stages complete).
---@field ALL_GRAPHICS integer All graphics pipeline stages combined.
---@field ALL_COMMANDS integer All commands in the queue.
---@field COPY integer Copy operations stage.
---@field RESOLV integer Multisample resolve stage.
---@field BLIT integer Blit operations stage.
---@field CLEAR integer Clear operations stage.

---@class vireo.ResourceState
---@field UNDEFINED integer Unknown / initial state. A barrier from UNDEFINED discards previous content.
---@field RENDER_TARGET_COLOR integer Image used as a color render target attachment.
---@field RENDER_TARGET_DEPTH integer Image used as a read/write depth attachment.
---@field RENDER_TARGET_DEPTH_READ integer Image used as a read-only depth attachment.
---@field RENDER_TARGET_DEPTH_STENCIL integer Image used as a read/write depth+stencil attachment.
---@field RENDER_TARGET_DEPTH_STENCIL_READ integer Image used as a read-only depth+stencil attachment.
---@field PRESENT integer Image ready to be presented by the swap chain.
---@field COPY_SRC integer Resource used as the source of a copy operation.
---@field COPY_DST integer Resource used as the destination of a copy operation.
---@field SHADER_READ integer Image/buffer read by a shader (SRV / sampled).
---@field COMPUTE_READ integer Resource read by a compute shader.
---@field COMPUTE_WRITE integer Resource written by a compute shader (UAV).
---@field INDIRECT_DRAW integer Buffer used as an indirect draw/dispatch argument source.
---@field VERTEX_INPUT integer Buffer used as a vertex/index input source.
---@field UNIFORM integer Buffer used as a uniform (constant) buffer.

---@class vireo.MSAA
---@field NONE integer No multisampling (1 sample).
---@field X2 integer 2× multisampling.
---@field X4 integer 4× multisampling.
---@field X8 integer 8× multisampling.
---@field X16 integer 16× multisampling.
---@field X32 integer 32× multisampling.
---@field X64 integer 64× multisampling.

---@class vireo.PresentMode
---@field IMMEDIATE integer Frames are presented immediately without waiting for VSync (may tear).
---@field VSYNC integer Frames are synchronized to the display refresh rate (no tearing).

---@class vireo.PipelineType
---@field GRAPHIC integer Graphics pipeline (vertex + fragment + optional hull/domain/geometry).
---@field COMPUTE integer Compute pipeline.

---@class vireo.SemaphoreType
---@field BINARY integer Binary semaphore: signaled / unsignaled.
---@field TIMELINE integer Timeline semaphore: monotonically increasing 64-bit counter.

------------------------------------------------------------------------
-- Aliases / typedefs
------------------------------------------------------------------------

---@alias vireo.DescriptorIndex integer  Binding index within a DescriptorLayout / DescriptorSet.

------------------------------------------------------------------------
-- Structs
------------------------------------------------------------------------

---@class vireo.ColorBlendDesc Color blend state for a single render-target attachment.
---@field blend_enable boolean True to enable blending on this attachment.
---@field src_color_blend_factor vireo.BlendFactor Blend factor applied to the source color.
---@field dst_color_blend_factor vireo.BlendFactor Blend factor applied to the destination color.
---@field color_blend_op vireo.BlendOp Blend operation for the color channels.
---@field src_alpha_blend_factor vireo.BlendFactor Blend factor applied to the source alpha.
---@field dst_alpha_blend_factor vireo.BlendFactor Blend factor applied to the destination alpha.
---@field alpha_blend_op vireo.BlendOp Blend operation for the alpha channel.
---@field color_write_mask vireo.ColorWriteMask Bitmask of color channels to write (see vireo.ColorWriteMask).

---@class vireo.StencilOpState Per-face stencil operation state.
---@field fail_op vireo.StencilOp Operation when the stencil test fails.
---@field pass_op vireo.StencilOp Operation when both the stencil and depth tests pass.
---@field depth_fail_op vireo.StencilOp Operation when the stencil test passes but depth fails.
---@field compare_op vireo.CompareOp Stencil comparison function.
---@field compare_mask integer Bitmask applied to both reference and stored values before comparison.
---@field write_mask integer Bitmask controlling which stencil bits are written.

---@class vireo.PhysicalDeviceDesc GPU device description returned by PhysicalDevice.get_description().
---@field name string Human-readable GPU name. (read-only)
---@field dedicated_video_memory integer Dedicated video memory in bytes. (read-only)
---@field dedicated_system_memory integer Dedicated system memory in bytes. (read-only)
---@field shared_system_memory integer Shared system memory in bytes. (read-only)

---@class vireo.Extent 2D integer dimensions.
---@field width integer Width in pixels/texels.
---@field height integer Height in pixels/texels.

---@class vireo.Rect 2D integer rectangle (position + size). Constructor overloads: Rect(width, height) sets x=y=0; Rect(x, y, width, height).
---@field x integer Left edge (pixels). Default 0.
---@field y integer Top edge (pixels). Default 0.
---@field width integer Rectangle width in pixels.
---@field height integer Rectangle height in pixels.

---@class vireo.Viewport Floating-point viewport with depth range. Constructor overloads: Viewport(width, height); Viewport(x, y, width, height).
---@field x number Left edge of the viewport (pixels).
---@field y number Top edge of the viewport (pixels).
---@field width number Viewport width in pixels.
---@field height number Viewport height in pixels.
---@field min_depth number Minimum depth value (default 0.0).
---@field max_depth number Maximum depth value (default 1.0).

---@class vireo.PushConstantsDesc Describes a push-constant range visible to one or more shader stages.
---@field stage vireo.ShaderStage Shader stage(s) that can access this push-constant range.
---@field size integer Size of the push-constant block in bytes.
---@field offset integer Offset into the push-constant block in bytes.

---@class vireo.VertexAttributeDesc Describes a single vertex attribute in a VertexInputLayout.
---@field binding string Input binding slot name.
---@field format vireo.AttributeFormat Data type and component count of the attribute.
---@field offset integer Byte offset of this attribute within the vertex structure.

---@class vireo.DepthClearValue Depth and stencil clear values used when clearing a depth/stencil attachment.
---@field depth number Clear value for the depth aspect (default 1.0).
---@field stencil integer Clear value for the stencil aspect (default 0).

---@class vireo.ClearValue Union holding either a color clear value (r/g/b/a) or a depth/stencil clear value (depth/stencil). Set r/g/b/a for color attachments; set depth/stencil for depth attachments.
---@field r number Red component of the color clear value (0.0–1.0).
---@field g number Green component of the color clear value (0.0–1.0).
---@field b number Blue component of the color clear value (0.0–1.0).
---@field a number Alpha component of the color clear value (0.0–1.0, default 1.0).
---@field depth number Depth clear value (default 1.0). Overrides r/g/b/a union when used.
---@field stencil integer Stencil clear value (default 0). Overrides r/g/b/a union when used.

---@class vireo.VideoMemoryAllocationDesc Describes a single GPU memory allocation.
---@field usage vireo.VideoMemoryAllocationUsage Whether this allocation backs a buffer or an image. (read-only)
---@field name string Debug name of the allocated resource. (read-only)
---@field size integer Allocation size in bytes. (read-only)
---@field ref any Opaque reference to the underlying resource object. (read-only)

---@class vireo.RenderTargetDesc Describes a single attachment slot in a RenderingConfiguration.
---@field swap_chain vireo.SwapChain|nil Swap chain providing this attachment (nil if using render_target).
---@field render_target vireo.RenderTarget|nil Render target for this attachment (nil if using swap_chain).
---@field multisampled_render_target vireo.RenderTarget|nil Optional MSAA resolve target.
---@field clear boolean True to clear this attachment at the start of the render pass.
---@field clear_value vireo.ClearValue Clear value used when clear is true.
---@field discard_after_render boolean True to discard the attachment content after the render pass.

---@class vireo.RenderingConfiguration Full render-pass configuration passed to CommandList.begin_rendering().
---@field color_render_targets vireo.RenderTargetDesc[] List of color attachment descriptors.
---@field depth_stencil_render_target vireo.RenderTarget|nil Depth/stencil attachment (nil to disable).
---@field multisampled_depth_stencil_render_target vireo.RenderTarget|nil Optional MSAA depth/stencil resolve target.
---@field use_depth_attachment boolean True to use the depth attachment during rendering.
---@field use_stencil_attachment boolean True to use the stencil attachment during rendering.
---@field clear_depth_stencil boolean True to clear the depth/stencil attachment at the start of the render pass.
---@field depth_stencil_clear_value vireo.ClearValue Clear value for depth/stencil (use .depth and .stencil fields).
---@field discard_depth_stencil_after_render boolean True to discard the depth/stencil content after the render pass.

---@class vireo.DrawIndirectCommand GPU-side structure for an indirect (non-indexed) draw call. Mirror of VkDrawIndirectCommand / D3D12_DRAW_ARGUMENTS.
---@field vertex_count integer Number of vertices to draw.
---@field instance_count integer Number of instances to draw.
---@field first_vertex integer Index of the first vertex.
---@field first_instance integer Instance ID of the first instance.

---@class vireo.DrawIndexedIndirectCommand GPU-side structure for an indirect indexed draw call. Mirror of VkDrawIndexedIndirectCommand / D3D12_DRAW_INDEXED_ARGUMENTS.
---@field index_count integer Number of indices to draw.
---@field instance_count integer Number of instances to draw.
---@field first_index integer Base index within the index buffer.
---@field vertex_offset integer Added to each index value before reading a vertex.
---@field first_instance integer Instance ID of the first instance.

---@class vireo.BufferCopyRegion Describes a source/destination region pair for a buffer-to-buffer copy.
---@field src_offset integer Byte offset into the source buffer.
---@field dst_offset integer Byte offset into the destination buffer.
---@field size integer Number of bytes to copy.

---@class vireo.GraphicPipelineConfiguration Full description of a graphics pipeline passed to Vireo.create_graphic_pipeline().
---@field resources vireo.PipelineResources Pipeline layout (descriptor layouts + push constants).
---@field color_render_formats vireo.ImageFormat[] Formats of the color render target attachments.
---@field color_blend_desc vireo.ColorBlendDesc[] Per-attachment color blend states.
---@field vertex_input_layout vireo.VertexInputLayout|nil Vertex attribute layout (nil for mesh/compute-only pipelines).
---@field vertex_shader vireo.ShaderModule|nil Vertex shader module.
---@field fragment_shader vireo.ShaderModule|nil Fragment shader module.
---@field hull_shader vireo.ShaderModule|nil Hull (tessellation control) shader module.
---@field domain_shader vireo.ShaderModule|nil Domain (tessellation evaluation) shader module.
---@field geometry_shader vireo.ShaderModule|nil Geometry shader module.
---@field primitive_topology vireo.PrimitiveTopology Input primitive assembly topology.
---@field msaa vireo.MSAA Multisampling level (NONE disables MSAA).
---@field cull_mode vireo.CullMode Face culling mode.
---@field polygon_mode vireo.PolygonMode Fill mode (FILL or WIREFRAME).
---@field front_face_counter_clockwise boolean True if counter-clockwise winding defines front faces.
---@field depth_stencil_image_format vireo.ImageFormat Format of the depth/stencil attachment (UNDEFINED if none).
---@field depth_test_enable boolean True to enable the depth test.
---@field depth_write_enable boolean True to write depth values when the depth test passes.
---@field depth_compare_op vireo.CompareOp Depth comparison function.
---@field depth_bias_enable boolean True to enable depth biasing (useful for shadow maps).
---@field depth_bias_constant_factor number Constant depth bias added to each fragment's depth.
---@field depth_bias_clamp number Maximum (or minimum) depth bias applied to a fragment.
---@field depth_bias_slope_factor number Depth bias scaled by the fragment's slope.
---@field stencil_test_enable boolean True to enable the stencil test.
---@field front_stencil_op_state vireo.StencilOpState Stencil operations for front-facing polygons.
---@field back_stencil_op_state vireo.StencilOpState Stencil operations for back-facing polygons.
---@field logic_op_enable boolean True to enable logical operations on color attachments.
---@field logic_op vireo.LogicOp Logical operation applied to color attachments when logic_op_enable is true.
---@field alpha_to_coverage_enable boolean True to derive a coverage mask from the fragment alpha value (MSAA).

------------------------------------------------------------------------
-- Classes / objects
------------------------------------------------------------------------

---@class vireo.Fence CPU/GPU synchronization primitive. Created by Vireo.create_fence().
---@field wait fun(self: vireo.Fence): nil Blocks the CPU until the fence is signaled.
---@field reset fun(self: vireo.Fence): nil Resets the fence to the unsignaled state.

---@class vireo.Semaphore GPU/GPU or GPU/CPU synchronization semaphore. Created by Vireo.create_semaphore().
---@field type vireo.SemaphoreType The semaphore type (BINARY or TIMELINE). (read-only)
---@field value integer Current counter value; assignable to set it (timeline semaphores only).
---@field increment_value fun(self: vireo.Semaphore): nil Increments the counter by one (timeline semaphores only).
---@field decrement_value fun(self: vireo.Semaphore): nil Decrements the counter by one (timeline semaphores only).

---@class vireo.Instance Underlying API instance (VkInstance / IDXGIFactory). Retrieved via Vireo.instance. Opaque; used for platform-specific window surface creation.

---@class vireo.PhysicalDevice Represents the physical GPU. Retrieved via Vireo.physical_device.
---@field description vireo.PhysicalDeviceDesc Name and memory info for the GPU. (read-only)

---@class vireo.Device Logical device. Retrieved via Vireo.device.
---@field have_dedicated_transfer_queue boolean True if the GPU exposes a dedicated transfer queue separate from the graphics queue. (read-only)

---@class vireo.Buffer A GPU buffer. Created by Vireo.create_buffer().
---@field size integer Total size of the buffer in bytes. (read-only)
---@field type vireo.BufferType Usage type of this buffer. (read-only)
---@field instance_size integer Size of a single element/instance in bytes (before alignment). (read-only)
---@field instance_size_aligned integer Aligned size of a single element in bytes. (read-only)
---@field instance_count integer Number of elements/instances in the buffer. (read-only)
---@field mapped_address lightuserdata|any CPU-side mapped pointer (only valid after map(), for upload/download buffers). (read-only)
---@field map fun(self: vireo.Buffer): nil Maps the buffer for CPU access. Call unmap() when done.
---@field unmap fun(self: vireo.Buffer): nil Unmaps the buffer after CPU writes.
---@field write fun(self: vireo.Buffer, data: lightuserdata|any, size: integer|nil, offset: integer|nil): nil Writes data into the mapped buffer. size defaults to the whole buffer; offset defaults to 0.
---@field get_memory_allocations fun(): vireo.VideoMemoryAllocationDesc[] Returns all current GPU memory allocations for Buffer objects. @static

---@class vireo.Sampler Texture sampler state. Created by Vireo.create_sampler(). Opaque.

---@class vireo.Image A GPU image (texture). Created by Vireo.create_image() or Vireo.create_read_write_image().
---@field format vireo.ImageFormat Pixel format of the image. (read-only)
---@field width integer Width in pixels of mip level 0. (read-only)
---@field height integer Height in pixels of mip level 0. (read-only)
---@field mip_levels integer Total number of mip levels. (read-only)
---@field array_size integer Number of array layers. (read-only)
---@field is_read_write boolean True if the image was created as a read/write (UAV/storage image). (read-only)
---@field get_row_pitch fun(self: vireo.Image, mipLevel: integer|nil): integer Row pitch in bytes for the given mip level (default 0).
---@field get_row_length fun(self: vireo.Image, mipLevel: integer|nil): integer Row length in pixels for the given mip level (default 0).
---@field get_image_size fun(self: vireo.Image, mipLevel: integer|nil): integer Size in bytes of one layer at the given mip level (default 0).
---@field get_aligned_image_size fun(self: vireo.Image, mipLevel: integer|nil): integer Aligned size in bytes of one layer at the given mip level.
---@field get_aligned_row_pitch fun(self: vireo.Image, mipLevel: integer|nil): integer Aligned row pitch in bytes for the given mip level.
---@field get_aligned_row_length fun(self: vireo.Image, mipLevel: integer|nil): integer Aligned row length in pixels for the given mip level.
---@field get_pixel_size fun(format: vireo.ImageFormat): integer Returns the pixel size in bytes for the given format. @static
---@field get_memory_allocations fun(): vireo.VideoMemoryAllocationDesc[] Returns all current GPU memory allocations for Image objects. @static

---@class vireo.RenderTarget A render-target image wrapper. Created by Vireo.create_render_target() or Vireo.create_render_target_from_swap_chain().
---@field image vireo.Image The underlying Image. (read-only)
---@field type vireo.RenderTargetType The render-target type (COLOR, DEPTH, or DEPTH_STENCIL). (read-only)

---@class vireo.DescriptorLayout Describes the bindings of a descriptor set. Created by Vireo.create_descriptor_layout().
---@field capacity integer Number of bindings in this layout. (read-only)
---@field is_dynamic_uniform boolean True if the layout was created for dynamic uniform buffers. (read-only)
---@field is_samplers boolean True if the layout was created for samplers. (read-only)
---@field add fun(self: vireo.DescriptorLayout, index: vireo.DescriptorIndex, type: vireo.DescriptorType, count: integer|nil): vireo.DescriptorLayout Adds a binding at index with the given type (and optional array count). Returns self for chaining.
---@field build fun(self: vireo.DescriptorLayout): nil Finalizes the layout. Must be called before creating descriptor sets.

---@class vireo.DescriptorSet A set of descriptors bound to a pipeline. Created by Vireo.create_descriptor_set().
---@field update_buffer fun(self: vireo.DescriptorSet, index: vireo.DescriptorIndex, buffer: vireo.Buffer): nil Binds a uniform or storage buffer at the given binding index.
---@field update_dynamic_buffer fun(self: vireo.DescriptorSet, buffer: vireo.Buffer): nil Binds a dynamic uniform buffer at binding index 0. The dynamic offset is supplied at CommandList.bind_descriptor_dynamic().
---@field update_image fun(self: vireo.DescriptorSet, index: vireo.DescriptorIndex, image: vireo.Image): nil Binds a sampled image or storage image at the given binding index.
---@field update_sampler fun(self: vireo.DescriptorSet, index: vireo.DescriptorIndex, sampler: vireo.Sampler): nil Binds a sampler at the given binding index.
---@field update_image_array fun(self: vireo.DescriptorSet, index: vireo.DescriptorIndex, images: vireo.Image[]): nil Binds an array of images at the given binding index.
---@field update_buffer_array fun(self: vireo.DescriptorSet, index: vireo.DescriptorIndex, buffers: vireo.Buffer[]): nil Binds an array of buffers at the given binding index.
---@field update_sampler_array fun(self: vireo.DescriptorSet, index: vireo.DescriptorIndex, samplers: vireo.Sampler[]): nil Binds an array of samplers at the given binding index.
---@field layout vireo.DescriptorLayout The DescriptorLayout this set was created from. (read-only)

---@class vireo.VertexInputLayout Describes the vertex input attributes of a pipeline. Created by Vireo.create_vertex_layout(). Opaque.

---@class vireo.ShaderModule A compiled shader. Created by Vireo.create_shader_module_from_file() or Vireo.create_shader_module_from_data(). Opaque.

---@class vireo.PipelineResources Describes the pipeline layout (descriptor layouts + push constants). Created by Vireo.create_pipeline_resources(). Opaque.

---@class vireo.Pipeline Base class for compiled pipelines. Use vireo.ComputePipeline or vireo.GraphicPipeline.
---@field resources vireo.PipelineResources The pipeline layout. (read-only)
---@field type vireo.PipelineType The pipeline type (GRAPHIC or COMPUTE). (read-only)

---@class vireo.ComputePipeline : vireo.Pipeline Compiled compute pipeline. Created by Vireo.create_compute_pipeline().

---@class vireo.GraphicPipeline : vireo.Pipeline Compiled graphics pipeline. Created by Vireo.create_graphic_pipeline().

---@class vireo.SwapChain Presentation swap chain. Created by Vireo.create_swap_chain().
---@field extent vireo.Extent Current swap chain size in pixels. (read-only)
---@field aspect_ratio number Width / height. (read-only)
---@field current_frame_index integer Index of the current back buffer (0 to frames_in_flight-1). (read-only)
---@field frames_in_flight integer Number of buffered frames. (read-only)
---@field format vireo.ImageFormat Back buffer pixel format. (read-only)
---@field next_frame_index fun(self: vireo.SwapChain): nil Advances the internal frame index to the next frame.
---@field acquire fun(self: vireo.SwapChain, fence: vireo.Fence): boolean Acquires the next available back buffer. Returns false if the swap chain must be recreated (resize).
---@field present fun(self: vireo.SwapChain): nil Presents the current back buffer to the display.
---@field recreate fun(self: vireo.SwapChain): nil Recreates the swap chain (e.g. after a window resize).
---@field wait_idle fun(self: vireo.SwapChain): nil Waits until all pending present operations complete.

---@class vireo.CommandList Records GPU commands. Obtained from CommandAllocator.create_command_list().
---@field begin fun(self: vireo.CommandList): nil Begins recording. Must be called before any other recording command.
---@field end fun(self: vireo.CommandList): nil Ends recording. Must be called before submitting.
---@field upload_buffer fun(self: vireo.CommandList, destination: vireo.Buffer, data: lightuserdata|any): nil Uploads CPU data into a BUFFER_UPLOAD buffer.
---@field upload_image fun(self: vireo.CommandList, destination: vireo.Image, data: lightuserdata|any, firstMipLevel: integer): nil Uploads CPU data into an IMAGE_UPLOAD staging buffer starting at firstMipLevel.
---@field copy_buffer_to_image fun(self: vireo.CommandList, src: vireo.Buffer, dst: vireo.Image, mipLevel: integer, arrayLayer: integer, generateMips: boolean): nil Copies a staging buffer into a single image mip level and array layer.
---@field copy_buffer_to_image_levels fun(self: vireo.CommandList, src: vireo.Buffer, dst: vireo.Image, offsets: integer[], generateMips: boolean): nil Copies a staging buffer into multiple mip levels using explicit byte offsets per level.
---@field copy_buffer_to_buffer fun(self: vireo.CommandList, src: vireo.Buffer, dst: vireo.Buffer, size: integer, srcOffset: integer, dstOffset: integer): nil Copies size bytes from src (at srcOffset) into dst (at dstOffset).
---@field copy_buffer_regions fun(self: vireo.CommandList, src: vireo.Buffer, dst: vireo.Buffer, regions: vireo.BufferCopyRegion[]): nil Copies multiple non-contiguous regions between two buffers.
---@field copy_image_to_swap_chain fun(self: vireo.CommandList, src: vireo.Image, swapChain: vireo.SwapChain): nil Blits or resolves src into the swap chain's current back buffer.
---@field begin_rendering fun(self: vireo.CommandList, config: vireo.RenderingConfiguration): nil Begins a dynamic render pass. Pair with end_rendering().
---@field end_rendering fun(self: vireo.CommandList): nil Ends the current dynamic render pass.
---@field dispatch fun(self: vireo.CommandList, x: integer, y: integer, z: integer): nil Dispatches a compute shader with the given thread-group counts.
---@field bind_pipeline fun(self: vireo.CommandList, pipeline: vireo.Pipeline, descriptorsAlreadyBound: boolean|nil): nil Binds a graphics or compute pipeline. Pass descriptorsAlreadyBound=true to skip re-binding unchanged descriptor sets.
---@field bind_vertex_buffer fun(self: vireo.CommandList, buffer: vireo.Buffer, offset: integer|nil): nil Binds a vertex buffer at the default slot. offset is a byte offset (default 0).
---@field bind_vertex_buffers fun(self: vireo.CommandList, buffers: vireo.Buffer[], offsets: integer[]|nil): nil Binds multiple vertex buffers at once.
---@field bind_index_buffer fun(self: vireo.CommandList, buffer: vireo.Buffer, indexType: vireo.IndexType|nil, firstIndex: integer|nil): nil Binds an index buffer. Defaults to UINT32 and firstIndex=0.
---@field bind_descriptors fun(self: vireo.CommandList, pipelineType: vireo.PipelineType, resources: vireo.PipelineResources, descriptors: vireo.DescriptorSet[]): nil Binds descriptor sets before the pipeline is bound (supply pipelineType + resources).
---@field bind_descriptors fun(self: vireo.CommandList, descriptors: vireo.DescriptorSet[]): nil Binds descriptor sets after the pipeline is bound (pipeline type is inferred).
---@field bind_descriptor fun(self: vireo.CommandList, descriptor: vireo.DescriptorSet, set: integer): nil Binds a single descriptor set at the given set index (after pipeline bind).
---@field bind_descriptor_dynamic fun(self: vireo.CommandList, descriptor: vireo.DescriptorSet, set: integer, offset: integer): nil Binds a dynamic-uniform descriptor set with a byte offset into the buffer.
---@field draw fun(self: vireo.CommandList, vertexCountPerInstance: integer, instanceCount: integer|nil, firstVertex: integer|nil, firstInstance: integer|nil): nil Issues a non-indexed draw call.
---@field draw_indexed fun(self: vireo.CommandList, indexCountPerInstance: integer, instanceCount: integer|nil, firstIndex: integer|nil, firstVertex: integer|nil, firstInstance: integer|nil): nil Issues an indexed draw call.
---@field draw_indirect fun(self: vireo.CommandList, buffer: vireo.Buffer, offset: integer, drawCount: integer, stride: integer, firstCommandOffset: integer): nil Issues indirect (non-indexed) draw calls sourced from a GPU buffer.
---@field draw_indexed_indirect_count fun(self: vireo.CommandList, buffer: vireo.Buffer, offset: integer, countBuffer: vireo.Buffer, countOffset: integer, maxDrawCount: integer, stride: integer, firstCommandOffset: integer): nil Issues indirect indexed draw calls with the draw count stored in a GPU buffer.
---@field draw_indexed_indirect fun(self: vireo.CommandList, buffer: vireo.Buffer, offset: integer, maxDrawCount: integer, stride: integer, firstCommandOffset: integer): nil Issues indirect indexed draw calls with a CPU-side draw count.
---@field barrier_image fun(self: vireo.CommandList, image: vireo.Image, oldState: vireo.ResourceState, newState: vireo.ResourceState): nil Inserts a memory barrier transitioning an image from oldState to newState.
---@field barrier_render_target fun(self: vireo.CommandList, renderTarget: vireo.RenderTarget, oldState: vireo.ResourceState, newState: vireo.ResourceState): nil Inserts a memory barrier transitioning a render-target image between resource states.
---@field barrier_swap_chain fun(self: vireo.CommandList, swapChain: vireo.SwapChain, oldState: vireo.ResourceState, newState: vireo.ResourceState): nil Inserts a memory barrier for the swap chain's current back buffer.
---@field barrier_buffer fun(self: vireo.CommandList, buffer: vireo.Buffer, oldState: vireo.ResourceState, newState: vireo.ResourceState): nil Inserts a memory barrier for a buffer resource.
---@field set_viewports fun(self: vireo.CommandList, viewports: vireo.Viewport[]): nil Sets multiple viewports.
---@field set_scissors fun(self: vireo.CommandList, rects: vireo.Rect[]): nil Sets multiple scissor rectangles.
---@field set_viewport fun(self: vireo.CommandList, viewport: vireo.Viewport): nil Sets a single viewport.
---@field set_scissor fun(self: vireo.CommandList, rect: vireo.Rect): nil Sets a single scissor rectangle.
---@field set_stencil_reference fun(self: vireo.CommandList, reference: integer): nil Sets the stencil reference value used in stencil comparisons.
---@field push_constants fun(self: vireo.CommandList, resources: vireo.PipelineResources, desc: vireo.PushConstantsDesc, data: any): nil Uploads push-constant data for the bound pipeline.
---@field cleanup fun(self: vireo.CommandList): nil Releases internal temporary resources after the command list has been submitted.

---@class vireo.CommandAllocator Manages a pool of command lists for a specific queue type. Created by Vireo.create_command_allocator().
---@field reset fun(self: vireo.CommandAllocator): nil Resets the allocator and all its command lists. Call once per frame before re-recording.
---@field create_command_list fun(self: vireo.CommandAllocator, pipeline: vireo.Pipeline|nil): vireo.CommandList Creates a new command list. Pass a pipeline to pre-bind it at creation time (graphics allocators only).
---@field command_list_type vireo.CommandType The command type this allocator was created for. (read-only)

---@class vireo.SubmitQueue GPU command submission queue. Created by Vireo.create_submit_queue().
---@field submit fun(self: vireo.SubmitQueue, commandLists: vireo.CommandList[]): nil Submits command lists with no synchronization.
---@field submit_fence fun(self: vireo.SubmitQueue, fence: vireo.Fence, commandLists: vireo.CommandList[]): nil Submits command lists and signals fence when the GPU finishes.
---@field submit_fence_swap fun(self: vireo.SubmitQueue, fence: vireo.Fence, swapChain: vireo.SwapChain, commandLists: vireo.CommandList[]): nil Submits command lists synchronized with a swap-chain present operation and a CPU fence.
---@field submit_wait_fence_swap fun(self: vireo.SubmitQueue, waitSemaphore: vireo.Semaphore, waitStage: vireo.WaitStage, fence: vireo.Fence, swapChain: vireo.SwapChain, commandLists: vireo.CommandList[]): nil Submits with GPU wait on waitSemaphore, CPU fence signal, and swap-chain present.
---@field submit_wait_signal fun(self: vireo.SubmitQueue, waitSemaphore: vireo.Semaphore, waitStage: vireo.WaitStage, signalStage: vireo.WaitStage, signalSemaphore: vireo.Semaphore, commandLists: vireo.CommandList[]): nil Submits with GPU/GPU synchronization: waits on waitSemaphore and signals signalSemaphore.
---@field submit_wait fun(self: vireo.SubmitQueue, waitSemaphore: vireo.Semaphore, waitStage: vireo.WaitStage, commandLists: vireo.CommandList[]): nil Submits after waiting on a GPU semaphore at the given pipeline stage.
---@field submit_signal fun(self: vireo.SubmitQueue, signalStage: vireo.WaitStage, signalSemaphore: vireo.Semaphore, commandLists: vireo.CommandList[]): nil Submits and signals a GPU semaphore when the given pipeline stage completes.
---@field wait_idle fun(self: vireo.SubmitQueue): nil Blocks the CPU until all submitted commands have finished executing.

---@class vireo.Vireo Main entry point. Obtained from the host application; not constructed in Lua.
---@field backend vireo.Backend The active rendering backend. (read-only)
---@field shader_file_extension string File extension for compiled shaders on the active backend (e.g. ".spv" or ".cso"). (read-only)
---@field physical_device vireo.PhysicalDevice The PhysicalDevice representing the active GPU. (read-only)
---@field device vireo.Device The logical Device. (read-only)
---@field instance vireo.Instance The underlying API Instance. (read-only)
---@field wait_idle fun(self: vireo.Vireo): nil Blocks until the GPU has finished all pending work on all queues.
---@field create_swap_chain fun(self: vireo.Vireo, format: vireo.ImageFormat, presentQueue: vireo.SubmitQueue, windowHandle: any, presentMode: vireo.PresentMode|nil, framesInFlight: integer|nil): vireo.SwapChain Creates a swap chain for the given OS window handle.
---@field create_submit_queue fun(self: vireo.Vireo, commandType: vireo.CommandType, name: string|nil): vireo.SubmitQueue Creates a GPU command submission queue.
---@field create_fence fun(self: vireo.Vireo, createSignaled: boolean|nil, name: string|nil): vireo.Fence Creates a CPU/GPU synchronization fence (unsignaled by default).
---@field create_semaphore fun(self: vireo.Vireo, type: vireo.SemaphoreType, name: string|nil): vireo.Semaphore Creates a GPU synchronization semaphore.
---@field create_command_allocator fun(self: vireo.Vireo, type: vireo.CommandType): vireo.CommandAllocator Creates a command allocator for the given queue type.
---@field create_vertex_layout fun(self: vireo.Vireo, size: integer, attributes: vireo.VertexAttributeDesc[]): vireo.VertexInputLayout Creates a vertex input layout from a stride and a list of attribute descriptors.
---@field create_shader_module_from_file fun(self: vireo.Vireo, path: string): vireo.ShaderModule Loads a compiled shader from a file path (use shader_file_extension for the right extension).
---@field create_shader_module_from_data fun(self: vireo.Vireo, data: any, name: string): vireo.ShaderModule Creates a shader module from raw compiled byte data with an optional debug name.
---@field create_pipeline_resources fun(self: vireo.Vireo, layouts: vireo.DescriptorLayout[]|nil, pushConstant: vireo.PushConstantsDesc|nil, name: string|nil): vireo.PipelineResources Creates a pipeline layout from descriptor layouts and an optional push-constant range.
---@field create_compute_pipeline fun(self: vireo.Vireo, resources: vireo.PipelineResources, shader: vireo.ShaderModule, name: string|nil): vireo.ComputePipeline Creates a compiled compute pipeline.
---@field create_graphic_pipeline fun(self: vireo.Vireo, config: vireo.GraphicPipelineConfiguration, name: string|nil): vireo.GraphicPipeline Creates a compiled graphics pipeline from a full configuration.
---@field create_buffer fun(self: vireo.Vireo, type: vireo.BufferType, size: integer, count: integer|nil, name: string|nil): vireo.Buffer Allocates a GPU buffer. size is the per-element size in bytes; count is the number of elements (default 1).
---@field create_image fun(self: vireo.Vireo, format: vireo.ImageFormat, width: integer, height: integer, mipLevels: integer|nil, arraySize: integer|nil, name: string|nil): vireo.Image Allocates a GPU image (shader-read-only).
---@field create_read_write_image fun(self: vireo.Vireo, format: vireo.ImageFormat, width: integer, height: integer, mipLevels: integer|nil, arraySize: integer|nil, name: string|nil): vireo.Image Allocates a GPU read/write image (UAV / storage image).
---@field create_render_target fun(self: vireo.Vireo, format: vireo.ImageFormat, width: integer, height: integer, type: vireo.RenderTargetType|nil, clearValue: vireo.ClearValue|nil, arraySize: integer|nil, msaa: vireo.MSAA|nil, name: string|nil): vireo.RenderTarget Creates a render target of the given format and dimensions.
---@field create_render_target_from_swap_chain fun(self: vireo.Vireo, swapChain: vireo.SwapChain, clearValue: vireo.ClearValue|nil, msaa: vireo.MSAA|nil, name: string|nil): vireo.RenderTarget Creates a render target that matches the format and size of the given swap chain.
---@field create_descriptor_layout fun(self: vireo.Vireo, name: string|nil): vireo.DescriptorLayout Creates an empty descriptor layout for uniform/storage/image/sampler bindings.
---@field create_sampler_descriptor_layout fun(self: vireo.Vireo, name: string|nil): vireo.DescriptorLayout Creates an empty descriptor layout for sampler-only bindings.
---@field create_dynamic_uniform_descriptor_layout fun(self: vireo.Vireo, name: string|nil): vireo.DescriptorLayout Creates an empty descriptor layout for dynamic uniform buffer bindings.
---@field create_descriptor_set fun(self: vireo.Vireo, layout: vireo.DescriptorLayout, name: string|nil): vireo.DescriptorSet Creates a descriptor set from a built DescriptorLayout.
---@field create_sampler fun(self: vireo.Vireo, minFilter: vireo.Filter, magFilter: vireo.Filter, addressModeU: vireo.AddressMode, addressModeV: vireo.AddressMode, addressModeW: vireo.AddressMode, minLod: number|nil, maxLod: number|nil, anisotropyEnable: boolean|nil, mipMapMode: vireo.FilterMode|nil, compareOp: vireo.CompareOp|nil): vireo.Sampler Creates a texture sampler with the given filtering and addressing parameters.
---@field is_backend_supported fun(backend: vireo.Backend): boolean Returns true if the given backend is available on this machine. @static

------------------------------------------------------------------------
-- Global namespace declaration
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