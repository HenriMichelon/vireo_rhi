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
    float getClearColorR(const ClearValue* v) { return v->color[0]; }
    void setClearColorR(ClearValue* v, float r) { v->color[0] = r; }

    float getClearColorG(const ClearValue* v) { return v->color[1]; }
    void setClearColorG(ClearValue* v, float g) { v->color[1] = g; }

    float getClearColorB(const ClearValue* v) { return v->color[2]; }
    void setClearColorB(ClearValue* v, float b) { v->color[2] = b; }

    float getClearColorA(const ClearValue* v) { return v->color[3]; }
    void setClearColorA(ClearValue* v, float a) { v->color[3] = a; }

    float getClearDepth(const ClearValue* v) { return v->depthStencil.depth; }
    void setClearDepth(ClearValue* v, float d) { v->depthStencil.depth = d; }

    std::uint32_t getClearStencil(const ClearValue* v) { return v->depthStencil.stencil; }
    void setClearStencil(ClearValue* v, std::uint32_t s) { v->depthStencil.stencil = s; }

    void DS_updateBufferByIndex(DescriptorSet& self,
                                const DescriptorIndex index,
                                const std::shared_ptr<const Buffer>& buffer) {
        self.update(index, buffer);
    }

    void DS_updateBufferDynamic(DescriptorSet& self, const std::shared_ptr<const Buffer>& buffer) {
        self.update(buffer);
    }

    void DS_updateImageByIndex(DescriptorSet& self, DescriptorIndex index, const std::shared_ptr<const Image>& image) {
        self.update(index, image);
    }

    void DS_updateImageByRef(DescriptorSet& self, DescriptorIndex index, const Image& image) {
        self.update(index, image);
    }

    void DS_updateSamplerByIndex(DescriptorSet& self,
                                 const DescriptorIndex index,
                                 const std::shared_ptr<const Sampler>& sampler) {
        self.update(index, sampler);
    }

    void DS_updateSamplerByRef(DescriptorSet& self,const  DescriptorIndex index, const Sampler& sampler) {
        self.update(index, sampler);
    }

    void DS_updateImageArray(
        DescriptorSet& self,
        const DescriptorIndex index,
        const std::vector<std::shared_ptr<Image>>& images) {
        self.update(index, images);
    }

    void DS_updateBufferArray(
        DescriptorSet& self,
        const DescriptorIndex index,
        const std::vector<std::shared_ptr<Buffer>>& buffers) {
        self.update(index, buffers);
    }

    void DS_updateSamplerArray(
        DescriptorSet& self,
        const DescriptorIndex index,
        const std::vector<std::shared_ptr<Sampler>>& samplers) {
        self.update(index, samplers);
    }

    std::shared_ptr<CommandList> CA_createCL(CommandAllocator& self) {
        return self.createCommandList();
    }

    std::shared_ptr<CommandList> CA_createCLWithPipeline(CommandAllocator& self,
                                                         const std::shared_ptr<const Pipeline>& p) {
        return self.createCommandList(p);
    }

    void SQ_submitFenceSwap(
        const SubmitQueue& self,
        const std::shared_ptr<Fence>& fence,
        const std::shared_ptr<const SwapChain>& swapChain,
        const std::vector<std::shared_ptr<const CommandList>>& cls) {
        self.submit(fence, swapChain, cls);
    }

    void SQ_submitNoSync(
        const SubmitQueue& self,
        const std::vector<std::shared_ptr<const CommandList>>& cls) {
        self.submit(cls);
    }

    void SQ_submitFenceOnly(
        const SubmitQueue& self,
        const std::shared_ptr<Fence>& fence,
        const std::vector<std::shared_ptr<const CommandList>>& cls) {
        self.submit(fence, cls);
    }

    void SQ_submitWaitFenceSwap(
        const SubmitQueue& self,
        const std::shared_ptr<Semaphore>& waitSemaphore,
        const WaitStage waitStage,
        const std::shared_ptr<Fence>& fence,
        const std::shared_ptr<const SwapChain>& swapChain,
        const std::vector<std::shared_ptr<const CommandList>>& cls) {
        self.submit(waitSemaphore, waitStage, fence, swapChain, cls);
    }

    void SQ_submitWaitSignal(
        const SubmitQueue& self,
        const std::shared_ptr<Semaphore>& waitSemaphore,
        const WaitStage waitStage,
        const WaitStage signalStage,
        const std::shared_ptr<Semaphore>& signalSemaphore,
        const std::vector<std::shared_ptr<const CommandList>>& cls) {
        self.submit(waitSemaphore, waitStage, signalStage, signalSemaphore, cls);
    }

    void SQ_submitWaitOnly(
        const SubmitQueue& self,
        const std::shared_ptr<Semaphore>& waitSemaphore,
        const WaitStage waitStage,
        const std::vector<std::shared_ptr<const CommandList>>& cls) {
        self.submit(waitSemaphore, waitStage, cls);
    }

    void SQ_submitSignalOnly(
        const SubmitQueue& self,
        const WaitStage signalStage,
        const std::shared_ptr<Semaphore>& signalSemaphore,
        const std::vector<std::shared_ptr<const CommandList>>& cls) {
        self.submit(signalStage, signalSemaphore, cls);
    }

    std::shared_ptr<RenderTarget> Vireo_createRTBasic(
        const Vireo& self,
        const ImageFormat format,
        const std::uint32_t width,
        const std::uint32_t height,
        const RenderTargetType type,
        const ClearValue clearValue,
        const std::uint32_t arraySize,
        const MSAA msaa,
        const std::string& name) {
        return self.createRenderTarget(format, width, height, type, clearValue, arraySize, msaa, name);
    }

    std::shared_ptr<RenderTarget> Vireo_createRTFromSwap(
        const Vireo& self,
        const std::shared_ptr<const SwapChain>& swapChain,
        const ClearValue clearValue,
        const MSAA msaa,
        const std::string& name) {
        return self.createRenderTarget(swapChain, clearValue, msaa, name);
    }

    std::shared_ptr<Vireo> Vireo_createDefault(const Backend backend) {
        return Vireo::create(backend);
    }

    void LuaBindings::_register(lua_State* L) {
        luabridge::getGlobalNamespace(L).beginNamespace("vireo")
        // enum classes
            .beginNamespace("Backend")
                .addProperty("UNDEFINED", []{ return (int)Backend::UNDEFINED; })
                .addProperty("DIRECTX",   []{ return (int)Backend::DIRECTX; })
                .addProperty("VULKAN",    []{ return (int)Backend::VULKAN; })
            .endNamespace()
            .beginNamespace("Filter")
                .addProperty("NEAREST", []{ return (int)Filter::NEAREST; })
                .addProperty("LINEAR",  []{ return (int)Filter::LINEAR; })
            .endNamespace()
            .beginNamespace("AddressMode")
                .addProperty("REPEAT",          []{ return (int)AddressMode::REPEAT; })
                .addProperty("MIRRORED_REPEAT", []{ return (int)AddressMode::MIRRORED_REPEAT; })
                .addProperty("CLAMP_TO_EDGE",   []{ return (int)AddressMode::CLAMP_TO_EDGE; })
                .addProperty("CLAMP_TO_BORDER", []{ return (int)AddressMode::CLAMP_TO_BORDER; })
            .endNamespace()
            .beginNamespace("MipMapMode")
                .addProperty("NEAREST", []{ return (int)MipMapMode::NEAREST; })
                .addProperty("LINEAR",  []{ return (int)MipMapMode::LINEAR; })
            .endNamespace()
            .beginNamespace("ImageFormat")
                .addProperty("R8_UNORM",               []{ return (int)ImageFormat::R8_UNORM; })
                .addProperty("R8_SNORM",               []{ return (int)ImageFormat::R8_SNORM; })
                .addProperty("R8_UINT",                []{ return (int)ImageFormat::R8_UINT; })
                .addProperty("R8_SINT",                []{ return (int)ImageFormat::R8_SINT; })

                .addProperty("R8G8_UNORM",             []{ return (int)ImageFormat::R8G8_UNORM; })
                .addProperty("R8G8_SNORM",             []{ return (int)ImageFormat::R8G8_SNORM; })
                .addProperty("R8G8_UINT",              []{ return (int)ImageFormat::R8G8_UINT; })
                .addProperty("R8G8_SINT",              []{ return (int)ImageFormat::R8G8_SINT; })

                .addProperty("R8G8B8A8_UNORM",         []{ return (int)ImageFormat::R8G8B8A8_UNORM; })
                .addProperty("R8G8B8A8_SNORM",         []{ return (int)ImageFormat::R8G8B8A8_SNORM; })
                .addProperty("R8G8B8A8_UINT",          []{ return (int)ImageFormat::R8G8B8A8_UINT; })
                .addProperty("R8G8B8A8_SINT",          []{ return (int)ImageFormat::R8G8B8A8_SINT; })
                .addProperty("R8G8B8A8_SRGB",          []{ return (int)ImageFormat::R8G8B8A8_SRGB; })

                .addProperty("B8G8R8A8_UNORM",         []{ return (int)ImageFormat::B8G8R8A8_UNORM; })
                .addProperty("B8G8R8A8_SRGB",          []{ return (int)ImageFormat::B8G8R8A8_SRGB; })
                .addProperty("B8G8R8X8_UNORM",         []{ return (int)ImageFormat::B8G8R8X8_UNORM; })
                .addProperty("B8G8R8X8_SRGB",          []{ return (int)ImageFormat::B8G8R8X8_SRGB; })

                .addProperty("A2B10G10R10_UNORM",      []{ return (int)ImageFormat::A2B10G10R10_UNORM; })
                .addProperty("A2B10G10R10_UINT",       []{ return (int)ImageFormat::A2B10G10R10_UINT; })

                .addProperty("R16_UNORM",              []{ return (int)ImageFormat::R16_UNORM; })
                .addProperty("R16_SNORM",              []{ return (int)ImageFormat::R16_SNORM; })
                .addProperty("R16_UINT",               []{ return (int)ImageFormat::R16_UINT; })
                .addProperty("R16_SINT",               []{ return (int)ImageFormat::R16_SINT; })
                .addProperty("R16_SFLOAT",             []{ return (int)ImageFormat::R16_SFLOAT; })

                .addProperty("R16G16_UNORM",           []{ return (int)ImageFormat::R16G16_UNORM; })
                .addProperty("R16G16_SNORM",           []{ return (int)ImageFormat::R16G16_SNORM; })
                .addProperty("R16G16_UINT",            []{ return (int)ImageFormat::R16G16_UINT; })
                .addProperty("R16G16_SINT",            []{ return (int)ImageFormat::R16G16_SINT; })
                .addProperty("R16G16_SFLOAT",          []{ return (int)ImageFormat::R16G16_SFLOAT; })

                .addProperty("R16G16B16A16_UNORM",     []{ return (int)ImageFormat::R16G16B16A16_UNORM; })
                .addProperty("R16G16B16A16_SNORM",     []{ return (int)ImageFormat::R16G16B16A16_SNORM; })
                .addProperty("R16G16B16A16_UINT",      []{ return (int)ImageFormat::R16G16B16A16_UINT; })
                .addProperty("R16G16B16A16_SINT",      []{ return (int)ImageFormat::R16G16B16A16_SINT; })
                .addProperty("R16G16B16A16_SFLOAT",    []{ return (int)ImageFormat::R16G16B16A16_SFLOAT; })

                .addProperty("R32_UINT",               []{ return (int)ImageFormat::R32_UINT; })
                .addProperty("R32_SINT",               []{ return (int)ImageFormat::R32_SINT; })
                .addProperty("R32_SFLOAT",             []{ return (int)ImageFormat::R32_SFLOAT; })

                .addProperty("R32G32_UINT",            []{ return (int)ImageFormat::R32G32_UINT; })
                .addProperty("R32G32_SINT",            []{ return (int)ImageFormat::R32G32_SINT; })
                .addProperty("R32G32_SFLOAT",          []{ return (int)ImageFormat::R32G32_SFLOAT; })

                .addProperty("R32G32B32_UINT",         []{ return (int)ImageFormat::R32G32B32_UINT; })
                .addProperty("R32G32B32_SINT",         []{ return (int)ImageFormat::R32G32B32_SINT; })
                .addProperty("R32G32B32_SFLOAT",       []{ return (int)ImageFormat::R32G32B32_SFLOAT; })

                .addProperty("R32G32B32A32_UINT",      []{ return (int)ImageFormat::R32G32B32A32_UINT; })
                .addProperty("R32G32B32A32_SINT",      []{ return (int)ImageFormat::R32G32B32A32_SINT; })
                .addProperty("R32G32B32A32_SFLOAT",    []{ return (int)ImageFormat::R32G32B32A32_SFLOAT; })

                .addProperty("D16_UNORM",              []{ return (int)ImageFormat::D16_UNORM; })
                .addProperty("D24_UNORM_S8_UINT",      []{ return (int)ImageFormat::D24_UNORM_S8_UINT; })
                .addProperty("D32_SFLOAT",             []{ return (int)ImageFormat::D32_SFLOAT; })
                .addProperty("D32_SFLOAT_S8_UINT",     []{ return (int)ImageFormat::D32_SFLOAT_S8_UINT; })

                .addProperty("BC1_UNORM",              []{ return (int)ImageFormat::BC1_UNORM; })
                .addProperty("BC1_UNORM_SRGB",         []{ return (int)ImageFormat::BC1_UNORM_SRGB; })
                .addProperty("BC2_UNORM",              []{ return (int)ImageFormat::BC2_UNORM; })
                .addProperty("BC2_UNORM_SRGB",         []{ return (int)ImageFormat::BC2_UNORM_SRGB; })
                .addProperty("BC3_UNORM",              []{ return (int)ImageFormat::BC3_UNORM; })
                .addProperty("BC3_UNORM_SRGB",         []{ return (int)ImageFormat::BC3_UNORM_SRGB; })
                .addProperty("BC4_UNORM",              []{ return (int)ImageFormat::BC4_UNORM; })
                .addProperty("BC4_SNORM",              []{ return (int)ImageFormat::BC4_SNORM; })
                .addProperty("BC5_UNORM",              []{ return (int)ImageFormat::BC5_UNORM; })
                .addProperty("BC5_SNORM",              []{ return (int)ImageFormat::BC5_SNORM; })
                .addProperty("BC6H_UFLOAT",            []{ return (int)ImageFormat::BC6H_UFLOAT; })
                .addProperty("BC6H_SFLOAT",            []{ return (int)ImageFormat::BC6H_SFLOAT; })
                .addProperty("BC7_UNORM",              []{ return (int)ImageFormat::BC7_UNORM; })
                .addProperty("BC7_UNORM_SRGB",         []{ return (int)ImageFormat::BC7_UNORM_SRGB; })
            .endNamespace()
            .beginNamespace("BufferType")
                .addProperty("VERTEX",           []{ return (int)BufferType::VERTEX; })
                .addProperty("INDEX",            []{ return (int)BufferType::INDEX; })
                .addProperty("INDIRECT",         []{ return (int)BufferType::INDIRECT; })
                .addProperty("UNIFORM",          []{ return (int)BufferType::UNIFORM; })
                .addProperty("STORAGE",          []{ return (int)BufferType::STORAGE; })
                .addProperty("DEVICE_STORAGE",   []{ return (int)BufferType::DEVICE_STORAGE; })
                .addProperty("READWRITE_STORAGE",[]{ return (int)BufferType::READWRITE_STORAGE; })
                .addProperty("BUFFER_UPLOAD",    []{ return (int)BufferType::BUFFER_UPLOAD; })
                .addProperty("BUFFER_DOWNLOAD",  []{ return (int)BufferType::BUFFER_DOWNLOAD; })
                .addProperty("IMAGE_UPLOAD",     []{ return (int)BufferType::IMAGE_UPLOAD; })
                .addProperty("IMAGE_DOWNLOAD",   []{ return (int)BufferType::IMAGE_DOWNLOAD; })
            .endNamespace()
            .beginNamespace("IndexType")
                .addProperty("UINT16", []{ return (int)IndexType::UINT16; })
                .addProperty("UINT32", []{ return (int)IndexType::UINT32; })
            .endNamespace()
            .beginNamespace("DescriptorType")
                .addProperty("UNIFORM",           []{ return (int)DescriptorType::UNIFORM; })
                .addProperty("UNIFORM_DYNAMIC",   []{ return (int)DescriptorType::UNIFORM_DYNAMIC; })
                .addProperty("STORAGE",           []{ return (int)DescriptorType::STORAGE; })
                .addProperty("DEVICE_STORAGE",    []{ return (int)DescriptorType::DEVICE_STORAGE; })
                .addProperty("READWRITE_STORAGE", []{ return (int)DescriptorType::READWRITE_STORAGE; })
                .addProperty("SAMPLED_IMAGE",     []{ return (int)DescriptorType::SAMPLED_IMAGE; })
                .addProperty("SAMPLER",           []{ return (int)DescriptorType::SAMPLER; })
                .addProperty("READWRITE_IMAGE",   []{ return (int)DescriptorType::READWRITE_IMAGE; })
            .endNamespace()
            .beginNamespace("CommandType")
                .addProperty("GRAPHIC",  []{ return (int)CommandType::GRAPHIC; })
                .addProperty("TRANSFER", []{ return (int)CommandType::TRANSFER; })
                .addProperty("COMPUTE",  []{ return (int)CommandType::COMPUTE; })
            .endNamespace()
            .beginNamespace("AttributeFormat")
                .addProperty("R32_FLOAT",          []{ return (int)AttributeFormat::R32_FLOAT; })
                .addProperty("R32G32_FLOAT",       []{ return (int)AttributeFormat::R32G32_FLOAT; })
                .addProperty("R32G32B32_FLOAT",    []{ return (int)AttributeFormat::R32G32B32_FLOAT; })
                .addProperty("R32G32B32A32_FLOAT", []{ return (int)AttributeFormat::R32G32B32A32_FLOAT; })
                .addProperty("R32_SINT",           []{ return (int)AttributeFormat::R32_SINT; })
                .addProperty("R32G32_SINT",        []{ return (int)AttributeFormat::R32G32_SINT; })
                .addProperty("R32G32B32_SINT",     []{ return (int)AttributeFormat::R32G32B32_SINT; })
                .addProperty("R32G32B32A32_SINT",  []{ return (int)AttributeFormat::R32G32B32A32_SINT; })
                .addProperty("R32_UINT",           []{ return (int)AttributeFormat::R32_UINT; })
                .addProperty("R32G32_UINT",        []{ return (int)AttributeFormat::R32G32_UINT; })
                .addProperty("R32G32B32_UINT",     []{ return (int)AttributeFormat::R32G32B32_UINT; })
                .addProperty("R32G32B32A32_UINT",  []{ return (int)AttributeFormat::R32G32B32A32_UINT; })
            .endNamespace()
            .beginNamespace("RenderTargetType")
                .addProperty("COLOR",          []{ return (int)RenderTargetType::COLOR; })
                .addProperty("DEPTH",          []{ return (int)RenderTargetType::DEPTH; })
                .addProperty("DEPTH_STENCIL",  []{ return (int)RenderTargetType::DEPTH_STENCIL; })
            .endNamespace()
            .beginNamespace("CullMode")
                .addProperty("NONE",  []{ return (int)CullMode::NONE; })
                .addProperty("FRONT", []{ return (int)CullMode::FRONT; })
                .addProperty("BACK",  []{ return (int)CullMode::BACK; })
            .endNamespace()
            .beginNamespace("PrimitiveTopology")
                .addProperty("POINT_LIST",     []{ return (int)PrimitiveTopology::POINT_LIST; })
                .addProperty("LINE_LIST",      []{ return (int)PrimitiveTopology::LINE_LIST; })
                .addProperty("LINE_STRIP",     []{ return (int)PrimitiveTopology::LINE_STRIP; })
                .addProperty("TRIANGLE_LIST",  []{ return (int)PrimitiveTopology::TRIANGLE_LIST; })
                .addProperty("TRIANGLE_STRIP", []{ return (int)PrimitiveTopology::TRIANGLE_STRIP; })
            .endNamespace()
            .beginNamespace("PolygonMode")
                .addProperty("FILL",      []{ return (int)PolygonMode::FILL; })
                .addProperty("WIREFRAME", []{ return (int)PolygonMode::WIREFRAME; })
            .endNamespace()
            .beginNamespace("CompareOp")
                .addProperty("NEVER",            []{ return (int)CompareOp::NEVER; })
                .addProperty("LESS",             []{ return (int)CompareOp::LESS; })
                .addProperty("EQUAL",            []{ return (int)CompareOp::EQUAL; })
                .addProperty("LESS_OR_EQUAL",    []{ return (int)CompareOp::LESS_OR_EQUAL; })
                .addProperty("GREATER",          []{ return (int)CompareOp::GREATER; })
                .addProperty("NOT_EQUAL",        []{ return (int)CompareOp::NOT_EQUAL; })
                .addProperty("GREATER_OR_EQUAL", []{ return (int)CompareOp::GREATER_OR_EQUAL; })
                .addProperty("ALWAYS",           []{ return (int)CompareOp::ALWAYS; })
            .endNamespace()
            .beginNamespace("StencilOp")
                .addProperty("KEEP",                 []{ return (int)StencilOp::KEEP; })
                .addProperty("ZERO",                 []{ return (int)StencilOp::ZERO; })
                .addProperty("REPLACE",              []{ return (int)StencilOp::REPLACE; })
                .addProperty("INCREMENT_AND_CLAMP",  []{ return (int)StencilOp::INCREMENT_AND_CLAMP; })
                .addProperty("DECREMENT_AND_CLAMP",  []{ return (int)StencilOp::DECREMENT_AND_CLAMP; })
                .addProperty("INVERT",               []{ return (int)StencilOp::INVERT; })
                .addProperty("INCREMENT_AND_WRAP",   []{ return (int)StencilOp::INCREMENT_AND_WRAP; })
                .addProperty("DECREMENT_AND_WRAP",   []{ return (int)StencilOp::DECREMENT_AND_WRAP; })
            .endNamespace()
            .beginNamespace("BlendFactor")
                .addProperty("ZERO",                    []{ return (int)BlendFactor::ZERO; })
                .addProperty("ONE",                     []{ return (int)BlendFactor::ONE; })
                .addProperty("SRC_COLOR",               []{ return (int)BlendFactor::SRC_COLOR; })
                .addProperty("ONE_MINUS_SRC_COLOR",     []{ return (int)BlendFactor::ONE_MINUS_SRC_COLOR; })
                .addProperty("DST_COLOR",               []{ return (int)BlendFactor::DST_COLOR; })
                .addProperty("ONE_MINUS_DST_COLOR",     []{ return (int)BlendFactor::ONE_MINUS_DST_COLOR; })
                .addProperty("SRC_ALPHA",               []{ return (int)BlendFactor::SRC_ALPHA; })
                .addProperty("ONE_MINUS_SRC_ALPHA",     []{ return (int)BlendFactor::ONE_MINUS_SRC_ALPHA; })
                .addProperty("DST_ALPHA",               []{ return (int)BlendFactor::DST_ALPHA; })
                .addProperty("ONE_MINUS_DST_ALPHA",     []{ return (int)BlendFactor::ONE_MINUS_DST_ALPHA; })
                .addProperty("CONSTANT_COLOR",          []{ return (int)BlendFactor::CONSTANT_COLOR; })
                .addProperty("ONE_MINUS_CONSTANT_COLOR",[]{ return (int)BlendFactor::ONE_MINUS_CONSTANT_COLOR; })
                .addProperty("CONSTANT_ALPHA",          []{ return (int)BlendFactor::CONSTANT_ALPHA; })
                .addProperty("ONE_MINUS_CONSTANT_ALPHA",[]{ return (int)BlendFactor::ONE_MINUS_CONSTANT_ALPHA; })
                .addProperty("SRC_ALPHA_SATURATE",      []{ return (int)BlendFactor::SRC_ALPHA_SATURATE; })
                .addProperty("SRC1_COLOR",              []{ return (int)BlendFactor::SRC1_COLOR; })
                .addProperty("ONE_MINUS_SRC1_COLOR",    []{ return (int)BlendFactor::ONE_MINUS_SRC1_COLOR; })
                .addProperty("SRC1_ALPHA",              []{ return (int)BlendFactor::SRC1_ALPHA; })
                .addProperty("ONE_MINUS_SRC1_ALPHA",    []{ return (int)BlendFactor::ONE_MINUS_SRC1_ALPHA; })
            .endNamespace()
            .beginNamespace("BlendOp")
                .addProperty("ADD",              []{ return (int)BlendOp::ADD; })
                .addProperty("SUBTRACT",         []{ return (int)BlendOp::SUBTRACT; })
                .addProperty("REVERSE_SUBTRACT", []{ return (int)BlendOp::REVERSE_SUBTRACT; })
                .addProperty("MIN",              []{ return (int)BlendOp::MIN; })
                .addProperty("MAX",              []{ return (int)BlendOp::MAX; })
            .endNamespace()
            .beginNamespace("LogicOp")
                .addProperty("CLEAR",         []{ return (int)LogicOp::CLEAR; })
                .addProperty("SET",           []{ return (int)LogicOp::SET; })
                .addProperty("COPY",          []{ return (int)LogicOp::COPY; })
                .addProperty("COPY_INVERTED", []{ return (int)LogicOp::COPY_INVERTED; })
                .addProperty("NOOP",          []{ return (int)LogicOp::NOOP; })
                .addProperty("INVERT",        []{ return (int)LogicOp::INVERT; })
                .addProperty("AND",           []{ return (int)LogicOp::AND; })
                .addProperty("NAND",          []{ return (int)LogicOp::NAND; })
                .addProperty("OR",            []{ return (int)LogicOp::OR; })
                .addProperty("NOR",           []{ return (int)LogicOp::NOR; })
                .addProperty("XOR",           []{ return (int)LogicOp::XOR; })
                .addProperty("EQUIV",         []{ return (int)LogicOp::EQUIV; })
                .addProperty("AND_REVERSE",   []{ return (int)LogicOp::AND_REVERSE; })
                .addProperty("AND_INVERTED",  []{ return (int)LogicOp::AND_INVERTED; })
                .addProperty("OR_REVERSE",    []{ return (int)LogicOp::OR_REVERSE; })
                .addProperty("OR_INVERTED",   []{ return (int)LogicOp::OR_INVERTED; })
            .endNamespace()
            .beginNamespace("ColorWriteMask")
                .addProperty("RED",   []{ return (int)ColorWriteMask::RED; })
                .addProperty("GREEN", []{ return (int)ColorWriteMask::GREEN; })
                .addProperty("BLUE",  []{ return (int)ColorWriteMask::BLUE; })
                .addProperty("ALPHA", []{ return (int)ColorWriteMask::ALPHA; })
                .addProperty("ALL",   []{ return (int)ColorWriteMask::ALL; })
            .endNamespace()
            .beginNamespace("ShaderStage")
                .addProperty("ALL",      []{ return (int)ShaderStage::ALL; })
                .addProperty("VERTEX",   []{ return (int)ShaderStage::VERTEX; })
                .addProperty("FRAGMENT", []{ return (int)ShaderStage::FRAGMENT; })
                .addProperty("HULL",     []{ return (int)ShaderStage::HULL; })
                .addProperty("DOMAIN",   []{ return (int)ShaderStage::DOMAIN; })
                .addProperty("GEOMETRY", []{ return (int)ShaderStage::GEOMETRY; })
                .addProperty("COMPUTE",  []{ return (int)ShaderStage::COMPUTE; })
            .endNamespace()
            .beginNamespace("WaitStage")
                .addProperty("NONE",                                  []{ return (int)WaitStage::NONE; })
                .addProperty("PIPELINE_TOP",                          []{ return (int)WaitStage::PIPELINE_TOP; })
                .addProperty("VERTEX_INPUT",                          []{ return (int)WaitStage::VERTEX_INPUT; })
                .addProperty("VERTEX_SHADER",                         []{ return (int)WaitStage::VERTEX_SHADER; })
                .addProperty("DEPTH_STENCIL_TEST_BEFORE_FRAGMENT_SHADER", []{ return (int)WaitStage::DEPTH_STENCIL_TEST_BEFORE_FRAGMENT_SHADER; })
                .addProperty("FRAGMENT_SHADER",                       []{ return (int)WaitStage::FRAGMENT_SHADER; })
                .addProperty("DEPTH_STENCIL_TEST_AFTER_FRAGMENT_SHADER",  []{ return (int)WaitStage::DEPTH_STENCIL_TEST_AFTER_FRAGMENT_SHADER; })
                .addProperty("COLOR_OUTPUT",                          []{ return (int)WaitStage::COLOR_OUTPUT; })
                .addProperty("COMPUTE_SHADER",                        []{ return (int)WaitStage::COMPUTE_SHADER; })
                .addProperty("TRANSFER",                              []{ return (int)WaitStage::TRANSFER; })
                .addProperty("PIPELINE_BOTTOM",                       []{ return (int)WaitStage::PIPELINE_BOTTOM; })
                .addProperty("ALL_GRAPHICS",                          []{ return (int)WaitStage::ALL_GRAPHICS; })
                .addProperty("ALL_COMMANDS",                          []{ return (int)WaitStage::ALL_COMMANDS; })
                .addProperty("COPY",                                   []{ return (int)WaitStage::COPY; })
                .addProperty("RESOLV",                                 []{ return (int)WaitStage::RESOLV; })
                .addProperty("BLIT",                                   []{ return (int)WaitStage::BLIT; })
                .addProperty("CLEAR",                                  []{ return (int)WaitStage::CLEAR; })
            .endNamespace()
            .beginNamespace("ResourceState")
                .addProperty("UNDEFINED",                    []{ return (int)ResourceState::UNDEFINED; })
                .addProperty("GENERAL",                      []{ return (int)ResourceState::GENERAL; })
                .addProperty("RENDER_TARGET_COLOR",          []{ return (int)ResourceState::RENDER_TARGET_COLOR; })
                .addProperty("RENDER_TARGET_DEPTH",          []{ return (int)ResourceState::RENDER_TARGET_DEPTH; })
                .addProperty("RENDER_TARGET_DEPTH_READ",     []{ return (int)ResourceState::RENDER_TARGET_DEPTH_READ; })
                .addProperty("RENDER_TARGET_DEPTH_STENCIL",  []{ return (int)ResourceState::RENDER_TARGET_DEPTH_STENCIL; })
                .addProperty("RENDER_TARGET_DEPTH_STENCIL_READ", []{ return (int)ResourceState::RENDER_TARGET_DEPTH_STENCIL_READ; })
                .addProperty("DISPATCH_TARGET",              []{ return (int)ResourceState::DISPATCH_TARGET; })
                .addProperty("PRESENT",                      []{ return (int)ResourceState::PRESENT; })
                .addProperty("COPY_SRC",                     []{ return (int)ResourceState::COPY_SRC; })
                .addProperty("COPY_DST",                     []{ return (int)ResourceState::COPY_DST; })
                .addProperty("SHADER_READ",                  []{ return (int)ResourceState::SHADER_READ; })
                .addProperty("COMPUTE_READ",                 []{ return (int)ResourceState::COMPUTE_READ; })
                .addProperty("COMPUTE_WRITE",                []{ return (int)ResourceState::COMPUTE_WRITE; })
                .addProperty("INDIRECT_DRAW",                []{ return (int)ResourceState::INDIRECT_DRAW; })
                .addProperty("VERTEX_INPUT",                 []{ return (int)ResourceState::VERTEX_INPUT; })
                .addProperty("UNIFORM",                      []{ return (int)ResourceState::UNIFORM; })
            .endNamespace()
            .beginNamespace("MSAA")
                .addProperty("NONE", []{ return (int)MSAA::NONE; })
                .addProperty("X2",   []{ return (int)MSAA::X2; })
                .addProperty("X4",   []{ return (int)MSAA::X4; })
                .addProperty("X8",   []{ return (int)MSAA::X8; })
                .addProperty("X16",  []{ return (int)MSAA::X16; })
                .addProperty("X32",  []{ return (int)MSAA::X32; })
                .addProperty("X64",  []{ return (int)MSAA::X64; })
            .endNamespace()
            .beginNamespace("PresentMode")
                .addProperty("IMMEDIATE", []{ return (int)PresentMode::IMMEDIATE; })
                .addProperty("VSYNC",     []{ return (int)PresentMode::VSYNC; })
            .endNamespace()
            .beginNamespace("PipelineType")
                .addProperty("GRAPHIC", []{ return (int)PipelineType::GRAPHIC; })
                .addProperty("COMPUTE", []{ return (int)PipelineType::COMPUTE; })
            .endNamespace()
            .beginNamespace("SemaphoreType")
                .addProperty("BINARY",   []{ return (int)SemaphoreType::BINARY; })
                .addProperty("TIMELINE", []{ return (int)SemaphoreType::TIMELINE; })
            .endNamespace()

        // structs
            .beginClass<ColorBlendDesc>("ColorBlendDesc")
                .addConstructor<void(*)()>()
                .addProperty("blendEnable", &ColorBlendDesc::blendEnable)
                .addProperty("srcColorBlendFactor", &ColorBlendDesc::srcColorBlendFactor)
                .addProperty("dstColorBlendFactor", &ColorBlendDesc::dstColorBlendFactor)
                .addProperty("colorBlendOp", &ColorBlendDesc::colorBlendOp)
                .addProperty("srcAlphaBlendFactor", &ColorBlendDesc::srcAlphaBlendFactor)
                .addProperty("dstAlphaBlendFactor", &ColorBlendDesc::dstAlphaBlendFactor)
                .addProperty("alphaBlendOp", &ColorBlendDesc::alphaBlendOp)
                .addProperty("colorWriteMask", &ColorBlendDesc::colorWriteMask)
            .endClass()
            .beginClass<StencilOpState>("StencilOpState")
                .addConstructor<void(*)()>()
                .addProperty("failOp", &StencilOpState::failOp)
                .addProperty("passOp", &StencilOpState::passOp)
                .addProperty("depthFailOp", &StencilOpState::depthFailOp)
                .addProperty("compareOp", &StencilOpState::compareOp)
                .addProperty("compareMask", &StencilOpState::compareMask)
                .addProperty("writeMask", &StencilOpState::writeMask)
            .endClass()
            .beginClass<PhysicalDeviceDesc>("PhysicalDeviceDesc")
                .addConstructor<void(*)()>()
                .addProperty("name", &PhysicalDeviceDesc::name)
                .addProperty("dedicatedVideoMemory", &PhysicalDeviceDesc::dedicatedVideoMemory)
                .addProperty("dedicatedSystemMemory", &PhysicalDeviceDesc::dedicatedSystemMemory)
                .addProperty("sharedSystemMemory", &PhysicalDeviceDesc::sharedSystemMemory)
            .endClass()
            .beginClass<Extent>("Extent")
                .addConstructor<void(*)()>()
                .addProperty("width", &Extent::width)
                .addProperty("height", &Extent::height)
            .endClass()
            .beginClass<Rect>("Rect")
                .addConstructor<void(*)()>()
                .addProperty("x", &Rect::x)
                .addProperty("y", &Rect::y)
                .addProperty("width", &Rect::width)
                .addProperty("height", &Rect::height)
            .endClass()
            .beginClass<Viewport>("Viewport")
                .addConstructor<void(*)()>()
                .addProperty("x", &Viewport::x)
                .addProperty("y", &Viewport::y)
                .addProperty("width", &Viewport::width)
                .addProperty("height", &Viewport::height)
                .addProperty("minDepth", &Viewport::minDepth)
                .addProperty("maxDepth", &Viewport::maxDepth)
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
                .addProperty("r", &getClearColorR, &setClearColorR)
                .addProperty("g", &getClearColorG, &setClearColorG)
                .addProperty("b", &getClearColorB, &setClearColorB)
                .addProperty("a", &getClearColorA, &setClearColorA)
                .addProperty("depth", &getClearDepth, &setClearDepth)
                .addProperty("stencil", &getClearStencil, &setClearStencil)
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
                .addProperty("swapChain", &RenderTargetDesc::swapChain)
                .addProperty("renderTarget", &RenderTargetDesc::renderTarget)
                .addProperty("multisampledRenderTarget", &RenderTargetDesc::multisampledRenderTarget)
                .addProperty("clear", &RenderTargetDesc::clear)
                .addProperty("clearValue", &RenderTargetDesc::clearValue)
                .addProperty("discardAfterRender", &RenderTargetDesc::discardAfterRender)
            .endClass()
            .beginClass<RenderingConfiguration>("RenderingConfiguration")
                .addConstructor<void(*)()>()
                .addProperty("colorRenderTargets", &RenderingConfiguration::colorRenderTargets)
                .addProperty("depthStencilRenderTarget", &RenderingConfiguration::depthStencilRenderTarget)
                .addProperty("multisampledDepthStencilRenderTarget", &RenderingConfiguration::multisampledDepthStencilRenderTarget)
                .addProperty("depthTestEnable", &RenderingConfiguration::depthTestEnable)
                .addProperty("stencilTestEnable", &RenderingConfiguration::stencilTestEnable)
                .addProperty("clearDepthStencil", &RenderingConfiguration::clearDepthStencil)
                .addProperty("depthStencilClearValue", &RenderingConfiguration::depthStencilClearValue)
                .addProperty("discardDepthStencilAfterRender", &RenderingConfiguration::discardDepthStencilAfterRender)
            .endClass()
            .beginClass<DrawIndirectCommand>("DrawIndirectCommand")
                .addConstructor<void(*)()>()
                .addProperty("vertexCount", &DrawIndirectCommand::vertexCount)
                .addProperty("instanceCount", &DrawIndirectCommand::instanceCount)
                .addProperty("firstVertex", &DrawIndirectCommand::firstVertex)
                .addProperty("firstInstance", &DrawIndirectCommand::firstInstance)
            .endClass()
            .beginClass<DrawIndexedIndirectCommand>("DrawIndexedIndirectCommand")
                .addConstructor<void(*)()>()
                .addProperty("indexCount", &DrawIndexedIndirectCommand::indexCount)
                .addProperty("instanceCount", &DrawIndexedIndirectCommand::instanceCount)
                .addProperty("firstIndex", &DrawIndexedIndirectCommand::firstIndex)
                .addProperty("vertexOffset", &DrawIndexedIndirectCommand::vertexOffset)
                .addProperty("firstInstance", &DrawIndexedIndirectCommand::firstInstance)
            .endClass()
            .beginClass<BufferCopyRegion>("BufferCopyRegion")
                .addConstructor<void(*)()>()
                .addProperty("srcOffset", &BufferCopyRegion::srcOffset)
                .addProperty("dstOffset", &BufferCopyRegion::dstOffset)
                .addProperty("size", &BufferCopyRegion::size)
            .endClass()
            .beginClass<GraphicPipelineConfiguration>("GraphicPipelineConfiguration")
                .addConstructor<void(*)()>()
                .addProperty("resources", &GraphicPipelineConfiguration::resources)
                .addProperty("colorRenderFormats", &GraphicPipelineConfiguration::colorRenderFormats)
                .addProperty("colorBlendDesc", &GraphicPipelineConfiguration::colorBlendDesc)
                .addProperty("vertexInputLayout", &GraphicPipelineConfiguration::vertexInputLayout)
                .addProperty("vertexShader", &GraphicPipelineConfiguration::vertexShader)
                .addProperty("fragmentShader", &GraphicPipelineConfiguration::fragmentShader)
                .addProperty("hullShader", &GraphicPipelineConfiguration::hullShader)
                .addProperty("domainShader", &GraphicPipelineConfiguration::domainShader)
                .addProperty("geometryShader", &GraphicPipelineConfiguration::geometryShader)
                .addProperty("primitiveTopology", &GraphicPipelineConfiguration::primitiveTopology)
                .addProperty("msaa", &GraphicPipelineConfiguration::msaa)
                .addProperty("cullMode", &GraphicPipelineConfiguration::cullMode)
                .addProperty("polygonMode", &GraphicPipelineConfiguration::polygonMode)
                .addProperty("frontFaceCounterClockwise", &GraphicPipelineConfiguration::frontFaceCounterClockwise)
                .addProperty("depthStencilImageFormat", &GraphicPipelineConfiguration::depthStencilImageFormat)
                .addProperty("depthTestEnable", &GraphicPipelineConfiguration::depthTestEnable)
                .addProperty("depthWriteEnable", &GraphicPipelineConfiguration::depthWriteEnable)
                .addProperty("depthCompareOp", &GraphicPipelineConfiguration::depthCompareOp)
                .addProperty("depthBiasEnable", &GraphicPipelineConfiguration::depthBiasEnable)
                .addProperty("depthBiasConstantFactor", &GraphicPipelineConfiguration::depthBiasConstantFactor)
                .addProperty("depthBiasClamp", &GraphicPipelineConfiguration::depthBiasClamp)
                .addProperty("depthBiasSlopeFactor", &GraphicPipelineConfiguration::depthBiasSlopeFactor)
                .addProperty("stencilTestEnable", &GraphicPipelineConfiguration::stencilTestEnable)
                .addProperty("frontStencilOpState", &GraphicPipelineConfiguration::frontStencilOpState)
                .addProperty("backStencilOpState", &GraphicPipelineConfiguration::backStencilOpState)
                .addProperty("logicOpEnable", &GraphicPipelineConfiguration::logicOpEnable)
                .addProperty("logicOp", &GraphicPipelineConfiguration::logicOp)
                .addProperty("alphaToCoverageEnable", &GraphicPipelineConfiguration::alphaToCoverageEnable)
            .endClass()

        // classes
            .beginClass<Fence>("Fence")
               .addFunction("wait", &Fence::wait)
               .addFunction("reset", &Fence::reset)
           .endClass()
           .beginClass<Semaphore>("Semaphore")
               .addFunction("getType", &Semaphore::getType)
               .addFunction("getValue", &Semaphore::getValue)
               .addFunction("setValue", &Semaphore::setValue)
               .addFunction("incrementValue", &Semaphore::incrementValue)
               .addFunction("decrementValue", &Semaphore::decrementValue)
           .endClass()
           .beginClass<Instance>("Instance")
           .endClass()
           .beginClass<PhysicalDevice>("PhysicalDevice")
               //.addFunction("getDescription", &PhysicalDevice::getDescription)
           .endClass()
           .beginClass<Device>("Device")
               .addFunction("haveDedicatedTransferQueue", &Device::haveDedicatedTransferQueue)
           .endClass()
            .beginClass<Buffer>("Buffer")
                .addFunction("getSize", &Buffer::getSize)
                .addFunction("getType", &Buffer::getType)
                .addFunction("getInstanceSize", &Buffer::getInstanceSize)
                .addFunction("getInstanceSizeAligned", &Buffer::getInstanceSizeAligned)
                .addFunction("getInstanceCount", &Buffer::getInstanceCount)
                .addFunction("getMappedAddress", &Buffer::getMappedAddress)
                .addFunction("map", &Buffer::map)
                .addFunction("unmap", &Buffer::unmap)
                .addFunction("write", &Buffer::write)
                .addStaticFunction("getMemoryAllocations", &Buffer::getMemoryAllocations)
            .endClass()
            .beginClass<Sampler>("Sampler")
            .endClass()
            .beginClass<Image>("Image")
               .addFunction("getFormat", &Image::getFormat)
               .addFunction("getWidth", &Image::getWidth)
               .addFunction("getHeight", &Image::getHeight)
               .addFunction("getMipLevels", &Image::getMipLevels)
               .addFunction("getArraySize", &Image::getArraySize)
               .addFunction("getRowPitch", &Image::getRowPitch)
               .addFunction("getRowLength", &Image::getRowLength)
               .addFunction("getImageSize", &Image::getImageSize)
               .addFunction("getAlignedImageSize", &Image::getAlignedImageSize)
               .addFunction("getAlignedRowPitch", &Image::getAlignedRowPitch)
               .addFunction("getAlignedRowLength", &Image::getAlignedRowLength)
               .addFunction("isReadWrite", &Image::isReadWrite)
               .addStaticFunction("getPixelSize", &Image::getPixelSize)
               //.addStaticFunction("isDepthFormat", &Image::isDepthFormat)
               //.addStaticFunction("isDepthStencilFormat", &Image::isDepthStencilFormat)
               .addStaticFunction("getMemoryAllocations", &Image::getMemoryAllocations)
            .endClass()
            .beginClass<RenderTarget>("RenderTarget")
               .addFunction("getImage", &RenderTarget::getImage)
               .addFunction("getType", &RenderTarget::getType)
            .endClass()
            .beginClass<DescriptorLayout>("DescriptorLayout")
                .addFunction("add", &DescriptorLayout::add)
                .addFunction("build", &DescriptorLayout::build)
                .addFunction("getCapacity", &DescriptorLayout::getCapacity)
                .addFunction("isDynamicUniform", &DescriptorLayout::isDynamicUniform)
                .addFunction("isSamplers", &DescriptorLayout::isSamplers)
            .endClass()
            .beginClass<DescriptorSet>("DescriptorSet")
                .addFunction("updateBuffer", &DS_updateBufferByIndex)
                .addFunction("updateDynamicBuffer", &DS_updateBufferDynamic)
                .addFunction("updateImage", &DS_updateImageByIndex)
                .addFunction("updateImageByRef", &DS_updateImageByRef)
                .addFunction("updateSampler", &DS_updateSamplerByIndex)
                .addFunction("updateSamplerByRef", &DS_updateSamplerByRef)
                .addFunction("updateImageArray", &DS_updateImageArray)
                .addFunction("updateBufferArray", &DS_updateBufferArray)
                .addFunction("updateSamplerArray", &DS_updateSamplerArray)
                .addFunction("getLayout", &DescriptorSet::getLayout)
            .endClass()
            .beginClass<VertexInputLayout>("VertexInputLayout")
                .endClass()
                .beginClass<ShaderModule>("ShaderModule")
                .endClass()
                .beginClass<PipelineResources>("PipelineResources")
                .endClass()
            .beginClass<Pipeline>("Pipeline")
               .addFunction("getResources", &Pipeline::getResources)
               .addFunction("getType", &Pipeline::getType)
           .endClass()
           .deriveClass<ComputePipeline, Pipeline>("ComputePipeline")
           .endClass()
           .deriveClass<GraphicPipeline, Pipeline>("GraphicPipeline")
           .endClass()
            .beginClass<SwapChain>("SwapChain")
               .addFunction("getExtent", &SwapChain::getExtent)
               .addFunction("getAspectRatio", &SwapChain::getAspectRatio)
               .addFunction("getCurrentFrameIndex", &SwapChain::getCurrentFrameIndex)
               .addFunction("getFramesInFlight", &SwapChain::getFramesInFlight)
               .addFunction("getFormat", &SwapChain::getFormat)
               .addFunction("nextFrameIndex", &SwapChain::nextFrameIndex)
               .addFunction("acquire", &SwapChain::acquire)
               .addFunction("present", &SwapChain::present)
               .addFunction("recreate", &SwapChain::recreate)
               .addFunction("waitIdle", &SwapChain::waitIdle)
           .endClass().beginClass<CommandList>("CommandList")
                .addFunction("begin", &CommandList::begin)
                .addFunction("end", &CommandList::end)
                .addFunction("uploadBuffer", (void (CommandList::*)(const Buffer&, const void*)) &CommandList::upload)
                .addFunction("uploadImage", (void (CommandList::*)(const Image&, const void*, std::uint32_t)) &CommandList::upload)
                //.addFunction("uploadBuffers", &CommandList::upload)
                //.addFunction("uploadImages", &CommandList::upload)
                .addFunction("copyBufferToImage", (void (CommandList::*)(const Buffer&, const Image&, std::uint32_t, std::uint32_t, bool) const) &CommandList::copy)
                .addFunction("copyBufferToImageLevels", (void (CommandList::*)(const Buffer&, const Image&, const std::vector<std::size_t>&, bool) const) &CommandList::copy)
                //.addFunction("copyImageToBuffer", &CommandList::copy)
                .addFunction("copyBufferToBuffer", (void (CommandList::*)(const Buffer&, const Buffer&, std::size_t, std::uint32_t, std::uint32_t) const) &CommandList::copy)
                .addFunction("copyBufferRegions", (void (CommandList::*)(const Buffer&, const Buffer&, const std::vector<BufferCopyRegion>&) const) &CommandList::copy)
                //.addFunction("uploadArray", &CommandList::uploadArray)
                .addFunction("copyImageToSwapChain", (void (CommandList::*)(const Image&, const SwapChain&) const) &CommandList::copy)
                .addFunction("beginRendering", &CommandList::beginRendering)
                .addFunction("endRendering", &CommandList::endRendering)
                .addFunction("dispatch", &CommandList::dispatch)
                .addFunction("bindVertexBuffer", (void (CommandList::*)(const Buffer&, std::size_t) const) &CommandList::bindVertexBuffer)
                .addFunction("bindVertexBuffers", &CommandList::bindVertexBuffers)
                //.addFunction("bindIndexBuffer", &CommandList::bindIndexBuffer)
                .addFunction("bindPipeline", (void (CommandList::*)(Pipeline&)) &CommandList::bindPipeline)
                .addFunction("bindPipelinePtr", (void (CommandList::*)(const std::shared_ptr<Pipeline>&)) &CommandList::bindPipeline)
                .addFunction("bindDescriptors", &CommandList::bindDescriptors)
                .addFunction("bindDescriptor", (void (CommandList::*)(const DescriptorSet&, std::uint32_t) const) &CommandList::bindDescriptor)
                .addFunction("bindDescriptorDynamic", (void (CommandList::*)(const DescriptorSet&, std::uint32_t, std::uint32_t) const) &CommandList::bindDescriptor)
                .addFunction("draw", &CommandList::draw)
                .addFunction("drawIndexed", &CommandList::drawIndexed)
                .addFunction("drawIndirect", (void (CommandList::*)(const Buffer&, std::size_t, std::uint32_t, std::uint32_t, std::uint32_t)) &CommandList::drawIndirect)
                .addFunction("drawIndirectPtr", (void (CommandList::*)(const std::shared_ptr<Buffer>&, std::size_t, std::uint32_t, std::uint32_t, std::uint32_t)) &CommandList::drawIndirect)
                .addFunction("drawIndexedIndirectCount", (void (CommandList::*)(Buffer&, std::size_t, Buffer&, std::size_t, std::uint32_t, std::uint32_t, std::uint32_t)) &CommandList::drawIndexedIndirectCount)
                .addFunction("drawIndexedIndirectCountPtr", (void (CommandList::*)(const std::shared_ptr<Buffer>&, std::size_t, const std::shared_ptr<Buffer>&, std::size_t, std::uint32_t, std::uint32_t, std::uint32_t)) &CommandList::drawIndexedIndirectCount)
                .addFunction("drawIndexedIndirect", (void (CommandList::*)(const Buffer&, std::size_t, std::uint32_t, std::uint32_t, std::uint32_t)) &CommandList::drawIndexedIndirect)
                .addFunction("setViewports", &CommandList::setViewports)
                .addFunction("setScissors", (void (CommandList::*)(const std::vector<Rect>&) const) &CommandList::setScissors)
                .addFunction("setViewport", &CommandList::setViewport)
                .addFunction("setScissor", (void (CommandList::*)(const Rect&) const) &CommandList::setScissors)
                .addFunction("setStencilReference", &CommandList::setStencilReference)
                .addFunction("barrierImage", (void (CommandList::*)(const std::shared_ptr<const Image>&, ResourceState, ResourceState, std::uint32_t, std::uint32_t) const) &CommandList::barrier)
                .addFunction("barrierRenderTarget", (void (CommandList::*)(const std::shared_ptr<const RenderTarget>&, ResourceState, ResourceState) const) &CommandList::barrier)
                .addFunction("barrierRenderTargets", (void (CommandList::*)(const std::vector<std::shared_ptr<const RenderTarget>>&, ResourceState, ResourceState) const) &CommandList::barrier)
                .addFunction("barrierSwapChain", (void (CommandList::*)(const std::shared_ptr<const SwapChain>&, ResourceState, ResourceState) const) &CommandList::barrier)
                .addFunction("barrierBuffer", (void (CommandList::*)(const Buffer&, ResourceState, ResourceState) const) &CommandList::barrier)
                .addFunction("pushConstants", &CommandList::pushConstants)
                .addFunction("cleanup", &CommandList::cleanup)
            .endClass()
            .beginClass<CommandAllocator>("CommandAllocator")
               .addFunction("reset", &CommandAllocator::reset)
               .addFunction("createCommandList", &CA_createCL)
               .addFunction("createCommandListWithPipeline", &CA_createCLWithPipeline)
               .addFunction("getCommandListType", &CommandAllocator::getCommandListType)
           .endClass()
            .beginClass<SubmitQueue>("SubmitQueue")
                .addFunction("submitFenceSwap", &SQ_submitFenceSwap)
                .addFunction("submitNoSync", &SQ_submitNoSync)
                .addFunction("submitFenceOnly", &SQ_submitFenceOnly)
                .addFunction("submitWaitFenceSwap", &SQ_submitWaitFenceSwap)
                .addFunction("submitWaitSignal", &SQ_submitWaitSignal)
                .addFunction("submitWaitOnly", &SQ_submitWaitOnly)
                .addFunction("submitSignalOnly", &SQ_submitSignalOnly)
                .addFunction("waitIdle", &SubmitQueue::waitIdle)
            .endClass()
            .beginClass<Vireo>("Vireo")
                .addFunction("waitIdle", &Vireo::waitIdle)
                .addFunction("createSwapChain", &Vireo::createSwapChain)
                .addFunction("createSubmitQueue", &Vireo::createSubmitQueue)
                .addFunction("createFence", &Vireo::createFence)
                .addFunction("createSemaphore", &Vireo::createSemaphore)
                .addFunction("createCommandAllocator", &Vireo::createCommandAllocator)
                .addFunction("createVertexLayout", &Vireo::createVertexLayout)
                .addFunction("createShaderModuleFromFile", (std::shared_ptr<ShaderModule> (Vireo::*)(const std::string&) const) &Vireo::createShaderModule)
                .addFunction("createShaderModuleFromData", (std::shared_ptr<ShaderModule> (Vireo::*)(const std::vector<char>&,const std::string&) const) &Vireo::createShaderModule)
                .addFunction("createPipelineResources", &Vireo::createPipelineResources)
                .addFunction("createComputePipeline", &Vireo::createComputePipeline)
                .addFunction("createGraphicPipeline", &Vireo::createGraphicPipeline)
                .addFunction("createBuffer", &Vireo::createBuffer)
                .addFunction("createImage", &Vireo::createImage)
                .addFunction("createReadWriteImage", &Vireo::createReadWriteImage)
                .addFunction("createRenderTarget", &Vireo_createRTBasic)
                .addFunction("createRenderTargetFromSwapChain", &Vireo_createRTFromSwap)
                .addFunction("createDescriptorLayout", &Vireo::createDescriptorLayout)
                .addFunction("createSamplerDescriptorLayout", &Vireo::createSamplerDescriptorLayout)
                .addFunction("createDynamicUniformDescriptorLayout", &Vireo::createDynamicUniformDescriptorLayout)
                .addFunction("createDescriptorSet", &Vireo::createDescriptorSet)
                .addFunction("createSampler", &Vireo::createSampler)
                .addStaticFunction("isBackendSupported", &Vireo::isBackendSupported)
                .addFunction("getShaderFileExtension", &Vireo::getShaderFileExtension)
                .addFunction("getPhysicalDevice", &Vireo::getPhysicalDevice)
                .addFunction("getDevice", &Vireo::getDevice)
                .addFunction("getInstance", &Vireo::getInstance)
            .endClass()
            // .beginClass<BufferUploadInfo>("BufferUploadInfo")
            //     .addProperty("buffer", &BufferUploadInfo::buffer )
            //     .addProperty("data", &BufferUploadInfo::data)
            // .endClass()
            // .beginClass<ImageUploadInfo>("ImageUploadInfo")
            //     .addProperty("image", &ImageUploadInfo::image)
            //     .addProperty("data", &ImageUploadInfo::data)
            // .endClass()
        .endNamespace();
    }

}