// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_MEMORY_TRACK_UNDERLYING_ALLOCATOR_HPP
#define WATER_MEMORY_TRACK_UNDERLYING_ALLOCATOR_HPP
// minimal dependency
#include <water/configuration.hpp>
#include <water/throw_if.hpp>
#ifdef WATER_MEMORY_TRACK_UNDERLYING_ALLOCATOR
    // nothing
#elif defined(WATER_NO_CHEADERS)
    #include <stdlib.h>
#else
    #include <cstdlib>
#endif
namespace water { namespace memory_track {

/*

Replaceable with WATER_MEMORY_TRACK_UNDERLYING_ALLOCATOR

memory_track::memory uses this to allocate memory, also useful to allocate untracked memory

*/

struct underlying_allocator_exception {};

template<bool>
struct underlying_allocator_exception_select {
    using result = underlying_allocator_exception;
};

template<>
struct underlying_allocator_exception_select<false> {
    using result = void;
};

template<bool exception_ = false>
struct underlying_allocator
{
    static bool constexpr is_noexcept = !exception_;
    
    void* allocate(size_t bytes) noexcept(is_noexcept) {
        #ifdef WATER_MEMORY_TRACK_UNDERLYING_ALLOCATOR
        void *r = WATER_MEMORY_TRACK_UNDERLYING_ALLOCATOR().allocate(bytes);
        #elif defined(WATER_NO_CHEADERS)
        void *r = ::malloc(bytes);
        #else
        void *r = std::malloc(bytes);
        #endif
        if(!r) throw_if<typename underlying_allocator_exception_select<exception_>::result>();
        return r;
    }

    void free(void *pointer, size_t /*bytes*/) noexcept {
        #ifdef WATER_MEMORY_TRACK_UNDERLYING_ALLOCATOR
        WATER_MEMORY_TRACK_UNDERLYING_ALLOCATOR().free(pointer, bytes);
        #elif defined(WATER_NO_CHEADERS)
        ::free(pointer);
        #else
        std::free(pointer);
        #endif
    }

    template<typename type_>
    type_* allocate(size_t count = 1) noexcept(is_noexcept) {
        return static_cast<type_*>(allocate(sizeof(type_) * count));
    }

    template<typename type_>
    void free(void *pointer, size_t /*count*/ = 1) noexcept {
        free(pointer, 0);
    }
    
    constexpr bool operator==(underlying_allocator const&) const noexcept {
        return true;
    }
    
    constexpr bool operator!=(underlying_allocator const&) const noexcept {
        return false;
    }
};

using underlying_allocator_throw = underlying_allocator<true>;
using underlying_allocator_nothrow = underlying_allocator<>;

}}
#endif
