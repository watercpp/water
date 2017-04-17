// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_TESTS_ONCE_ALL_HPP
#define WATER_THREADS_TESTS_ONCE_ALL_HPP
#include <water/threads/tests/bits.hpp>
#include <water/threads/once.hpp>
#include <water/threads/spin_once.hpp>
#include <water/threads/sleep.hpp>
namespace water { namespace threads { namespace tests {

template<typename once_> struct
 once_hold {
	static once_ once;
	static unsigned count;
	};
	template<typename once_> once_ once_hold<once_>::once;
	template<typename once_> unsigned once_hold<once_>::count;

template<typename once_> void once_add() {
	sleep(0.5);
	++once_hold<once_>::count;
	}

template<typename once_> struct
 once_once {
	void operator()() {
		once_hold<once_>::once(&once_add<once_>);
		___water_test(once_hold<once_>::count == 1);
		}
	};

template<typename once_> struct
 once_all_test {
 	once_all_test() {
		once_once<once_> once;
		once();
		once();
		run_many(once, 10);
		}
	};
template<> struct
 once_all_test<spin_once<false>>	
	{};

inline void once_all() {
	once_all_test<threads::once>();
	once_all_test<spin_once<>>();
	}

}}}
#endif
