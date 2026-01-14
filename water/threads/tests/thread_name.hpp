// Copyright 2026 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_TESTS_THREAD_NAME_HPP
#define WATER_THREADS_TESTS_THREAD_NAME_HPP
#include <water/threads/tests/bits.hpp>
#include <water/threads/condition.hpp>
#include <water/threads/mutex.hpp>
#include <water/threads/thread_name.hpp>
#include <water/cstring.hpp>
namespace water { namespace threads { namespace tests {

// This test will only fail if setting and getting the name succeeds, but the names do not match.
// Because it might be possible to set and not get the name (on older Andorid versions)

class thread_name_test
{
    static constexpr char const* name_cstring = "name";

    condition<> mycondition;
    mutex_for_condition<condition<>> mylock;
    bool myset = false;
    bool myset_ok = false;
    bool mydone = false;
    
public:
    
    thread_name_test() {
        join_t j;
        bool run_ok = run(*this, j);
        ___water_test(run_ok);
        if(!run_ok)
            return;
        bool set_ok;
        {
            auto lock = lock_move(mylock);
            while(!myset)
                mycondition.wait(mylock);
            set_ok = myset_ok;
        }
        
        auto set = cstring(name_cstring);
        char name[128] {};
        size_t name_size = thread_name(name, sizeof(name), j);
        if(set_ok && name_size) {
            auto get = begin_end_from(name, name_size);
            ___water_test(set == get);
        }
        name_size = thread_name(name, sizeof(name), thread(j));
        if(set_ok && name_size) {
            auto get = begin_end_from(name, name_size);
            ___water_test(set == get);
        }
        
        auto name3 = thread_name_cstring(j);
        auto name4 = thread_name_cstring(thread(j));
        ___water_test(begin_end_from(name3), begin_end_from(name4));
        
        {
            auto lock = lock_move(mylock);
            mydone = true;
            mycondition.wake();
        }
        join(j);
    }
    
    void operator()() {
        bool set_ok = thread_name(name_cstring);
        auto name = thread_name_cstring();
        if(name.begin() != name.end()) {
            auto set = cstring(name_cstring);
            auto get = begin_end_from(name);
            ___water_test(set == get);
        }
        
        char name2_array[128];
        auto name2 = begin_end_from(name2_array, thread_name(name2_array, sizeof(name2_array)));
        ___water_test(begin_end_from(name) == name2);
        
        auto lock = lock_move(mylock);
        myset = true;
        myset_ok = set_ok;
        mycondition.wake();
        while(!mydone)
            mycondition.wait(mylock);
    }
};

inline void thread_name_all() {
    thread_name_test();
}

}}}
#endif
