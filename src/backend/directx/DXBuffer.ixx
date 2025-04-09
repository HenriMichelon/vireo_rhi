/*
* Copyright (c) 2025-present Henri Michelon
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/
module;
#include "backend/directx/Libraries.h"

export module vireo.backend.directx.buffer;

import vireo.backend.buffer;

export namespace vireo::backend {

    class DXBuffer : public Buffer {
    public:
        static constexpr D3D12_RESOURCE_STATES ResourceStates[] {
            D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
            D3D12_RESOURCE_STATE_INDEX_BUFFER,
            D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER
        };

        DXBuffer(
            const ComPtr<ID3D12Device>& device,
            Type type,
            size_t size,
            size_t count,
            size_t minOffsetAlignment,
            const std::wstring& name);

        void map() override;

        void unmap() override;

        void write(const void* data, size_t size = WHOLE_SIZE, size_t offset = 0) override;

        const auto& getBufferView() const { return bufferView; }

        auto& getBuffer() const { return buffer; }

    private:
        ComPtr<ID3D12Device>        device;
        ComPtr<ID3D12Resource>      buffer;
        D3D12_VERTEX_BUFFER_VIEW    bufferView;
        CD3DX12_RESOURCE_DESC       resourceDesc;
    };

}