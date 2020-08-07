// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_TESTS_THREAD_ALL_HPP
#define WATER_THREADS_TESTS_THREAD_ALL_HPP
#include <water/threads/tests/thread_priority.hpp>
#include <water/threads/tests/thread_run.hpp>
#include <water/threads/tests/thread_sleep_yield.hpp>
#include <water/threads/tests/thread_stack_size.hpp>
namespace water { namespace threads { namespace tests {

inline void thread_all() {
    thread_priority();
    thread_run();
    thread_sleep_yield();
    thread_stack_size();
}

}}}
#endif
