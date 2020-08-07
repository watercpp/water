// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_TESTS_CONDITION_ALL_HPP
#define WATER_THREADS_TESTS_CONDITION_ALL_HPP
#include <water/threads/tests/condition_functions.hpp>
#include <water/threads/tests/condition_pong.hpp>
#include <water/threads/tests/condition_wake_all.hpp>
namespace water { namespace threads { namespace tests {

template<typename condition_>
struct condition_all_tests {
    condition_all_tests() {
        condition_functions<condition_>();
        condition_pong<condition_>();
        condition_wake_all<condition_>();
    }
};

inline void condition_all() {
    test_list<condition_all_tests, condition_list>();
}

}}}
#endif
