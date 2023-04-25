// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_MEMORY_TRACK_TESTS_BITS_HPP
#define WATER_MEMORY_TRACK_TESTS_BITS_HPP
#include <water/memory_track/memory_track.hpp>
#include <water/test.hpp>
namespace water { namespace memory_track { namespace tests {


template<typename ...a_>
void unused(a_ const&...)
{}


class cookie_copy_callback
{
public:
    using cookie_type = memory_track::cookie<>;

private:
    cookie_type my {};
    char const* myerror = 0;
    bool myfail = false;

public:
    void allocate_will_fail(bool a) noexcept {
        myfail = a;
    }

    bool allocate(cookie_type const *cookie, size_t /*bytes_allocated_now*/) noexcept {
        my = *cookie;
        my.next = my.prev = 0;
        return !myfail; // return false to make allocation fail
    }

    void free(cookie_type const *cookie) noexcept {
        my = *cookie;
        my.next = my.prev = 0;
    }

    template<typename name_, typename tag_>
    bool free_error(cookie_type const *cookie, void* pointer, size_t bytes, size_t align, name_ const& name, tag_ const& tag, char const* error) noexcept {
        // cookie can be 0
        if(cookie)
            free(cookie);
        else
            my = {};
        myerror = error;
        unused(pointer, bytes, align, name, tag);
        return false; // dont breakpoint
    }

    cookie_type const& cookie() const {
        return my;
    }

    char const* error() const {
        return myerror;
    }
    
};


using test_memory = memory<void, cookie_copy_callback>;


}}}
#endif
