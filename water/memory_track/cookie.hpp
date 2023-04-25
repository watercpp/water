// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_MEMORY_TRACK_COOKIE_HPP
#define WATER_MEMORY_TRACK_COOKIE_HPP
#include <water/memory_track/allocator_tag.hpp>
#include <water/memory_track/names.hpp>
namespace water { namespace memory_track {

/*

linked list, added to all allocations

*/



template<typename tag_ = void>
struct cookie
{
    using tag_type = tag_;
    cookie
        *next = 0,
        *prev = 0;
    large_t id = 0; // unique and ordered, incremented for each allocation, except if it reaches max
    byte *pointer = 0; // the allocation
    size_t bytes = 0;
    size_t align = 0;
    memory_track::name name;
    memory_track::allocator_tag allocator_tag;
    tag_type tag {};
    
    tag_type const& tag_or_not() const {
        return tag;
    }
    
    void tag_or_not(tag_type const& a) noexcept {
        tag = a;
    }
};

template<>
struct cookie<void>
{
    cookie
        *next = 0,
        *prev = 0;
    large_t id = 0;
    byte *pointer = 0;
    size_t bytes = 0;
    size_t align = 0;
    memory_track::name name;
    memory_track::allocator_tag allocator_tag;
    
    no_tag tag_or_not() const noexcept {
        return {};
    }
    
    void tag_or_not(no_tag) noexcept {
    }
};



template<typename tag_>
void const* overrun(cookie<tag_> const* c) noexcept {
    return c && c->pointer ? pattern_check(c->pointer + c->bytes, pattern_size) : 0;
}

template<typename tag_>
void const* underrun(cookie<tag_> const* c) noexcept {
    if(!c || !c->pointer)
        return 0;
    auto unaligned = c->pointer;
    if(align_differently(c->align)) {
        unaligned = static_cast<byte*>(unalign(c->pointer, c->align));
        if(!unaligned)
            return c->pointer - 1; // or somewhere close
    }
    byte const *b = static_cast<byte const*>(static_cast<void const*>(c + 1));
    return unaligned ? pattern_check(b, static_cast<size_t>(unaligned - b)) : 0;
}



template<typename cookie_>
class cookie_iterator
{
public:
    using value_type = cookie_;
    using pointer = value_type const*;
    using reference = value_type const&;

private:
    cookie_ const *my = 0;

public:
    cookie_iterator() noexcept = default;

    cookie_iterator(cookie_ const *a) noexcept :
        my(a)
    {}

    explicit operator bool() const noexcept {
        return my != 0;
    }

    pointer operator->() const noexcept {
        return my;
    }

    reference operator*() const noexcept {
        return *my;
    }

    cookie_iterator& operator++() noexcept {
        my = my->next;
        return *this;
    }

    cookie_iterator& operator--() noexcept {
        my = my->prev;
        return *this;
    }

    cookie_iterator operator++(int) noexcept {
        cookie_iterator r{*this};
        my = my->next;
        return r;
    }

    cookie_iterator operator--(int) noexcept {
        cookie_iterator r{*this};
        my = my->prev;
        return r;
    }

    bool operator==(cookie_iterator a) const noexcept {
        return my == a.my;
    }

    bool operator!=(cookie_iterator a) const noexcept {
        return my != a.my;
    }
};



}}
#endif
