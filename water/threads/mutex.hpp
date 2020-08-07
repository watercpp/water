// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_MUTEX_HPP
#define WATER_THREADS_MUTEX_HPP
#include <water/threads/configuration.hpp>
#include <water/threads/functions.hpp>
#include WATER_THREADS_INCLUDE(mutex.hpp)
namespace water { namespace threads {

template<typename ...needs_>
using mutex = typename need_select<needs<needs_...>, mutex_list>::result;

#if 0

template<typename ...needs_>
class mutex {
public:
    mutex();
    mutex(mutex const&) = delete;
    mutex& operator=(mutex const&) = delete;
    void lock();
    bool lock(double seconds);
    bool lock(deadline);
    bool try_lock();
    void unlock();
};

#endif

}}
#endif
