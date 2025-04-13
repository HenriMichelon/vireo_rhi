/*
* Copyright (c) 2025-present Henri Michelon
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/
module;
#include "vireo/backend/directx/Libraries.h"
export module vireo.directx.pipelines;

import vireo;

export namespace vireo {

    class DXVertexInputLayout : public VertexInputLayout {
    public:
        static constexpr DXGI_FORMAT DXFormat[] {
            DXGI_FORMAT_R32G32_FLOAT,
            DXGI_FORMAT_R32G32B32_FLOAT,
            DXGI_FORMAT_R32G32B32A32_FLOAT
        };

        DXVertexInputLayout(const vector<AttributeDescription>& attributesDescriptions);

        const auto& getInputElementDescs() const { return inputElementDescs; }

    private:
        vector<D3D12_INPUT_ELEMENT_DESC> inputElementDescs;
    };

    class DXShaderModule: public ShaderModule {
    public:
        DXShaderModule(const string& fileName);

        auto getShader() const { return shader; }

    private:
        ComPtr<ID3DBlob> shader;
    };

    class DXPipelineResources : public PipelineResources {
    public:
        DXPipelineResources(
            const ComPtr<ID3D12Device>& device,
            const vector<shared_ptr<DescriptorLayout>>& descriptorLayouts,
            const wstring& name);

        auto getRootSignature() const { return rootSignature; }

    private:
        ComPtr<ID3D12RootSignature> rootSignature;
    };

    class DXPipeline : public Pipeline {
    public:
        DXPipeline(
            const ComPtr<ID3D12Device>& device,
            PipelineResources& pipelineResources,
            VertexInputLayout& vertexInputLayout,
            ShaderModule& vertexShader,
            ShaderModule& fragmentShader,
            const wstring& name);

        auto getPipelineState() { return pipelineState; }

    private:
        ComPtr<ID3D12PipelineState> pipelineState;
    };

}