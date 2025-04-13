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
        DXSubmitQueue(CommandList::Type type, const ComPtr<ID3D12Device>& device);

        auto getCommandQueue() { return commandQueue; }

        void submit(const shared_ptr<FrameData>& frameData, vector<shared_ptr<CommandList>> commandLists) override;

        void submit(vector<shared_ptr<CommandList>> commandLists) override;

        void waitIdle() override;

    private:
        ComPtr<ID3D12Device>       device;
        ComPtr<ID3D12CommandQueue> commandQueue;
    };

    class DXCommandAllocator : public CommandAllocator {
    public:
        DXCommandAllocator(CommandList::Type type, const ComPtr<ID3D12Device>& device);

        shared_ptr<CommandList> createCommandList(shared_ptr<Pipeline>& pipeline) const override;

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
            Type type,
            const ComPtr<ID3D12Device>& device,
            const ComPtr<ID3D12CommandAllocator>& commandAllocator,
            const ComPtr<ID3D12PipelineState>& pipelineState = nullptr);

        void reset() override;

        void begin(shared_ptr<Pipeline>& pipeline) override;

        void begin() override;

        void end() override;

        void bindVertexBuffer(shared_ptr<Buffer>& buffer) override;

        void drawInstanced(uint32_t vertexCountPerInstance, uint32_t instanceCount = 1) override;

        void upload(shared_ptr<Buffer>& destination, const void* source) override;

        void upload(shared_ptr<Image>& destination, const void* source) override;

        void cleanup() override;

        auto getCommandList() { return commandList; }

        ~DXCommandList() override = default;

    private:
        ComPtr<ID3D12Device>                device;
        ComPtr<ID3D12GraphicsCommandList>   commandList;
        ComPtr<ID3D12CommandAllocator>      commandAllocator;
        vector<ComPtr<ID3D12Resource>> stagingBuffers{};
    };

}