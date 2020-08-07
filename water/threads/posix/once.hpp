// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_POSIX_ONCE_HPP
#define WATER_THREADS_POSIX_ONCE_HPP
#include <water/threads/posix/bits.hpp>
namespace water { namespace threads {

class once
{
    pthread_once_t my = PTHREAD_ONCE_INIT;

public:
    constexpr once() noexcept = default;
    once(once const&) = delete;
    once& operator=(once const&) = delete;

    void operator()(void (*function)()) {
        int e = pthread_once(&my, function);
        ___water_assert(!e);
        panic_if(e);
    }
};

}}
#endif
