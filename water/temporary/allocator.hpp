// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_TEMPORARY_ALLOCATOR_HPP
#define WATER_TEMPORARY_ALLOCATOR_HPP
#include <water/temporary/block.hpp>
#include <water/std_allocator.hpp>
namespace water { namespace temporary {

template<typename memory_, typename exception_ = void>
class allocator
{
public:
    using memory_type = memory_;
    using exception = exception_;
    static bool constexpr is_noexcept = allocator_noexcept<typename memory_::allocator_type>() && equal<void, exception>;

private:
    memory_type *my;

public:
    constexpr explicit allocator(memory_type *a = 0) noexcept :
        my{a}
    {}

    allocator(memory_type& a) noexcept :
        my(&a)
    {}
    
    bool operator==(allocator const& a) const noexcept {
        return my == a.my;
    }
    
    bool operator!=(allocator const& a) const noexcept {
        return my != a.my;
    }

public:

    // temporary
    
    block* find(void const* pointer) const noexcept {
        return my ? temporary::find(my->list(), pointer) : 0;
    }

    size_t release(void const* pointer) noexcept {
        return my ? temporary::release(my->list(), pointer) : 0;
    }

    size_t retain(void const* pointer) noexcept {
        return my ? temporary::retain(my->list(), pointer) : 0;
    }

    void* push(size_t bytes, size_t align = 0, bool allocate_block = true) noexcept(is_noexcept) {
        void *r = my ? temporary::push(my->list(), bytes, align, allocate_block ? my->allocate_and_keep_blocks() : 0, my->allocator_pointer(), my->sizer()) : 0;
        if(!r) throw_if<exception>();
        return r;
    }

    void* push_at_least(size_t& pushed, size_t at_least, size_t multiply = 1, size_t align = 0, bool allocate_block = true) noexcept(is_noexcept) {
        pushed = 0;
        void *r = my ? temporary::push_at_least(my->list(), pushed, at_least, multiply, align, allocate_block ? my->allocate_and_keep_blocks() : 0, my->allocator_pointer(), my->sizer()) : 0;
        if(!r) throw_if<exception>();
        return r;
    }

    void pop(void const* pointer, size_t bytes) noexcept {
        ___water_assert(my || !pointer);
        if(my)
            temporary::pop(my->list(), pointer, bytes);
    }

    void pop(void const* begin, void const* end) noexcept {
        ___water_assert(my || !begin);
        if(my)
            temporary::pop(my->list(), begin, end);
    }

    void* resize(void const* pointer, size_t old_size, size_t new_size) noexcept {
        return my ? temporary::resize(my->list(), pointer, old_size, new_size) : 0;
    }

    template<typename type_>
    type_* push(size_t count, bool allocate_block = true) noexcept(is_noexcept) {
        return static_cast<type_*>(push(count * sizeof(type_), alignof(type_), allocate_block));
    }

    template<typename type_>
    type_* push_at_least(size_t& pushed, size_t at_least, bool allocate_block = true) noexcept(is_noexcept) {
        return static_cast<type_*>(push_at_least(pushed, at_least, sizeof(type_), alignof(type_), allocate_block));
    }

    template<typename type_>
    void pop(type_ const* pointer, size_t count) noexcept {
        pop(static_cast<void const*>(pointer), count * sizeof(type_));
    }

    template<typename type_>
    type_* resize(type_ const* pointer, size_t old_count, size_t new_count) noexcept {
        return static_cast<type_*>(resize(static_cast<void const*>(pointer), old_count * sizeof(type_), new_count * sizeof(type_)));
    }

public:

    // water::allocator interface
    
    void* allocate(size_t bytes) noexcept(is_noexcept) {
        return push(bytes);
    }

    void free(void *pointer, size_t bytes) {
        pop(pointer, bytes);
    }

    template<typename type_>
    type_* allocate(size_t count = 1) noexcept(is_noexcept) {
        return push<type_>(count);
    }

    template<typename type_>
    void free(void *pointer, size_t count = 1) noexcept {
        pop(static_cast<type_*>(pointer), count);
    }
};

template<typename memory_> using allocator_throw = allocator<memory_, exception>;
template<typename memory_> using allocator_nothrow = allocator<memory_>;

template<typename type_, typename memory_> using std_allocator = std_allocator<type_, allocator_throw<memory_>>;

}}
#endif
