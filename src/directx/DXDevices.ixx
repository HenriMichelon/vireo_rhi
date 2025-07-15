/*
* Copyright (c) 2025-present Henri Michelon
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/
module;
#include "vireo/backend/directx/Libraries.h"
export module vireo.directx.devices;

import vireo;

export namespace vireo {

    class DXInstance : public Instance {
    public:
        DXInstance();

        auto getFactory() { return factory; }

    private:
        ComPtr<IDXGIFactory4> factory;
    };

    class DXPhysicalDevice : public PhysicalDevice {
    public:
        static constexpr UINT dxSampleCount[] {
            1,
            2,
            4,
            8,
            16,
            32,
            64,
        };

        DXPhysicalDevice(const ComPtr<IDXGIFactory4>& factory);

        auto getHardwareAdapter() { return hardwareAdapter4; }

        const PhysicalDeviceDesc getDescription() const override;

    private:
        ComPtr<IDXGIAdapter4> hardwareAdapter4;
    };

    class DXDevice : public Device {
    public:
        DXDevice(const ComPtr<IDXGIAdapter4>& hardwareAdapter4);

        auto getDevice() { return device; }

        bool haveDedicatedTransferQueue() const override { return true;}

    private:
        ComPtr<ID3D12Device> device;
    };

}