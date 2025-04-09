/*
* Copyright (c) 2025-present Henri Michelon
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/
module;
#include "Tools.h"

export module vireo.backend.device;

import vireo.backend.framedata;

export namespace vireo::backend {

    class Instance {
    public:
        virtual ~Instance() = default;
    };

    class PhysicalDevice {
    public:
        virtual ~PhysicalDevice() = default;
    };

    class Device {
    public:
        virtual ~Device() = default;
    };

}