// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_MEMORY_TRACK_BITS_HPP
#define WATER_MEMORY_TRACK_BITS_HPP
#include <water/water.hpp>
#include <water/int.hpp>
#include <water/throw_if.hpp>
#include <water/types.hpp>
#include <water/new_here.hpp>
#include <water/align_max.hpp>
#include <water/align_with_unalign.hpp>
namespace water { namespace memory_track {


using byte = unsigned char;

using large_t = uint_bits_at_least<64>;
// this will be at least 64 bits.
// if 1GB is allocated every second, it will overflow after 544 years
// used as a counter, it will overflow after 544 years if the count is incremented every nanosecond

using upointer_t = if_not_void<uint_size_at_least<sizeof(void*)>, size_t>;

inline upointer_t upointer(void const* a) noexcept {
    return reinterpret_cast<upointer_t>(a);
}


struct exception {};


size_t constexpr align_allocations = water::align_allocations;

constexpr bool align_differently(size_t align) {
    return align && (align > align_allocations || align_allocations % align);
}



struct no_tag
{
    constexpr bool operator==(no_tag const&) const noexcept {
        return true;
    }

    constexpr bool operator!=(no_tag const&) const noexcept {
        return false;
    }
};



// the default callback, that does nothing.
// if you replace it, never use the memory object from it beacuse it will deadlock.
struct no_callback
{
    template<typename cookie_>
    bool allocate(cookie_ const* /*cookie*/, size_t /*bytes_allocated_now*/) noexcept {
        // return false to make allocation fail
        // cookie is the current allocation
        // bytes_allocated_now is total number of bytes allocated now, excluding this allocation
        return true;
    }

    template<typename cookie_>
    void free(cookie_ const* /*cookie*/) noexcept {
        // free without errors
        // cookie is the current allocation
    }

    template<typename cookie_, typename name_, typename tag_>
    bool free_error(cookie_ const* /*cookie*/, void* /*pointer*/, size_t /*bytes*/, size_t /*algin*/, name_ const& /*name*/, tag_ const& /*tag*/, char const* /*error*/) noexcept {
        // free failed
        // cookie can be 0
        return true; // return true to breakpoint/assert, false is useful for testing
    }
};



size_t constexpr pattern_size = sizeof(void*);


inline void pattern_set(void *pointer, size_t bytes) {
    if(pointer && bytes) {
        byte const dead[] = {0xde, 0xad};
        unsigned d = static_cast<unsigned>(upointer(pointer) & 1);
        auto
            i = static_cast<byte*>(pointer),
            e = i + bytes;
        do *i = dead[d = !d]; while(++i != e);
    }
}


inline void const* pattern_check(void const* pointer, size_t bytes) {
    // return address where it does not match pattern, 0 if it matches
    if(!pointer || !bytes)
        return 0;
    byte const dead[] = {0xde, 0xad};
    unsigned d = static_cast<unsigned>(upointer(pointer) & 1);
    auto
        i = static_cast<byte const*>(pointer),
        e = i + bytes;
    do {
        if(*i != dead[d = !d])
            return i;
    } while(++i != e);
    return 0;
}



template<typename unsigned_>
void add(unsigned_& a, first<unsigned_> b = 1) {
    if(a > static_cast<unsigned_>(-1) - b)
        a = static_cast<unsigned_>(-1);
    else
        a += b;
}

template<typename unsigned_>
void sub(unsigned_& a, first<unsigned_> b = 1) {
    if(a < b)
        a = 0;
    else
        a -= b;
}



}}
#endif
