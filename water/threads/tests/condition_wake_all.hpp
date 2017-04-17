// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_TESTS_CONDITION_WAKE_ALL_HPP
#define WATER_THREADS_TESTS_CONDITION_WAKE_ALL_HPP
#include <water/threads/tests/bits.hpp>
#include <water/threads/condition.hpp>
namespace water { namespace threads { namespace tests {

/*

test wake all, condition functions

*/

template<typename condition_> class
 condition_wake_all {
	condition_ my[2];
	mutex_for_condition<condition_> mylock[2];
	unsigned myvalue[5] = {};
	unsigned myat[5] = {};
	unsigned mydone = 0;
	public:
		condition_wake_all() {
			___water_threads_statistics(name_if(statistics_pointer(my[0])) << "water::threads::tests::condition_wake_all condition 0");
			___water_threads_statistics(name_if(statistics_pointer(mylock[0])) << "water::threads::tests::condition_wake_all mutex 0");
			___water_threads_statistics(name_if(statistics_pointer(my[1])) << "water::threads::tests::condition_wake_all condition 1");
			___water_threads_statistics(name_if(statistics_pointer(mylock[1])) << "water::threads::tests::condition_wake_all mutex 1");
			run_many_reference(*this, 11);
			___water_test(mydone == 11);
			}
		void operator()() {
			unsigned i = 0;
			do {
				lock(mylock[0]);
				unsigned at = myat[i]++;
				if(myat[i] == 11)
					___water_test(my[0].wake_all());
				if(!at)
					while(myat[i] != 11)
						___water_test(my[0].wait(mylock[0]));
				unlock(mylock[0]);
			
				if(!at) {
					sleep(0.1);
					auto l = lock_move(mylock[1]);
					myvalue[i] = 1;
					___water_test(my[1].wake_all());
					}
				else {
					auto l = lock_move(mylock[1]);
					while(myvalue[i] != 1)
						___water_test(my[1].wait(mylock[1]));
					}
				} while(++i != 5);
			lock(mylock[0]);
			++mydone;
			unlock(mylock[0]);
			}
	};


}}}
#endif
