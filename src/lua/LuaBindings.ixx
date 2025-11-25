/*
* Copyright (c) 2025-present Henri Michelon
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/
module;
#include "lua.h"
export module vireo.lua;

import std;
import vireo;
export import lua_bridge;

export namespace vireo {

    class LuaBindings {
    public:
        static void _register(lua_State*L);
    };
}

template <> struct luabridge::Stack<vireo::Backend> : Enum<vireo::Backend> {};
template <> struct luabridge::Stack<vireo::Filter> : Enum<vireo::Filter> {};
template <> struct luabridge::Stack<vireo::AddressMode> : Enum<vireo::AddressMode> {};
template <> struct luabridge::Stack<vireo::MipMapMode> : Enum<vireo::MipMapMode> {};
template <> struct luabridge::Stack<vireo::ImageFormat> : Enum<vireo::ImageFormat> {};
template <> struct luabridge::Stack<vireo::BufferType> : Enum<vireo::BufferType> {};
template <> struct luabridge::Stack<vireo::IndexType> : Enum<vireo::IndexType> {};
template <> struct luabridge::Stack<vireo::DescriptorType> : Enum<vireo::DescriptorType> {};
template <> struct luabridge::Stack<vireo::CommandType> : Enum<vireo::CommandType> {};
template <> struct luabridge::Stack<vireo::AttributeFormat> : Enum<vireo::AttributeFormat> {};
template <> struct luabridge::Stack<vireo::RenderTargetType> : Enum<vireo::RenderTargetType> {};
template <> struct luabridge::Stack<vireo::CullMode> : Enum<vireo::CullMode> {};
template <> struct luabridge::Stack<vireo::PrimitiveTopology> : Enum<vireo::PrimitiveTopology> {};
template <> struct luabridge::Stack<vireo::PolygonMode> : Enum<vireo::PolygonMode> {};
template <> struct luabridge::Stack<vireo::CompareOp> : Enum<vireo::CompareOp> {};
template <> struct luabridge::Stack<vireo::StencilOp> : Enum<vireo::StencilOp> {};
template <> struct luabridge::Stack<vireo::BlendFactor> : Enum<vireo::BlendFactor> {};
template <> struct luabridge::Stack<vireo::BlendOp> : Enum<vireo::BlendOp> {};
template <> struct luabridge::Stack<vireo::LogicOp> : Enum<vireo::LogicOp> {};
template <> struct luabridge::Stack<vireo::ColorWriteMask> : Enum<vireo::ColorWriteMask> {};
template <> struct luabridge::Stack<vireo::ShaderStage> : Enum<vireo::ShaderStage> {};
template <> struct luabridge::Stack<vireo::WaitStage> : Enum<vireo::WaitStage> {};
template <> struct luabridge::Stack<vireo::ResourceState> : Enum<vireo::ResourceState> {};
template <> struct luabridge::Stack<vireo::MSAA> : Enum<vireo::MSAA> {};
template <> struct luabridge::Stack<vireo::PresentMode> : Enum<vireo::PresentMode> {};
template <> struct luabridge::Stack<vireo::PipelineType> : Enum<vireo::PipelineType> {};
template <> struct luabridge::Stack<vireo::SemaphoreType> : Enum<vireo::SemaphoreType> {};
