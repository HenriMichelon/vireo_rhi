/*
* Copyright (c) 2025-present Henri Michelon
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/
module;
#include "vireo/backend/directx/Libraries.h"
module vireo.directx.pipelines;

import std;
import vireo.tools;
import vireo.directx.descriptors;
import vireo.directx.devices;
import vireo.directx.resources;
import vireo.directx.swapchains;
import vireo.directx.tools;

namespace vireo {

    DXVertexInputLayout::DXVertexInputLayout(const std::vector<VertexAttributeDesc>& attributesDescriptions) {
        for (const auto& attributesDescription : attributesDescriptions) {
            inputElementsDesc.push_back({
                .SemanticName = attributesDescription.binding.c_str(),
                .SemanticIndex = 0,
                .Format = DXFormat[static_cast<int>(attributesDescription.format)],
                .InputSlot = 0,
                .AlignedByteOffset = attributesDescription.offset,
                .InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
                .InstanceDataStepRate = 0
            });
        }
    }

    DXShaderModule::DXShaderModule(const std::string& fileName) {
        std::ifstream shaderFile(fileName + ".dxil", std::ios::binary | std::ios::ate);
        if (!shaderFile) {
            throw Exception("Error loading shader ", fileName);
        }
        const std::streamsize size = shaderFile.tellg();
        shaderFile.seekg(0, std::ios::beg);
        load(shaderFile, size);
        shaderFile.close();
    }

    DXShaderModule::DXShaderModule(std::ifstream& inputStream, const size_t size) {
        load(inputStream, size);
    }

    DXShaderModule::DXShaderModule(const std::vector<char>& data) {
        dxCheck(D3DCreateBlob(data.size(), &shader), "Error creating blob for  shader ");
        memcpy(shader->GetBufferPointer(), data.data(), data.size());
    }

    void DXShaderModule::load(std::ifstream& inputStream, const size_t size) {
        dxCheck(D3DCreateBlob(size, &shader), "Error creating blob for  shader ");
        inputStream.read(static_cast<char*>(shader->GetBufferPointer()), size);
    }

    DXPipelineResources::DXPipelineResources(
        const ComPtr<ID3D12Device>& device,
        const std::vector<std::shared_ptr<DescriptorLayout>>& descriptorLayouts,
        const PushConstantsDesc& pushConstant,
        const std::string& name) {

        constexpr D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
               D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

        D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};
        featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;
        if (FAILED(device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData)))) {
            featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
        }

        std::vector<CD3DX12_ROOT_PARAMETER1> rootParameters(descriptorLayouts.size());
        for (int i = 0; i < descriptorLayouts.size(); i++) {
            const auto layout = static_pointer_cast<DXDescriptorLayout>(descriptorLayouts[i]);
            if (layout->isDynamicUniform()) {
                rootParameters[i].InitAsConstantBufferView(
                    0,
                    i,
                    D3D12_ROOT_DESCRIPTOR_FLAG_DATA_STATIC,
                    D3D12_SHADER_VISIBILITY_ALL);
            } else {
                for (auto& range : layout->getRanges()) {
                    range.RegisterSpace = i;
                }
                rootParameters[i].InitAsDescriptorTable(
                    layout->getRanges().size(),
                    layout->getRanges().data(),
                    D3D12_SHADER_VISIBILITY_ALL);
            }
        }

        if (pushConstant.size > 0) {
            auto pushConstantRootParams = CD3DX12_ROOT_PARAMETER1 {};
            pushConstantRootParams.InitAsConstants(
                pushConstant.size / sizeof(uint32_t),
                0,
                rootParameters.size(),
                D3D12_SHADER_VISIBILITY_ALL
            );
            if (pushConstant.stage == ShaderStage::VERTEX) {
                pushConstantRootParams.ShaderVisibility  = D3D12_SHADER_VISIBILITY_VERTEX;
            } else if (pushConstant.stage == ShaderStage::FRAGMENT) {
                pushConstantRootParams.ShaderVisibility  = D3D12_SHADER_VISIBILITY_PIXEL;
            }
            pushConstantsRootParameterIndex = rootParameters.size();
            rootParameters.push_back(pushConstantRootParams);
        }

        CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
        rootSignatureDesc.Init_1_1(
            rootParameters.size(),
            rootParameters.empty() ? nullptr : rootParameters.data(),
            0,
            nullptr,
            rootSignatureFlags);

        ComPtr<ID3DBlob> signature;
        ComPtr<ID3DBlob> error;
        const auto hr = D3DX12SerializeVersionedRootSignature(
            &rootSignatureDesc,
            featureData.HighestVersion,
            &signature,
            &error);
        if (FAILED(hr)){
            throw Exception(static_cast<char*>(error->GetBufferPointer()));
        }

        dxCheck(device->CreateRootSignature(
            0,
            signature->GetBufferPointer(),
            signature->GetBufferSize(),
            IID_PPV_ARGS(&rootSignature)));
#ifdef _DEBUG
        //rootSignature->SetName(("DXPipelineResources : " + name).c_str());
#endif
    }

    DXComputePipeline::DXComputePipeline(
        const ComPtr<ID3D12Device>& device,
        const std::shared_ptr<PipelineResources>& pipelineResources,
        const std::shared_ptr<const ShaderModule>& shader,
        const std::string& name):
        ComputePipeline{pipelineResources} {
        assert(shader != nullptr);
        const auto dxPipelineResources = static_pointer_cast<const DXPipelineResources>(pipelineResources);
        const auto dxShader = static_pointer_cast<const DXShaderModule>(shader);
        const auto psoDesc = D3D12_COMPUTE_PIPELINE_STATE_DESC{
            .pRootSignature = dxPipelineResources->getRootSignature().Get(),
            .CS = CD3DX12_SHADER_BYTECODE(dxShader->getShader().Get()),
        };

        dxCheck(device->CreateComputePipelineState(&psoDesc, IID_PPV_ARGS(&pipelineState)));
#ifdef _DEBUG
        //pipelineState->SetName(("DXComputePipeline : " + name).c_str());
#endif
    }

    DXGraphicPipeline::DXGraphicPipeline(
        const ComPtr<ID3D12Device>& device,
        const GraphicPipelineConfiguration& configuration,
        const std::string& name):
        GraphicPipeline{configuration.resources},
        primitiveTopology{dxPrimitives[static_cast<int>(configuration.primitiveTopology)]} {
        assert(configuration.resources != nullptr);
        assert(configuration.vertexShader != nullptr);
        assert(configuration.colorRenderFormats.size() == configuration.colorBlendDesc.size());
        const auto dxPipelineResources = static_pointer_cast<const DXPipelineResources>(configuration.resources);

        auto rasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
        rasterizerState.FillMode = configuration.polygonMode == PolygonMode::FILL ? D3D12_FILL_MODE_SOLID  : D3D12_FILL_MODE_WIREFRAME;
        rasterizerState.CullMode = dxCullMode[static_cast<int>(configuration.cullMode)];
        rasterizerState.FrontCounterClockwise = configuration.frontFaceCounterClockwise;
        rasterizerState.DepthBias = static_cast<INT>(configuration.depthBiasEnable ? configuration.depthBiasConstantFactor : 0.0f);
        rasterizerState.DepthBiasClamp = configuration.depthBiasEnable ? configuration.depthBiasClamp : 0.0f;
        rasterizerState.SlopeScaledDepthBias = configuration.depthBiasEnable ? configuration.depthBiasSlopeFactor : 0.0f;

        auto depthStencil = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
        depthStencil.DepthEnable = configuration.depthTestEnable;
        depthStencil.DepthWriteMask = configuration.depthWriteEnable ? D3D12_DEPTH_WRITE_MASK_ALL  : D3D12_DEPTH_WRITE_MASK_ZERO;
        depthStencil.DepthFunc = dxCompareOp[static_cast<int>(configuration.depthCompareOp)];
        depthStencil.StencilEnable = configuration.stencilTestEnable;
        depthStencil.FrontFace.StencilFailOp      = dxStencilOp[static_cast<int>(configuration.frontStencilOpState.failOp)];
        depthStencil.FrontFace.StencilDepthFailOp = dxStencilOp[static_cast<int>(configuration.frontStencilOpState.depthFailOp)];
        depthStencil.FrontFace.StencilPassOp      = dxStencilOp[static_cast<int>(configuration.frontStencilOpState.passOp)];
        depthStencil.FrontFace.StencilFunc        = dxCompareOp[static_cast<int>(configuration.frontStencilOpState.compareOp)];
        depthStencil.BackFace.StencilFailOp      = dxStencilOp[static_cast<int>(configuration.backStencilOpState.failOp)];
        depthStencil.BackFace.StencilDepthFailOp = dxStencilOp[static_cast<int>(configuration.backStencilOpState.depthFailOp)];
        depthStencil.BackFace.StencilPassOp      = dxStencilOp[static_cast<int>(configuration.backStencilOpState.passOp)];
        depthStencil.BackFace.StencilFunc        = dxCompareOp[static_cast<int>(configuration.backStencilOpState.compareOp)];
        depthStencil.StencilReadMask = 0xFF;
        depthStencil.StencilWriteMask = 0xFF;

        const auto samples = DXPhysicalDevice::dxSampleCount[static_cast<int>(configuration.msaa)];

        UINT quality = 0;
        if (configuration.msaa != MSAA::NONE && configuration.colorRenderFormats.size() > 1) {
            auto qualityLevels = D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS {
                .Format = DXImage::dxFormats[static_cast<int>(configuration.colorRenderFormats[0])],
                .SampleCount = samples,
                .Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE,
            };
            dxCheck(device->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &qualityLevels, sizeof(qualityLevels)));
            quality = qualityLevels.NumQualityLevels > 0 ? qualityLevels.NumQualityLevels - 1 : 0;
        }

        auto psoDesc = D3D12_GRAPHICS_PIPELINE_STATE_DESC {
            .pRootSignature = dxPipelineResources->getRootSignature().Get(),
            .BlendState = {
                .AlphaToCoverageEnable = configuration.alphaToCoverageEnable,
                .IndependentBlendEnable = TRUE
            },
            .SampleMask = UINT_MAX,
            .RasterizerState = rasterizerState,
            .DepthStencilState = depthStencil,
            .PrimitiveTopologyType = dxPrimitivesTypes[static_cast<int>(configuration.primitiveTopology)],
            .NumRenderTargets = static_cast<UINT>(configuration.colorRenderFormats.size()),
            .DSVFormat = DXImage::dxFormats[static_cast<int>(configuration.depthStencilImageFormat)],
            .SampleDesc = {
                .Count = samples,
                .Quality = quality
            }
        };
        if (configuration.vertexInputLayout) {
            const auto dxVertexInputLayout = static_pointer_cast<const DXVertexInputLayout>(configuration.vertexInputLayout);
            psoDesc.InputLayout.NumElements = dxVertexInputLayout->getInputElementsDesc().size();
            psoDesc.InputLayout.pInputElementDescs = dxVertexInputLayout->getInputElementsDesc().data();
        }

        psoDesc.VS = CD3DX12_SHADER_BYTECODE(static_pointer_cast<const DXShaderModule>(configuration.vertexShader)->getShader().Get());
        if (configuration.fragmentShader) {
            psoDesc.PS = CD3DX12_SHADER_BYTECODE(static_pointer_cast<const DXShaderModule>(configuration.fragmentShader)->getShader().Get());
        }
        if (configuration.hullShader) {
            psoDesc.HS = CD3DX12_SHADER_BYTECODE(static_pointer_cast<const DXShaderModule>(configuration.hullShader)->getShader().Get());
        }
        if (configuration.domainShader) {
            psoDesc.DS = CD3DX12_SHADER_BYTECODE(static_pointer_cast<const DXShaderModule>(configuration.domainShader)->getShader().Get());
        }
        if (configuration.geometryShader) {
            psoDesc.GS = CD3DX12_SHADER_BYTECODE(static_pointer_cast<const DXShaderModule>(configuration.geometryShader)->getShader().Get());
        }
        for (int i = 0; i < configuration.colorRenderFormats.size(); i++) {
            psoDesc.RTVFormats[i] = DXImage::dxFormats[static_cast<int>(configuration.colorRenderFormats[i])];
            psoDesc.BlendState.RenderTarget[i].BlendEnable   = configuration.colorBlendDesc[i].blendEnable;
            psoDesc.BlendState.RenderTarget[i].LogicOpEnable = configuration.logicOpEnable;
            psoDesc.BlendState.RenderTarget[i].SrcBlend      = dxBlendFactor[static_cast<size_t>(configuration.colorBlendDesc[i].srcColorBlendFactor)];
            psoDesc.BlendState.RenderTarget[i].DestBlend     = dxBlendFactor[static_cast<size_t>(configuration.colorBlendDesc[i].dstColorBlendFactor)];
            psoDesc.BlendState.RenderTarget[i].BlendOp       = dxBlendOp[static_cast<size_t>(configuration.colorBlendDesc[i].colorBlendOp)];
            psoDesc.BlendState.RenderTarget[i].SrcBlendAlpha = dxBlendFactor[static_cast<size_t>(configuration.colorBlendDesc[i].srcAlphaBlendFactor)];
            psoDesc.BlendState.RenderTarget[i].DestBlendAlpha= dxBlendFactor[static_cast<size_t>(configuration.colorBlendDesc[i].dstAlphaBlendFactor)];
            psoDesc.BlendState.RenderTarget[i].BlendOpAlpha  = dxBlendOp[static_cast<size_t>(configuration.colorBlendDesc[i].alphaBlendOp)];
            psoDesc.BlendState.RenderTarget[i].LogicOp       = dxLogicOp[static_cast<size_t>(configuration.logicOp)];
            psoDesc.BlendState.RenderTarget[i].RenderTargetWriteMask = static_cast<UINT8>(configuration.colorBlendDesc[i].colorWriteMask);
        }
        psoDesc.BlendState.AlphaToCoverageEnable = configuration.alphaToCoverageEnable;
        dxCheck(device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pipelineState)));
#ifdef _DEBUG
        //pipelineState->SetName(("DXGraphicPipeline : " + name).c_str());
#endif
    }


}
