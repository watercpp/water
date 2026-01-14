// Copyright 2022-2026 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_THREAD_NAME_HPP
#define WATER_THREADS_THREAD_NAME_HPP
#include <water/threads/configuration.hpp>
#include WATER_THREADS_INCLUDE(thread_name.hpp)
namespace water { namespace threads {

// Useful for debugging and maybe crash reports. Debuggers usually show the thread names.
// Probably not useful for logging because getting the thread name could be slow.
// Keep the name short, Linux/Android seems to have a limit of 16 characters including the 0 at the end.
// Use only on running (or waiting to be joined) threads.

#if 0

// set the name of this thread

bool thread_name(char const* name) noexcept;


// copy the name into to
// the string is always zero terminated. (except if to or to_size are 0)
// returns the length *without* the zero at the end.
// returns 0 on error or if there was no name.

size_t thread_name(char* to, size_t to_size) noexcept;
size_t thread_name(char* to, size_t to_size, thread_t thread) noexcept;
size_t thread_name(char* to, size_t to_size, join_t thread) noexcept;

#endif


// get name as a convertible-to-cstring.
// begin() end() size() excludes the zero at end, so begin()[size()] == 0 and *end() == 0

class thread_name_cstring
{
    char my[64];
    size_t mysize = 0; // without zero at the end

public:
    
    thread_name_cstring() {
        mysize = thread_name(my, sizeof(my));
    }

    thread_name_cstring(thread_t thread) {
        mysize = thread_name(my, sizeof(my), thread);
    }

    thread_name_cstring(join_t thread) {
        mysize = thread_name(my, sizeof(my), thread);
    }

    operator char const*() const {
        return my;
    }

    char const* begin() const {
        return my;
    }

    char const* end() const {
        return my + mysize;
    }

    size_t size() const {
        return mysize;
    }
};


}}
#endif
