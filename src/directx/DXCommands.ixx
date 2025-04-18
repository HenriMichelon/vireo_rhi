/*
* Copyright (c) 2025-present Henri Michelon
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/
module;
#include "vireo/backend/directx/Libraries.h"
export module vireo.directx.commands;

import vireo;

export namespace vireo {

    class DXSubmitQueue : public SubmitQueue{
    public:
        DXSubmitQueue(const ComPtr<ID3D12Device>& device, CommandType type);

        auto getCommandQueue() { return commandQueue; }

        void submit(const shared_ptr<Fence>& fence, const shared_ptr<const FrameData>& frameData, const vector<shared_ptr<const CommandList>>& commandLists) const override;

        void submit(const vector<shared_ptr<const CommandList>>& commandLists) const override;

        void waitIdle() const override;

    private:
        ComPtr<ID3D12Device>       device;
        ComPtr<ID3D12CommandQueue> commandQueue;
    };

    class DXCommandAllocator : public CommandAllocator {
    public:
        DXCommandAllocator(const ComPtr<ID3D12Device>& device, CommandType type);

        void reset() const override;

        shared_ptr<CommandList> createCommandList(const shared_ptr<const Pipeline>& pipeline) const override;

        shared_ptr<CommandList> createCommandList() const override;

    private:
        ComPtr<ID3D12Device>           device;
        ComPtr<ID3D12CommandAllocator> commandAllocator;
    };

    class DXCommandList : public CommandList {
    public:
        static constexpr D3D12_COMMAND_LIST_TYPE ListType[] {
            D3D12_COMMAND_LIST_TYPE_DIRECT,
            D3D12_COMMAND_LIST_TYPE_DIRECT, //D3D12_COMMAND_LIST_TYPE_COPY,
            D3D12_COMMAND_LIST_TYPE_COMPUTE,
        };

        DXCommandList(
            CommandType type,
            const ComPtr<ID3D12Device>& device,
            const ComPtr<ID3D12CommandAllocator>& commandAllocator,
            const ComPtr<ID3D12PipelineState>& pipelineState = nullptr);

        void begin() const override;

        void end() const override;

        void upload(const shared_ptr<const Buffer>& destination, const void* source) override;

        void upload(const shared_ptr<const Image>& destination, const void* source) override;

        void copy(
            const shared_ptr<const Image>& source,
            const shared_ptr<const FrameData>& frameData,
            const shared_ptr<const SwapChain>& swapChain) const override;

        void blit(
            const shared_ptr<const Image>& source,
            const shared_ptr<const FrameData>& frameData,
            const shared_ptr<const SwapChain>& swapChain,
            Filter filter) const override;

        void beginRendering(
            const shared_ptr<SwapChain>& swapChain,
            const float clearColor[]) const override;

        void beginRendering(
            const shared_ptr<RenderTarget>& multisampledRenderTarget,
            const shared_ptr<SwapChain>& swapChain,
            const float clearColor[]) const override { throw runtime_error("DXCommandList::beginRendering(const shared_ptr<FrameData>&, const shared_ptr<SwapChain>&, const float clearColor[]) const override");};

        void beginRendering(
            const shared_ptr<RenderTarget>& renderTarget,
            const float clearColor[]) const override;

        void beginRendering(
            const shared_ptr<RenderTarget>& multisampledRenderTarget,
            const shared_ptr<RenderTarget>& renderTarget,
            const float clearColor[]) const override { throw runtime_error("DXCommandList::beginRendering(const shared_ptr<FrameData>&, const shared_ptr<RenderTarget>&, const float clearColor[]) const override");};

        void dispatch(uint32_t x, uint32_t y, uint32_t z) const override;

        void bindVertexBuffer(const shared_ptr<const Buffer>& buffer) const override;

        void bindPipeline(const shared_ptr<const Pipeline>& pipeline) override;

        void bindDescriptors(
            const shared_ptr<const Pipeline>& pipeline,
            const vector<shared_ptr<const DescriptorSet>>& descriptors) const override;

        void drawInstanced(uint32_t vertexCountPerInstance, uint32_t instanceCount = 1) const override;

        void setViewports(uint32_t count, const vector<Extent>& extent) const override;

        void setScissors(uint32_t count, const vector<Extent>& extent) const override;

        void barrier(
            const shared_ptr<const Image>& image,
            ResourceState oldState,
            ResourceState newState) const override;

        void barrier(
            const shared_ptr<const RenderTarget>& renderTarget,
            ResourceState oldState,
            ResourceState newState) const override { throw runtime_error("DXCommandList::barrier(const shared_ptr<const RenderTarget>&, ResourceState oldState, ResourceState newState) const override");};

        void barrier(
            const shared_ptr<const FrameData>& frameData,
            const shared_ptr<const SwapChain>& swapChain,
            ResourceState oldState,
            ResourceState newState) const override;

        void pushConstants(
            const shared_ptr<const PipelineResources>& pipelineResources,
            const PushConstantsDesc& pushConstants,
            const void* data) const;

        void cleanup() override;

        auto getCommandList() const  { return commandList; }

        ~DXCommandList() override = default;

    private:
        ComPtr<ID3D12Device>              device;
        ComPtr<ID3D12GraphicsCommandList> commandList;
        ComPtr<ID3D12CommandAllocator>    commandAllocator;
        vector<ComPtr<ID3D12Resource>>    stagingBuffers{};

        void beginRendering(
            const D3D12_CPU_DESCRIPTOR_HANDLE& handle,
            const float clearColor[]) const;

        void barrier(
           const ComPtr<ID3D12Resource>& resource,
           ResourceState oldState,
           ResourceState newState) const;
    };

}