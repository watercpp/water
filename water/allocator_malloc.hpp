// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_ALLOCATOR_MALLOC_HPP
#define WATER_ALLOCATOR_MALLOC_HPP
#include <water/water.hpp>
#include <water/throw_if.hpp>
#include <water/align_max.hpp>
#include <water/align_with_unalign.hpp>
#ifdef WATER_NO_CHEADERS
    #include <stdlib.h>
#else
    #include <cstdlib>
#endif
namespace water {

struct allocator_malloc_nothrow
{
    void* allocate(size_t bytes) noexcept {
        #ifdef WATER_NO_CHEADERS
        return ::malloc(bytes);
        #else
        return std::malloc(bytes);
        #endif
    }

    void free(void *pointer, size_t = 0) noexcept {
        #ifdef WATER_NO_CHEADERS
        return ::free(pointer);
        #else
        return std::free(pointer);
        #endif
    }

    template<typename type_>
    type_* allocate(size_t count = 1) noexcept {
        // assumes that malloc returns something aligned to water::align_max, and
        // the default alignment will work when alignof(type_) < align_max
        auto align = alignof(type_); // avoids visual c++ conditional expression is constant
        if(align > align_max)
            return static_cast<type_*>(align_with_unalign(allocate(sizeof(type_) * count + align), align));
        return static_cast<type_*>(allocate(sizeof(type_) * count));
    }

    template<typename type_>
    void free(void *pointer, size_t /*count*/ = 1) noexcept {
        auto align = alignof(type_);
        if(align > align_max)
            free(unalign(pointer));
        else
            free(pointer);
    }
    
    constexpr bool operator==(allocator_malloc_nothrow const&) const noexcept {
        return true;
    }
    
    constexpr bool operator!=(allocator_malloc_nothrow const&) const noexcept {
        return false;
    }
};


struct allocator_malloc
{
    struct exception {};
    
    void* allocate(size_t bytes) noexcept(false) {
        void *r = allocator_malloc_nothrow{}.allocate(bytes);
        if(!r) throw_if<exception>();
        return r;
    }

    void free(void *pointer, size_t = 0) noexcept {
        allocator_malloc_nothrow{}.free(pointer);
    }

    template<typename type_>
    type_* allocate(size_t count = 1) noexcept(false) {
        void *r = allocator_malloc_nothrow{}.template allocate<type_>(count);
        if(!r) throw_if<exception>();
        return static_cast<type_*>(r);
    }

    template<typename type_>
    void free(void *pointer, size_t count = 1) noexcept {
        allocator_malloc_nothrow{}.template free<type_>(pointer, count);
    }
    
    constexpr bool operator==(allocator_malloc const&) const noexcept {
        return true;
    }
    
    constexpr bool operator!=(allocator_malloc const&) const noexcept {
        return false;
    }
};

}
#endif
