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

    class DXFence : public Fence {
    public:
        DXFence(const ComPtr<ID3D12Device>& device);

        auto getValue() const { return fenceValue; }

        void setValue(const UINT64 value) { fenceValue = value; }

        void wait() const override;

        void reset() override { fenceValue++; }

        auto getFence() const { return fence; }

        ~DXFence() override;

    private:
        ComPtr<ID3D12Fence>  fence;
        UINT64               fenceValue{0};
        HANDLE               fenceEvent{nullptr};
    };

    class DXSemaphore : public Semaphore {
    public:
        DXSemaphore(const ComPtr<ID3D12Device>& device, SemaphoreType type);

        auto getFence() const { return fence; }

    private:
        ComPtr<ID3D12Fence>  fence;
    };

    class DXSubmitQueue : public SubmitQueue{
    public:
        DXSubmitQueue(const ComPtr<ID3D12Device>& device, CommandType type);

        auto getCommandQueue() const { return commandQueue; }

        void submit(
            const std::shared_ptr<Fence>& fence,
            const std::shared_ptr<const SwapChain>& swapChain,
            const std::vector<std::shared_ptr<const CommandList>>& commandLists) const override;

        void submit(
            const std::shared_ptr<Semaphore>& waitSemaphore,
            WaitStage waitStage,
            const std::shared_ptr<Fence>& fence,
            const std::shared_ptr<const SwapChain>& swapChain,
            const std::vector<std::shared_ptr<const CommandList>>& commandLists) const override;

        void submit(const std::vector<std::shared_ptr<const CommandList>>& commandLists) const override;

        void submit(
            const std::shared_ptr<Semaphore>& waitSemaphore,
            WaitStage waitStage,
            WaitStage signalStage,
            const std::shared_ptr<Semaphore>& signalSemaphore,
            const std::vector<std::shared_ptr<const CommandList>>& commandLists) const override;

        void submit(
            const std::shared_ptr<Fence>& fence,
            const std::vector<std::shared_ptr<const CommandList>>& commandLists) const override;

        void waitIdle() const override;

    private:
        ComPtr<ID3D12Device>       device;
        ComPtr<ID3D12CommandQueue> commandQueue;
    };

    class DXCommandAllocator : public CommandAllocator {
    public:
        DXCommandAllocator(const ComPtr<ID3D12Device>& device, CommandType type);

        void reset() const override;

        std::shared_ptr<CommandList> createCommandList(const std::shared_ptr<const Pipeline>& pipeline) const override;

        std::shared_ptr<CommandList> createCommandList() const override;

    private:
        ComPtr<ID3D12Device>           device;
        ComPtr<ID3D12CommandAllocator> commandAllocator;
    };

    class DXCommandList : public CommandList {
    public:
        static constexpr D3D12_COMMAND_LIST_TYPE dxType[] {
            D3D12_COMMAND_LIST_TYPE_DIRECT,
            D3D12_COMMAND_LIST_TYPE_DIRECT, //D3D12_COMMAND_LIST_TYPE_COPY,
            D3D12_COMMAND_LIST_TYPE_COMPUTE,
            D3D12_COMMAND_LIST_TYPE_DIRECT,
        };
        static constexpr DXGI_FORMAT dxIndexType[] {
            DXGI_FORMAT_R16_UINT,
            DXGI_FORMAT_R32_UINT,
        };

        DXCommandList(
            CommandType type,
            const ComPtr<ID3D12Device>& device,
            const ComPtr<ID3D12CommandAllocator>& commandAllocator,
            const ComPtr<ID3D12PipelineState>& pipelineState = nullptr);

        ~DXCommandList() override;

        void begin() const override;

        void end() const override;

        void upload(const std::shared_ptr<const Buffer>& destination, const void* source) override;

        void upload(
            const std::shared_ptr<const Image>& destination,
            const void* source,
            uint32_t firstMipLevel) override;

        void copy(
            const std::shared_ptr<Buffer>& source,
            const std::shared_ptr<const Image>& destination,
            uint32_t sourceOffset,
            uint32_t firstMipLevel) override;

        void uploadArray(
            const std::shared_ptr<const Image>& destination,
            const std::vector<void*>& sources,
            uint32_t firstMipLevel) override;

        void copy(
            const std::shared_ptr<const Image>& source,
            const std::shared_ptr<const SwapChain>& swapChain) const override;

        void beginRendering(const RenderingConfiguration& conf) override;

        void endRendering() override;

        void dispatch(uint32_t x, uint32_t y, uint32_t z) const override;

        void bindVertexBuffers(
            const std::vector<std::shared_ptr<const Buffer>>& buffers,
            std::vector<size_t> offsets = {}) const override;

        void bindVertexBuffer(const std::shared_ptr<const Buffer>& buffer, size_t offset) const override;

        void bindIndexBuffer(const std::shared_ptr<const Buffer>& buffer, IndexType indexType, size_t offset) const override;

        void bindPipeline(const std::shared_ptr<const Pipeline>& pipeline) override;

        void bindDescriptors(
            const std::shared_ptr<const Pipeline>& pipeline,
            const std::vector<std::shared_ptr<const DescriptorSet>>& descriptors) const override;

        void draw(
            uint32_t vertexCountPerInstance,
            uint32_t instanceCount = 1,
            uint32_t firstVertex = 0,
            uint32_t firstInstance = 0) const override;

        void drawIndexed(
            uint32_t indexCountPerInstance,
            uint32_t instanceCount = 0,
            uint32_t firstIndex = 0,
            uint32_t vertexOffset = 0,
            uint32_t firstInstance = 0) const override;

        void setViewports(const std::vector<Extent>& extents) const override;

        void setScissors(const std::vector<Extent>& extents) const override;

        void setViewport(const Extent& extent) const override;

        void setScissors(const Extent& extent) const override;

        void barrier(
            const std::shared_ptr<const Image>& image,
            ResourceState oldState,
            ResourceState newState,
            uint32_t firstMipLevel,
            uint32_t levelCount) const override;

        void barrier(
            const std::shared_ptr<const RenderTarget>& renderTarget,
            ResourceState oldState,
            ResourceState newState) const override;

        void barrier(
            const std::shared_ptr<const SwapChain>& swapChain,
            ResourceState oldState,
            ResourceState newState) const override;

        void barrier(
            const std::vector<std::shared_ptr<const RenderTarget>>& renderTargets,
            ResourceState oldState,
            ResourceState newState) const override;

        void pushConstants(
            const std::shared_ptr<const PipelineResources>& pipelineResources,
            const PushConstantsDesc& pushConstants,
            const void* data) const override;

        void cleanup() override;

        auto getCommandList() const  { return commandList; }

    private:
        ComPtr<ID3D12Device>              device;
        ComPtr<ID3D12GraphicsCommandList> commandList;
        ComPtr<ID3D12CommandAllocator>    commandAllocator;
        std::vector<ComPtr<ID3D12Resource>>    stagingBuffers{};
        std::vector<std::shared_ptr<Image>>         resolveSource;
        std::vector<ComPtr<ID3D12Resource>>    resolveDestination;
        ComPtr<ID3D12Resource>            depthTargetToDiscard;
        std::vector<ComPtr<ID3D12Resource>>    colorTargetsToDiscard;

        static void convertState(
            ResourceState oldState,
            ResourceState newState,
            D3D12_RESOURCE_STATES& srcState,
            D3D12_RESOURCE_STATES& dstState);

        void barrier(
            const ComPtr<ID3D12Resource>& resource,
            ResourceState oldState,
            ResourceState newState,
            uint32_t firstMipLevel = 0,
            uint32_t levelCount = 1,
            uint32_t arraySize = 1) const;

        void barrier(
            const std::vector<ID3D12Resource*>& resources,
            ResourceState oldState,
            ResourceState newState) const;
        };

}