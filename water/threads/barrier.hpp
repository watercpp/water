// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_BARRIER_HPP
#define WATER_THREADS_BARRIER_HPP
#include <water/threads/configuration.hpp>
#include WATER_THREADS_INCLUDE(barrier.hpp)
namespace water { namespace threads {

template<typename ...needs_>
using barrier = typename need_select<needs<needs_...>, barrier_list>::result;

#if 0

template<typename ...needs_>
class barrier {
public:
    barrier(unsigned count);
    barrier(barrier const&) = delete;
    barrier& operator=(barrier const&) = delete;
    explicit operator bool() const; // true if constructed ok
    bool operator()(); // returns true for one of the threads, the reset get false
    bool reset(unsigned count); // change the thread count, this must not be in use while doing this
};

#endif

}}
#endif
