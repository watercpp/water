// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_TEMPORARY_ALLOCATOR_TRACK_HPP
#define WATER_TEMPORARY_ALLOCATOR_TRACK_HPP
#ifndef WATER_TEMPORARY_MEMORY_TRACK_HPP
    #error include memory_track.hpp
#endif
#include <water/temporary/allocator.hpp>
namespace water { namespace temporary {

// specializations of allocator for memory_track
// means the only thing needed for tracking is swapping memory to memory_track

template<typename allocator_, typename sizer_, typename out_, typename exception_>
class allocator<memory_track<allocator_, sizer_, out_>, exception_>
{
public:
    using memory_type = memory_track<allocator_, sizer_, out_>;
    using exception = exception_;
    static bool constexpr is_noexcept = allocator_noexcept<typename memory_type::allocator_type>() && types::equal_plain<void, exception>::result;

private:
    memory_type *my;

public:
    constexpr explicit allocator(memory_type *a = 0) noexcept :
        my{a}
    {}

    allocator(memory_type& a) noexcept :
        my(&a)
    {}

private:
    void block_count() {
        auto *b = my->list();
        if(!b) return;
        size_t
            c = 0,
            m = 0,
            r = 0;
        do {
            ++c;
            m += b->memory_use();
            r += b->retain;
        } while((b = b->next) != my->list());
        auto &s = my->statistics();
        if(s.block_count_max < c) s.block_count_max = c;
        if(s.block_memory_use_peak < m) s.block_memory_use_peak = m;
        if(s.retain_max < r) s.retain_max = r;
    }

    void pop_do(void const *pointer) {
        block *b = my->list(); // will be block with pointer if pop was success
        auto &s = my->statistics();
        if(b->begin <= pointer && pointer < b) {
            if(b->free <= pointer)
                _::statistic::add(s.pop.popped);
            _::statistic::add(s.pop.success);
        }
        else
            _::statistic::add(s.pop.failure);
    }

public:
    
    // temporary
    
    block* find(void const* pointer) const noexcept {
        return my->statistics().find(temporary::find(my->list(), pointer));
    }

    size_t release(void const* pointer) noexcept {
        return my->statistics().release(temporary::release(my->list(), pointer));
    }

    size_t retain(void const* pointer) noexcept {
        auto r = my->statistics().retain(temporary::retain(my->list(), pointer));
        if(r)
            block_count();
        return r;
    }

    void* push(size_t bytes, size_t align = 0, bool allocate_block = true) noexcept(is_noexcept) {
        auto &s = my->statistics();
        auto undo = s.push.failure;
        s.push.failure(bytes);
        void *r = temporary::push(my->list(), bytes, align, allocate_block ? my->allocate_and_keep_blocks() : 0, my->allocator_pointer(), my->sizer());
        if(!r) throw_if<exception>();
        else {
            s.push.failure = undo;
            s.push.success(bytes);
            block_count();
        }
        return r;
    }

    void* push_at_least(size_t& pushed, size_t at_least, size_t multiply = 1, size_t align = 0, bool allocate_block = true) noexcept(is_noexcept) {
        auto &s = my->statistics();
        auto undo = s.push_at_least.failure;
        s.push_at_least.failure(at_least * (multiply ? multiply : 1));
        void *r = temporary::push_at_least(my->list(), pushed, at_least, multiply, align, allocate_block ? my->allocate_and_keep_blocks() : 0, my->allocator_pointer(), my->sizer());
        if(!r) throw_if<exception>();
        else {
            s.push_at_least.failure = undo;
            s.push_at_least.success(at_least * (multiply ? multiply : 1));
            block_count();
        }
        return r;
    }

    void pop(void const* pointer, size_t bytes) noexcept {
        temporary::pop(my->list(), pointer, bytes);
        pop_do(pointer);
    }

    void pop(void const* begin, void const* end) noexcept {
        temporary::pop(my->list(), begin, end);
        pop_do(begin);
    }

    void* resize(void const* pointer, size_t old_size, size_t new_size) noexcept {
        return my->statistics().resize(temporary::resize(my->list(), pointer, old_size, new_size));
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

}}
#endif
