// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_MEMORY_TRACK_ALLOCATOR_HPP
#define WATER_MEMORY_TRACK_ALLOCATOR_HPP
#include <water/memory_track/allocator_base.hpp>
namespace water { namespace memory_track {

/*

Allocates from memory_track::memory

The constructor needs a memory_track::memory object or it cannot allocate memory.

Look at allocator_base.hpp.

*/

template<typename memory_, typename exception_ = void>
class allocator :
    public allocator_base<allocator<memory_, exception_>, memory_, exception_>
{
    using base_ = allocator_base<allocator<memory_, exception_>, memory_, exception_>;
    
    memory_ *mymemory = 0;

public:

    constexpr allocator() = default;

    constexpr allocator(memory_& memory, typename memory_::tag_type const& tag = {}) :
        base_{tag},
        mymemory{&memory}
    {}
    
    memory_* memory_pointer() noexcept {
        return mymemory;
    }
    
    memory_ const* memory_pointer() const noexcept {
        return mymemory;
    }
    
    using base_::operator==;
    using base_::operator!=;
};


template<typename memory_> using allocator_throw = allocator<memory_, exception>;
template<typename memory_> using allocator_nothrow = allocator<memory_>;


}}
#endif
