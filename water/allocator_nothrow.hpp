// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_ALLOCATOR_NOTHROW_HPP
#define WATER_ALLOCATOR_NOTHROW_HPP

// The main non-throwing dynamic memory allocator used by water. Replace with your own by defining WATER_ALLOCATOR_NOTHROW

#ifdef WATER_ALLOCATOR_NOTHROW
namespace water {
using allocator_nothrow = WATER_ALLOCATOR_NOTHROW;
}

#elif defined(WATER_NO_STD)

#include <water/allocator_malloc.hpp>
namespace water {
using allocator_nothrow = allocator_malloc_nothrow;
}

#else

#include <water/water.hpp>
#include <new>
#include <water/align_max.hpp>
#ifndef __cpp_aligned_new
    #include <water/align_with_unalign.hpp>
#endif
namespace water {

struct allocator_nothrow
{
    void* allocate(size_t bytes) noexcept {
        return ::operator new(bytes, std::nothrow);
    }

    #ifdef __cpp_sized_deallocation
    
    void free(void *pointer, size_t bytes) noexcept {
        ::operator delete(pointer, bytes); // using the not-nothrow delete because it has size
    }
    
    #else
    
    void free(void *pointer, size_t) noexcept {
        ::operator delete(pointer);
    }
    
    #endif

    template<typename type_>
    type_* allocate(size_t count = 1) noexcept {
        void *r = 0;
        auto align = alignof(type_); // avoids visual c++ conditional expression is constant
        if(align > align_allocations) {
            #ifdef __cpp_aligned_new
            r = ::operator new(sizeof(type_) * count, static_cast<std::align_val_t>(align), std::nothrow);
            #else
            r = align_with_unalign(::operator new(sizeof(type_) * count + align, std::nothrow), align);
            #endif
        }
        else
            r = ::operator new(sizeof(type_) * count, std::nothrow);
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
    
    constexpr bool operator==(allocator_nothrow const&) const noexcept {
        return true;
    }
    
    constexpr bool operator!=(allocator_nothrow const&) const noexcept {
        return false;
    }
};

}

#endif
#endif
