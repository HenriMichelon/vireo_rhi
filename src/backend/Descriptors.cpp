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

    DescriptorAllocator::DescriptorAllocator(const DescriptorType type, const uint32_t capacity) :
        type{type},
        capacity{capacity} {
        for (DescriptorHandle i = 0; i < capacity; i++) {
            freeHandles.push_back(capacity - 1 - i);
        }
    }

    DescriptorHandle DescriptorAllocator::allocate() {
        assert(!freeHandles.empty());
        auto handle = freeHandles.back();
        freeHandles.pop_back();
        return handle;
    }

    void DescriptorAllocator::free(DescriptorHandle handle){
        assert(handle < capacity);
        freeHandles.push_back(handle);
    }

}