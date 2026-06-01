/*
* Copyright (c) 2025-present Henri Michelon
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/
module;
extern "C"
{
    #define LUA_LIB
    #include "lua.h"
}
module vireo.lua;

import std;
import vireo;

namespace vireo {

    void LuaBindings::_register(lua_State* L) {
        if (registered) return;
        registered = true;
        luabridge::getGlobalNamespace(L).beginNamespace("vireo")

        // enums
        .beginNamespace("Backend")
            .addVariable("UNDEFINED", Backend::UNDEFINED)
            .addVariable("DIRECTX",   Backend::DIRECTX)
            .addVariable("VULKAN",    Backend::VULKAN)
        .endNamespace()
        .beginNamespace("Filter")
            .addVariable("NEAREST", Filter::NEAREST)
            .addVariable("LINEAR",  Filter::LINEAR)
        .endNamespace()
        .beginNamespace("AddressMode")
            .addVariable("REPEAT",          AddressMode::REPEAT)
            .addVariable("MIRRORED_REPEAT", AddressMode::MIRRORED_REPEAT)
            .addVariable("CLAMP_TO_EDGE",   AddressMode::CLAMP_TO_EDGE)
            .addVariable("CLAMP_TO_BORDER", AddressMode::CLAMP_TO_BORDER)
        .endNamespace()
        .beginNamespace("FilterMode")
            .addVariable("NEAREST", FilterMode::NEAREST)
            .addVariable("LINEAR",  FilterMode::LINEAR)
        .endNamespace()
        .beginNamespace("ImageFormat")
            .addVariable("R8_UNORM",               ImageFormat::R8_UNORM)
            .addVariable("R8_SNORM",               ImageFormat::R8_SNORM)
            .addVariable("R8_UINT",                ImageFormat::R8_UINT)
            .addVariable("R8_SINT",                ImageFormat::R8_SINT)
            .addVariable("R8G8_UNORM",             ImageFormat::R8G8_UNORM)
            .addVariable("R8G8_SNORM",             ImageFormat::R8G8_SNORM)
            .addVariable("R8G8_UINT",              ImageFormat::R8G8_UINT)
            .addVariable("R8G8_SINT",              ImageFormat::R8G8_SINT)
            .addVariable("R8G8B8A8_UNORM",         ImageFormat::R8G8B8A8_UNORM)
            .addVariable("R8G8B8A8_SNORM",         ImageFormat::R8G8B8A8_SNORM)
            .addVariable("R8G8B8A8_UINT",          ImageFormat::R8G8B8A8_UINT)
            .addVariable("R8G8B8A8_SINT",          ImageFormat::R8G8B8A8_SINT)
            .addVariable("R8G8B8A8_SRGB",          ImageFormat::R8G8B8A8_SRGB)
            .addVariable("B8G8R8A8_UNORM",         ImageFormat::B8G8R8A8_UNORM)
            .addVariable("B8G8R8A8_SRGB",          ImageFormat::B8G8R8A8_SRGB)
            .addVariable("B8G8R8X8_UNORM",         ImageFormat::B8G8R8X8_UNORM)
            .addVariable("B8G8R8X8_SRGB",          ImageFormat::B8G8R8X8_SRGB)
            .addVariable("A2B10G10R10_UNORM",      ImageFormat::A2B10G10R10_UNORM)
            .addVariable("A2B10G10R10_UINT",       ImageFormat::A2B10G10R10_UINT)
            .addVariable("R16_UNORM",              ImageFormat::R16_UNORM)
            .addVariable("R16_SNORM",              ImageFormat::R16_SNORM)
            .addVariable("R16_UINT",               ImageFormat::R16_UINT)
            .addVariable("R16_SINT",               ImageFormat::R16_SINT)
            .addVariable("R16_SFLOAT",             ImageFormat::R16_SFLOAT)
            .addVariable("R16G16_UNORM",           ImageFormat::R16G16_UNORM)
            .addVariable("R16G16_SNORM",           ImageFormat::R16G16_SNORM)
            .addVariable("R16G16_UINT",            ImageFormat::R16G16_UINT)
            .addVariable("R16G16_SINT",            ImageFormat::R16G16_SINT)
            .addVariable("R16G16_SFLOAT",          ImageFormat::R16G16_SFLOAT)
            .addVariable("R16G16B16A16_UNORM",     ImageFormat::R16G16B16A16_UNORM)
            .addVariable("R16G16B16A16_SNORM",     ImageFormat::R16G16B16A16_SNORM)
            .addVariable("R16G16B16A16_UINT",      ImageFormat::R16G16B16A16_UINT)
            .addVariable("R16G16B16A16_SINT",      ImageFormat::R16G16B16A16_SINT)
            .addVariable("R16G16B16A16_SFLOAT",    ImageFormat::R16G16B16A16_SFLOAT)
            .addVariable("R32_UINT",               ImageFormat::R32_UINT)
            .addVariable("R32_SINT",               ImageFormat::R32_SINT)
            .addVariable("R32_SFLOAT",             ImageFormat::R32_SFLOAT)
            .addVariable("R32G32_UINT",            ImageFormat::R32G32_UINT)
            .addVariable("R32G32_SINT",            ImageFormat::R32G32_SINT)
            .addVariable("R32G32_SFLOAT",          ImageFormat::R32G32_SFLOAT)
            .addVariable("R32G32B32_UINT",         ImageFormat::R32G32B32_UINT)
            .addVariable("R32G32B32_SINT",         ImageFormat::R32G32B32_SINT)
            .addVariable("R32G32B32_SFLOAT",       ImageFormat::R32G32B32_SFLOAT)
            .addVariable("R32G32B32A32_UINT",      ImageFormat::R32G32B32A32_UINT)
            .addVariable("R32G32B32A32_SINT",      ImageFormat::R32G32B32A32_SINT)
            .addVariable("R32G32B32A32_SFLOAT",    ImageFormat::R32G32B32A32_SFLOAT)
            .addVariable("D16_UNORM",              ImageFormat::D16_UNORM)
            .addVariable("D24_UNORM_S8_UINT",      ImageFormat::D24_UNORM_S8_UINT)
            .addVariable("D32_SFLOAT",             ImageFormat::D32_SFLOAT)
            .addVariable("D32_SFLOAT_S8_UINT",     ImageFormat::D32_SFLOAT_S8_UINT)
            .addVariable("BC1_UNORM",              ImageFormat::BC1_UNORM)
            .addVariable("BC1_UNORM_SRGB",         ImageFormat::BC1_UNORM_SRGB)
            .addVariable("BC2_UNORM",              ImageFormat::BC2_UNORM)
            .addVariable("BC2_UNORM_SRGB",         ImageFormat::BC2_UNORM_SRGB)
            .addVariable("BC3_UNORM",              ImageFormat::BC3_UNORM)
            .addVariable("BC3_UNORM_SRGB",         ImageFormat::BC3_UNORM_SRGB)
            .addVariable("BC4_UNORM",              ImageFormat::BC4_UNORM)
            .addVariable("BC4_SNORM",              ImageFormat::BC4_SNORM)
            .addVariable("BC5_UNORM",              ImageFormat::BC5_UNORM)
            .addVariable("BC5_SNORM",              ImageFormat::BC5_SNORM)
            .addVariable("BC6H_UFLOAT",            ImageFormat::BC6H_UFLOAT)
            .addVariable("BC6H_SFLOAT",            ImageFormat::BC6H_SFLOAT)
            .addVariable("BC7_UNORM",              ImageFormat::BC7_UNORM)
            .addVariable("BC7_UNORM_SRGB",         ImageFormat::BC7_UNORM_SRGB)
        .endNamespace()
        .beginNamespace("BufferType")
            .addVariable("VERTEX",            BufferType::VERTEX)
            .addVariable("INDEX",             BufferType::INDEX)
            .addVariable("INDIRECT",          BufferType::INDIRECT)
            .addVariable("UNIFORM",           BufferType::UNIFORM)
            .addVariable("STORAGE",           BufferType::STORAGE)
            .addVariable("DEVICE_STORAGE",    BufferType::DEVICE_STORAGE)
            .addVariable("READWRITE_STORAGE", BufferType::READWRITE_STORAGE)
            .addVariable("BUFFER_UPLOAD",     BufferType::BUFFER_UPLOAD)
            .addVariable("BUFFER_DOWNLOAD",   BufferType::BUFFER_DOWNLOAD)
            .addVariable("IMAGE_UPLOAD",      BufferType::IMAGE_UPLOAD)
            .addVariable("IMAGE_DOWNLOAD",    BufferType::IMAGE_DOWNLOAD)
        .endNamespace()
        .beginNamespace("IndexType")
            .addVariable("UINT16", IndexType::UINT16)
            .addVariable("UINT32", IndexType::UINT32)
        .endNamespace()
        .beginNamespace("DescriptorType")
            .addVariable("UNIFORM",           DescriptorType::UNIFORM)
            .addVariable("UNIFORM_DYNAMIC",   DescriptorType::UNIFORM_DYNAMIC)
            .addVariable("STORAGE",           DescriptorType::STORAGE)
            .addVariable("DEVICE_STORAGE",    DescriptorType::DEVICE_STORAGE)
            .addVariable("READWRITE_STORAGE", DescriptorType::READWRITE_STORAGE)
            .addVariable("SAMPLED_IMAGE",     DescriptorType::SAMPLED_IMAGE)
            .addVariable("SAMPLER",           DescriptorType::SAMPLER)
            .addVariable("READWRITE_IMAGE",   DescriptorType::READWRITE_IMAGE)
        .endNamespace()
        .beginNamespace("CommandType")
            .addVariable("GRAPHIC",  CommandType::GRAPHIC)
            .addVariable("TRANSFER", CommandType::TRANSFER)
            .addVariable("COMPUTE",  CommandType::COMPUTE)
        .endNamespace()
        .beginNamespace("AttributeFormat")
            .addVariable("R32_FLOAT",          AttributeFormat::R32_FLOAT)
            .addVariable("R32G32_FLOAT",       AttributeFormat::R32G32_FLOAT)
            .addVariable("R32G32B32_FLOAT",    AttributeFormat::R32G32B32_FLOAT)
            .addVariable("R32G32B32A32_FLOAT", AttributeFormat::R32G32B32A32_FLOAT)
            .addVariable("R32_SINT",           AttributeFormat::R32_SINT)
            .addVariable("R32G32_SINT",        AttributeFormat::R32G32_SINT)
            .addVariable("R32G32B32_SINT",     AttributeFormat::R32G32B32_SINT)
            .addVariable("R32G32B32A32_SINT",  AttributeFormat::R32G32B32A32_SINT)
            .addVariable("R32_UINT",           AttributeFormat::R32_UINT)
            .addVariable("R32G32_UINT",        AttributeFormat::R32G32_UINT)
            .addVariable("R32G32B32_UINT",     AttributeFormat::R32G32B32_UINT)
            .addVariable("R32G32B32A32_UINT",  AttributeFormat::R32G32B32A32_UINT)
        .endNamespace()
        .beginNamespace("RenderTargetType")
            .addVariable("COLOR",         RenderTargetType::COLOR)
            .addVariable("DEPTH",         RenderTargetType::DEPTH)
            .addVariable("DEPTH_STENCIL", RenderTargetType::DEPTH_STENCIL)
        .endNamespace()
        .beginNamespace("CullMode")
            .addVariable("NONE",  CullMode::NONE)
            .addVariable("FRONT", CullMode::FRONT)
            .addVariable("BACK",  CullMode::BACK)
        .endNamespace()
        .beginNamespace("PrimitiveTopology")
            .addVariable("POINT_LIST",     PrimitiveTopology::POINT_LIST)
            .addVariable("LINE_LIST",      PrimitiveTopology::LINE_LIST)
            .addVariable("LINE_STRIP",     PrimitiveTopology::LINE_STRIP)
            .addVariable("TRIANGLE_LIST",  PrimitiveTopology::TRIANGLE_LIST)
            .addVariable("TRIANGLE_STRIP", PrimitiveTopology::TRIANGLE_STRIP)
        .endNamespace()
        .beginNamespace("PolygonMode")
            .addVariable("FILL",      PolygonMode::FILL)
            .addVariable("WIREFRAME", PolygonMode::WIREFRAME)
        .endNamespace()
        .beginNamespace("CompareOp")
            .addVariable("NEVER",            CompareOp::NEVER)
            .addVariable("LESS",             CompareOp::LESS)
            .addVariable("EQUAL",            CompareOp::EQUAL)
            .addVariable("LESS_OR_EQUAL",    CompareOp::LESS_OR_EQUAL)
            .addVariable("GREATER",          CompareOp::GREATER)
            .addVariable("NOT_EQUAL",        CompareOp::NOT_EQUAL)
            .addVariable("GREATER_OR_EQUAL", CompareOp::GREATER_OR_EQUAL)
            .addVariable("ALWAYS",           CompareOp::ALWAYS)
        .endNamespace()
        .beginNamespace("StencilOp")
            .addVariable("KEEP",                 StencilOp::KEEP)
            .addVariable("ZERO",                 StencilOp::ZERO)
            .addVariable("REPLACE",              StencilOp::REPLACE)
            .addVariable("INCREMENT_AND_CLAMP",  StencilOp::INCREMENT_AND_CLAMP)
            .addVariable("DECREMENT_AND_CLAMP",  StencilOp::DECREMENT_AND_CLAMP)
            .addVariable("INVERT",               StencilOp::INVERT)
            .addVariable("INCREMENT_AND_WRAP",   StencilOp::INCREMENT_AND_WRAP)
            .addVariable("DECREMENT_AND_WRAP",   StencilOp::DECREMENT_AND_WRAP)
        .endNamespace()
        .beginNamespace("BlendFactor")
            .addVariable("ZERO",                     BlendFactor::ZERO)
            .addVariable("ONE",                      BlendFactor::ONE)
            .addVariable("SRC_COLOR",                BlendFactor::SRC_COLOR)
            .addVariable("ONE_MINUS_SRC_COLOR",      BlendFactor::ONE_MINUS_SRC_COLOR)
            .addVariable("DST_COLOR",                BlendFactor::DST_COLOR)
            .addVariable("ONE_MINUS_DST_COLOR",      BlendFactor::ONE_MINUS_DST_COLOR)
            .addVariable("SRC_ALPHA",                BlendFactor::SRC_ALPHA)
            .addVariable("ONE_MINUS_SRC_ALPHA",      BlendFactor::ONE_MINUS_SRC_ALPHA)
            .addVariable("DST_ALPHA",                BlendFactor::DST_ALPHA)
            .addVariable("ONE_MINUS_DST_ALPHA",      BlendFactor::ONE_MINUS_DST_ALPHA)
            .addVariable("CONSTANT_COLOR",           BlendFactor::CONSTANT_COLOR)
            .addVariable("ONE_MINUS_CONSTANT_COLOR", BlendFactor::ONE_MINUS_CONSTANT_COLOR)
            .addVariable("CONSTANT_ALPHA",           BlendFactor::CONSTANT_ALPHA)
            .addVariable("ONE_MINUS_CONSTANT_ALPHA", BlendFactor::ONE_MINUS_CONSTANT_ALPHA)
            .addVariable("SRC_ALPHA_SATURATE",       BlendFactor::SRC_ALPHA_SATURATE)
            .addVariable("SRC1_COLOR",               BlendFactor::SRC1_COLOR)
            .addVariable("ONE_MINUS_SRC1_COLOR",     BlendFactor::ONE_MINUS_SRC1_COLOR)
            .addVariable("SRC1_ALPHA",               BlendFactor::SRC1_ALPHA)
            .addVariable("ONE_MINUS_SRC1_ALPHA",     BlendFactor::ONE_MINUS_SRC1_ALPHA)
        .endNamespace()
        .beginNamespace("BlendOp")
            .addVariable("ADD",              BlendOp::ADD)
            .addVariable("SUBTRACT",         BlendOp::SUBTRACT)
            .addVariable("REVERSE_SUBTRACT", BlendOp::REVERSE_SUBTRACT)
            .addVariable("MIN",              BlendOp::MIN)
            .addVariable("MAX",              BlendOp::MAX)
        .endNamespace()
        .beginNamespace("LogicOp")
            .addVariable("CLEAR",         LogicOp::CLEAR)
            .addVariable("SET",           LogicOp::SET)
            .addVariable("COPY",          LogicOp::COPY)
            .addVariable("COPY_INVERTED", LogicOp::COPY_INVERTED)
            .addVariable("NOOP",          LogicOp::NOOP)
            .addVariable("INVERT",        LogicOp::INVERT)
            .addVariable("AND",           LogicOp::AND)
            .addVariable("NAND",          LogicOp::NAND)
            .addVariable("OR",            LogicOp::OR)
            .addVariable("NOR",           LogicOp::NOR)
            .addVariable("XOR",           LogicOp::XOR)
            .addVariable("EQUIV",         LogicOp::EQUIV)
            .addVariable("AND_REVERSE",   LogicOp::AND_REVERSE)
            .addVariable("AND_INVERTED",  LogicOp::AND_INVERTED)
            .addVariable("OR_REVERSE",    LogicOp::OR_REVERSE)
            .addVariable("OR_INVERTED",   LogicOp::OR_INVERTED)
        .endNamespace()
        .beginNamespace("ColorWriteMask")
            .addVariable("RED",   ColorWriteMask::RED)
            .addVariable("GREEN", ColorWriteMask::GREEN)
            .addVariable("BLUE",  ColorWriteMask::BLUE)
            .addVariable("ALPHA", ColorWriteMask::ALPHA)
            .addVariable("ALL",   ColorWriteMask::ALL)
        .endNamespace()
        .beginNamespace("ShaderStage")
            .addVariable("ALL",      ShaderStage::ALL)
            .addVariable("VERTEX",   ShaderStage::VERTEX)
            .addVariable("FRAGMENT", ShaderStage::FRAGMENT)
            .addVariable("HULL",     ShaderStage::HULL)
            .addVariable("DOMAIN",   ShaderStage::DOMAIN)
            .addVariable("GEOMETRY", ShaderStage::GEOMETRY)
            .addVariable("COMPUTE",  ShaderStage::COMPUTE)
        .endNamespace()
        .beginNamespace("WaitStage")
            .addVariable("NONE",                                      WaitStage::NONE)
            .addVariable("PIPELINE_TOP",                              WaitStage::PIPELINE_TOP)
            .addVariable("VERTEX_INPUT",                              WaitStage::VERTEX_INPUT)
            .addVariable("VERTEX_SHADER",                             WaitStage::VERTEX_SHADER)
            .addVariable("DEPTH_STENCIL_TEST_BEFORE_FRAGMENT_SHADER", WaitStage::DEPTH_STENCIL_TEST_BEFORE_FRAGMENT_SHADER)
            .addVariable("FRAGMENT_SHADER",                           WaitStage::FRAGMENT_SHADER)
            .addVariable("DEPTH_STENCIL_TEST_AFTER_FRAGMENT_SHADER",  WaitStage::DEPTH_STENCIL_TEST_AFTER_FRAGMENT_SHADER)
            .addVariable("COLOR_OUTPUT",                              WaitStage::COLOR_OUTPUT)
            .addVariable("COMPUTE_SHADER",                            WaitStage::COMPUTE_SHADER)
            .addVariable("TRANSFER",                                  WaitStage::TRANSFER)
            .addVariable("PIPELINE_BOTTOM",                           WaitStage::PIPELINE_BOTTOM)
            .addVariable("ALL_GRAPHICS",                              WaitStage::ALL_GRAPHICS)
            .addVariable("ALL_COMMANDS",                              WaitStage::ALL_COMMANDS)
            .addVariable("COPY",                                      WaitStage::COPY)
            .addVariable("RESOLV",                                    WaitStage::RESOLV)
            .addVariable("BLIT",                                      WaitStage::BLIT)
            .addVariable("CLEAR",                                     WaitStage::CLEAR)
        .endNamespace()
        .beginNamespace("ResourceState")
            .addVariable("UNDEFINED",                        ResourceState::UNDEFINED)
            .addVariable("RENDER_TARGET_COLOR",              ResourceState::RENDER_TARGET_COLOR)
            .addVariable("RENDER_TARGET_DEPTH",              ResourceState::RENDER_TARGET_DEPTH)
            .addVariable("RENDER_TARGET_DEPTH_READ",         ResourceState::RENDER_TARGET_DEPTH_READ)
            .addVariable("RENDER_TARGET_DEPTH_STENCIL",      ResourceState::RENDER_TARGET_DEPTH_STENCIL)
            .addVariable("RENDER_TARGET_DEPTH_STENCIL_READ", ResourceState::RENDER_TARGET_DEPTH_STENCIL_READ)
            .addVariable("PRESENT",                          ResourceState::PRESENT)
            .addVariable("COPY_SRC",                         ResourceState::COPY_SRC)
            .addVariable("COPY_DST",                         ResourceState::COPY_DST)
            .addVariable("SHADER_READ",                      ResourceState::SHADER_READ)
            .addVariable("COMPUTE_READ",                     ResourceState::COMPUTE_READ)
            .addVariable("COMPUTE_WRITE",                    ResourceState::COMPUTE_WRITE)
            .addVariable("INDIRECT_DRAW",                    ResourceState::INDIRECT_DRAW)
            .addVariable("VERTEX_INPUT",                     ResourceState::VERTEX_INPUT)
            .addVariable("UNIFORM",                          ResourceState::UNIFORM)
        .endNamespace()
        .beginNamespace("MSAA")
            .addVariable("NONE", MSAA::NONE)
            .addVariable("X2",   MSAA::X2)
            .addVariable("X4",   MSAA::X4)
            .addVariable("X8",   MSAA::X8)
            .addVariable("X16",  MSAA::X16)
            .addVariable("X32",  MSAA::X32)
            .addVariable("X64",  MSAA::X64)
        .endNamespace()
        .beginNamespace("PresentMode")
            .addVariable("IMMEDIATE", PresentMode::IMMEDIATE)
            .addVariable("VSYNC",     PresentMode::VSYNC)
        .endNamespace()
        .beginNamespace("PipelineType")
            .addVariable("GRAPHIC", PipelineType::GRAPHIC)
            .addVariable("COMPUTE", PipelineType::COMPUTE)
        .endNamespace()
        .beginNamespace("SemaphoreType")
            .addVariable("BINARY",   SemaphoreType::BINARY)
            .addVariable("TIMELINE", SemaphoreType::TIMELINE)
        .endNamespace()

        // structs
        .beginClass<ColorBlendDesc>("ColorBlendDesc")
            .addConstructor<void(*)()>()
            .addProperty("blend_enable",             &ColorBlendDesc::blendEnable)
            .addProperty("src_color_blend_factor",   &ColorBlendDesc::srcColorBlendFactor)
            .addProperty("dst_color_blend_factor",   &ColorBlendDesc::dstColorBlendFactor)
            .addProperty("color_blend_op",           &ColorBlendDesc::colorBlendOp)
            .addProperty("src_alpha_blend_factor",   &ColorBlendDesc::srcAlphaBlendFactor)
            .addProperty("dst_alpha_blend_factor",   &ColorBlendDesc::dstAlphaBlendFactor)
            .addProperty("alpha_blend_op",           &ColorBlendDesc::alphaBlendOp)
            .addProperty("color_write_mask",         &ColorBlendDesc::colorWriteMask)
        .endClass()
        .beginClass<StencilOpState>("StencilOpState")
            .addConstructor<void(*)()>()
            .addProperty("fail_op",       &StencilOpState::failOp)
            .addProperty("pass_op",       &StencilOpState::passOp)
            .addProperty("depth_fail_op", &StencilOpState::depthFailOp)
            .addProperty("compare_op",    &StencilOpState::compareOp)
            .addProperty("compare_mask",  &StencilOpState::compareMask)
            .addProperty("write_mask",    &StencilOpState::writeMask)
        .endClass()
        .beginClass<PhysicalDeviceDesc>("PhysicalDeviceDesc")
            .addConstructor<void(*)()>()
            .addProperty("name",                    &PhysicalDeviceDesc::name)
            .addProperty("dedicated_video_memory",  &PhysicalDeviceDesc::dedicatedVideoMemory)
            .addProperty("dedicated_system_memory", &PhysicalDeviceDesc::dedicatedSystemMemory)
            .addProperty("shared_system_memory",    &PhysicalDeviceDesc::sharedSystemMemory)
        .endClass()
        .beginClass<Extent>("Extent")
            .addConstructor<void(*)()>()
            .addProperty("width",  &Extent::width)
            .addProperty("height", &Extent::height)
        .endClass()
        .beginClass<Rect>("Rect")
            .addConstructor<void(uint32_t, uint32_t), void(int32_t, int32_t, uint32_t, uint32_t)>()
            .addProperty("x",      &Rect::x)
            .addProperty("y",      &Rect::y)
            .addProperty("width",  &Rect::width)
            .addProperty("height", &Rect::height)
        .endClass()
        .beginClass<Viewport>("Viewport")
            .addConstructor<void(float, float), void(float, float, float, float)>()
            .addProperty("x",         &Viewport::x)
            .addProperty("y",         &Viewport::y)
            .addProperty("width",     &Viewport::width)
            .addProperty("height",    &Viewport::height)
            .addProperty("min_depth", &Viewport::minDepth)
            .addProperty("max_depth", &Viewport::maxDepth)
        .endClass()
        .beginClass<PushConstantsDesc>("PushConstantsDesc")
            .addConstructor<void(*)()>()
            .addProperty("stage",  &PushConstantsDesc::stage)
            .addProperty("size",   &PushConstantsDesc::size)
            .addProperty("offset", &PushConstantsDesc::offset)
        .endClass()
        .beginClass<VertexAttributeDesc>("VertexAttributeDesc")
            .addConstructor<void(*)()>()
            .addProperty("binding", &VertexAttributeDesc::binding)
            .addProperty("format",  &VertexAttributeDesc::format)
            .addProperty("offset",  &VertexAttributeDesc::offset)
        .endClass()
        .beginClass<DepthClearValue>("DepthClearValue")
            .addConstructor<void(*)()>()
            .addProperty("depth",   &DepthClearValue::depth)
            .addProperty("stencil", &DepthClearValue::stencil)
        .endClass()
        .beginClass<ClearValue>("ClearValue")
            .addConstructor<void(*)()>()
            .addProperty("r",
                +[](const ClearValue* v) { return v->color[0]; },
                +[](ClearValue* v, float f) { v->color[0] = f; })
            .addProperty("g",
                +[](const ClearValue* v) { return v->color[1]; },
                +[](ClearValue* v, float f) { v->color[1] = f; })
            .addProperty("b",
                +[](const ClearValue* v) { return v->color[2]; },
                +[](ClearValue* v, float f) { v->color[2] = f; })
            .addProperty("a",
                +[](const ClearValue* v) { return v->color[3]; },
                +[](ClearValue* v, float f) { v->color[3] = f; })
            .addProperty("depth",
                +[](const ClearValue* v) { return v->depthStencil.depth; },
                +[](ClearValue* v, float f) { v->depthStencil.depth = f; })
            .addProperty("stencil",
                +[](const ClearValue* v) { return v->depthStencil.stencil; },
                +[](ClearValue* v, std::uint32_t s) { v->depthStencil.stencil = s; })
        .endClass()
        .beginClass<VideoMemoryAllocationDesc>("VideoMemoryAllocationDesc")
            .addConstructor<void(*)()>()
            .addProperty("usage", &VideoMemoryAllocationDesc::usage)
            .addProperty("name",  &VideoMemoryAllocationDesc::name)
            .addProperty("size",  &VideoMemoryAllocationDesc::size)
            .addProperty("ref",   &VideoMemoryAllocationDesc::ref)
        .endClass()
        .beginClass<RenderTargetDesc>("RenderTargetDesc")
            .addConstructor<void(*)()>()
            .addProperty("swap_chain",                    &RenderTargetDesc::swapChain)
            .addProperty("render_target",                 &RenderTargetDesc::renderTarget)
            .addProperty("multisampled_render_target",    &RenderTargetDesc::multisampledRenderTarget)
            .addProperty("clear",                         &RenderTargetDesc::clear)
            .addProperty("clear_value",                   &RenderTargetDesc::clearValue)
            .addProperty("discard_after_render",          &RenderTargetDesc::discardAfterRender)
        .endClass()
        .beginClass<RenderingConfiguration>("RenderingConfiguration")
            .addConstructor<void(*)()>()
            .addProperty("color_render_targets",                      &RenderingConfiguration::colorRenderTargets)
            .addProperty("depth_stencil_render_target",               &RenderingConfiguration::depthStencilRenderTarget)
            .addProperty("multisampled_depth_stencil_render_target",  &RenderingConfiguration::multisampledDepthStencilRenderTarget)
            .addProperty("use_depth_attachment",                      &RenderingConfiguration::useDepthAttachment)
            .addProperty("use_stencil_attachment",                    &RenderingConfiguration::useStencilAttachment)
            .addProperty("clear_depth_stencil",                       &RenderingConfiguration::clearDepthStencil)
            .addProperty("depth_stencil_clear_value",                 &RenderingConfiguration::depthStencilClearValue)
            .addProperty("discard_depth_stencil_after_render",        &RenderingConfiguration::discardDepthStencilAfterRender)
        .endClass()
        .beginClass<DrawIndirectCommand>("DrawIndirectCommand")
            .addConstructor<void(*)()>()
            .addProperty("vertex_count",    &DrawIndirectCommand::vertexCount)
            .addProperty("instance_count",  &DrawIndirectCommand::instanceCount)
            .addProperty("first_vertex",    &DrawIndirectCommand::firstVertex)
            .addProperty("first_instance",  &DrawIndirectCommand::firstInstance)
        .endClass()
        .beginClass<DrawIndexedIndirectCommand>("DrawIndexedIndirectCommand")
            .addConstructor<void(*)()>()
            .addProperty("index_count",     &DrawIndexedIndirectCommand::indexCount)
            .addProperty("instance_count",  &DrawIndexedIndirectCommand::instanceCount)
            .addProperty("first_index",     &DrawIndexedIndirectCommand::firstIndex)
            .addProperty("vertex_offset",   &DrawIndexedIndirectCommand::vertexOffset)
            .addProperty("first_instance",  &DrawIndexedIndirectCommand::firstInstance)
        .endClass()
        .beginClass<BufferCopyRegion>("BufferCopyRegion")
            .addConstructor<void(*)()>()
            .addProperty("src_offset", &BufferCopyRegion::srcOffset)
            .addProperty("dst_offset", &BufferCopyRegion::dstOffset)
            .addProperty("size",       &BufferCopyRegion::size)
        .endClass()
        .beginClass<GraphicPipelineConfiguration>("GraphicPipelineConfiguration")
            .addConstructor<void(*)()>()
            .addProperty("resources",                     &GraphicPipelineConfiguration::resources)
            .addProperty("color_render_formats",          &GraphicPipelineConfiguration::colorRenderFormats)
            .addProperty("color_blend_desc",              &GraphicPipelineConfiguration::colorBlendDesc)
            .addProperty("vertex_input_layout",           &GraphicPipelineConfiguration::vertexInputLayout)
            .addProperty("vertex_shader",                 &GraphicPipelineConfiguration::vertexShader)
            .addProperty("fragment_shader",               &GraphicPipelineConfiguration::fragmentShader)
            .addProperty("hull_shader",                   &GraphicPipelineConfiguration::hullShader)
            .addProperty("domain_shader",                 &GraphicPipelineConfiguration::domainShader)
            .addProperty("geometry_shader",               &GraphicPipelineConfiguration::geometryShader)
            .addProperty("primitive_topology",            &GraphicPipelineConfiguration::primitiveTopology)
            .addProperty("msaa",                          &GraphicPipelineConfiguration::msaa)
            .addProperty("cull_mode",                     &GraphicPipelineConfiguration::cullMode)
            .addProperty("polygon_mode",                  &GraphicPipelineConfiguration::polygonMode)
            .addProperty("front_face_counter_clockwise",  &GraphicPipelineConfiguration::frontFaceCounterClockwise)
            .addProperty("depth_stencil_image_format",    &GraphicPipelineConfiguration::depthStencilImageFormat)
            .addProperty("depth_test_enable",             &GraphicPipelineConfiguration::depthTestEnable)
            .addProperty("depth_write_enable",            &GraphicPipelineConfiguration::depthWriteEnable)
            .addProperty("depth_compare_op",              &GraphicPipelineConfiguration::depthCompareOp)
            .addProperty("depth_bias_enable",             &GraphicPipelineConfiguration::depthBiasEnable)
            .addProperty("depth_bias_constant_factor",    &GraphicPipelineConfiguration::depthBiasConstantFactor)
            .addProperty("depth_bias_clamp",              &GraphicPipelineConfiguration::depthBiasClamp)
            .addProperty("depth_bias_slope_factor",       &GraphicPipelineConfiguration::depthBiasSlopeFactor)
            .addProperty("stencil_test_enable",           &GraphicPipelineConfiguration::stencilTestEnable)
            .addProperty("front_stencil_op_state",        &GraphicPipelineConfiguration::frontStencilOpState)
            .addProperty("back_stencil_op_state",         &GraphicPipelineConfiguration::backStencilOpState)
            .addProperty("logic_op_enable",               &GraphicPipelineConfiguration::logicOpEnable)
            .addProperty("logic_op",                      &GraphicPipelineConfiguration::logicOp)
            .addProperty("alpha_to_coverage_enable",      &GraphicPipelineConfiguration::alphaToCoverageEnable)
        .endClass()

        // classes
        .beginClass<Fence>("Fence")
            .addFunction("wait",  &Fence::wait)
            .addFunction("reset", &Fence::reset)
        .endClass()
        .beginClass<Semaphore>("Semaphore")
            .addFunction("get_type",        &Semaphore::getType)
            .addFunction("get_value",       &Semaphore::getValue)
            .addFunction("set_value",       &Semaphore::setValue)
            .addFunction("increment_value", &Semaphore::incrementValue)
            .addFunction("decrement_value", &Semaphore::decrementValue)
        .endClass()
        .beginClass<Instance>("Instance")
        .endClass()
        .beginClass<PhysicalDevice>("PhysicalDevice")
            .addFunction("get_description", &PhysicalDevice::getDescription)
        .endClass()
        .beginClass<Device>("Device")
            .addFunction("have_dedicated_transfer_queue", &Device::haveDedicatedTransferQueue)
        .endClass()
        .beginClass<Buffer>("Buffer")
            .addFunction("get_size",                  &Buffer::getSize)
            .addFunction("get_type",                  &Buffer::getType)
            .addFunction("get_instance_size",         &Buffer::getInstanceSize)
            .addFunction("get_instance_size_aligned", &Buffer::getInstanceSizeAligned)
            .addFunction("get_instance_count",        &Buffer::getInstanceCount)
            .addFunction("get_mapped_address",        &Buffer::getMappedAddress)
            .addFunction("map",                       &Buffer::map)
            .addFunction("unmap",                     &Buffer::unmap)
            .addFunction("write",                     &Buffer::write)
            .addStaticFunction("get_memory_allocations", &Buffer::getMemoryAllocations)
        .endClass()
        .beginClass<Sampler>("Sampler")
        .endClass()
        .beginClass<Image>("Image")
            .addFunction("get_format",              &Image::getFormat)
            .addFunction("get_width",               &Image::getWidth)
            .addFunction("get_height",              &Image::getHeight)
            .addFunction("get_mip_levels",          &Image::getMipLevels)
            .addFunction("get_array_size",          &Image::getArraySize)
            .addFunction("get_row_pitch",           &Image::getRowPitch)
            .addFunction("get_row_length",          &Image::getRowLength)
            .addFunction("get_image_size",          &Image::getImageSize)
            .addFunction("get_aligned_image_size",  &Image::getAlignedImageSize)
            .addFunction("get_aligned_row_pitch",   &Image::getAlignedRowPitch)
            .addFunction("get_aligned_row_length",  &Image::getAlignedRowLength)
            .addFunction("is_read_write",           &Image::isReadWrite)
            .addStaticFunction("get_pixel_size",    &Image::getPixelSize)
            .addStaticFunction("get_memory_allocations", &Image::getMemoryAllocations)
        .endClass()
        .beginClass<RenderTarget>("RenderTarget")
            .addFunction("get_image", &RenderTarget::getImage)
            .addFunction("get_type",  &RenderTarget::getType)
        .endClass()
        .beginClass<DescriptorLayout>("DescriptorLayout")
            .addFunction("add",               &DescriptorLayout::add)
            .addFunction("build",             &DescriptorLayout::build)
            .addFunction("get_capacity",      &DescriptorLayout::getCapacity)
            .addFunction("is_dynamic_uniform",&DescriptorLayout::isDynamicUniform)
            .addFunction("is_samplers",       &DescriptorLayout::isSamplers)
        .endClass()
        .beginClass<DescriptorSet>("DescriptorSet")
            .addFunction("update_buffer",
                +[](DescriptorSet* self, const DescriptorIndex index, const Buffer& buffer) {
                    self->update(index, buffer);
                })
            .addFunction("update_dynamic_buffer",
                +[](DescriptorSet* self, const Buffer& buffer) {
                    self->update(static_cast<DescriptorIndex>(0), buffer);
                })
            .addFunction("update_image",
                +[](DescriptorSet* self, const DescriptorIndex index, const Image& image) {
                    self->update(index, image);
                })
            .addFunction("update_sampler",
                +[](DescriptorSet* self, const DescriptorIndex index, const Sampler& sampler) {
                    self->update(index, sampler);
                })
            .addFunction("update_image_array",
                +[](DescriptorSet* self, const DescriptorIndex index,
                    const std::vector<std::shared_ptr<Image>>& images) {
                    self->update(index, images);
                })
            .addFunction("update_buffer_array",
                +[](DescriptorSet* self, const DescriptorIndex index,
                    const std::vector<std::shared_ptr<Buffer>>& buffers) {
                    self->update(index, buffers);
                })
            .addFunction("update_sampler_array",
                +[](DescriptorSet* self, const DescriptorIndex index,
                    const std::vector<std::shared_ptr<Sampler>>& samplers) {
                    self->update(index, samplers);
                })
            .addFunction("get_layout", &DescriptorSet::getLayout)
        .endClass()
        .beginClass<VertexInputLayout>("VertexInputLayout")
        .endClass()
        .beginClass<ShaderModule>("ShaderModule")
        .endClass()
        .beginClass<PipelineResources>("PipelineResources")
        .endClass()
        .beginClass<Pipeline>("Pipeline")
            .addFunction("get_resources", &Pipeline::getResources)
            .addFunction("get_type",      &Pipeline::getType)
        .endClass()
        .deriveClass<ComputePipeline, Pipeline>("ComputePipeline")
        .endClass()
        .deriveClass<GraphicPipeline, Pipeline>("GraphicPipeline")
        .endClass()
        .beginClass<SwapChain>("SwapChain")
            .addFunction("get_extent",              &SwapChain::getExtent)
            .addFunction("get_aspect_ratio",        &SwapChain::getAspectRatio)
            .addFunction("get_current_frame_index", &SwapChain::getCurrentFrameIndex)
            .addFunction("get_frames_in_flight",    &SwapChain::getFramesInFlight)
            .addFunction("get_format",              &SwapChain::getFormat)
            .addFunction("next_frame_index",        &SwapChain::nextFrameIndex)
            .addFunction("acquire",                 &SwapChain::acquire)
            .addFunction("present",                 &SwapChain::present)
            .addFunction("recreate",                &SwapChain::recreate)
            .addFunction("wait_idle",               &SwapChain::waitIdle)
        .endClass()
        .beginClass<CommandList>("CommandList")
            .addFunction("begin", &CommandList::begin)
            .addFunction("end",   &CommandList::end)
            .addFunction("upload_buffer",
                (void (CommandList::*)(const Buffer&, const void*)) &CommandList::upload)
            .addFunction("upload_image",
                (void (CommandList::*)(const Image&, const void*, std::uint32_t)) &CommandList::upload)
            .addFunction("copy_buffer_to_image",
                (void (CommandList::*)(const Buffer&, const Image&, std::uint32_t, std::uint32_t, bool) const) &CommandList::copy)
            .addFunction("copy_buffer_to_image_levels",
                (void (CommandList::*)(const Buffer&, const Image&, const std::vector<std::size_t>&, bool) const) &CommandList::copy)
            .addFunction("copy_buffer_to_buffer",
                (void (CommandList::*)(const Buffer&, const Buffer&, std::size_t, std::uint32_t, std::uint32_t) const) &CommandList::copy)
            .addFunction("copy_buffer_regions",
                (void (CommandList::*)(const Buffer&, const Buffer&, const std::vector<BufferCopyRegion>&) const) &CommandList::copy)
            .addFunction("copy_image_to_swap_chain",
                (void (CommandList::*)(const Image&, const SwapChain&) const) &CommandList::copy)
            .addFunction("begin_rendering",  &CommandList::beginRendering)
            .addFunction("end_rendering",    &CommandList::endRendering)
            .addFunction("dispatch",         &CommandList::dispatch)
            .addFunction("bind_pipeline",
                +[](CommandList* self, Pipeline& pipeline, const bool descriptorsAlreadyBound = false) {
                    self->bindPipeline(pipeline, descriptorsAlreadyBound);
                })
            .addFunction("bind_vertex_buffer",
                (void (CommandList::*)(const Buffer&, std::size_t) const) &CommandList::bindVertexBuffer)
            .addFunction("bind_vertex_buffers", &CommandList::bindVertexBuffers)
            .addFunction("bind_index_buffer",
                +[](const CommandList* self, const Buffer& buffer,
                    const IndexType indexType = IndexType::UINT32,
                    const std::uint32_t firstIndex = 0) {
                    self->bindIndexBuffer(buffer, indexType, firstIndex);
                })
            .addFunction("bind_descriptors",
                luabridge::overload<
                    PipelineType,
                    const std::shared_ptr<PipelineResources>&,
                    const std::vector<std::shared_ptr<const DescriptorSet>>&>(
                    +[](const CommandList* self,
                        const PipelineType type,
                        const std::shared_ptr<PipelineResources>& resources,
                        const std::vector<std::shared_ptr<const DescriptorSet>>& descriptors) {
                        self->bindDescriptors(type, resources, descriptors);
                    }),
                luabridge::overload<
                    const std::vector<std::shared_ptr<const DescriptorSet>>&>(
                    +[](const CommandList* self,
                        const std::vector<std::shared_ptr<const DescriptorSet>>& descriptors) {
                        self->bindDescriptors(descriptors);
                    }))
            .addFunction("bind_descriptor",
                (void (CommandList::*)(const DescriptorSet&, std::uint32_t) const) &CommandList::bindDescriptor)
            .addFunction("bind_descriptor_dynamic",
                (void (CommandList::*)(const DescriptorSet&, std::uint32_t, std::uint32_t) const) &CommandList::bindDescriptor)
            .addFunction("draw",         &CommandList::draw)
            .addFunction("draw_indexed", &CommandList::drawIndexed)
            .addFunction("draw_indirect",
                (void (CommandList::*)(const Buffer&, std::size_t, std::uint32_t, std::uint32_t, std::uint32_t)) &CommandList::drawIndirect)
            .addFunction("draw_indexed_indirect_count",
                (void (CommandList::*)(Buffer&, std::size_t, Buffer&, std::size_t, std::uint32_t, std::uint32_t, std::uint32_t)) &CommandList::drawIndexedIndirectCount)
            .addFunction("draw_indexed_indirect",
                (void (CommandList::*)(const Buffer&, std::size_t, std::uint32_t, std::uint32_t, std::uint32_t)) &CommandList::drawIndexedIndirect)
            .addFunction("barrier_image",
                +[](const CommandList* self, const std::shared_ptr<const Image>& image,
                    const ResourceState oldState, const ResourceState newState) {
                    self->barrier(image, oldState, newState);
                })
            .addFunction("barrier_render_target",
                +[](const CommandList* self, const std::shared_ptr<const RenderTarget>& rt,
                    const ResourceState oldState, const ResourceState newState) {
                    self->barrier(rt, oldState, newState);
                })
            .addFunction("barrier_swap_chain",
                +[](const CommandList* self, const std::shared_ptr<const SwapChain>& sc,
                    const ResourceState oldState, const ResourceState newState) {
                    self->barrier(sc, oldState, newState);
                })
            .addFunction("barrier_buffer",
                +[](const CommandList* self, const Buffer& buffer,
                    const ResourceState oldState, const ResourceState newState) {
                    self->barrier(buffer, oldState, newState);
                })
            .addFunction("set_viewports", &CommandList::setViewports)
            .addFunction("set_scissors",
                (void (CommandList::*)(const std::vector<Rect>&) const) &CommandList::setScissors)
            .addFunction("set_viewport",  &CommandList::setViewport)
            .addFunction("set_scissor",
                (void (CommandList::*)(const Rect&) const) &CommandList::setScissors)
            .addFunction("set_stencil_reference", &CommandList::setStencilReference)
            .addFunction("push_constants",        &CommandList::pushConstants)
            .addFunction("cleanup",               &CommandList::cleanup)
        .endClass()
        .beginClass<CommandAllocator>("CommandAllocator")
            .addFunction("reset",
                &CommandAllocator::reset)
            .addFunction("create_command_list",
                luabridge::overload<>(
                    +[](const CommandAllocator* self) {
                        return self->createCommandList();
                    }),
                luabridge::overload<const Pipeline&>(
                    +[](const CommandAllocator* self, const Pipeline& pipeline) {
                        return self->createCommandList(pipeline);
                    }))
            .addFunction("get_command_list_type", &CommandAllocator::getCommandListType)
        .endClass()
        .beginClass<SubmitQueue>("SubmitQueue")
            .addFunction("submit",
                +[](const SubmitQueue* self,
                    const std::vector<std::shared_ptr<const CommandList>>& cls) {
                    self->submit(cls);
                })
            .addFunction("submit_fence",
                +[](const SubmitQueue* self,
                    const std::shared_ptr<Fence>& fence,
                    const std::vector<std::shared_ptr<const CommandList>>& cls) {
                    self->submit(fence, cls);
                })
            .addFunction("submit_fence_swap",
                +[](const SubmitQueue* self,
                    const std::shared_ptr<Fence>& fence,
                    const std::shared_ptr<const SwapChain>& swapChain,
                    const std::vector<std::shared_ptr<const CommandList>>& cls) {
                    self->submit(fence, swapChain, cls);
                })
            .addFunction("submit_wait_fence_swap",
                +[](const SubmitQueue* self,
                    const std::shared_ptr<Semaphore>& waitSemaphore,
                    const WaitStage waitStage,
                    const std::shared_ptr<Fence>& fence,
                    const std::shared_ptr<const SwapChain>& swapChain,
                    const std::vector<std::shared_ptr<const CommandList>>& cls) {
                    self->submit(waitSemaphore, waitStage, fence, swapChain, cls);
                })
            .addFunction("submit_wait_signal",
                +[](const SubmitQueue* self,
                    const std::shared_ptr<Semaphore>& waitSemaphore,
                    const WaitStage waitStage,
                    const WaitStage signalStage,
                    const std::shared_ptr<Semaphore>& signalSemaphore,
                    const std::vector<std::shared_ptr<const CommandList>>& cls) {
                    self->submit(waitSemaphore, waitStage, signalStage, signalSemaphore, cls);
                })
            .addFunction("submit_wait",
                +[](const SubmitQueue* self,
                    const std::shared_ptr<Semaphore>& waitSemaphore,
                    const WaitStage waitStage,
                    const std::vector<std::shared_ptr<const CommandList>>& cls) {
                    self->submit(waitSemaphore, waitStage, cls);
                })
            .addFunction("submit_signal",
                +[](const SubmitQueue* self,
                    const WaitStage signalStage,
                    const std::shared_ptr<Semaphore>& signalSemaphore,
                    const std::vector<std::shared_ptr<const CommandList>>& cls) {
                    self->submit(signalStage, signalSemaphore, cls);
                })
            .addFunction("wait_idle", &SubmitQueue::waitIdle)
        .endClass()
        .beginClass<Vireo>("Vireo")
            .addProperty("backend",   &Vireo::getBackend)
            .addFunction("wait_idle", &Vireo::waitIdle)
            .addFunction("create_swap_chain",       &Vireo::createSwapChain)
            .addFunction("create_submit_queue",     &Vireo::createSubmitQueue)
            .addFunction("create_fence",            &Vireo::createFence)
            .addFunction("create_semaphore",        &Vireo::createSemaphore)
            .addFunction("create_command_allocator",&Vireo::createCommandAllocator)
            .addFunction("create_vertex_layout",    &Vireo::createVertexLayout)
            .addFunction("create_shader_module_from_file",
                (std::shared_ptr<ShaderModule> (Vireo::*)(const std::string&) const) &Vireo::createShaderModule)
            .addFunction("create_shader_module_from_data",
                (std::shared_ptr<ShaderModule> (Vireo::*)(const std::vector<char>&, const std::string&) const) &Vireo::createShaderModule)
            .addFunction("create_pipeline_resources",  &Vireo::createPipelineResources)
            .addFunction("create_compute_pipeline",    &Vireo::createComputePipeline)
            .addFunction("create_graphic_pipeline",    &Vireo::createGraphicPipeline)
            .addFunction("create_buffer",              &Vireo::createBuffer)
            .addFunction("create_image",               &Vireo::createImage)
            .addFunction("create_read_write_image",    &Vireo::createReadWriteImage)
            .addFunction("create_render_target",
                +[](const Vireo* self,
                    const ImageFormat format,
                    const std::uint32_t width,
                    const std::uint32_t height,
                    const RenderTargetType type = RenderTargetType::COLOR,
                    const ClearValue clearValue = {},
                    const std::uint32_t arraySize = 1,
                    const MSAA msaa = MSAA::NONE,
                    const std::string& name = "RenderTarget") {
                    return self->createRenderTarget(format, width, height, type, clearValue, arraySize, msaa, name);
                })
            .addFunction("create_render_target_from_swap_chain",
                +[](const Vireo* self,
                    const std::shared_ptr<const SwapChain>& swapChain,
                    const ClearValue clearValue = {},
                    const MSAA msaa = MSAA::NONE,
                    const std::string& name = "RenderTarget") {
                    return self->createRenderTarget(swapChain, clearValue, msaa, name);
                })
            .addFunction("create_descriptor_layout",         &Vireo::createDescriptorLayout)
            .addFunction("create_sampler_descriptor_layout", &Vireo::createSamplerDescriptorLayout)
            .addFunction("create_dynamic_uniform_descriptor_layout", &Vireo::createDynamicUniformDescriptorLayout)
            .addFunction("create_descriptor_set",  &Vireo::createDescriptorSet)
            .addFunction("create_sampler",         &Vireo::createSampler)
            .addStaticFunction("is_backend_supported",   &Vireo::isBackendSupported)
            .addFunction("get_shader_file_extension",    &Vireo::getShaderFileExtension)
            .addFunction("get_physical_device",          &Vireo::getPhysicalDevice)
            .addFunction("get_device",                   &Vireo::getDevice)
            .addFunction("get_instance",                 &Vireo::getInstance)
        .endClass()

        .endNamespace();
    }

}