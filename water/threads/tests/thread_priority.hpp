// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_TESTS_THREAD_PRIORITY_HPP
#define WATER_THREADS_TESTS_THREAD_PRIORITY_HPP
#include <water/threads/tests/bits.hpp>
#include <water/threads/thread.hpp>
#include <water/threads/mutex.hpp>
namespace water { namespace threads { namespace tests {


class thread_priority
{
    unsigned mydefault = 0;
    unsigned mycount = 0;
    join_t mythreads[3];
    mutex<> mylock;

public:
    thread_priority() {
        if(priority_exists) {
            priority p;
            if(p && p.min() && p.max()) { // they will be 0 on android
                mydefault = priority();
                ___water_test(mydefault);
                mydefault = p;
                mydefault = priority_default();
                ___water_test(mydefault);
                ___water_test(p.min() < p.max());
                unsigned count = 3;
                unsigned at = 0;
                while(at != count) {
                    unsigned pp = (p - 1) + at;
                    if(pp < p.min()) pp = p.min();
                    if(pp > p.max()) pp = p.max();
                    bool run_ok = run(*this, mythreads[at], run_options().priority(pp));
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
    }

    void operator()() {
        priority p(thread());
        ___water_test(p);
        ___water_test(p.set(mydefault));
        auto l = lock_move(mylock);
        ++mycount;
    }
};

}}}
#endif
