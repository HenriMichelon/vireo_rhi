/*
* Copyright (c) 2025-present Henri Michelon
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/
module;
#include "vireo/backend/directx/Libraries.h"
export module vireo.directx.device;

import vireo.device;

export namespace vireo {

    class DXInstance : public Instance {
    public:
        DXInstance(HWND hWnd);

        auto getFactory() { return factory; }

    private:
        ComPtr<IDXGIFactory4> factory;
    };

    class DXPhysicalDevice : public PhysicalDevice {
    public:
        DXPhysicalDevice(const ComPtr<IDXGIFactory4>& factory);

        auto getHardwareAdapter() { return hardwareAdapter4; }

    private:
        ComPtr<IDXGIAdapter4> hardwareAdapter4;
    };

    class DXDevice : public Device {
    public:
        DXDevice(const ComPtr<IDXGIAdapter4>& hardwareAdapter4);
        ~DXDevice() override;

        auto getDevice() { return device; }

        auto getInFlightFence() { return inFlightFence; }

        auto getInFlightFenceEvent() const { return inFlightFenceEvent;}

    private:
        ComPtr<ID3D12Device> device;
        ComPtr<ID3D12Fence>  inFlightFence;
        HANDLE               inFlightFenceEvent;
    };

}