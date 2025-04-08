module;
#include "Libraries.h"

export module dxvk.backend.framedata;

export namespace dxvk::backend {

    struct FrameData {
        virtual ~FrameData() = default;
    };

}