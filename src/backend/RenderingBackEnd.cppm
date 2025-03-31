module;
#include "Libraries.h"
import std;

export module dxvk.backend;

export namespace dxvk::backend {

    class Instance {
    public:
        virtual ~Instance() = default;
    };

    class RenderingBackEnd
    {
    public:
        virtual ~RenderingBackEnd() = default;

        auto getInstance() { return instance; }

    protected:
        std::shared_ptr<Instance> instance;

    };
}