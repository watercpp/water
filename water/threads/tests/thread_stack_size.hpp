// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_TESTS_THREAD_STACK_SIZE_HPP
#define WATER_THREADS_TESTS_THREAD_STACK_SIZE_HPP
#include <water/threads/tests/bits.hpp>
#include <water/threads/thread.hpp>
#include <water/threads/mutex.hpp>
namespace water { namespace threads { namespace tests {

class thread_stack_size
{
    unsigned mycount = 0;
    join_t mythreads[3];
    mutex<> mylock;

public:
    thread_stack_size() {
        if(stack_size_exists) {
            unsigned count = 3;
            unsigned at = 0;
            while(at != count) {
                size_t s = 512 * 1024 * (at + 1);
                bool run_ok = run(*this, mythreads[at], run_options().stack_size(s));
                ___water_test(run_ok);
                if(run_ok) ++at;
                else --count;
            }
            at = 0;
            while(at != count) {
                ___water_test(join(mythreads[at]));
                ++at;
            }
            ___water_test(mycount == count);
        }
    }

    void operator()() {
        auto l = lock_move(mylock);
        ++mycount;
    }
};

}}}
#endif
