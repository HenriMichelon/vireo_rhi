---@meta
---@diagnostic disable: missing-return, lowercase-global

------------------------------------------------------------------------
-- Enums (exposed as namespaces with addVariable)
------------------------------------------------------------------------

---@class vireo.Backend Rendering backend identifier.
---@field UNDEFINED integer Unknown or uninitialized backend.
---@field DIRECTX integer DirectX 12 backend.
---@field VULKAN integer Vulkan backend.

---@class vireo.Filter Texture filtering mode for min/mag sampling.
---@field NEAREST integer Nearest-neighbor filtering (no interpolation, sharp).
---@field LINEAR integer Bilinear filtering (smoothly interpolated).

---@class vireo.AddressMode Texture coordinate wrapping mode applied when UV coordinates fall outside [0, 1].
---@field REPEAT integer Texture coordinates wrap around (tile the texture).
---@field MIRRORED_REPEAT integer Texture coordinates mirror on each wrap boundary.
---@field CLAMP_TO_EDGE integer Coordinates clamp to [0, 1]; edge texels are repeated at the border.
---@field CLAMP_TO_BORDER integer Coordinates outside [0, 1] sample the border color.

---@class vireo.FilterMode Mip-map selection mode.
---@field NEAREST integer Selects the nearest mip level (no blending between mips).
---@field LINEAR integer Linearly interpolates between the two nearest mip levels.

---@class vireo.ImageFormat Pixel format constants for images, render targets, and swap chains.
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
---@field B8G8R8A8_UNORM integer 8-bit BGRA, normalized unsigned (common Windows swapchain format).
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

---@class vireo.BufferType Constants specifying the intended GPU usage of a Buffer.
---@field VERTEX integer Vertex buffer — read on the GPU during draw calls.
---@field INDEX integer Index buffer — read on the GPU during indexed draw calls.
---@field INDIRECT integer Indirect draw/dispatch argument buffer — stores GPU-side draw commands.
---@field UNIFORM integer Uniform (constant) buffer — CPU-writable, shader-readable per draw.
---@field STORAGE integer Read-only storage buffer — shader-readable (SRV / SSBO).
---@field DEVICE_STORAGE integer Device-local storage buffer — GPU-only, highest bandwidth.
---@field READWRITE_STORAGE integer Read/write storage buffer — shader-readable and writable (UAV / SSBO).
---@field BUFFER_UPLOAD integer CPU staging buffer — used to upload data to a VERTEX/INDEX/UNIFORM/STORAGE buffer.
---@field BUFFER_DOWNLOAD integer CPU readback buffer — used to download data from a GPU buffer to the CPU.
---@field IMAGE_UPLOAD integer CPU staging buffer — used to upload image data to a GPU Image.
---@field IMAGE_DOWNLOAD integer CPU readback buffer — used to download image data from a GPU Image.

---@class vireo.IndexType Data type of index elements in an index buffer.
---@field UINT16 integer 16-bit unsigned integer indices (max 65 535 vertices per draw).
---@field UINT32 integer 32-bit unsigned integer indices (max ~4 billion vertices per draw).

---@class vireo.DescriptorType Binding type constants for DescriptorLayout entries.
---@field UNIFORM integer Uniform (constant) buffer binding.
---@field UNIFORM_DYNAMIC integer Dynamic uniform buffer binding — byte offset supplied at bind time.
---@field STORAGE integer Read-only storage buffer binding (SRV / SSBO).
---@field DEVICE_STORAGE integer Device-local storage buffer binding.
---@field READWRITE_STORAGE integer Read/write storage buffer binding (UAV / SSBO).
---@field SAMPLED_IMAGE integer Sampled image (texture) binding — combined with a sampler.
---@field SAMPLER integer Sampler-only binding.
---@field READWRITE_IMAGE integer Read/write image binding (storage image / UAV).

---@class vireo.CommandType Queue and command-allocator type constants.
---@field GRAPHIC integer Graphics command queue — supports draw, render-pass, and compute commands.
---@field TRANSFER integer Transfer (copy) command queue — optimized for CPU↔GPU data transfers.
---@field COMPUTE integer Compute command queue — supports dispatch commands only.

---@class vireo.AttributeFormat Vertex attribute data format constants used in VertexInputLayout descriptions.
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

---@class vireo.RenderTargetType Attachment type for render targets.
---@field COLOR integer Color render target (stores RGBA output).
---@field DEPTH integer Depth-only render target.
---@field DEPTH_STENCIL integer Combined depth and stencil render target.

---@class vireo.CullMode Polygon face-culling mode.
---@field NONE integer No face culling — both sides are rendered.
---@field FRONT integer Cull front-facing polygons.
---@field BACK integer Cull back-facing polygons (default, hides back faces).

---@class vireo.PrimitiveTopology Input assembly topology — defines how vertices are grouped into primitives.
---@field POINT_LIST integer Each vertex is a separate point.
---@field LINE_LIST integer Every pair of vertices forms an independent line segment.
---@field LINE_STRIP integer Vertices form a connected polyline; each vertex after the first extends the line.
---@field TRIANGLE_LIST integer Every three vertices form an independent triangle (default).
---@field TRIANGLE_STRIP integer Vertices form a strip of connected triangles sharing edges.

---@class vireo.PolygonMode Rasterization fill mode for polygons.
---@field FILL integer Polygons are filled solidly (default).
---@field WIREFRAME integer Polygons are drawn as wireframe outlines only.

---@class vireo.CompareOp Depth, stencil, and sampler comparison operators.
---@field NEVER integer The test never passes.
---@field LESS integer Passes if source < destination.
---@field EQUAL integer Passes if source == destination.
---@field LESS_OR_EQUAL integer Passes if source <= destination.
---@field GREATER integer Passes if source > destination.
---@field NOT_EQUAL integer Passes if source != destination.
---@field GREATER_OR_EQUAL integer Passes if source >= destination.
---@field ALWAYS integer The test always passes.

---@class vireo.StencilOp Operations applied to a stencil buffer value based on test results.
---@field KEEP integer Keep the current stencil value unchanged.
---@field ZERO integer Set the stencil value to 0.
---@field REPLACE integer Replace the stencil value with the stencil reference value.
---@field INCREMENT_AND_CLAMP integer Increment the stencil value, clamped to the maximum representable value.
---@field DECREMENT_AND_CLAMP integer Decrement the stencil value, clamped to 0.
---@field INVERT integer Bitwise-invert all bits of the stencil value.
---@field INCREMENT_AND_WRAP integer Increment the stencil value, wrapping to 0 on overflow.
---@field DECREMENT_AND_WRAP integer Decrement the stencil value, wrapping to the maximum on underflow.

---@class vireo.BlendFactor Source or destination multiplier constants for color blending equations.
---@field ZERO integer Factor is (0, 0, 0, 0) — nullifies the term.
---@field ONE integer Factor is (1, 1, 1, 1) — passes the term unchanged.
---@field SRC_COLOR integer Factor is the source color (Rs, Gs, Bs, As).
---@field ONE_MINUS_SRC_COLOR integer Factor is (1-Rs, 1-Gs, 1-Bs, 1-As).
---@field DST_COLOR integer Factor is the destination color (Rd, Gd, Bd, Ad).
---@field ONE_MINUS_DST_COLOR integer Factor is (1-Rd, 1-Gd, 1-Bd, 1-Ad).
---@field SRC_ALPHA integer Factor is the source alpha replicated to all channels (As, As, As, As).
---@field ONE_MINUS_SRC_ALPHA integer Factor is (1-As, 1-As, 1-As, 1-As) — standard alpha blending denominator.
---@field DST_ALPHA integer Factor is the destination alpha replicated to all channels (Ad, Ad, Ad, Ad).
---@field ONE_MINUS_DST_ALPHA integer Factor is (1-Ad, 1-Ad, 1-Ad, 1-Ad).
---@field CONSTANT_COLOR integer Factor is the pipeline blend constant color.
---@field ONE_MINUS_CONSTANT_COLOR integer Factor is one minus the pipeline blend constant color.
---@field CONSTANT_ALPHA integer Factor is the pipeline blend constant alpha.
---@field ONE_MINUS_CONSTANT_ALPHA integer Factor is one minus the pipeline blend constant alpha.
---@field SRC_ALPHA_SATURATE integer Factor is (f, f, f, 1) where f = min(As, 1-Ad).
---@field SRC1_COLOR integer Factor is the second source color (dual-source blending).
---@field ONE_MINUS_SRC1_COLOR integer Factor is one minus the second source color.
---@field SRC1_ALPHA integer Factor is the second source alpha (dual-source blending).
---@field ONE_MINUS_SRC1_ALPHA integer Factor is one minus the second source alpha.

---@class vireo.BlendOp Arithmetic operation combining the source and destination terms in a blend equation.
---@field ADD integer Result = Src * SrcFactor + Dst * DstFactor.
---@field SUBTRACT integer Result = Src * SrcFactor - Dst * DstFactor.
---@field REVERSE_SUBTRACT integer Result = Dst * DstFactor - Src * SrcFactor.
---@field MIN integer Result = min(Src, Dst) (factors are ignored).
---@field MAX integer Result = max(Src, Dst) (factors are ignored).

---@class vireo.LogicOp Bitwise logical operation applied to color attachment values (enabled via logic_op_enable).
---@field CLEAR integer Result = 0.
---@field SET integer Result = 1.
---@field COPY integer Result = Src.
---@field COPY_INVERTED integer Result = ~Src.
---@field NOOP integer Result = Dst (no-op).
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

---@class vireo.ColorWriteMask Bitmask controlling which color channels are written by a pipeline.
---@field RED integer Enable writes to the red channel.
---@field GREEN integer Enable writes to the green channel.
---@field BLUE integer Enable writes to the blue channel.
---@field ALPHA integer Enable writes to the alpha channel.
---@field ALL integer Enable writes to all four channels (R+G+B+A).

---@class vireo.ShaderStage Bitmask constants identifying one or more shader stages in a pipeline.
---@field ALL integer All shader stages combined.
---@field VERTEX integer Vertex shader stage.
---@field FRAGMENT integer Fragment (pixel) shader stage.
---@field HULL integer Hull (tessellation control) shader stage.
---@field DOMAIN integer Domain (tessellation evaluation) shader stage.
---@field GEOMETRY integer Geometry shader stage.
---@field COMPUTE integer Compute shader stage.

---@class vireo.WaitStage Pipeline stage constants used in semaphore wait/signal operations and barrier scopes.
---@field NONE integer No stage / no wait.
---@field PIPELINE_TOP integer Top of the pipeline — before any stage begins processing commands.
---@field VERTEX_INPUT integer Vertex input stage — when vertex and index buffers are read.
---@field VERTEX_SHADER integer Vertex shader stage.
---@field DEPTH_STENCIL_TEST_BEFORE_FRAGMENT_SHADER integer Early depth/stencil test, before fragment shading.
---@field FRAGMENT_SHADER integer Fragment (pixel) shader stage.
---@field DEPTH_STENCIL_TEST_AFTER_FRAGMENT_SHADER integer Late depth/stencil test, after fragment shading.
---@field COLOR_OUTPUT integer Color attachment output stage — when color values are written.
---@field COMPUTE_SHADER integer Compute shader stage.
---@field TRANSFER integer Transfer (copy/blit) stage.
---@field PIPELINE_BOTTOM integer Bottom of the pipeline — all stages have completed.
---@field ALL_GRAPHICS integer All graphics pipeline stages combined.
---@field ALL_COMMANDS integer All commands in the queue.
---@field COPY integer Copy operations stage.
---@field RESOLV integer Multisample resolve stage.
---@field BLIT integer Blit operations stage.
---@field CLEAR integer Clear operations stage.

---@class vireo.ResourceState Resource state constants used in pipeline barriers to declare how a resource is accessed.
---@field UNDEFINED integer Unknown / initial state. A barrier from UNDEFINED discards previous content.
---@field RENDER_TARGET_COLOR integer Image is used as a read/write color render target attachment.
---@field RENDER_TARGET_DEPTH integer Image is used as a read/write depth attachment.
---@field RENDER_TARGET_DEPTH_READ integer Image is used as a read-only depth attachment (depth test without writes).
---@field RENDER_TARGET_DEPTH_STENCIL integer Image is used as a read/write depth+stencil attachment.
---@field RENDER_TARGET_DEPTH_STENCIL_READ integer Image is used as a read-only depth+stencil attachment.
---@field PRESENT integer Image is ready to be presented by the swap chain.
---@field COPY_SRC integer Resource is used as the source of a copy/transfer operation.
---@field COPY_DST integer Resource is used as the destination of a copy/transfer operation.
---@field SHADER_READ integer Image or buffer is read by a shader (SRV / sampled image).
---@field COMPUTE_READ integer Resource is read by a compute shader.
---@field COMPUTE_WRITE integer Resource is written by a compute shader (UAV / storage image or buffer).
---@field INDIRECT_DRAW integer Buffer is used as an indirect draw/dispatch argument source.
---@field VERTEX_INPUT integer Buffer is used as a vertex or index input source.
---@field UNIFORM integer Buffer is used as a uniform (constant) buffer.

---@class vireo.MSAA Multisampling anti-aliasing sample-count constants.
---@field NONE integer No multisampling (1 sample per pixel).
---@field X2 integer 2× multisampling.
---@field X4 integer 4× multisampling.
---@field X8 integer 8× multisampling.
---@field X16 integer 16× multisampling.
---@field X32 integer 32× multisampling.
---@field X64 integer 64× multisampling.

---@class vireo.PresentMode Swap-chain frame presentation mode.
---@field IMMEDIATE integer Frames are presented immediately without waiting for VSync (may produce tearing).
---@field VSYNC integer Frames are synchronized to the display refresh rate (no tearing, adds latency).

---@class vireo.PipelineType Pipeline kind constants.
---@field GRAPHIC integer Graphics pipeline (vertex + fragment + optional hull/domain/geometry stages).
---@field COMPUTE integer Compute pipeline (compute shader only).

---@class vireo.SemaphoreType GPU synchronization semaphore type.
---@field BINARY integer Binary semaphore — toggles between signaled and unsignaled.
---@field TIMELINE integer Timeline semaphore — monotonically increasing 64-bit counter.

------------------------------------------------------------------------
-- Aliases / typedefs
------------------------------------------------------------------------

---@alias vireo.DescriptorIndex integer Binding slot index within a DescriptorLayout / DescriptorSet.

------------------------------------------------------------------------
-- Structs
------------------------------------------------------------------------

---@class vireo.ColorBlendDesc Color blend state for a single render-target color attachment.
---@field blend_enable boolean True to enable alpha/color blending on this attachment.
---@field src_color_blend_factor vireo.BlendFactor Blend factor applied to the source (incoming) color.
---@field dst_color_blend_factor vireo.BlendFactor Blend factor applied to the destination (existing) color.
---@field color_blend_op vireo.BlendOp Blend arithmetic operation for the color channels.
---@field src_alpha_blend_factor vireo.BlendFactor Blend factor applied to the source alpha.
---@field dst_alpha_blend_factor vireo.BlendFactor Blend factor applied to the destination alpha.
---@field alpha_blend_op vireo.BlendOp Blend arithmetic operation for the alpha channel.
---@field color_write_mask vireo.ColorWriteMask Bitmask of color channels that receive writes (see vireo.ColorWriteMask).

---@class vireo.StencilOpState Per-face stencil test and write operations.
---@field fail_op vireo.StencilOp Operation applied when the stencil test fails.
---@field pass_op vireo.StencilOp Operation applied when both the stencil and depth tests pass.
---@field depth_fail_op vireo.StencilOp Operation applied when the stencil test passes but the depth test fails.
---@field compare_op vireo.CompareOp Comparison function used for the stencil test.
---@field compare_mask integer Bitmask applied to both the reference and stored stencil values before comparison.
---@field write_mask integer Bitmask controlling which stencil bits are written on a passing test.

---@class vireo.PhysicalDeviceDesc GPU device description returned by PhysicalDevice.description.
---@field name string Human-readable GPU device name. (read-only)
---@field dedicated_video_memory integer Dedicated video memory in bytes (on-board VRAM). (read-only)
---@field dedicated_system_memory integer Dedicated system memory in bytes (UMA-style on-board memory). (read-only)
---@field shared_system_memory integer Shared system memory in bytes (RAM shared with the CPU). (read-only)

---@class vireo.Extent 2-D integer dimensions in pixels or texels.
---@field width integer Width in pixels or texels.
---@field height integer Height in pixels or texels.

---@class vireo.Rect 2-D integer rectangle (pixel position + size). Constructor: Rect(width, height) sets x=y=0; Rect(x, y, width, height).
---@field x integer Left edge in pixels (default 0).
---@field y integer Top edge in pixels (default 0).
---@field width integer Rectangle width in pixels.
---@field height integer Rectangle height in pixels.

---@class vireo.Viewport Floating-point viewport with depth range. Constructor: Viewport(width, height); Viewport(x, y, width, height).
---@field x number Left edge of the viewport in pixels.
---@field y number Top edge of the viewport in pixels.
---@field width number Viewport width in pixels.
---@field height number Viewport height in pixels.
---@field min_depth number Minimum depth value mapped to this viewport (default 0.0).
---@field max_depth number Maximum depth value mapped to this viewport (default 1.0).

---@class vireo.PushConstantsDesc Describes a push-constant range visible to one or more shader stages.
---@field stage vireo.ShaderStage Shader stage(s) that can read this push-constant range.
---@field size integer Size of the push-constant block in bytes.
---@field offset integer Byte offset of this range within the push-constant block.

---@class vireo.VertexAttributeDesc Describes a single vertex attribute within a VertexInputLayout.
---@field binding string Name of the input binding slot this attribute reads from.
---@field format vireo.AttributeFormat Data type and component count of the attribute.
---@field offset integer Byte offset of this attribute from the start of the vertex structure.

---@class vireo.DepthClearValue Depth and stencil values used when clearing a depth/stencil attachment.
---@field depth number Clear value for the depth aspect (default 1.0 = far plane).
---@field stencil integer Clear value for the stencil aspect (default 0).

---@class vireo.ClearValue Union holding either a color clear value (r/g/b/a) or a depth/stencil clear value (depth/stencil). Set r/g/b/a for color attachments; set depth/stencil for depth attachments.
---@field r number Red component of the color clear value (0.0–1.0).
---@field g number Green component of the color clear value (0.0–1.0).
---@field b number Blue component of the color clear value (0.0–1.0).
---@field a number Alpha component of the color clear value (0.0–1.0, default 1.0).
---@field depth number Depth clear value used for depth attachments (default 1.0).
---@field stencil integer Stencil clear value used for stencil attachments (default 0).

---@class vireo.VideoMemoryAllocationDesc Describes a single GPU memory allocation reported by Buffer.get_memory_allocations() or Image.get_memory_allocations().
---@field usage integer Whether this allocation backs a buffer or an image (implementation-defined integer). (read-only)
---@field name string Debug name of the allocated resource. (read-only)
---@field size integer Allocation size in bytes. (read-only)
---@field ref any Opaque reference to the underlying API resource object. (read-only)

---@class vireo.RenderTargetDesc Describes a single attachment slot in a RenderingConfiguration.
---@field swap_chain vireo.SwapChain|nil Swap chain providing this attachment; mutually exclusive with render_target.
---@field render_target vireo.RenderTarget|nil Explicit render target for this attachment; mutually exclusive with swap_chain.
---@field multisampled_render_target vireo.RenderTarget|nil Optional MSAA render target to resolve into render_target after the pass.
---@field clear boolean True to clear this attachment at the start of the render pass.
---@field clear_value vireo.ClearValue Clear value applied when clear is true.
---@field discard_after_render boolean True to discard the attachment content after the render pass (saves bandwidth on tile-based GPUs).

---@class vireo.RenderingConfiguration Full render-pass configuration passed to CommandList.begin_rendering().
---@field color_render_targets vireo.RenderTargetDesc[] Ordered list of color attachment descriptors.
---@field depth_stencil_render_target vireo.RenderTarget|nil Depth/stencil attachment (nil to disable depth/stencil).
---@field multisampled_depth_stencil_render_target vireo.RenderTarget|nil Optional MSAA depth/stencil resolve target.
---@field use_depth_attachment boolean True to use the depth aspect of the depth/stencil attachment.
---@field use_stencil_attachment boolean True to use the stencil aspect of the depth/stencil attachment.
---@field clear_depth_stencil boolean True to clear the depth/stencil attachment at the start of the render pass.
---@field depth_stencil_clear_value vireo.ClearValue Clear value for depth/stencil (use .depth and .stencil fields).
---@field discard_depth_stencil_after_render boolean True to discard the depth/stencil content after the render pass.

---@class vireo.DrawIndirectCommand GPU-side structure for an indirect (non-indexed) draw call. Mirror of VkDrawIndirectCommand / D3D12_DRAW_ARGUMENTS.
---@field vertex_count integer Number of vertices to draw per instance.
---@field instance_count integer Number of instances to draw.
---@field first_vertex integer Index of the first vertex to read from the vertex buffer.
---@field first_instance integer Instance ID of the first instance (added to gl_InstanceIndex / SV_InstanceID).

---@class vireo.DrawIndexedIndirectCommand GPU-side structure for an indirect indexed draw call. Mirror of VkDrawIndexedIndirectCommand / D3D12_DRAW_INDEXED_ARGUMENTS.
---@field index_count integer Number of indices to draw per instance.
---@field instance_count integer Number of instances to draw.
---@field first_index integer Base offset (in indices) into the index buffer.
---@field vertex_offset integer Value added to each index before reading a vertex from the vertex buffer.
---@field first_instance integer Instance ID of the first instance.

---@class vireo.BufferCopyRegion Describes a single source/destination region pair for a buffer-to-buffer copy operation.
---@field src_offset integer Byte offset into the source buffer where the copy begins.
---@field dst_offset integer Byte offset into the destination buffer where the copy begins.
---@field size integer Number of bytes to copy.

---@class vireo.GraphicPipelineConfiguration Full description of a graphics pipeline passed to Vireo.create_graphic_pipeline().
---@field resources vireo.PipelineResources Pipeline layout (descriptor set layouts + push-constant ranges).
---@field color_render_formats vireo.ImageFormat[] Pixel formats of the color render target attachments, in order.
---@field color_blend_desc vireo.ColorBlendDesc[] Per-attachment color blend states, in the same order as color_render_formats.
---@field vertex_input_layout vireo.VertexInputLayout|nil Vertex attribute layout (nil for pipelines without vertex buffers).
---@field vertex_shader vireo.ShaderModule|nil Vertex shader module (nil for mesh-shader pipelines).
---@field fragment_shader vireo.ShaderModule|nil Fragment shader module (nil for depth-only passes).
---@field hull_shader vireo.ShaderModule|nil Hull (tessellation control) shader module (nil if unused).
---@field domain_shader vireo.ShaderModule|nil Domain (tessellation evaluation) shader module (nil if unused).
---@field geometry_shader vireo.ShaderModule|nil Geometry shader module (nil if unused).
---@field primitive_topology vireo.PrimitiveTopology Input assembly topology for vertex grouping.
---@field msaa vireo.MSAA Multisampling level (NONE disables MSAA).
---@field cull_mode vireo.CullMode Face culling mode applied during rasterization.
---@field polygon_mode vireo.PolygonMode Polygon fill mode (FILL or WIREFRAME).
---@field front_face_counter_clockwise boolean True if counter-clockwise winding order defines front faces.
---@field depth_stencil_image_format vireo.ImageFormat Pixel format of the depth/stencil attachment (UNDEFINED if none is used).
---@field depth_test_enable boolean True to enable the per-fragment depth test.
---@field depth_write_enable boolean True to write depth values when the depth test passes.
---@field depth_compare_op vireo.CompareOp Comparison function for the depth test.
---@field depth_bias_enable boolean True to enable depth biasing (useful to avoid z-fighting in shadow maps).
---@field depth_bias_constant_factor number Constant depth bias added to every fragment's depth.
---@field depth_bias_clamp number Maximum (or minimum, if negative) depth bias allowed on any fragment.
---@field depth_bias_slope_factor number Depth bias scaled by the fragment's depth slope.
---@field stencil_test_enable boolean True to enable the per-fragment stencil test.
---@field front_stencil_op_state vireo.StencilOpState Stencil test and write operations for front-facing polygons.
---@field back_stencil_op_state vireo.StencilOpState Stencil test and write operations for back-facing polygons.
---@field logic_op_enable boolean True to enable logical operations on color attachments instead of blending.
---@field logic_op vireo.LogicOp Logical operation applied to color attachment values when logic_op_enable is true.
---@field alpha_to_coverage_enable boolean True to derive a per-sample coverage mask from the fragment alpha value (requires MSAA).

------------------------------------------------------------------------
-- Classes / objects
------------------------------------------------------------------------

---@class vireo.Fence CPU/GPU synchronization primitive. Created by Vireo.create_fence(). Signaled by the GPU after a submit; waited on by the CPU.
---@field wait fun(self: vireo.Fence): nil Blocks the calling CPU thread until the GPU signals this fence.
---@field reset fun(self: vireo.Fence): nil Resets the fence to the unsignaled state so it can be reused.

---@class vireo.Semaphore GPU/GPU or GPU/CPU synchronization primitive. Created by Vireo.create_semaphore().
---@field type vireo.SemaphoreType The semaphore type (BINARY or TIMELINE). (read-only)
---@field value integer Current counter value (timeline semaphores only); assignable to set it from the CPU.
---@field increment_value fun(self: vireo.Semaphore): nil Increments the timeline counter by one (timeline semaphores only).
---@field decrement_value fun(self: vireo.Semaphore): nil Decrements the timeline counter by one (timeline semaphores only).

---@class vireo.Instance The underlying API instance (VkInstance / IDXGIFactory). Retrieved via Vireo.instance. Opaque; used for platform-specific window surface creation.

---@class vireo.PhysicalDevice Represents the physical GPU selected by the engine. Retrieved via Vireo.physical_device.
---@field description vireo.PhysicalDeviceDesc Name and memory capacity of the GPU. (read-only)

---@class vireo.Device The logical device wrapping the selected GPU. Retrieved via Vireo.device.
---@field have_dedicated_transfer_queue boolean True if the GPU exposes a dedicated transfer queue separate from the graphics queue. (read-only)

---@class vireo.Buffer A GPU buffer allocation. Created by Vireo.create_buffer().
---@field size integer Total size of the buffer in bytes. (read-only)
---@field type vireo.BufferType Usage type of this buffer (see vireo.BufferType). (read-only)
---@field instance_size integer Size of a single element before alignment padding in bytes. (read-only)
---@field instance_size_aligned integer Aligned size of a single element in bytes (may be larger than instance_size). (read-only)
---@field instance_count integer Number of elements/instances the buffer was allocated for. (read-only)
---@field mapped_address lightuserdata CPU-side mapped pointer (only valid between map() and unmap(), for upload/download buffers). (read-only)
---@field map fun(self: vireo.Buffer): nil Maps the buffer for CPU read/write access. Must call unmap() when done.
---@field unmap fun(self: vireo.Buffer): nil Unmaps the buffer; CPU access is invalid after this call.
---@field write fun(self: vireo.Buffer, data: lightuserdata|any, size: integer|nil, offset: integer|nil): nil Writes data into the mapped buffer. size defaults to the full buffer size; offset defaults to 0.
---@field get_memory_allocations fun(): vireo.VideoMemoryAllocationDesc[] Returns all current GPU memory allocations for all Buffer objects. @static

---@class vireo.Sampler Immutable texture sampler state object. Created by Vireo.create_sampler(). Opaque; bind it to descriptor sets via DescriptorSet.update_sampler().

---@class vireo.Image A GPU image (texture or render attachment). Created by Vireo.create_image() or Vireo.create_read_write_image().
---@field format vireo.ImageFormat Pixel format of the image. (read-only)
---@field width integer Width in pixels of mip level 0. (read-only)
---@field height integer Height in pixels of mip level 0. (read-only)
---@field mip_levels integer Total number of mip levels. (read-only)
---@field array_size integer Number of array layers (1 for non-array images). (read-only)
---@field is_read_write boolean True if the image was created with read/write (UAV / storage image) access. (read-only)
---@field get_row_pitch fun(self: vireo.Image, mipLevel: integer|nil): integer Row pitch in bytes for the given mip level (default 0).
---@field get_row_length fun(self: vireo.Image, mipLevel: integer|nil): integer Row length in texels for the given mip level (default 0).
---@field get_image_size fun(self: vireo.Image, mipLevel: integer|nil): integer Unaligned size in bytes of one array layer at the given mip level (default 0).
---@field get_aligned_image_size fun(self: vireo.Image, mipLevel: integer|nil): integer Aligned size in bytes of one array layer at the given mip level (includes any backend-required padding).
---@field get_aligned_row_pitch fun(self: vireo.Image, mipLevel: integer|nil): integer Aligned row pitch in bytes for the given mip level.
---@field get_aligned_row_length fun(self: vireo.Image, mipLevel: integer|nil): integer Aligned row length in texels for the given mip level.
---@field get_pixel_size fun(format: vireo.ImageFormat): integer Returns the size in bytes of a single pixel in the given format. @static
---@field get_memory_allocations fun(): vireo.VideoMemoryAllocationDesc[] Returns all current GPU memory allocations for all Image objects. @static

---@class vireo.RenderTarget A render-target image wrapper. Created by Vireo.create_render_target() or Vireo.create_render_target_from_swap_chain().
---@field image vireo.Image The underlying GPU Image used as the render attachment. (read-only)
---@field type vireo.RenderTargetType The render-target attachment type (COLOR, DEPTH, or DEPTH_STENCIL). (read-only)

---@class vireo.DescriptorLayout Describes the binding slots of a descriptor set. Created by Vireo.create_descriptor_layout(). Must be built with build() before creating descriptor sets.
---@field capacity integer Number of binding slots declared in this layout. (read-only)
---@field is_dynamic_uniform boolean True if this layout was created for dynamic uniform buffer bindings. (read-only)
---@field is_samplers boolean True if this layout was created for sampler-only bindings. (read-only)
---@field add fun(self: vireo.DescriptorLayout, index: vireo.DescriptorIndex, type: vireo.DescriptorType, count: integer|nil): vireo.DescriptorLayout Declares a binding at the given index with the specified type and optional array count. Returns self for method chaining.
---@field build fun(self: vireo.DescriptorLayout): nil Finalizes the layout. Must be called before passing to Vireo.create_descriptor_set().

---@class vireo.DescriptorSet A bound collection of resources (buffers, images, samplers) for a pipeline. Created by Vireo.create_descriptor_set().
---@field update_buffer fun(self: vireo.DescriptorSet, index: vireo.DescriptorIndex, buffer: vireo.Buffer): nil Binds a uniform or storage buffer at the given binding index.
---@field update_dynamic_buffer fun(self: vireo.DescriptorSet, buffer: vireo.Buffer): nil Binds a dynamic uniform buffer at binding index 0; the byte offset is supplied at bind time.
---@field update_image fun(self: vireo.DescriptorSet, index: vireo.DescriptorIndex, image: vireo.Image): nil Binds a sampled image or storage image at the given binding index.
---@field update_sampler fun(self: vireo.DescriptorSet, index: vireo.DescriptorIndex, sampler: vireo.Sampler): nil Binds a sampler at the given binding index.
---@field update_image_array fun(self: vireo.DescriptorSet, index: vireo.DescriptorIndex, images: vireo.Image[]): nil Binds an array of images starting at the given binding index.
---@field update_buffer_array fun(self: vireo.DescriptorSet, index: vireo.DescriptorIndex, buffers: vireo.Buffer[]): nil Binds an array of buffers starting at the given binding index.
---@field update_sampler_array fun(self: vireo.DescriptorSet, index: vireo.DescriptorIndex, samplers: vireo.Sampler[]): nil Binds an array of samplers starting at the given binding index.
---@field layout vireo.DescriptorLayout The DescriptorLayout this set was created from. (read-only)

---@class vireo.VertexInputLayout Describes vertex buffer bindings and per-attribute formats for a graphics pipeline. Created by Vireo.create_vertex_layout(). Opaque; pass to GraphicPipelineConfiguration.vertex_input_layout.

---@class vireo.ShaderModule A compiled shader binary. Created by Vireo.create_shader_module_from_file() or Vireo.create_shader_module_from_data(). Opaque; pass to GraphicPipelineConfiguration or Vireo.create_compute_pipeline().

---@class vireo.PipelineResources The pipeline layout — describes which descriptor sets and push constants a pipeline uses. Created by Vireo.create_pipeline_resources(). Opaque.

---@class vireo.Pipeline Base type for compiled GPU pipelines. Use vireo.ComputePipeline or vireo.GraphicPipeline.
---@field resources vireo.PipelineResources The pipeline layout used by this pipeline. (read-only)
---@field type vireo.PipelineType The pipeline kind (GRAPHIC or COMPUTE). (read-only)

---@class vireo.ComputePipeline : vireo.Pipeline A compiled compute pipeline binding a single compute shader. Created by Vireo.create_compute_pipeline().

---@class vireo.GraphicPipeline : vireo.Pipeline A compiled graphics pipeline with full rasterization state. Created by Vireo.create_graphic_pipeline().

---@class vireo.SwapChain Presentation swap chain managing a set of back buffers for a window. Created by Vireo.create_swap_chain().
---@field extent vireo.Extent Current back-buffer size in pixels. (read-only)
---@field aspect_ratio number Width divided by height of the current back buffer. (read-only)
---@field current_frame_index integer Index of the currently acquired back buffer (0 to frames_in_flight-1). (read-only)
---@field frames_in_flight integer Number of back buffers in the swap chain. (read-only)
---@field format vireo.ImageFormat Pixel format of the back buffers. (read-only)
---@field next_frame_index fun(self: vireo.SwapChain): nil Advances the internal frame index to the next frame slot.
---@field acquire fun(self: vireo.SwapChain, fence: vireo.Fence): boolean Acquires the next available back buffer. Returns false if the swap chain must be recreated (window resized).
---@field present fun(self: vireo.SwapChain): nil Presents the current back buffer to the display.
---@field recreate fun(self: vireo.SwapChain): nil Recreates the swap chain and all back buffers (call after a window resize).
---@field wait_idle fun(self: vireo.SwapChain): nil Blocks the CPU until all pending present operations for this swap chain have completed.

---@class vireo.CommandList Records a sequence of GPU commands for later submission. Obtained from CommandAllocator.create_command_list().
---@field begin fun(self: vireo.CommandList): nil Begins command recording. Must be called before any other recording command.
---@field end fun(self: vireo.CommandList): nil Ends command recording. Must be called before submitting to a queue.
---@field upload_buffer fun(self: vireo.CommandList, destination: vireo.Buffer, data: lightuserdata|any): nil Copies CPU data into a BUFFER_UPLOAD staging buffer.
---@field upload_image fun(self: vireo.CommandList, destination: vireo.Image, data: lightuserdata|any, firstMipLevel: integer): nil Copies CPU data into an IMAGE_UPLOAD staging buffer starting at the given mip level.
---@field copy_buffer_to_image fun(self: vireo.CommandList, src: vireo.Buffer, dst: vireo.Image, mipLevel: integer, arrayLayer: integer, generateMips: boolean): nil Copies a staging buffer into a single image mip level and array layer; optionally generates remaining mips.
---@field copy_buffer_to_image_levels fun(self: vireo.CommandList, src: vireo.Buffer, dst: vireo.Image, offsets: integer[], generateMips: boolean): nil Copies a staging buffer into multiple mip levels using explicit byte offsets per mip level.
---@field copy_buffer_to_buffer fun(self: vireo.CommandList, src: vireo.Buffer, dst: vireo.Buffer, size: integer, srcOffset: integer, dstOffset: integer): nil Copies size bytes from src (at srcOffset) into dst (at dstOffset).
---@field copy_buffer_regions fun(self: vireo.CommandList, src: vireo.Buffer, dst: vireo.Buffer, regions: vireo.BufferCopyRegion[]): nil Copies multiple non-contiguous regions between two buffers in a single command.
---@field copy_image_to_swap_chain fun(self: vireo.CommandList, src: vireo.Image, swapChain: vireo.SwapChain): nil Blits or resolves src into the swap chain's current back buffer.
---@field begin_rendering fun(self: vireo.CommandList, config: vireo.RenderingConfiguration): nil Begins a dynamic render pass. Must be paired with end_rendering().
---@field end_rendering fun(self: vireo.CommandList): nil Ends the current dynamic render pass.
---@field dispatch fun(self: vireo.CommandList, x: integer, y: integer, z: integer): nil Dispatches a compute shader with the given thread-group counts in X, Y, and Z.
---@field bind_pipeline fun(self: vireo.CommandList, pipeline: vireo.Pipeline, descriptorsAlreadyBound: boolean|nil): nil Binds a graphics or compute pipeline. Pass descriptorsAlreadyBound=true to skip re-binding unchanged descriptor sets.
---@field bind_vertex_buffer fun(self: vireo.CommandList, buffer: vireo.Buffer, offset: integer|nil): nil Binds a vertex buffer at the default slot with an optional byte offset (default 0).
---@field bind_vertex_buffers fun(self: vireo.CommandList, buffers: vireo.Buffer[], offsets: integer[]|nil): nil Binds multiple vertex buffers at once with optional per-buffer byte offsets.
---@field bind_index_buffer fun(self: vireo.CommandList, buffer: vireo.Buffer, indexType: vireo.IndexType|nil, firstIndex: integer|nil): nil Binds an index buffer with an optional index type (default UINT32) and first-index offset (default 0).
---@field bind_descriptors fun(self: vireo.CommandList, pipelineType: vireo.PipelineType, resources: vireo.PipelineResources, descriptors: vireo.DescriptorSet[]): nil Binds descriptor sets before a pipeline is bound; supply pipelineType and resources explicitly. @overload
---@field bind_descriptors fun(self: vireo.CommandList, descriptors: vireo.DescriptorSet[]): nil Binds descriptor sets after a pipeline is bound; pipeline type is inferred. @overload
---@field bind_descriptor fun(self: vireo.CommandList, descriptor: vireo.DescriptorSet, set: integer): nil Binds a single descriptor set at the given set index (pipeline must already be bound).
---@field bind_descriptor_dynamic fun(self: vireo.CommandList, descriptor: vireo.DescriptorSet, set: integer, offset: integer): nil Binds a dynamic-uniform descriptor set at the given set index with a byte offset into the uniform buffer.
---@field draw fun(self: vireo.CommandList, vertexCountPerInstance: integer, instanceCount: integer|nil, firstVertex: integer|nil, firstInstance: integer|nil): nil Issues a non-indexed draw call.
---@field draw_indexed fun(self: vireo.CommandList, indexCountPerInstance: integer, instanceCount: integer|nil, firstIndex: integer|nil, firstVertex: integer|nil, firstInstance: integer|nil): nil Issues an indexed draw call.
---@field draw_indirect fun(self: vireo.CommandList, buffer: vireo.Buffer, offset: integer, drawCount: integer, stride: integer, firstCommandOffset: integer): nil Issues indirect (non-indexed) draw calls whose arguments are read from a GPU buffer.
---@field draw_indexed_indirect_count fun(self: vireo.CommandList, buffer: vireo.Buffer, offset: integer, countBuffer: vireo.Buffer, countOffset: integer, maxDrawCount: integer, stride: integer, firstCommandOffset: integer): nil Issues indirect indexed draw calls with the actual draw count stored in a GPU buffer.
---@field draw_indexed_indirect fun(self: vireo.CommandList, buffer: vireo.Buffer, offset: integer, maxDrawCount: integer, stride: integer, firstCommandOffset: integer): nil Issues indirect indexed draw calls with a CPU-specified maximum draw count.
---@field barrier_image fun(self: vireo.CommandList, image: vireo.Image, oldState: vireo.ResourceState, newState: vireo.ResourceState): nil Inserts a pipeline barrier transitioning an image from oldState to newState.
---@field barrier_render_target fun(self: vireo.CommandList, renderTarget: vireo.RenderTarget, oldState: vireo.ResourceState, newState: vireo.ResourceState): nil Inserts a pipeline barrier transitioning a render target's image between resource states.
---@field barrier_swap_chain fun(self: vireo.CommandList, swapChain: vireo.SwapChain, oldState: vireo.ResourceState, newState: vireo.ResourceState): nil Inserts a pipeline barrier for the swap chain's currently acquired back buffer.
---@field barrier_buffer fun(self: vireo.CommandList, buffer: vireo.Buffer, oldState: vireo.ResourceState, newState: vireo.ResourceState): nil Inserts a pipeline barrier transitioning a buffer between resource states.
---@field set_viewports fun(self: vireo.CommandList, viewports: vireo.Viewport[]): nil Sets multiple viewports for the rasterizer.
---@field set_scissors fun(self: vireo.CommandList, rects: vireo.Rect[]): nil Sets multiple scissor rectangles for the rasterizer.
---@field set_viewport fun(self: vireo.CommandList, viewport: vireo.Viewport): nil Sets a single viewport for the rasterizer.
---@field set_scissor fun(self: vireo.CommandList, rect: vireo.Rect): nil Sets a single scissor rectangle for the rasterizer.
---@field set_stencil_reference fun(self: vireo.CommandList, reference: integer): nil Sets the stencil reference value used in stencil comparison operations.
---@field push_constants fun(self: vireo.CommandList, resources: vireo.PipelineResources, desc: vireo.PushConstantsDesc, data: any): nil Uploads push-constant data for the currently bound pipeline.
---@field cleanup fun(self: vireo.CommandList): nil Releases internal temporary resources. Call after the command list has been submitted and the GPU has finished.

---@class vireo.CommandAllocator Manages a pool of command lists for a specific queue type. Created by Vireo.create_command_allocator().
---@field reset fun(self: vireo.CommandAllocator): nil Resets the allocator and all command lists it owns. Call once per frame before re-recording.
---@field create_command_list fun(self: vireo.CommandAllocator, pipeline: vireo.Pipeline|nil): vireo.CommandList Creates a new command list. Pass a pipeline to pre-bind it at creation time (graphics allocators only).
---@field command_list_type vireo.CommandType The command type this allocator was created for (GRAPHIC, TRANSFER, or COMPUTE). (read-only)

---@class vireo.SubmitQueue GPU command submission queue. Created by Vireo.create_submit_queue().
---@field submit fun(self: vireo.SubmitQueue, commandLists: vireo.CommandList[]): nil Submits command lists with no additional GPU/CPU synchronization.
---@field submit_fence fun(self: vireo.SubmitQueue, fence: vireo.Fence, commandLists: vireo.CommandList[]): nil Submits command lists and signals the given CPU fence when the GPU finishes.
---@field submit_fence_swap fun(self: vireo.SubmitQueue, fence: vireo.Fence, swapChain: vireo.SwapChain, commandLists: vireo.CommandList[]): nil Submits command lists synchronized with a swap-chain present operation; signals the CPU fence on completion.
---@field submit_wait_fence_swap fun(self: vireo.SubmitQueue, waitSemaphore: vireo.Semaphore, waitStage: vireo.WaitStage, fence: vireo.Fence, swapChain: vireo.SwapChain, commandLists: vireo.CommandList[]): nil Submits with a GPU wait on waitSemaphore at waitStage, a CPU fence signal, and a swap-chain present.
---@field submit_wait_signal fun(self: vireo.SubmitQueue, waitSemaphore: vireo.Semaphore, waitStage: vireo.WaitStage, signalStage: vireo.WaitStage, signalSemaphore: vireo.Semaphore, commandLists: vireo.CommandList[]): nil Submits with GPU/GPU synchronization: waits on waitSemaphore at waitStage, then signals signalSemaphore at signalStage.
---@field submit_wait fun(self: vireo.SubmitQueue, waitSemaphore: vireo.Semaphore, waitStage: vireo.WaitStage, commandLists: vireo.CommandList[]): nil Submits after waiting on a GPU semaphore at the given pipeline stage.
---@field submit_signal fun(self: vireo.SubmitQueue, signalStage: vireo.WaitStage, signalSemaphore: vireo.Semaphore, commandLists: vireo.CommandList[]): nil Submits and signals a GPU semaphore when the given pipeline stage completes.
---@field wait_idle fun(self: vireo.SubmitQueue): nil Blocks the CPU until all submitted commands on this queue have finished executing.

---@class vireo.Vireo Main RHI entry point. Obtained from the host application; not constructed in Lua.
---@field backend vireo.Backend The active rendering backend (DIRECTX or VULKAN). (read-only)
---@field shader_file_extension string File extension for pre-compiled shaders on the active backend (e.g. ".spv" for Vulkan, ".cso" for DirectX). (read-only)
---@field physical_device vireo.PhysicalDevice The PhysicalDevice representing the selected GPU. (read-only)
---@field device vireo.Device The logical Device wrapping the GPU. (read-only)
---@field instance vireo.Instance The underlying API Instance (VkInstance / IDXGIFactory). (read-only)
---@field wait_idle fun(self: vireo.Vireo): nil Blocks the CPU until the GPU has finished all pending work on all queues.
---@field create_swap_chain fun(self: vireo.Vireo, format: vireo.ImageFormat, presentQueue: vireo.SubmitQueue, windowHandle: any, presentMode: vireo.PresentMode|nil, framesInFlight: integer|nil): vireo.SwapChain Creates a swap chain for the given OS window handle with optional presentation mode and frame count.
---@field create_submit_queue fun(self: vireo.Vireo, commandType: vireo.CommandType, name: string|nil): vireo.SubmitQueue Creates a GPU command submission queue for the given command type.
---@field create_fence fun(self: vireo.Vireo, createSignaled: boolean|nil, name: string|nil): vireo.Fence Creates a CPU/GPU fence (unsignaled by default; pass createSignaled=true to start in the signaled state).
---@field create_semaphore fun(self: vireo.Vireo, type: vireo.SemaphoreType, name: string|nil): vireo.Semaphore Creates a GPU synchronization semaphore of the given type.
---@field create_command_allocator fun(self: vireo.Vireo, type: vireo.CommandType): vireo.CommandAllocator Creates a command allocator for the given queue type.
---@field create_vertex_layout fun(self: vireo.Vireo, size: integer, attributes: vireo.VertexAttributeDesc[]): vireo.VertexInputLayout Creates a vertex input layout from a per-vertex stride (bytes) and a list of attribute descriptors.
---@field create_shader_module_from_file fun(self: vireo.Vireo, path: string): vireo.ShaderModule Loads a compiled shader binary from a file path (append shader_file_extension for the correct format).
---@field create_shader_module_from_data fun(self: vireo.Vireo, data: any, name: string): vireo.ShaderModule Creates a shader module from raw compiled byte data with an optional debug name.
---@field create_pipeline_resources fun(self: vireo.Vireo, layouts: vireo.DescriptorLayout[]|nil, pushConstant: vireo.PushConstantsDesc|nil, name: string|nil): vireo.PipelineResources Creates a pipeline layout from an ordered list of descriptor layouts and an optional push-constant range.
---@field create_compute_pipeline fun(self: vireo.Vireo, resources: vireo.PipelineResources, shader: vireo.ShaderModule, name: string|nil): vireo.ComputePipeline Compiles and returns a compute pipeline from a layout and a compute shader module.
---@field create_graphic_pipeline fun(self: vireo.Vireo, config: vireo.GraphicPipelineConfiguration, name: string|nil): vireo.GraphicPipeline Compiles and returns a graphics pipeline from a full configuration descriptor.
---@field create_buffer fun(self: vireo.Vireo, type: vireo.BufferType, size: integer, count: integer|nil, name: string|nil): vireo.Buffer Allocates a GPU buffer. size is the per-element byte size; count is the number of elements (default 1).
---@field create_image fun(self: vireo.Vireo, format: vireo.ImageFormat, width: integer, height: integer, mipLevels: integer|nil, arraySize: integer|nil, name: string|nil): vireo.Image Allocates a shader-read-only GPU image.
---@field create_read_write_image fun(self: vireo.Vireo, format: vireo.ImageFormat, width: integer, height: integer, mipLevels: integer|nil, arraySize: integer|nil, name: string|nil): vireo.Image Allocates a GPU read/write image (UAV / storage image).
---@field create_render_target fun(self: vireo.Vireo, format: vireo.ImageFormat, width: integer, height: integer, type: vireo.RenderTargetType|nil, clearValue: vireo.ClearValue|nil, arraySize: integer|nil, msaa: vireo.MSAA|nil, name: string|nil): vireo.RenderTarget Creates a render target of the given format and dimensions.
---@field create_render_target_from_swap_chain fun(self: vireo.Vireo, swapChain: vireo.SwapChain, clearValue: vireo.ClearValue|nil, msaa: vireo.MSAA|nil, name: string|nil): vireo.RenderTarget Creates a color render target whose format and dimensions match the given swap chain.
---@field create_descriptor_layout fun(self: vireo.Vireo, name: string|nil): vireo.DescriptorLayout Creates an empty descriptor layout for uniform, storage, image, and sampler bindings.
---@field create_sampler_descriptor_layout fun(self: vireo.Vireo, name: string|nil): vireo.DescriptorLayout Creates an empty descriptor layout intended for sampler-only bindings.
---@field create_dynamic_uniform_descriptor_layout fun(self: vireo.Vireo, name: string|nil): vireo.DescriptorLayout Creates an empty descriptor layout for dynamic uniform buffer bindings.
---@field create_descriptor_set fun(self: vireo.Vireo, layout: vireo.DescriptorLayout, name: string|nil): vireo.DescriptorSet Creates a descriptor set from a finalized DescriptorLayout.
---@field create_sampler fun(self: vireo.Vireo, minFilter: vireo.Filter, magFilter: vireo.Filter, addressModeU: vireo.AddressMode, addressModeV: vireo.AddressMode, addressModeW: vireo.AddressMode, minLod: number|nil, maxLod: number|nil, anisotropyEnable: boolean|nil, mipMapMode: vireo.FilterMode|nil, compareOp: vireo.CompareOp|nil): vireo.Sampler Creates an immutable texture sampler with the given filtering, addressing, LOD, and comparison parameters.
---@field is_backend_supported fun(backend: vireo.Backend): boolean Returns true if the given rendering backend is available and supported on this machine. @static

------------------------------------------------------------------------
-- Global namespace declaration
------------------------------------------------------------------------

---@class vireo The vireo RHI module — low-level GPU resource creation and command recording.
---@field Backend vireo.Backend Rendering backend identifier constants.
---@field Filter vireo.Filter Texture min/mag filtering mode constants.
---@field AddressMode vireo.AddressMode Texture coordinate wrapping mode constants.
---@field FilterMode vireo.FilterMode Mip-map selection mode constants.
---@field ImageFormat vireo.ImageFormat Pixel format constants for images, render targets, and swap chains.
---@field BufferType vireo.BufferType Buffer usage type constants.
---@field IndexType vireo.IndexType Index element data type constants.
---@field DescriptorType vireo.DescriptorType Descriptor binding type constants.
---@field CommandType vireo.CommandType Queue and command-allocator type constants.
---@field AttributeFormat vireo.AttributeFormat Vertex attribute data format constants.
---@field RenderTargetType vireo.RenderTargetType Render target attachment type constants.
---@field CullMode vireo.CullMode Polygon face-culling mode constants.
---@field PrimitiveTopology vireo.PrimitiveTopology Input assembly topology constants.
---@field PolygonMode vireo.PolygonMode Polygon fill mode constants.
---@field CompareOp vireo.CompareOp Depth/stencil/sampler comparison operator constants.
---@field StencilOp vireo.StencilOp Stencil write operation constants.
---@field BlendFactor vireo.BlendFactor Color blend factor constants.
---@field BlendOp vireo.BlendOp Color blend arithmetic operation constants.
---@field LogicOp vireo.LogicOp Bitwise logical operation constants for color attachments.
---@field ColorWriteMask vireo.ColorWriteMask Per-channel color write enable bitmask constants.
---@field ShaderStage vireo.ShaderStage Shader stage bitmask constants.
---@field WaitStage vireo.WaitStage Pipeline stage constants for semaphore and barrier scopes.
---@field ResourceState vireo.ResourceState Resource state constants for pipeline barriers.
---@field MSAA vireo.MSAA Multisampling sample-count constants.
---@field PresentMode vireo.PresentMode Swap-chain presentation mode constants.
---@field PipelineType vireo.PipelineType Pipeline kind constants.
---@field SemaphoreType vireo.SemaphoreType GPU semaphore type constants.
---@field ColorBlendDesc vireo.ColorBlendDesc Color blend state descriptor type.
---@field StencilOpState vireo.StencilOpState Per-face stencil operation state type.
---@field PhysicalDeviceDesc vireo.PhysicalDeviceDesc GPU device description type.
---@field Extent vireo.Extent 2-D integer dimensions type.
---@field Rect vireo.Rect 2-D integer rectangle type.
---@field Viewport vireo.Viewport Floating-point viewport with depth range type.
---@field PushConstantsDesc vireo.PushConstantsDesc Push-constant range descriptor type.
---@field VertexAttributeDesc vireo.VertexAttributeDesc Vertex attribute descriptor type.
---@field DepthClearValue vireo.DepthClearValue Depth/stencil clear value type.
---@field ClearValue vireo.ClearValue Attachment clear value union type.
---@field VideoMemoryAllocationDesc vireo.VideoMemoryAllocationDesc GPU memory allocation descriptor type.
---@field RenderTargetDesc vireo.RenderTargetDesc Render-pass attachment descriptor type.
---@field RenderingConfiguration vireo.RenderingConfiguration Full render-pass configuration type.
---@field DrawIndirectCommand vireo.DrawIndirectCommand Indirect draw command structure type.
---@field DrawIndexedIndirectCommand vireo.DrawIndexedIndirectCommand Indirect indexed draw command structure type.
---@field BufferCopyRegion vireo.BufferCopyRegion Buffer-to-buffer copy region descriptor type.
---@field GraphicPipelineConfiguration vireo.GraphicPipelineConfiguration Full graphics pipeline configuration type.
---@field Fence vireo.Fence CPU/GPU synchronization fence type.
---@field Semaphore vireo.Semaphore GPU synchronization semaphore type.
---@field Instance vireo.Instance Underlying API instance type.
---@field PhysicalDevice vireo.PhysicalDevice Physical GPU representation type.
---@field Device vireo.Device Logical GPU device type.
---@field Buffer vireo.Buffer GPU buffer type.
---@field Sampler vireo.Sampler Immutable texture sampler type.
---@field Image vireo.Image GPU image (texture) type.
---@field RenderTarget vireo.RenderTarget Render target image wrapper type.
---@field DescriptorLayout vireo.DescriptorLayout Descriptor set layout type.
---@field DescriptorSet vireo.DescriptorSet Bound resource descriptor set type.
---@field VertexInputLayout vireo.VertexInputLayout Vertex attribute layout type.
---@field ShaderModule vireo.ShaderModule Compiled shader binary type.
---@field PipelineResources vireo.PipelineResources Pipeline layout type.
---@field Pipeline vireo.Pipeline Base compiled pipeline type.
---@field ComputePipeline vireo.ComputePipeline Compiled compute pipeline type.
---@field GraphicPipeline vireo.GraphicPipeline Compiled graphics pipeline type.
---@field SwapChain vireo.SwapChain Presentation swap chain type.
---@field CommandList vireo.CommandList GPU command recording list type.
---@field CommandAllocator vireo.CommandAllocator Command list pool type.
---@field SubmitQueue vireo.SubmitQueue GPU command submission queue type.
---@field Vireo vireo.Vireo Main RHI entry point type.

---@type vireo
vireo = {}