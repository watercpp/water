// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_TESTS_MUTEX_ALL_HPP
#define WATER_THREADS_TESTS_MUTEX_ALL_HPP
#include <water/threads/tests/bits.hpp>
#include <water/threads/tests/mutex_count.hpp>
#include <water/threads/tests/mutex_functions.hpp>
#include <water/threads/mutex.hpp>
namespace water { namespace threads { namespace tests {

template<typename mutex_> struct
 mutex_all_tests {
  mutex_all_tests() {
 		mutex_functions<mutex_>();
 		mutex_count<mutex_>();
 		}
 	};

inline void mutex_all() {
	test_list<mutex_all_tests, mutex_list>();
	}

//inline void mutex_all() {
	//mutex<> m1;
	//mutex<need_water> m2;
	//mutex<need_system> m3;
	//mutex<need_water, need_trivial_destructor> m4;
	//auto l1 = lock_move(m1);
	//auto w2 = lock(m2, 0.5);
	//___water_test(w2);
	//auto u2 = unlock_move(m2);
	//auto t3 = try_lock(m3);
	//___water_test(t3);
	//unlock(m3);
	//auto t4 = try_lock(m4);
	//___water_test(t4);
	//unlock(m4);
//	}

}}}
#endif
