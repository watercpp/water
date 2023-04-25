// Copyright 2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_TESTS_THREAD_RUN_COPY_HPP
#define WATER_THREADS_TESTS_THREAD_RUN_COPY_HPP
#include <water/threads/tests/bits.hpp>
#include <water/threads/thread.hpp>
#include <water/threads/condition.hpp>
namespace water { namespace threads { namespace tests {

/*

test run_copy()

*/

template<typename function_>
class thread_run_move
{
    function_ *my;

public:

    explicit thread_run_move(function_ *a) :
        my{a}
    {}
    
    thread_run_move(thread_run_move&& a) :
        my{a.my}
    {
        a.my = 0;
    }
    
    thread_run_move(thread_run_move const&) = delete;
    thread_run_move& operator=(thread_run_move const&) = delete;
    thread_run_move& operator=(thread_run_move&&) = delete;
    
    void operator()() {
        (*my)();
    }
};


class thread_run_copy
{
    unsigned mycount = 0;
    condition<> mycondition;
    mutex_for_condition<condition<> > mylock;

public:
    thread_run_copy() {
        unsigned count = 0;
        bool run_ok;
        thread_t t;
        join_t j;
        
        auto function = [this] { this->run(); };
        auto const& function_const = function;
        using function_type = decltype(function);
        
        // no thread
        ___water_test(run_ok = run_copy(function));
        if(run_ok) ++count;
        ___water_test(run_ok = run_copy(function_const));
        if(run_ok) ++count;
        ___water_test(run_ok = run_copy(function_type{function}));
        if(run_ok) ++count;
        ___water_test(run_ok = run_copy(thread_run_move<function_type>{&function}));
        if(run_ok) ++count;
        
        // thread
        ___water_test(run_ok = run_copy(function, t));
        if(run_ok) ++count;
        ___water_test(run_ok = run_copy(thread_run_move<function_type>{&function}, t));
        if(run_ok) ++count;
        
        // join
        ___water_test(run_ok = run_copy(function, j));
        if(run_ok) {
            ___water_test(equal(thread(j), thread(j)));
            ++count;
            ___water_test(join(j));
        }
        ___water_test(run_ok = run_copy(thread_run_move<function_type>{&function}, j));
        if(run_ok) {
            ++count;
            ___water_test(join(j));
        }
        
        // no thread + options
        ___water_test(run_ok = run_copy(function, run_options()));
        if(run_ok) ++count;
        
        // thread + options
        ___water_test(run_ok = run_copy(function, t, run_options()));
        if(run_ok) ++count;
        
        // join + options
        ___water_test(run_ok = run_copy(function, j, run_options()));
        if(run_ok) {
            ++count;
            ___water_test(join(j));
        }
        
        deadline d(10);
        auto l = lock_move(mylock);
        do {
            if(mycount == count)
                break;
            mycondition.wait(mylock, d);
        } while(d.left());
        ___water_test(d.left());
    }

    void run() {
        auto l = lock_move(mylock);
        ++mycount;
        mycondition.wake();
    }
};

}}}
#endif
