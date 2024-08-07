// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_TESTS_BARRIER_ALL_HPP
#define WATER_THREADS_TESTS_BARRIER_ALL_HPP
#include <water/threads/tests/bits.hpp>
#include <water/threads/tests/barrier_1.hpp>
#include <water/threads/tests/barrier_2.hpp>
#include <water/threads/tests/barrier_3.hpp>
namespace water { namespace threads { namespace tests {

template<typename barrier_>
struct barrier_all_tests {
    barrier_all_tests(bool skip3 = false) {
        barrier_1<barrier_>();
        barrier_2<barrier_>();
        if(!skip3)
            barrier_3<barrier_>();
    }
};

struct barrier_dummy {};

inline void barrier_all() {

    #ifdef WATER_THREADS_WINDOWS

    barrier_all_tests<barrier_semaphore>();
    barrier_all_tests<spin_barrier<>>();
    ifel<synchronization_barrier::needs::need != 0, barrier_1<synchronization_barrier>, barrier_dummy>{};
    ifel<synchronization_barrier::needs::need != 0, barrier_2<synchronization_barrier>, barrier_dummy>{};
    // synchronization_barrier cannot do barrier_3
    barrier_all_tests<synchronization_barrier>(true);
    
    #elif defined(WATER_SYSTEM_ANDROID)
    
    #ifdef WATER_POSIX_BARRIERS
    barrier_all_tests<pthread_barrier>(true); // barrier_3 fails
    #endif
    barrier_all_tests<barrier_futex<>>();
    barrier_all_tests<spin_barrier<>>();
        
    #else
    
    test_list<barrier_all_tests, barrier_list>();

    #endif
}

}}}
#endif
