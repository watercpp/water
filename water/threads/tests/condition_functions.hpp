// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_TESTS_CONDITION_FUNCTIONS_HPP
#define WATER_THREADS_TESTS_CONDITION_FUNCTIONS_HPP
#include <water/threads/tests/bits.hpp>
#include <water/threads/condition.hpp>
namespace water { namespace threads { namespace tests {

/*

test condition functions

*/

template<typename condition_> class
 condition_functions {
	condition_ my[2];
	mutex_for_condition<condition_> mylock[2];
	unsigned myvalue[2] = {};
	unsigned myat = 0;
	public:
		condition_functions() {
			___water_threads_statistics(name_if(statistics_pointer(my[0])) << "water::threads::tests::condition_functions condition 0");
			___water_threads_statistics(name_if(statistics_pointer(mylock[0])) << "water::threads::tests::condition_functions mutex 0");
			___water_threads_statistics(name_if(statistics_pointer(my[1])) << "water::threads::tests::condition_functions condition 1");
			___water_threads_statistics(name_if(statistics_pointer(mylock[1])) << "water::threads::tests::condition_functions mutex 1");
			run_many_reference(*this, 2);
			}
		void operator()() {
			unsigned constexpr max_errors = 10;
			unsigned errors = 0;
			lock(mylock[0]);
			unsigned at = myat++;
			unlock(mylock[0]);
			
			lock(mylock[!at]);
			++myvalue[!at];
			___water_test(my[!at].wake());
			unlock(mylock[!at]);
			
			lock(mylock[at]);
			errors = 0;
			while(myvalue[at] < 1)
				errors += !my[at].wait(mylock[at]);
			___water_test(errors < max_errors);
			unlock(mylock[at]);
				
			lock(mylock[!at]);
			++myvalue[!at];
			___water_test(wake(my[!at]));
			unlock(mylock[!at]);
			
			lock(mylock[at]);
			errors = 0;
			while(myvalue[at] < 2)
				errors += !wait(my[at], mylock[at]);
			___water_test(errors < max_errors);
			unlock(mylock[at]);
			
			deadline d(1);
			
			lock(mylock[!at]);
			++myvalue[!at];
			___water_test(my[!at].wake_all());
			unlock(mylock[!at]);
			
			lock(mylock[at]);
			errors = 0;
			while(myvalue[at] < 3)
				errors += !my[at].wait(mylock[at], d);
			___water_test(errors < max_errors);
			unlock(mylock[at]);
			
			lock(mylock[!at]);
			++myvalue[!at];
			___water_test(wake_all(my[!at]));
			unlock(mylock[!at]);
			
			lock(mylock[at]);
			errors = 0;
			while(myvalue[at] < 4)
				errors += !wait(my[at], mylock[at], d);
			___water_test(errors < max_errors);
			unlock(mylock[at]);
			
			lock(mylock[!at]);
			++myvalue[!at];
			___water_test(my[!at].wake_all());
			unlock(mylock[!at]);
			
			lock(mylock[at]);
			errors = 0;
			while(myvalue[at] < 5)
				errors += !my[at].wait(mylock[at], 1.23);
			___water_test(errors < max_errors);
			unlock(mylock[at]);
			
			lock(mylock[!at]);
			++myvalue[!at];
			___water_test(wake_all(my[!at]));
			unlock(mylock[!at]);
			
			lock(mylock[at]);
			errors = 0;
			while(myvalue[at] < 6)
				errors += !wait(my[at], mylock[at], 0.456);
			___water_test(errors < max_errors);
			unlock(mylock[at]);
			
			lock(mylock[at]);
			___water_test(!my[at].wait(mylock[at], 0.01));
			___water_test(!my[at].wait(mylock[at], deadline(0.01)));
			unlock(mylock[at]);
			}
	};


}}}
#endif
