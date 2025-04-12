/*
* Copyright (c) 2025-present Henri Michelon
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/
module;
#include "vireo/Tools.h"
export module vireo.backend.descriptors;

import vireo.backend.resources;

export namespace vireo::backend {

    enum class DescriptorType : uint8_t {
        BUFFER  = 0,
        IMAGE   = 1,
        SAMPLER = 2,
    };

    using DescriptorIndex = uint32_t;

    class DescriptorLayout {
    public:
        virtual ~DescriptorLayout() = default;

        virtual DescriptorLayout& add(DescriptorIndex index, DescriptorType type, size_t count = 1) = 0;

        virtual void build() {}

        auto getCapacity() const { return capacity; }

    protected:
        size_t capacity{0};
    };

    class DescriptorSet {
    public:
        virtual ~DescriptorSet() = default;

        virtual void update(DescriptorIndex index, const shared_ptr<Buffer>& buffer) = 0;

        virtual void update(DescriptorIndex index, const shared_ptr<Image>& image) = 0;

        virtual void update(DescriptorIndex index, const shared_ptr<Sampler>& sampler) = 0;

        virtual void update(DescriptorIndex index, const vector<shared_ptr<Image>>& images) = 0;

        virtual void update(DescriptorIndex index, const vector<shared_ptr<Buffer>>& buffer) = 0;

        virtual void update(DescriptorIndex index, const vector<shared_ptr<Sampler>>& samplers) = 0;

    protected:
        const shared_ptr<DescriptorLayout> layout;
        DescriptorSet(const shared_ptr<DescriptorLayout>& layout) : layout{layout} {}
    };


}