// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_FIXED_ALLOCATOR_HPP
#define WATER_FIXED_ALLOCATOR_HPP
#include <water/water.hpp>
#include <water/throw_if.hpp>
namespace water { namespace fixed {

struct exception {};

template<typename memory_, typename exception_, bool lock_free_ = false>
class allocator
{
    using select_ = typename types::ifel<lock_free_, short, long>::result*;
    
    memory_ *my;

public:
    constexpr explicit allocator(memory_ *a = 0) noexcept :
        my(a)
    {}

    allocator(memory_& a = 0) noexcept :
        my(&a)
    {}

    void* allocate(size_t bytes) noexcept(types::equal<exception_, void>::result) {
        ___water_assert(my && bytes <= my->bytes());
        void *r = 0;
        if(bytes <= my->bytes())
            r = allocate_select(select_());
        if(!r)
            throw_if<exception_>();
        return r;
    }

    void free(void* pointer, size_t ___water_debug(bytes)) noexcept {
        ___water_assert(my && bytes <= my->bytes());
        my->free(pointer);
    }

    template<typename type_>
    type_* allocate(size_t count = 1) noexcept(types::equal<exception_, void>::result) {
        return static_cast<type_*>(allocate(sizeof(type_) * count));
    }

    template<typename type_>
    void free(void *pointer, size_t count = 1) noexcept {
        free(pointer, sizeof(type_) * count);
    }
    
    bool operator==(allocator const& a) const noexcept {
        return my == a.my;
    }
    
    bool operator!=(allocator const& a) const noexcept {
        return my != a.my;
    }

private:
    void* allocate_select(long*) {
        return my->allocate();
    }

    void* allocate_select(short*) {
        return my->allocate_lock_free();
    }
};

template<typename memory_> using allocator_throw = allocator<memory_, exception>;
template<typename memory_> using allocator_nothrow = allocator<memory_, void>;
template<typename memory_> using allocator_throw_lock_free = allocator<memory_, exception, true>;
template<typename memory_> using allocator_nothrow_lock_free = allocator<memory_, void, true>;

}}
#endif
