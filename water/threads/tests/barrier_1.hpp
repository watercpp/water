// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_TESTS_BARRIER_1_HPP
#define WATER_THREADS_TESTS_BARRIER_1_HPP
#include <water/threads/tests/bits.hpp>
#include <water/threads/barrier.hpp>
namespace water { namespace threads { namespace tests {

template<typename barrier_> struct
 barrier_1 {
	barrier_1() {
		barrier_ b(1);
		___water_threads_statistics(name_if(statistics_pointer(b), "water::threads::tests::barrier_1"));
		___water_test(b);
		bool one = b();
		___water_test(one);
		}
	};

}}}
#endif
