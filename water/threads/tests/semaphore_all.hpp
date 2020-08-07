// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_TESTS_SEMAPHORE_ALL_HPP
#define WATER_THREADS_TESTS_SEMAPHORE_ALL_HPP
#include <water/threads/tests/semaphore_pong.hpp>
namespace water { namespace threads { namespace tests {

template<typename semaphore_>
struct semaphore_all_tests {
    semaphore_all_tests() {
        semaphore_pong<semaphore_>();
    }
};

inline void semaphore_all() {
    test_list<semaphore_all_tests, semaphore_list>();
}

}}}
#endif
