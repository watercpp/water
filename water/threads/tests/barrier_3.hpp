// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_TESTS_BARRIER_3_HPP
#define WATER_THREADS_TESTS_BARRIER_3_HPP
#include <water/threads/tests/bits.hpp>
#include <water/threads/barrier.hpp>
#include <water/threads/mutex.hpp>
namespace water { namespace threads { namespace tests {

// cannot use the same barrier over and over with to many threads, but can use different barriers

template<typename barrier_> class
 barrier_3 {
	barrier_ my[16];
	mutex<> mylock;
	unsigned mydone = 0;
	public:
		barrier_3(unsigned a = 5, unsigned b = 3) :
			my{
				{a},{a},{a},{a},
				{b},{b},{b},{b},
				{a},{a},{a},{a},
				{b},{b},{b},{b}
				}
			{
			___water_threads_statistics(name_if(statistics_pointer(mylock)) << "water::threads::tests::barrier_3");
			for(auto& m :	my) {
				___water_test(m);
				___water_threads_statistics(name_if(statistics_pointer(m)) << "water::threads::tests::barrier_3 " << (&m - my));
				}
			run_many_reference(*this, a * b);
			___water_test(mydone == a * 8 + b * 8);
			}
		void operator()() {
			unsigned i = 0;
			do {
				if(my[i]()) {
					auto l = lock_move(mylock);
					++mydone;
					}
				} while(++i != 16);
			}
	};

}}}
#endif
