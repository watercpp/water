// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_TEMPORARY_BITS_HPP
#define WATER_TEMPORARY_BITS_HPP
#include <water/align_max.hpp>
#include <water/int.hpp>
#include <water/throw_if.hpp>
#include <water/types.hpp>
namespace water { namespace temporary {

using byte = unsigned char;

using upointer_t = if_not_void<uint_size_at_least<sizeof(void*)>, uint_largest_t>;

// waste is minimum allocation size, and also maximum align. see temporary::pop
size_t constexpr waste =
    #ifdef WATER_TEMPORARY_WASTE
    WATER_TEMPORARY_WASTE;
    #else
    align_allocations;
    #endif
    
struct sizer {
    size_t mymin;
public:
    constexpr explicit sizer(size_t min_size = 1024 * sizeof(void*)) noexcept :
        mymin{min_size}
    {}
    size_t operator()(size_t a) const noexcept {
        if(a >= (static_cast<size_t>(-1) >> 1))
            return a;
        size_t r = mymin;
        while(r < a) r <<= 1;
        return r;
    }
};

struct exception {};

template<typename allocator_>
inline constexpr bool allocator_noexcept() {
    return noexcept(make_type<allocator_&>().allocate(1));
}

template<>
inline constexpr bool allocator_noexcept<void>() {
    return true;
}

}}
#endif
