/*
* Copyright (c) 2025-present Henri Michelon
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/
module;
#include "backend/directx/Libraries.h"
export module vireo.backend.directx.samplers;

import vireo.backend.samplers;
import vireo.backend.directx.device;

export namespace vireo::backend {

    class DXSampler : public Sampler {
    public:
        static constexpr D3D12_TEXTURE_ADDRESS_MODE addressModes[] {
            D3D12_TEXTURE_ADDRESS_MODE_WRAP,
            D3D12_TEXTURE_ADDRESS_MODE_MIRROR,
            D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
            D3D12_TEXTURE_ADDRESS_MODE_BORDER,
        };

        DXSampler(
            Filter minFilter,
            Filter magFilter,
            AddressMode addressModeU,
            AddressMode addressModeV,
            AddressMode addressModeW,
            float minLod = 0.0f,
            float maxLod = 1.0f,
            bool anisotropyEnable = true,
            MipMapMode mipMapMode = MipMapMode::LINEAR);

        auto getSamplerDesc() const { return samplerDesc; }

    private:
        D3D12_STATIC_SAMPLER_DESC samplerDesc;
    };

}