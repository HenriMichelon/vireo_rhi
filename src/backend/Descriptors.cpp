/*
* Copyright (c) 2025-present Henri Michelon
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/
module;
#include "Tools.h"
module vireo.backend.descriptors;

namespace vireo::backend {

    DescriptorSet::DescriptorSet(const DescriptorType type, const size_t capacity) :
        type{type},
        capacity{capacity} {
        for (DescriptorHandle i = 0; i < capacity; i++) {
            freeHandles.push_back(capacity - 1 - i);
        }
    }

    DescriptorHandle DescriptorSet::allocate() {
        assert(!freeHandles.empty());
        auto handle = freeHandles.back();
        freeHandles.pop_back();
        return handle;
    }

    void DescriptorSet::free(DescriptorHandle handle){
        assert(handle < capacity);
        freeHandles.push_back(handle);
    }

}