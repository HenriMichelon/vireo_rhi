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
    /// REWRITE THIS

    // float getClearColorR(const ClearValue* v) { return v->color[0]; }
    // void setClearColorR(ClearValue* v, float r) { v->color[0] = r; }
    //
    // float getClearColorG(const ClearValue* v) { return v->color[1]; }
    // void setClearColorG(ClearValue* v, float g) { v->color[1] = g; }
    //
    // float getClearColorB(const ClearValue* v) { return v->color[2]; }
    // void setClearColorB(ClearValue* v, float b) { v->color[2] = b; }
    //
    // float getClearColorA(const ClearValue* v) { return v->color[3]; }
    // void setClearColorA(ClearValue* v, float a) { v->color[3] = a; }
    //
    // float getClearDepth(const ClearValue* v) { return v->depthStencil.depth; }
    // void setClearDepth(ClearValue* v, float d) { v->depthStencil.depth = d; }
    //
    // std::uint32_t getClearStencil(const ClearValue* v) { return v->depthStencil.stencil; }
    // void setClearStencil(ClearValue* v, std::uint32_t s) { v->depthStencil.stencil = s; }
    //
    // void DS_updateBufferByIndex(DescriptorSet& self,
    //                             const DescriptorIndex index,
    //                             const std::shared_ptr<const Buffer>& buffer) {
    //     self.update(index, buffer);
    // }
    //
    // void DS_updateBufferDynamic(DescriptorSet& self, const std::shared_ptr<const Buffer>& buffer) {
    //     self.update(buffer);
    // }
    //
    // void DS_updateImageByIndex(DescriptorSet& self, DescriptorIndex index, const std::shared_ptr<const Image>& image) {
    //     self.update(index, image);
    // }
    //
    // void DS_updateImageByRef(DescriptorSet& self, DescriptorIndex index, const Image& image) {
    //     self.update(index, image);
    // }
    //
    // void DS_updateSamplerByIndex(DescriptorSet& self,
    //                              const DescriptorIndex index,
    //                              const std::shared_ptr<const Sampler>& sampler) {
    //     self.update(index, sampler);
    // }
    //
    // void DS_updateSamplerByRef(DescriptorSet& self,const  DescriptorIndex index, const Sampler& sampler) {
    //     self.update(index, sampler);
    // }
    //
    // void DS_updateImageArray(
    //     DescriptorSet& self,
    //     const DescriptorIndex index,
    //     const std::vector<std::shared_ptr<Image>>& images) {
    //     self.update(index, images);
    // }
    //
    // void DS_updateBufferArray(
    //     DescriptorSet& self,
    //     const DescriptorIndex index,
    //     const std::vector<std::shared_ptr<Buffer>>& buffers) {
    //     self.update(index, buffers);
    // }
    //
    // void DS_updateSamplerArray(
    //     DescriptorSet& self,
    //     const DescriptorIndex index,
    //     const std::vector<std::shared_ptr<Sampler>>& samplers) {
    //     self.update(index, samplers);
    // }
    //
    // std::shared_ptr<CommandList> CA_createCL(CommandAllocator& self) {
    //     return self.createCommandList();
    // }
    //
    // std::shared_ptr<CommandList> CA_createCLWithPipeline(CommandAllocator& self,
    //                                                      const std::shared_ptr<const Pipeline>& p) {
    //     return self.createCommandList(p);
    // }
    //
    // void SQ_submitFenceSwap(
    //     const SubmitQueue& self,
    //     const std::shared_ptr<Fence>& fence,
    //     const std::shared_ptr<const SwapChain>& swapChain,
    //     const std::vector<std::shared_ptr<const CommandList>>& cls) {
    //     self.submit(fence, swapChain, cls);
    // }
    //
    // void SQ_submitNoSync(
    //     const SubmitQueue& self,
    //     const std::vector<std::shared_ptr<const CommandList>>& cls) {
    //     self.submit(cls);
    // }
    //
    // void SQ_submitFenceOnly(
    //     const SubmitQueue& self,
    //     const std::shared_ptr<Fence>& fence,
    //     const std::vector<std::shared_ptr<const CommandList>>& cls) {
    //     self.submit(fence, cls);
    // }
    //
    // void SQ_submitWaitFenceSwap(
    //     const SubmitQueue& self,
    //     const std::shared_ptr<Semaphore>& waitSemaphore,
    //     const WaitStage waitStage,
    //     const std::shared_ptr<Fence>& fence,
    //     const std::shared_ptr<const SwapChain>& swapChain,
    //     const std::vector<std::shared_ptr<const CommandList>>& cls) {
    //     self.submit(waitSemaphore, waitStage, fence, swapChain, cls);
    // }
    //
    // void SQ_submitWaitSignal(
    //     const SubmitQueue& self,
    //     const std::shared_ptr<Semaphore>& waitSemaphore,
    //     const WaitStage waitStage,
    //     const WaitStage signalStage,
    //     const std::shared_ptr<Semaphore>& signalSemaphore,
    //     const std::vector<std::shared_ptr<const CommandList>>& cls) {
    //     self.submit(waitSemaphore, waitStage, signalStage, signalSemaphore, cls);
    // }
    //
    // void SQ_submitWaitOnly(
    //     const SubmitQueue& self,
    //     const std::shared_ptr<Semaphore>& waitSemaphore,
    //     const WaitStage waitStage,
    //     const std::vector<std::shared_ptr<const CommandList>>& cls) {
    //     self.submit(waitSemaphore, waitStage, cls);
    // }
    //
    // void SQ_submitSignalOnly(
    //     const SubmitQueue& self,
    //     const WaitStage signalStage,
    //     const std::shared_ptr<Semaphore>& signalSemaphore,
    //     const std::vector<std::shared_ptr<const CommandList>>& cls) {
    //     self.submit(signalStage, signalSemaphore, cls);
    // }
    //
    // std::shared_ptr<RenderTarget> Vireo_createRTBasic(
    //     const Vireo& self,
    //     const ImageFormat format,
    //     const std::uint32_t width,
    //     const std::uint32_t height,
    //     const RenderTargetType type,
    //     const ClearValue clearValue,
    //     const std::uint32_t arraySize,
    //     const MSAA msaa,
    //     const std::string& name) {
    //     return self.createRenderTarget(format, width, height, type, clearValue, arraySize, msaa, name);
    // }
    //
    // std::shared_ptr<RenderTarget> Vireo_createRTFromSwap(
    //     const Vireo& self,
    //     const std::shared_ptr<const SwapChain>& swapChain,
    //     const ClearValue clearValue,
    //     const MSAA msaa,
    //     const std::string& name) {
    //     return self.createRenderTarget(swapChain, clearValue, msaa, name);
    // }
    //
    // std::shared_ptr<Vireo> Vireo_createDefault(const BackendConfiguration& configuration) {
    //     return Vireo::create(configuration);
    // }

    void LuaBindings::_register(lua_State* L) {
        luabridge::getGlobalNamespace(L).beginNamespace("vireo")
        .beginNamespace("Backend")
            .addProperty("UNDEFINED", []{ return static_cast<int>(Backend::UNDEFINED); })
            .addProperty("DIRECTX",   []{ return static_cast<int>(Backend::DIRECTX); })
            .addProperty("VULKAN",    []{ return static_cast<int>(Backend::VULKAN); })
        .endNamespace()
        .beginNamespace("Filter")
            .addProperty("NEAREST", []{ return static_cast<int>(Filter::NEAREST); })
            .addProperty("LINEAR",  []{ return static_cast<int>(Filter::LINEAR); })
        .endNamespace()
        .beginNamespace("AddressMode")
            .addProperty("REPEAT",          []{ return static_cast<int>(AddressMode::REPEAT); })
            .addProperty("MIRRORED_REPEAT", []{ return static_cast<int>(AddressMode::MIRRORED_REPEAT); })
            .addProperty("CLAMP_TO_EDGE",   []{ return static_cast<int>(AddressMode::CLAMP_TO_EDGE); })
            .addProperty("CLAMP_TO_BORDER", []{ return static_cast<int>(AddressMode::CLAMP_TO_BORDER); })
        .endNamespace()
        .beginNamespace("FilterMode")
            .addProperty("NEAREST", []{ return static_cast<int>(FilterMode::NEAREST); })
            .addProperty("LINEAR",  []{ return static_cast<int>(FilterMode::LINEAR); })
        .endNamespace()
        .beginNamespace("ImageFormat")
            .addProperty("R8_UNORM",               []{ return static_cast<int>(ImageFormat::R8_UNORM); })
            .addProperty("R8_SNORM",               []{ return static_cast<int>(ImageFormat::R8_SNORM); })
            .addProperty("R8_UINT",                []{ return static_cast<int>(ImageFormat::R8_UINT); })
            .addProperty("R8_SINT",                []{ return static_cast<int>(ImageFormat::R8_SINT); })

            .addProperty("R8G8_UNORM",             []{ return static_cast<int>(ImageFormat::R8G8_UNORM); })
            .addProperty("R8G8_SNORM",             []{ return static_cast<int>(ImageFormat::R8G8_SNORM); })
            .addProperty("R8G8_UINT",              []{ return static_cast<int>(ImageFormat::R8G8_UINT); })
            .addProperty("R8G8_SINT",              []{ return static_cast<int>(ImageFormat::R8G8_SINT); })

            .addProperty("R8G8B8A8_UNORM",         []{ return static_cast<int>(ImageFormat::R8G8B8A8_UNORM); })
            .addProperty("R8G8B8A8_SNORM",         []{ return static_cast<int>(ImageFormat::R8G8B8A8_SNORM); })
            .addProperty("R8G8B8A8_UINT",          []{ return static_cast<int>(ImageFormat::R8G8B8A8_UINT); })
            .addProperty("R8G8B8A8_SINT",          []{ return static_cast<int>(ImageFormat::R8G8B8A8_SINT); })
            .addProperty("R8G8B8A8_SRGB",          []{ return static_cast<int>(ImageFormat::R8G8B8A8_SRGB); })

            .addProperty("B8G8R8A8_UNORM",         []{ return static_cast<int>(ImageFormat::B8G8R8A8_UNORM); })
            .addProperty("B8G8R8A8_SRGB",          []{ return static_cast<int>(ImageFormat::B8G8R8A8_SRGB); })
            .addProperty("B8G8R8X8_UNORM",         []{ return static_cast<int>(ImageFormat::B8G8R8X8_UNORM); })
            .addProperty("B8G8R8X8_SRGB",          []{ return static_cast<int>(ImageFormat::B8G8R8X8_SRGB); })

            .addProperty("A2B10G10R10_UNORM",      []{ return static_cast<int>(ImageFormat::A2B10G10R10_UNORM); })
            .addProperty("A2B10G10R10_UINT",       []{ return static_cast<int>(ImageFormat::A2B10G10R10_UINT); })

            .addProperty("R16_UNORM",              []{ return static_cast<int>(ImageFormat::R16_UNORM); })
            .addProperty("R16_SNORM",              []{ return static_cast<int>(ImageFormat::R16_SNORM); })
            .addProperty("R16_UINT",               []{ return static_cast<int>(ImageFormat::R16_UINT); })
            .addProperty("R16_SINT",               []{ return static_cast<int>(ImageFormat::R16_SINT); })
            .addProperty("R16_SFLOAT",             []{ return static_cast<int>(ImageFormat::R16_SFLOAT); })

            .addProperty("R16G16_UNORM",           []{ return static_cast<int>(ImageFormat::R16G16_UNORM); })
            .addProperty("R16G16_SNORM",           []{ return static_cast<int>(ImageFormat::R16G16_SNORM); })
            .addProperty("R16G16_UINT",            []{ return static_cast<int>(ImageFormat::R16G16_UINT); })
            .addProperty("R16G16_SINT",            []{ return static_cast<int>(ImageFormat::R16G16_SINT); })
            .addProperty("R16G16_SFLOAT",          []{ return static_cast<int>(ImageFormat::R16G16_SFLOAT); })

            .addProperty("R16G16B16A16_UNORM",     []{ return static_cast<int>(ImageFormat::R16G16B16A16_UNORM); })
            .addProperty("R16G16B16A16_SNORM",     []{ return static_cast<int>(ImageFormat::R16G16B16A16_SNORM); })
            .addProperty("R16G16B16A16_UINT",      []{ return static_cast<int>(ImageFormat::R16G16B16A16_UINT); })
            .addProperty("R16G16B16A16_SINT",      []{ return static_cast<int>(ImageFormat::R16G16B16A16_SINT); })
            .addProperty("R16G16B16A16_SFLOAT",    []{ return static_cast<int>(ImageFormat::R16G16B16A16_SFLOAT); })

            .addProperty("R32_UINT",               []{ return static_cast<int>(ImageFormat::R32_UINT); })
            .addProperty("R32_SINT",               []{ return static_cast<int>(ImageFormat::R32_SINT); })
            .addProperty("R32_SFLOAT",             []{ return static_cast<int>(ImageFormat::R32_SFLOAT); })

            .addProperty("R32G32_UINT",            []{ return static_cast<int>(ImageFormat::R32G32_UINT); })
            .addProperty("R32G32_SINT",            []{ return static_cast<int>(ImageFormat::R32G32_SINT); })
            .addProperty("R32G32_SFLOAT",          []{ return static_cast<int>(ImageFormat::R32G32_SFLOAT); })

            .addProperty("R32G32B32_UINT",         []{ return static_cast<int>(ImageFormat::R32G32B32_UINT); })
            .addProperty("R32G32B32_SINT",         []{ return static_cast<int>(ImageFormat::R32G32B32_SINT); })
            .addProperty("R32G32B32_SFLOAT",       []{ return static_cast<int>(ImageFormat::R32G32B32_SFLOAT); })

            .addProperty("R32G32B32A32_UINT",      []{ return static_cast<int>(ImageFormat::R32G32B32A32_UINT); })
            .addProperty("R32G32B32A32_SINT",      []{ return static_cast<int>(ImageFormat::R32G32B32A32_SINT); })
            .addProperty("R32G32B32A32_SFLOAT",    []{ return static_cast<int>(ImageFormat::R32G32B32A32_SFLOAT); })

            .addProperty("D16_UNORM",              []{ return static_cast<int>(ImageFormat::D16_UNORM); })
            .addProperty("D24_UNORM_S8_UINT",      []{ return static_cast<int>(ImageFormat::D24_UNORM_S8_UINT); })
            .addProperty("D32_SFLOAT",             []{ return static_cast<int>(ImageFormat::D32_SFLOAT); })
            .addProperty("D32_SFLOAT_S8_UINT",     []{ return static_cast<int>(ImageFormat::D32_SFLOAT_S8_UINT); })

            .addProperty("BC1_UNORM",              []{ return static_cast<int>(ImageFormat::BC1_UNORM); })
            .addProperty("BC1_UNORM_SRGB",         []{ return static_cast<int>(ImageFormat::BC1_UNORM_SRGB); })
            .addProperty("BC2_UNORM",              []{ return static_cast<int>(ImageFormat::BC2_UNORM); })
            .addProperty("BC2_UNORM_SRGB",         []{ return static_cast<int>(ImageFormat::BC2_UNORM_SRGB); })
            .addProperty("BC3_UNORM",              []{ return static_cast<int>(ImageFormat::BC3_UNORM); })
            .addProperty("BC3_UNORM_SRGB",         []{ return static_cast<int>(ImageFormat::BC3_UNORM_SRGB); })
            .addProperty("BC4_UNORM",              []{ return static_cast<int>(ImageFormat::BC4_UNORM); })
            .addProperty("BC4_SNORM",              []{ return static_cast<int>(ImageFormat::BC4_SNORM); })
            .addProperty("BC5_UNORM",              []{ return static_cast<int>(ImageFormat::BC5_UNORM); })
            .addProperty("BC5_SNORM",              []{ return static_cast<int>(ImageFormat::BC5_SNORM); })
            .addProperty("BC6H_UFLOAT",            []{ return static_cast<int>(ImageFormat::BC6H_UFLOAT); })
            .addProperty("BC6H_SFLOAT",            []{ return static_cast<int>(ImageFormat::BC6H_SFLOAT); })
            .addProperty("BC7_UNORM",              []{ return static_cast<int>(ImageFormat::BC7_UNORM); })
            .addProperty("BC7_UNORM_SRGB",         []{ return static_cast<int>(ImageFormat::BC7_UNORM_SRGB); })
        .endNamespace()
        .beginNamespace("BufferType")
            .addProperty("VERTEX",           []{ return static_cast<int>(BufferType::VERTEX); })
            .addProperty("INDEX",            []{ return static_cast<int>(BufferType::INDEX); })
            .addProperty("INDIRECT",         []{ return static_cast<int>(BufferType::INDIRECT); })
            .addProperty("UNIFORM",          []{ return static_cast<int>(BufferType::UNIFORM); })
            .addProperty("STORAGE",          []{ return static_cast<int>(BufferType::STORAGE); })
            .addProperty("DEVICE_STORAGE",   []{ return static_cast<int>(BufferType::DEVICE_STORAGE); })
            .addProperty("READWRITE_STORAGE",[]{ return static_cast<int>(BufferType::READWRITE_STORAGE); })
            .addProperty("BUFFER_UPLOAD",    []{ return static_cast<int>(BufferType::BUFFER_UPLOAD); })
            .addProperty("BUFFER_DOWNLOAD",  []{ return static_cast<int>(BufferType::BUFFER_DOWNLOAD); })
            .addProperty("IMAGE_UPLOAD",     []{ return static_cast<int>(BufferType::IMAGE_UPLOAD); })
            .addProperty("IMAGE_DOWNLOAD",   []{ return static_cast<int>(BufferType::IMAGE_DOWNLOAD); })
        .endNamespace()
        .beginNamespace("IndexType")
            .addProperty("UINT16", []{ return static_cast<int>(IndexType::UINT16); })
            .addProperty("UINT32", []{ return static_cast<int>(IndexType::UINT32); })
        .endNamespace()
        .beginNamespace("DescriptorType")
            .addProperty("UNIFORM",           []{ return static_cast<int>(DescriptorType::UNIFORM); })
            .addProperty("UNIFORM_DYNAMIC",   []{ return static_cast<int>(DescriptorType::UNIFORM_DYNAMIC); })
            .addProperty("STORAGE",           []{ return static_cast<int>(DescriptorType::STORAGE); })
            .addProperty("DEVICE_STORAGE",    []{ return static_cast<int>(DescriptorType::DEVICE_STORAGE); })
            .addProperty("READWRITE_STORAGE", []{ return static_cast<int>(DescriptorType::READWRITE_STORAGE); })
            .addProperty("SAMPLED_IMAGE",     []{ return static_cast<int>(DescriptorType::SAMPLED_IMAGE); })
            .addProperty("SAMPLER",           []{ return static_cast<int>(DescriptorType::SAMPLER); })
            .addProperty("READWRITE_IMAGE",   []{ return static_cast<int>(DescriptorType::READWRITE_IMAGE); })
        .endNamespace()
        .beginNamespace("CommandType")
            .addProperty("GRAPHIC",  []{ return static_cast<int>(CommandType::GRAPHIC); })
            .addProperty("TRANSFER", []{ return static_cast<int>(CommandType::TRANSFER); })
            .addProperty("COMPUTE",  []{ return static_cast<int>(CommandType::COMPUTE); })
        .endNamespace()
        .beginNamespace("AttributeFormat")
            .addProperty("R32_FLOAT",          []{ return static_cast<int>(AttributeFormat::R32_FLOAT); })
            .addProperty("R32G32_FLOAT",       []{ return static_cast<int>(AttributeFormat::R32G32_FLOAT); })
            .addProperty("R32G32B32_FLOAT",    []{ return static_cast<int>(AttributeFormat::R32G32B32_FLOAT); })
            .addProperty("R32G32B32A32_FLOAT", []{ return static_cast<int>(AttributeFormat::R32G32B32A32_FLOAT); })
            .addProperty("R32_SINT",           []{ return static_cast<int>(AttributeFormat::R32_SINT); })
            .addProperty("R32G32_SINT",        []{ return static_cast<int>(AttributeFormat::R32G32_SINT); })
            .addProperty("R32G32B32_SINT",     []{ return static_cast<int>(AttributeFormat::R32G32B32_SINT); })
            .addProperty("R32G32B32A32_SINT",  []{ return static_cast<int>(AttributeFormat::R32G32B32A32_SINT); })
            .addProperty("R32_UINT",           []{ return static_cast<int>(AttributeFormat::R32_UINT); })
            .addProperty("R32G32_UINT",        []{ return static_cast<int>(AttributeFormat::R32G32_UINT); })
            .addProperty("R32G32B32_UINT",     []{ return static_cast<int>(AttributeFormat::R32G32B32_UINT); })
            .addProperty("R32G32B32A32_UINT",  []{ return static_cast<int>(AttributeFormat::R32G32B32A32_UINT); })
        .endNamespace()
        .beginNamespace("RenderTargetType")
            .addProperty("COLOR",          []{ return static_cast<int>(RenderTargetType::COLOR); })
            .addProperty("DEPTH",          []{ return static_cast<int>(RenderTargetType::DEPTH); })
            .addProperty("DEPTH_STENCIL",  []{ return static_cast<int>(RenderTargetType::DEPTH_STENCIL); })
        .endNamespace()
        .beginNamespace("CullMode")
            .addProperty("NONE",  []{ return static_cast<int>(CullMode::NONE); })
            .addProperty("FRONT", []{ return static_cast<int>(CullMode::FRONT); })
            .addProperty("BACK",  []{ return static_cast<int>(CullMode::BACK); })
        .endNamespace()
        .beginNamespace("PrimitiveTopology")
            .addProperty("POINT_LIST",     []{ return static_cast<int>(PrimitiveTopology::POINT_LIST); })
            .addProperty("LINE_LIST",      []{ return static_cast<int>(PrimitiveTopology::LINE_LIST); })
            .addProperty("LINE_STRIP",     []{ return static_cast<int>(PrimitiveTopology::LINE_STRIP); })
            .addProperty("TRIANGLE_LIST",  []{ return static_cast<int>(PrimitiveTopology::TRIANGLE_LIST); })
            .addProperty("TRIANGLE_STRIP", []{ return static_cast<int>(PrimitiveTopology::TRIANGLE_STRIP); })
        .endNamespace()
        .beginNamespace("PolygonMode")
            .addProperty("FILL",      []{ return static_cast<int>(PolygonMode::FILL); })
            .addProperty("WIREFRAME", []{ return static_cast<int>(PolygonMode::WIREFRAME); })
        .endNamespace()
        .beginNamespace("CompareOp")
            .addProperty("NEVER",            []{ return static_cast<int>(CompareOp::NEVER); })
            .addProperty("LESS",             []{ return static_cast<int>(CompareOp::LESS); })
            .addProperty("EQUAL",            []{ return static_cast<int>(CompareOp::EQUAL); })
            .addProperty("LESS_OR_EQUAL",    []{ return static_cast<int>(CompareOp::LESS_OR_EQUAL); })
            .addProperty("GREATER",          []{ return static_cast<int>(CompareOp::GREATER); })
            .addProperty("NOT_EQUAL",        []{ return static_cast<int>(CompareOp::NOT_EQUAL); })
            .addProperty("GREATER_OR_EQUAL", []{ return static_cast<int>(CompareOp::GREATER_OR_EQUAL); })
            .addProperty("ALWAYS",           []{ return static_cast<int>(CompareOp::ALWAYS); })
        .endNamespace()
        .beginNamespace("StencilOp")
            .addProperty("KEEP",                 []{ return static_cast<int>(StencilOp::KEEP); })
            .addProperty("ZERO",                 []{ return static_cast<int>(StencilOp::ZERO); })
            .addProperty("REPLACE",              []{ return static_cast<int>(StencilOp::REPLACE); })
            .addProperty("INCREMENT_AND_CLAMP",  []{ return static_cast<int>(StencilOp::INCREMENT_AND_CLAMP); })
            .addProperty("DECREMENT_AND_CLAMP",  []{ return static_cast<int>(StencilOp::DECREMENT_AND_CLAMP); })
            .addProperty("INVERT",               []{ return static_cast<int>(StencilOp::INVERT); })
            .addProperty("INCREMENT_AND_WRAP",   []{ return static_cast<int>(StencilOp::INCREMENT_AND_WRAP); })
            .addProperty("DECREMENT_AND_WRAP",   []{ return static_cast<int>(StencilOp::DECREMENT_AND_WRAP); })
        .endNamespace()
        .beginNamespace("BlendFactor")
            .addProperty("ZERO",                    []{ return static_cast<int>(BlendFactor::ZERO); })
            .addProperty("ONE",                     []{ return static_cast<int>(BlendFactor::ONE); })
            .addProperty("SRC_COLOR",               []{ return static_cast<int>(BlendFactor::SRC_COLOR); })
            .addProperty("ONE_MINUS_SRC_COLOR",     []{ return static_cast<int>(BlendFactor::ONE_MINUS_SRC_COLOR); })
            .addProperty("DST_COLOR",               []{ return static_cast<int>(BlendFactor::DST_COLOR); })
            .addProperty("ONE_MINUS_DST_COLOR",     []{ return static_cast<int>(BlendFactor::ONE_MINUS_DST_COLOR); })
            .addProperty("SRC_ALPHA",               []{ return static_cast<int>(BlendFactor::SRC_ALPHA); })
            .addProperty("ONE_MINUS_SRC_ALPHA",     []{ return static_cast<int>(BlendFactor::ONE_MINUS_SRC_ALPHA); })
            .addProperty("DST_ALPHA",               []{ return static_cast<int>(BlendFactor::DST_ALPHA); })
            .addProperty("ONE_MINUS_DST_ALPHA",     []{ return static_cast<int>(BlendFactor::ONE_MINUS_DST_ALPHA); })
            .addProperty("CONSTANT_COLOR",          []{ return static_cast<int>(BlendFactor::CONSTANT_COLOR); })
            .addProperty("ONE_MINUS_CONSTANT_COLOR",[]{ return static_cast<int>(BlendFactor::ONE_MINUS_CONSTANT_COLOR); })
            .addProperty("CONSTANT_ALPHA",          []{ return static_cast<int>(BlendFactor::CONSTANT_ALPHA); })
            .addProperty("ONE_MINUS_CONSTANT_ALPHA",[]{ return static_cast<int>(BlendFactor::ONE_MINUS_CONSTANT_ALPHA); })
            .addProperty("SRC_ALPHA_SATURATE",      []{ return static_cast<int>(BlendFactor::SRC_ALPHA_SATURATE); })
            .addProperty("SRC1_COLOR",              []{ return static_cast<int>(BlendFactor::SRC1_COLOR); })
            .addProperty("ONE_MINUS_SRC1_COLOR",    []{ return static_cast<int>(BlendFactor::ONE_MINUS_SRC1_COLOR); })
            .addProperty("SRC1_ALPHA",              []{ return static_cast<int>(BlendFactor::SRC1_ALPHA); })
            .addProperty("ONE_MINUS_SRC1_ALPHA",    []{ return static_cast<int>(BlendFactor::ONE_MINUS_SRC1_ALPHA); })
        .endNamespace()
        .beginNamespace("BlendOp")
            .addProperty("ADD",              []{ return static_cast<int>(BlendOp::ADD); })
            .addProperty("SUBTRACT",         []{ return static_cast<int>(BlendOp::SUBTRACT); })
            .addProperty("REVERSE_SUBTRACT", []{ return static_cast<int>(BlendOp::REVERSE_SUBTRACT); })
            .addProperty("MIN",              []{ return static_cast<int>(BlendOp::MIN); })
            .addProperty("MAX",              []{ return static_cast<int>(BlendOp::MAX); })
        .endNamespace()
        .beginNamespace("LogicOp")
            .addProperty("CLEAR",         []{ return static_cast<int>(LogicOp::CLEAR); })
            .addProperty("SET",           []{ return static_cast<int>(LogicOp::SET); })
            .addProperty("COPY",          []{ return static_cast<int>(LogicOp::COPY); })
            .addProperty("COPY_INVERTED", []{ return static_cast<int>(LogicOp::COPY_INVERTED); })
            .addProperty("NOOP",          []{ return static_cast<int>(LogicOp::NOOP); })
            .addProperty("INVERT",        []{ return static_cast<int>(LogicOp::INVERT); })
            .addProperty("AND",           []{ return static_cast<int>(LogicOp::AND); })
            .addProperty("NAND",          []{ return static_cast<int>(LogicOp::NAND); })
            .addProperty("OR",            []{ return static_cast<int>(LogicOp::OR); })
            .addProperty("NOR",           []{ return static_cast<int>(LogicOp::NOR); })
            .addProperty("XOR",           []{ return static_cast<int>(LogicOp::XOR); })
            .addProperty("EQUIV",         []{ return static_cast<int>(LogicOp::EQUIV); })
            .addProperty("AND_REVERSE",   []{ return static_cast<int>(LogicOp::AND_REVERSE); })
            .addProperty("AND_INVERTED",  []{ return static_cast<int>(LogicOp::AND_INVERTED); })
            .addProperty("OR_REVERSE",    []{ return static_cast<int>(LogicOp::OR_REVERSE); })
            .addProperty("OR_INVERTED",   []{ return static_cast<int>(LogicOp::OR_INVERTED); })
        .endNamespace()
        .beginNamespace("ColorWriteMask")
            .addProperty("RED",   []{ return static_cast<int>(ColorWriteMask::RED); })
            .addProperty("GREEN", []{ return static_cast<int>(ColorWriteMask::GREEN); })
            .addProperty("BLUE",  []{ return static_cast<int>(ColorWriteMask::BLUE); })
            .addProperty("ALPHA", []{ return static_cast<int>(ColorWriteMask::ALPHA); })
            .addProperty("ALL",   []{ return static_cast<int>(ColorWriteMask::ALL); })
        .endNamespace()
        .beginNamespace("ShaderStage")
            .addProperty("ALL",      []{ return static_cast<int>(ShaderStage::ALL); })
            .addProperty("VERTEX",   []{ return static_cast<int>(ShaderStage::VERTEX); })
            .addProperty("FRAGMENT", []{ return static_cast<int>(ShaderStage::FRAGMENT); })
            .addProperty("HULL",     []{ return static_cast<int>(ShaderStage::HULL); })
            .addProperty("DOMAIN",   []{ return static_cast<int>(ShaderStage::DOMAIN); })
            .addProperty("GEOMETRY", []{ return static_cast<int>(ShaderStage::GEOMETRY); })
            .addProperty("COMPUTE",  []{ return static_cast<int>(ShaderStage::COMPUTE); })
        .endNamespace()
        .beginNamespace("WaitStage")
            .addProperty("NONE",                                      []{ return static_cast<int>(WaitStage::NONE); })
            .addProperty("PIPELINE_TOP",                              []{ return static_cast<int>(WaitStage::PIPELINE_TOP); })
            .addProperty("VERTEX_INPUT",                              []{ return static_cast<int>(WaitStage::VERTEX_INPUT); })
            .addProperty("VERTEX_SHADER",                             []{ return static_cast<int>(WaitStage::VERTEX_SHADER); })
            .addProperty("DEPTH_STENCIL_TEST_BEFORE_FRAGMENT_SHADER", []{ return static_cast<int>(WaitStage::DEPTH_STENCIL_TEST_BEFORE_FRAGMENT_SHADER); })
            .addProperty("FRAGMENT_SHADER",                           []{ return static_cast<int>(WaitStage::FRAGMENT_SHADER); })
            .addProperty("DEPTH_STENCIL_TEST_AFTER_FRAGMENT_SHADER",  []{ return static_cast<int>(WaitStage::DEPTH_STENCIL_TEST_AFTER_FRAGMENT_SHADER); })
            .addProperty("COLOR_OUTPUT",                              []{ return static_cast<int>(WaitStage::COLOR_OUTPUT); })
            .addProperty("COMPUTE_SHADER",                            []{ return static_cast<int>(WaitStage::COMPUTE_SHADER); })
            .addProperty("TRANSFER",                                  []{ return static_cast<int>(WaitStage::TRANSFER); })
            .addProperty("PIPELINE_BOTTOM",                           []{ return static_cast<int>(WaitStage::PIPELINE_BOTTOM); })
            .addProperty("ALL_GRAPHICS",                              []{ return static_cast<int>(WaitStage::ALL_GRAPHICS); })
            .addProperty("ALL_COMMANDS",                              []{ return static_cast<int>(WaitStage::ALL_COMMANDS); })
            .addProperty("COPY",                                      []{ return static_cast<int>(WaitStage::COPY); })
            .addProperty("RESOLV",                                    []{ return static_cast<int>(WaitStage::RESOLV); })
            .addProperty("BLIT",                                      []{ return static_cast<int>(WaitStage::BLIT); })
            .addProperty("CLEAR",                                     []{ return static_cast<int>(WaitStage::CLEAR); })
        .endNamespace()
        .beginNamespace("ResourceState")
            .addProperty("UNDEFINED",                        []{ return static_cast<int>(ResourceState::UNDEFINED); })
            .addProperty("RENDER_TARGET_COLOR",              []{ return static_cast<int>(ResourceState::RENDER_TARGET_COLOR); })
            .addProperty("RENDER_TARGET_DEPTH",              []{ return static_cast<int>(ResourceState::RENDER_TARGET_DEPTH); })
            .addProperty("RENDER_TARGET_DEPTH_READ",         []{ return static_cast<int>(ResourceState::RENDER_TARGET_DEPTH_READ); })
            .addProperty("RENDER_TARGET_DEPTH_STENCIL",      []{ return static_cast<int>(ResourceState::RENDER_TARGET_DEPTH_STENCIL); })
            .addProperty("RENDER_TARGET_DEPTH_STENCIL_READ", []{ return static_cast<int>(ResourceState::RENDER_TARGET_DEPTH_STENCIL_READ); })
            .addProperty("PRESENT",                          []{ return static_cast<int>(ResourceState::PRESENT); })
            .addProperty("COPY_SRC",                         []{ return static_cast<int>(ResourceState::COPY_SRC); })
            .addProperty("COPY_DST",                         []{ return static_cast<int>(ResourceState::COPY_DST); })
            .addProperty("SHADER_READ",                      []{ return static_cast<int>(ResourceState::SHADER_READ); })
            .addProperty("COMPUTE_READ",                     []{ return static_cast<int>(ResourceState::COMPUTE_READ); })
            .addProperty("COMPUTE_WRITE",                    []{ return static_cast<int>(ResourceState::COMPUTE_WRITE); })
            .addProperty("INDIRECT_DRAW",                    []{ return static_cast<int>(ResourceState::INDIRECT_DRAW); })
            .addProperty("VERTEX_INPUT",                     []{ return static_cast<int>(ResourceState::VERTEX_INPUT); })
            .addProperty("UNIFORM",                          []{ return static_cast<int>(ResourceState::UNIFORM); })
        .endNamespace()
        .beginNamespace("MSAA")
            .addProperty("NONE", []{ return static_cast<int>(MSAA::NONE); })
            .addProperty("X2",   []{ return static_cast<int>(MSAA::X2); })
            .addProperty("X4",   []{ return static_cast<int>(MSAA::X4); })
            .addProperty("X8",   []{ return static_cast<int>(MSAA::X8); })
            .addProperty("X16",  []{ return static_cast<int>(MSAA::X16); })
            .addProperty("X32",  []{ return static_cast<int>(MSAA::X32); })
            .addProperty("X64",  []{ return static_cast<int>(MSAA::X64); })
        .endNamespace()
        .beginNamespace("PresentMode")
            .addProperty("IMMEDIATE", []{ return static_cast<int>(PresentMode::IMMEDIATE); })
            .addProperty("VSYNC",     []{ return static_cast<int>(PresentMode::VSYNC); })
        .endNamespace()
        .beginNamespace("PipelineType")
            .addProperty("GRAPHIC", []{ return static_cast<int>(PipelineType::GRAPHIC); })
            .addProperty("COMPUTE", []{ return static_cast<int>(PipelineType::COMPUTE); })
        .endNamespace()
        .beginNamespace("SemaphoreType")
            .addProperty("BINARY",   []{ return static_cast<int>(SemaphoreType::BINARY); })
            .addProperty("TIMELINE", []{ return static_cast<int>(SemaphoreType::TIMELINE); })
        .endNamespace()

        // structs
        .beginClass<ColorBlendDesc>("ColorBlendDesc")
            .addConstructor<void(*)()>()
            .addProperty("blend_enable", &ColorBlendDesc::blendEnable)
            .addProperty("src_color_blend_factor", &ColorBlendDesc::srcColorBlendFactor)
            .addProperty("dst_color_blend_factor", &ColorBlendDesc::dstColorBlendFactor)
            .addProperty("color_blend_op", &ColorBlendDesc::colorBlendOp)
            .addProperty("src_alpha_blend_factor", &ColorBlendDesc::srcAlphaBlendFactor)
            .addProperty("dst_alpha_blend_factor", &ColorBlendDesc::dstAlphaBlendFactor)
            .addProperty("alpha_blend_op", &ColorBlendDesc::alphaBlendOp)
            .addProperty("color_write_mask", &ColorBlendDesc::colorWriteMask)
        .endClass()
        .beginClass<StencilOpState>("StencilOpState")
            .addConstructor<void(*)()>()
            .addProperty("fail_op", &StencilOpState::failOp)
            .addProperty("pass_op", &StencilOpState::passOp)
            .addProperty("depth_fail_op", &StencilOpState::depthFailOp)
            .addProperty("compare_op", &StencilOpState::compareOp)
            .addProperty("compare_mask", &StencilOpState::compareMask)
            .addProperty("write_mask", &StencilOpState::writeMask)
        .endClass()
        .beginClass<PhysicalDeviceDesc>("PhysicalDeviceDesc")
            .addConstructor<void(*)()>()
            .addProperty("name", &PhysicalDeviceDesc::name)
            .addProperty("dedicated_video_memory", &PhysicalDeviceDesc::dedicatedVideoMemory)
            .addProperty("dedicated_system_memory", &PhysicalDeviceDesc::dedicatedSystemMemory)
            .addProperty("shared_system_memory", &PhysicalDeviceDesc::sharedSystemMemory)
        .endClass()
        .beginClass<Extent>("Extent")
            .addConstructor<void(*)()>()
            .addProperty("width", &Extent::width)
            .addProperty("height", &Extent::height)
        .endClass()
        .beginClass<Rect>("Rect")
            .addConstructor<void(uint32_t, uint32_t), void(int32_t, int32_t, uint32_t, uint32_t)>()
            .addProperty("x", &Rect::x)
            .addProperty("y", &Rect::y)
            .addProperty("width", &Rect::width)
            .addProperty("height", &Rect::height)
        .endClass()
        .beginClass<Viewport>("Viewport")
            .addConstructor<void(float, float), void(float, float, float, float)>()
            .addProperty("x", &Viewport::x)
            .addProperty("y", &Viewport::y)
            .addProperty("width", &Viewport::width)
            .addProperty("height", &Viewport::height)
            .addProperty("min_depth", &Viewport::minDepth)
            .addProperty("max_depth", &Viewport::maxDepth)
        .endClass()
        .beginClass<PushConstantsDesc>("PushConstantsDesc")
            .addConstructor<void(*)()>()
            .addProperty("stage", &PushConstantsDesc::stage)
            .addProperty("size", &PushConstantsDesc::size)
            .addProperty("offset", &PushConstantsDesc::offset)
        .endClass()
        .beginClass<VertexAttributeDesc>("VertexAttributeDesc")
            .addConstructor<void(*)()>()
            .addProperty("binding", &VertexAttributeDesc::binding)
            .addProperty("format", &VertexAttributeDesc::format)
            .addProperty("offset", &VertexAttributeDesc::offset)
        .endClass()
        .beginClass<DepthClearValue>("DepthClearValue")
            .addConstructor<void(*)()>()
            .addProperty("depth", &DepthClearValue::depth)
            .addProperty("stencil", &DepthClearValue::stencil)
        .endClass()
        .beginClass<ClearValue>("ClearValue")
            .addConstructor<void(*)()>()
            // .addProperty("r", &getClearColorR, &setClearColorR)
            // .addProperty("g", &getClearColorG, &setClearColorG)
            // .addProperty("b", &getClearColorB, &setClearColorB)
            // .addProperty("a", &getClearColorA, &setClearColorA)
            // .addProperty("depth", &getClearDepth, &setClearDepth)
            // .addProperty("stencil", &getClearStencil, &setClearStencil)
        .endClass()
        .beginClass<VideoMemoryAllocationDesc>("VideoMemoryAllocationDesc")
            .addConstructor<void(*)()>()
            .addProperty("usage", &VideoMemoryAllocationDesc::usage)
            .addProperty("name", &VideoMemoryAllocationDesc::name)
            .addProperty("size", &VideoMemoryAllocationDesc::size)
            .addProperty("ref", &VideoMemoryAllocationDesc::ref)
        .endClass()
        .beginClass<RenderTargetDesc>("RenderTargetDesc")
            .addConstructor<void(*)()>()
            .addProperty("swap_chain", &RenderTargetDesc::swapChain)
            .addProperty("render_target", &RenderTargetDesc::renderTarget)
            .addProperty("multisampled_render_target", &RenderTargetDesc::multisampledRenderTarget)
            .addProperty("clear", &RenderTargetDesc::clear)
            .addProperty("clear_value", &RenderTargetDesc::clearValue)
            .addProperty("discard_after_render", &RenderTargetDesc::discardAfterRender)
        .endClass()
        .beginClass<RenderingConfiguration>("RenderingConfiguration")
            .addConstructor<void(*)()>()
            .addProperty("color_render_targets", &RenderingConfiguration::colorRenderTargets)
            .addProperty("depth_stencil_render_target", &RenderingConfiguration::depthStencilRenderTarget)
            .addProperty("multisampled_depth_stencil_render_target", &RenderingConfiguration::multisampledDepthStencilRenderTarget)
            .addProperty("depth_test_enable", &RenderingConfiguration::depthTestEnable)
            .addProperty("stencil_test_enable", &RenderingConfiguration::stencilTestEnable)
            .addProperty("clear_depth_stencil", &RenderingConfiguration::clearDepthStencil)
            .addProperty("depth_stencil_clear_value", &RenderingConfiguration::depthStencilClearValue)
            .addProperty("discard_depth_stencil_after_render", &RenderingConfiguration::discardDepthStencilAfterRender)
        .endClass()
        .beginClass<DrawIndirectCommand>("DrawIndirectCommand")
            .addConstructor<void(*)()>()
            .addProperty("vertex_count", &DrawIndirectCommand::vertexCount)
            .addProperty("instance_count", &DrawIndirectCommand::instanceCount)
            .addProperty("first_vertex", &DrawIndirectCommand::firstVertex)
            .addProperty("first_instance", &DrawIndirectCommand::firstInstance)
        .endClass()
        .beginClass<DrawIndexedIndirectCommand>("DrawIndexedIndirectCommand")
            .addConstructor<void(*)()>()
            .addProperty("index_count", &DrawIndexedIndirectCommand::indexCount)
            .addProperty("instance_count", &DrawIndexedIndirectCommand::instanceCount)
            .addProperty("first_index", &DrawIndexedIndirectCommand::firstIndex)
            .addProperty("vertex_offset", &DrawIndexedIndirectCommand::vertexOffset)
            .addProperty("first_instance", &DrawIndexedIndirectCommand::firstInstance)
        .endClass()
        .beginClass<BufferCopyRegion>("BufferCopyRegion")
            .addConstructor<void(*)()>()
            .addProperty("src_offset", &BufferCopyRegion::srcOffset)
            .addProperty("dst_offset", &BufferCopyRegion::dstOffset)
            .addProperty("size", &BufferCopyRegion::size)
        .endClass()
        .beginClass<GraphicPipelineConfiguration>("GraphicPipelineConfiguration")
            .addConstructor<void(*)()>()
            .addProperty("resources", &GraphicPipelineConfiguration::resources)
            .addProperty("color_render_formats", &GraphicPipelineConfiguration::colorRenderFormats)
            .addProperty("color_blend_desc", &GraphicPipelineConfiguration::colorBlendDesc)
            .addProperty("vertex_input_layout", &GraphicPipelineConfiguration::vertexInputLayout)
            .addProperty("vertex_shader", &GraphicPipelineConfiguration::vertexShader)
            .addProperty("fragment_shader", &GraphicPipelineConfiguration::fragmentShader)
            .addProperty("hull_shader", &GraphicPipelineConfiguration::hullShader)
            .addProperty("domain_shader", &GraphicPipelineConfiguration::domainShader)
            .addProperty("geometry_shader", &GraphicPipelineConfiguration::geometryShader)
            .addProperty("primitive_topology", &GraphicPipelineConfiguration::primitiveTopology)
            .addProperty("msaa", &GraphicPipelineConfiguration::msaa)
            .addProperty("cull_mode", &GraphicPipelineConfiguration::cullMode)
            .addProperty("polygon_mode", &GraphicPipelineConfiguration::polygonMode)
            .addProperty("front_face_counter_clockwise", &GraphicPipelineConfiguration::frontFaceCounterClockwise)
            .addProperty("depth_stencil_image_format", &GraphicPipelineConfiguration::depthStencilImageFormat)
            .addProperty("depth_test_enable", &GraphicPipelineConfiguration::depthTestEnable)
            .addProperty("depth_write_enable", &GraphicPipelineConfiguration::depthWriteEnable)
            .addProperty("depth_compare_op", &GraphicPipelineConfiguration::depthCompareOp)
            .addProperty("depth_bias_enable", &GraphicPipelineConfiguration::depthBiasEnable)
            .addProperty("depth_bias_constant_factor", &GraphicPipelineConfiguration::depthBiasConstantFactor)
            .addProperty("depth_bias_clamp", &GraphicPipelineConfiguration::depthBiasClamp)
            .addProperty("depth_bias_slope_factor", &GraphicPipelineConfiguration::depthBiasSlopeFactor)
            .addProperty("stencil_test_enable", &GraphicPipelineConfiguration::stencilTestEnable)
            .addProperty("front_stencil_op_state", &GraphicPipelineConfiguration::frontStencilOpState)
            .addProperty("back_stencil_op_state", &GraphicPipelineConfiguration::backStencilOpState)
            .addProperty("logic_op_enable", &GraphicPipelineConfiguration::logicOpEnable)
            .addProperty("logic_op", &GraphicPipelineConfiguration::logicOp)
            .addProperty("alpha_to_coverage_enable", &GraphicPipelineConfiguration::alphaToCoverageEnable)
        .endClass()

        // classes
        .beginClass<Fence>("Fence")
           .addFunction("wait", &Fence::wait)
           .addFunction("reset", &Fence::reset)
       .endClass()
       .beginClass<Semaphore>("Semaphore")
           .addFunction("get_type", &Semaphore::getType)
           .addFunction("get_value", &Semaphore::getValue)
           .addFunction("set_value", &Semaphore::setValue)
           .addFunction("increment_value", &Semaphore::incrementValue)
           .addFunction("decrement_value", &Semaphore::decrementValue)
       .endClass()
       .beginClass<Instance>("Instance")
       .endClass()
       .beginClass<PhysicalDevice>("PhysicalDevice")
           //.addFunction("get_description", &PhysicalDevice::getDescription)
       .endClass()
       .beginClass<Device>("Device")
           .addFunction("have_dedicated_transfer_queue", &Device::haveDedicatedTransferQueue)
       .endClass()
        .beginClass<Buffer>("Buffer")
            .addFunction("get_size", &Buffer::getSize)
            .addFunction("get_type", &Buffer::getType)
            .addFunction("get_instance_size", &Buffer::getInstanceSize)
            .addFunction("get_instance_size_aligned", &Buffer::getInstanceSizeAligned)
            .addFunction("get_instance_count", &Buffer::getInstanceCount)
            .addFunction("get_mapped_address", &Buffer::getMappedAddress)
            .addFunction("map", &Buffer::map)
            .addFunction("unmap", &Buffer::unmap)
            .addFunction("write", &Buffer::write)
            .addStaticFunction("get_memory_allocations", &Buffer::getMemoryAllocations)
        .endClass()
        .beginClass<Sampler>("Sampler")
        .endClass()
        .beginClass<Image>("Image")
           .addFunction("get_format", &Image::getFormat)
           .addFunction("get_width", &Image::getWidth)
           .addFunction("get_height", &Image::getHeight)
           .addFunction("get_mip_levels", &Image::getMipLevels)
           .addFunction("get_array_size", &Image::getArraySize)
           .addFunction("get_row_pitch", &Image::getRowPitch)
           .addFunction("get_row_length", &Image::getRowLength)
           .addFunction("get_image_size", &Image::getImageSize)
           .addFunction("get_aligned_image_size", &Image::getAlignedImageSize)
           .addFunction("get_aligned_row_pitch", &Image::getAlignedRowPitch)
           .addFunction("get_aligned_row_length", &Image::getAlignedRowLength)
           .addFunction("is_read_write", &Image::isReadWrite)
           .addStaticFunction("get_pixel_size", &Image::getPixelSize)
           //.addStaticFunction("is_depth_format", &Image::isDepthFormat)
           //.addStaticFunction("is_depth_stencil_format", &Image::isDepthStencilFormat)
           .addStaticFunction("get_memory_allocations", &Image::getMemoryAllocations)
        .endClass()
        .beginClass<RenderTarget>("RenderTarget")
           .addFunction("get_image", &RenderTarget::getImage)
           .addFunction("get_type", &RenderTarget::getType)
        .endClass()
        .beginClass<DescriptorLayout>("DescriptorLayout")
            .addFunction("add", &DescriptorLayout::add)
            .addFunction("build", &DescriptorLayout::build)
            .addFunction("get_capacity", &DescriptorLayout::getCapacity)
            .addFunction("is_dynamic_uniform", &DescriptorLayout::isDynamicUniform)
            .addFunction("is_samplers", &DescriptorLayout::isSamplers)
        .endClass()
        .beginClass<DescriptorSet>("DescriptorSet")
            // .addFunction("update_buffer", &DS_updateBufferByIndex)
            // .addFunction("update_dynamic_buffer", &DS_updateBufferDynamic)
            // .addFunction("update_image", &DS_updateImageByIndex)
            // .addFunction("update_image_by_ref", &DS_updateImageByRef)
            // .addFunction("update_sampler", &DS_updateSamplerByIndex)
            // .addFunction("update_sampler_by_ref", &DS_updateSamplerByRef)
            // .addFunction("update_image_array", &DS_updateImageArray)
            // .addFunction("update_buffer_array", &DS_updateBufferArray)
            // .addFunction("update_sampler_array", &DS_updateSamplerArray)
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
           .addFunction("get_type", &Pipeline::getType)
       .endClass()
       .deriveClass<ComputePipeline, Pipeline>("ComputePipeline")
       .endClass()
       .deriveClass<GraphicPipeline, Pipeline>("GraphicPipeline")
       .endClass()
        .beginClass<SwapChain>("SwapChain")
           .addFunction("get_extent", &SwapChain::getExtent)
           .addFunction("get_aspect_ratio", &SwapChain::getAspectRatio)
           .addFunction("get_current_frame_index", &SwapChain::getCurrentFrameIndex)
           .addFunction("get_frames_in_flight", &SwapChain::getFramesInFlight)
           .addFunction("get_format", &SwapChain::getFormat)
           .addFunction("next_frame_index", &SwapChain::nextFrameIndex)
           .addFunction("acquire", &SwapChain::acquire)
           .addFunction("present", &SwapChain::present)
           .addFunction("recreate", &SwapChain::recreate)
           .addFunction("wait_idle", &SwapChain::waitIdle)
       .endClass()
       .beginClass<CommandList>("CommandList")
            .addFunction("begin", &CommandList::begin)
            .addFunction("end", &CommandList::end)
            .addFunction("upload_buffer", (void (CommandList::*)(const Buffer&, const void*)) &CommandList::upload)
            .addFunction("upload_image", (void (CommandList::*)(const Image&, const void*, std::uint32_t)) &CommandList::upload)
            //.addFunction("upload_buffers", &CommandList::upload)
            //.addFunction("upload_images", &CommandList::upload)
            .addFunction("copy_buffer_to_image", (void (CommandList::*)(const Buffer&, const Image&, std::uint32_t, std::uint32_t, bool) const) &CommandList::copy)
            .addFunction("copy_buffer_to_image_levels", (void (CommandList::*)(const Buffer&, const Image&, const std::vector<std::size_t>&, bool) const) &CommandList::copy)
            //.addFunction("copy_image_to_buffer", &CommandList::copy)
            .addFunction("copy_buffer_to_buffer", (void (CommandList::*)(const Buffer&, const Buffer&, std::size_t, std::uint32_t, std::uint32_t) const) &CommandList::copy)
            .addFunction("copy_buffer_regions", (void (CommandList::*)(const Buffer&, const Buffer&, const std::vector<BufferCopyRegion>&) const) &CommandList::copy)
            //.addFunction("upload_array", &CommandList::uploadArray)
            .addFunction("copy_image_to_swap_chain", (void (CommandList::*)(const Image&, const SwapChain&) const) &CommandList::copy)
            .addFunction("begin_rendering", &CommandList::beginRendering)
            .addFunction("end_rendering", &CommandList::endRendering)
            .addFunction("dispatch", &CommandList::dispatch)
            .addFunction("bind_vertex_buffer", (void (CommandList::*)(const Buffer&, std::size_t) const) &CommandList::bindVertexBuffer)
            .addFunction("bind_vertex_buffers", &CommandList::bindVertexBuffers)
            //.addFunction("bind_index_buffer", &CommandList::bindIndexBuffer)
            //.addFunction("bind_pipeline", (void (CommandList::*)(Pipeline&)) &CommandList::bindPipeline)
            //.addFunction("bind_pipeline_ptr", (void (CommandList::*)(const std::shared_ptr<Pipeline>&)) &CommandList::bindPipeline)
            //.addFunction("bind_descriptors", &CommandList::bindDescriptors)
            .addFunction("bind_descriptor", (void (CommandList::*)(const DescriptorSet&, std::uint32_t) const) &CommandList::bindDescriptor)
            .addFunction("bind_descriptor_dynamic", (void (CommandList::*)(const DescriptorSet&, std::uint32_t, std::uint32_t) const) &CommandList::bindDescriptor)
            .addFunction("draw", &CommandList::draw)
            .addFunction("draw_indexed", &CommandList::drawIndexed)
            .addFunction("draw_indirect", (void (CommandList::*)(const Buffer&, std::size_t, std::uint32_t, std::uint32_t, std::uint32_t)) &CommandList::drawIndirect)
            // .addFunction("draw_indirect_ptr", (void (CommandList::*)(const std::shared_ptr<Buffer>&, std::size_t, std::uint32_t, std::uint32_t, std::uint32_t)) &CommandList::drawIndirect)
            .addFunction("draw_indexed_indirect_count", (void (CommandList::*)(Buffer&, std::size_t, Buffer&, std::size_t, std::uint32_t, std::uint32_t, std::uint32_t)) &CommandList::drawIndexedIndirectCount)
            //// .addFunction("draw_indexed_indirect_count_ptr", (void (CommandList::*)(const std::shared_ptr<Buffer>&, std::size_t, const std::shared_ptr<Buffer>&, std::size_t, std::uint32_t, std::uint32_t, std::uint32_t)) &CommandList::drawIndexedIndirectCount)
            .addFunction("draw_indexed_indirect", (void (CommandList::*)(const Buffer&, std::size_t, std::uint32_t, std::uint32_t, std::uint32_t)) &CommandList::drawIndexedIndirect)
            .addFunction("set_viewports", &CommandList::setViewports)
            .addFunction("set_scissors", (void (CommandList::*)(const std::vector<Rect>&) const) &CommandList::setScissors)
            .addFunction("set_viewport", &CommandList::setViewport)
            .addFunction("set_scissor", (void (CommandList::*)(const Rect&) const) &CommandList::setScissors)
            .addFunction("set_stencil_reference", &CommandList::setStencilReference)
            //.addFunction("barrier_image", (void (CommandList::*)(const std::shared_ptr<const Image>&, ResourceState, ResourceState, std::uint32_t, std::uint32_t) const) &CommandList::barrier)
            //.addFunction("barrier_render_target", (void (CommandList::*)(const std::shared_ptr<const RenderTarget>&, ResourceState, ResourceState) const) &CommandList::barrier)
            //.addFunction("barrier_render_targets", (void (CommandList::*)(const std::vector<std::shared_ptr<const RenderTarget>>&, ResourceState, ResourceState) const) &CommandList::barrier)
            //.addFunction("barrier_swap_chain", (void (CommandList::*)(const std::shared_ptr<const SwapChain>&, ResourceState, ResourceState) const) &CommandList::barrier)
            //.addFunction("barrier_buffer", (void (CommandList::*)(const Buffer&, ResourceState, ResourceState) const) &CommandList::barrier)
            .addFunction("push_constants", &CommandList::pushConstants)
            .addFunction("cleanup", &CommandList::cleanup)
        .endClass()
        .beginClass<CommandAllocator>("CommandAllocator")
           .addFunction("reset", &CommandAllocator::reset)
           // .addFunction("create_command_list", &CA_createCL)
           // .addFunction("create_command_list_with_pipeline", &CA_createCLWithPipeline)
           .addFunction("get_command_list_type", &CommandAllocator::getCommandListType)
       .endClass()
        .beginClass<SubmitQueue>("SubmitQueue")
            // .addFunction("submit_fence_swap", &SQ_submitFenceSwap)
            // .addFunction("submit_no_sync", &SQ_submitNoSync)
            // .addFunction("submit_fence_only", &SQ_submitFenceOnly)
            // .addFunction("submit_wait_fence_swap", &SQ_submitWaitFenceSwap)
            // .addFunction("submit_wait_signal", &SQ_submitWaitSignal)
            // .addFunction("submit_wait_only", &SQ_submitWaitOnly)
            // .addFunction("submit_signal_only", &SQ_submitSignalOnly)
            .addFunction("wait_idle", &SubmitQueue::waitIdle)
        .endClass()
        .beginClass<Vireo>("Vireo")
            .addProperty("backend", &Vireo::getBackend)
            .addFunction("wait_idle", &Vireo::waitIdle)
            .addFunction("create_swap_chain", &Vireo::createSwapChain)
            .addFunction("create_submit_queue", &Vireo::createSubmitQueue)
            .addFunction("create_fence", &Vireo::createFence)
            .addFunction("create_semaphore", &Vireo::createSemaphore)
            .addFunction("create_command_allocator", &Vireo::createCommandAllocator)
            .addFunction("create_vertex_layout", &Vireo::createVertexLayout)
            .addFunction("create_shader_module_from_file", (std::shared_ptr<ShaderModule> (Vireo::*)(const std::string&) const) &Vireo::createShaderModule)
            .addFunction("create_shader_module_from_data", (std::shared_ptr<ShaderModule> (Vireo::*)(const std::vector<char>&,const std::string&) const) &Vireo::createShaderModule)
            .addFunction("create_pipeline_resources", &Vireo::createPipelineResources)
            .addFunction("create_compute_pipeline", &Vireo::createComputePipeline)
            .addFunction("create_graphic_pipeline", &Vireo::createGraphicPipeline)
            .addFunction("create_buffer", &Vireo::createBuffer)
            .addFunction("create_image", &Vireo::createImage)
            .addFunction("create_read_write_image", &Vireo::createReadWriteImage)
            // .addFunction("create_render_target", &Vireo_createRTBasic)
            // .addFunction("create_render_target_from_swap_chain", &Vireo_createRTFromSwap)
            .addFunction("create_descriptor_layout", &Vireo::createDescriptorLayout)
            .addFunction("create_sampler_descriptor_layout", &Vireo::createSamplerDescriptorLayout)
            .addFunction("create_dynamic_uniform_descriptor_layout", &Vireo::createDynamicUniformDescriptorLayout)
            .addFunction("create_descriptor_set", &Vireo::createDescriptorSet)
            .addFunction("create_sampler", &Vireo::createSampler)
            .addStaticFunction("is_backend_supported", &Vireo::isBackendSupported)
            .addFunction("get_shader_file_extension", &Vireo::getShaderFileExtension)
            .addFunction("get_physical_device", &Vireo::getPhysicalDevice)
            .addFunction("get_device", &Vireo::getDevice)
            .addFunction("get_instance", &Vireo::getInstance)
        .endClass()
        // .beginClass<BufferUploadInfo>("BufferUploadInfo")
        //     .addProperty("buffer", &BufferUploadInfo::buffer)
        //     .addProperty("data", &BufferUploadInfo::data)
        // .endClass()
        // .beginClass<ImageUploadInfo>("ImageUploadInfo")
        //     .addProperty("image", &ImageUploadInfo::image)
        //     .addProperty("data", &ImageUploadInfo::data)
        // .endClass()
        .endNamespace();
    }

}