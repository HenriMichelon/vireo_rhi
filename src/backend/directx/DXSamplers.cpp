/*
* Copyright (c) 2025-present Henri Michelon
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/
module;
#include "backend/directx/Tools.h"
module vireo.backend.directx.samplers;

namespace vireo::backend {

    DXSampler::DXSampler(
        const Filter minFilter,
        const Filter magFilter,
        const AddressMode addressModeU,
        const AddressMode addressModeV,
        const AddressMode addressModeW,
        const float minLod,
        const float maxLod,
        const bool anisotropyEnable,
        const MipMapMode mipMapMode) {

        int filter = anisotropyEnable ? D3D12_FILTER_ANISOTROPIC : 0;
        if ((minFilter == Filter::LINEAR) && (magFilter == Filter::LINEAR)) {
            filter |= mipMapMode == MipMapMode::LINEAR ? D3D12_FILTER_MIN_MAG_MIP_LINEAR : D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT;
        } else if ((minFilter == Filter::NEAREST) && (magFilter == Filter::NEAREST)) {
            filter |= mipMapMode == MipMapMode::LINEAR ? D3D12_FILTER_MIN_MAG_POINT_MIP_LINEAR : D3D12_FILTER_MIN_MAG_MIP_POINT;
        } else if ((minFilter == Filter::NEAREST) && (magFilter == Filter::LINEAR)) {
            filter |= mipMapMode == MipMapMode::LINEAR ? D3D12_FILTER_MIN_POINT_MAG_MIP_LINEAR : D3D12_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
        } else if ((minFilter == Filter::LINEAR) && (magFilter == Filter::NEAREST)) {
            filter |= mipMapMode == MipMapMode::LINEAR ? D3D12_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR : D3D12_FILTER_MIN_LINEAR_MAG_MIP_POINT;
        }

        sampler = D3D12_STATIC_SAMPLER_DESC{
            .Filter = static_cast<D3D12_FILTER>(filter),
            .AddressU = addressModes[static_cast<int>(addressModeU)],
            .AddressV = addressModes[static_cast<int>(addressModeV)],
            .AddressW = addressModes[static_cast<int>(addressModeW)],
            .MipLODBias = 0,
            .MaxAnisotropy = 0,
            .ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER,
            .BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK,
            .MinLOD = minLod,
            .MaxLOD = maxLod,
            .ShaderRegister = 0,
            .RegisterSpace = 0,
            .ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL,
        };
    }

}