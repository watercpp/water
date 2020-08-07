// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_TESTS_READ_WRITE_ALL_HPP
#define WATER_THREADS_TESTS_READ_WRITE_ALL_HPP
#include <water/threads/tests/bits.hpp>
#include <water/threads/read_write.hpp>
#include <water/threads/tests/read_write_functions.hpp>
#include <water/threads/tests/read_write_count.hpp>
namespace water { namespace threads { namespace tests {

template<typename read_write_>
struct read_write_all_tests {
    read_write_all_tests() {
        read_write_functions<read_write_>();
        read_write_count<read_write_>();
    }
};

inline void read_write_all() {
    #ifdef WATER_SYSTEM_LINUX
    // for some reason read_write_count deadlocks on android with pthread_rwlock ????
    read_write_functions<pthread_rwlock>();
    test_list<read_write_all_tests, list<threads::read_write_count<>, threads::spin_read_write<>>>();
    #else
    test_list<read_write_all_tests, read_write_list>();
    #endif
}

}}}
#endif
