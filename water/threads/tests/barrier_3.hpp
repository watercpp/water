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
	barrier_ // this was an array once
		my00,
		my01,
		my02,
		my03,
		my04,
		my05,
		my06,
		my07,
		my08,
		my09,
		my10,
		my11,
		my12,
		my13,
		my14,
		my15;
	mutex<> mylock;
	unsigned mydone = 0;
	public:
		barrier_3(unsigned a = 5, unsigned b = 3) :
			my00{a},
			my01{a},
			my02{a},
			my03{a},
			my04{b},
			my05{b},
			my06{b},
			my07{b},
			my08{a},
			my09{a},
			my10{a},
			my11{a},
			my12{b},
			my13{b},
			my14{b},
			my15{b}
			{
			___water_threads_statistics(name_if(statistics_pointer(mylock)) << "water::threads::tests::barrier_3");
			auto m = &my00, me = &my15 + 1;
			do {
				___water_test(*m);
				___water_threads_statistics(name_if(statistics_pointer(*m)) << "water::threads::tests::barrier_3 " << (m - &my00));
				} while(++m != me);
			run_many_reference(*this, a * b);
			___water_test(mydone == a * 8 + b * 8);
			}
		void operator()() {
			unsigned i = 0;
			do {
				if((&my00)[i]()) {
					auto l = lock_move(mylock);
					++mydone;
					}
				} while(++i != 16);
			}
	};

}}}
#endif
