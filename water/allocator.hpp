// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_ALLOCATOR_HPP
#define WATER_ALLOCATOR_HPP

/*

The main dynamic memory allocator used by water. Replace with your own by defining WATER_ALLOCATOR

This type of allocator classes are used throught water for dynamic memory allocation.

*/

#ifdef WATER_ALLOCATOR
namespace water {
using allocator = WATER_ALLOCATOR;
}

#elif defined(WATER_NO_STD)

#include <water/allocator_malloc.hpp>
namespace water {
using allocator = allocator_malloc;
}

#else

#include <water/water.hpp>
#include <new>
#include <water/align_max.hpp>
#ifndef __cpp_aligned_new
    #include <water/align_with_unalign.hpp>
#endif
namespace water {

struct allocator
{
    void* allocate(size_t bytes) noexcept(false) {
        return ::operator new(bytes);
    }

    #ifdef __cpp_sized_deallocation
    
    void free(void *pointer, size_t bytes) noexcept {
        ::operator delete(pointer, bytes);
    }
    
    #else
    
    void free(void *pointer, size_t) noexcept {
        ::operator delete(pointer);
    }
    
    #endif

    template<typename type_>
    type_* allocate(size_t count = 1) noexcept(false) {
        // when alignof(type_) < align_allocations, this assumes the default alignment will work for that type.
        void *r = 0;
        auto align = alignof(type_); // avoids visual c++ conditional expression is constant
        if(align > align_allocations) {
            #ifdef __cpp_aligned_new
            r = ::operator new(sizeof(type_) * count, static_cast<std::align_val_t>(align));
            #else
            r = align_with_unalign(::operator new(sizeof(type_) * count + align), align);
            #endif
        }
        else
            r = ::operator new(sizeof(type_) * count);
        return static_cast<type_*>(r);
    }

    template<typename type_>
    void free(void *pointer, size_t count = 1) noexcept {
        auto align = alignof(type_);
        if(align > align_allocations) {
            #ifdef __cpp_aligned_new
            ::operator delete(pointer, sizeof(type_) * count, static_cast<std::align_val_t>(align));
            #else
            free(unalign(pointer), sizeof(type_) * count + align);
            #endif
        }
        else
            free(pointer, sizeof(type_) * count);
    }
    
    constexpr bool operator==(allocator const&) const noexcept {
        return true;
    }
    
    constexpr bool operator!=(allocator const&) const noexcept {
        return false;
    }
};

}
#endif
#endif
