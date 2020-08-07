// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_TESTS_THREAD_SLEEP_YIELD_HPP
#define WATER_THREADS_TESTS_THREAD_SLEEP_YIELD_HPP
#include <water/threads/tests/bits.hpp>
#include <water/threads/sleep.hpp>
#include <water/threads/yield.hpp>
namespace water { namespace threads { namespace tests {

inline void thread_sleep_yield() {
    ___water_test(sleep(deadline(0.2)));
    ___water_test(sleep(0.1));
    ___water_test(sleep(0));
    yield();
    #if defined(WATER_SYSTEM_APPLE) && !defined(WATER_THREADS_NO_APPLE)
    ___water_test(mach_wait_until(mach_time() - mach_time_from_seconds(60)) == 0);
    #endif
}

}}}
#endif
