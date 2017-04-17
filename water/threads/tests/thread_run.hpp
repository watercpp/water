// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_TESTS_THREAD_RUN_HPP
#define WATER_THREADS_TESTS_THREAD_RUN_HPP
#include <water/threads/tests/bits.hpp>
#include <water/threads/thread.hpp>
#include <water/threads/condition.hpp>
namespace water { namespace threads { namespace tests {

/*

test all variants of run()

also tests
- equal
- thread
- join
- dont_join

*/

template<typename class_> void
 thread_run_function(class_ *pointer) {
 	___water_test(pointer);
 	if(pointer) (*pointer)();
 	}

class thread_run {
	unsigned mycount = 0;
	condition<> mycondition;
	mutex_for_condition<condition<> > mylock;
	public:
		thread_run() {
			unsigned count = 0;
			bool run_ok;
			thread_t t;
			join_t j;
			
			using function_ = threads::function<thread_run, &thread_run_function<thread_run> >;
			using member_function_ = member_function<thread_run, &thread_run::function>;
			
			___water_test(equal(thread(), thread()));
			
			// no thread
			___water_test(run_ok = run(*this));
			if(run_ok) ++count;
			___water_test(run_ok = run<function_>(this));
			if(run_ok) ++count;
			___water_test(run_ok = run<member_function_>(this));
			if(run_ok) ++count;
			
			// thread
			___water_test(run_ok = run(*this, t));
			if(run_ok) ++count;
			___water_test(run_ok = run<function_>(this, t));
			if(run_ok) ++count;
			___water_test(run_ok = run<member_function_>(this, t));
			if(run_ok) ++count;
			
			// join
			___water_test(run_ok = run(*this, j));
			if(run_ok) {
				___water_test(equal(thread(j), thread(j)));
				++count;
				___water_test(join(j));
				}
			___water_test(run_ok = run<function_>(this, j));
			if(run_ok) {
				++count;
				___water_test(join(j));
				}
			___water_test(run_ok = run<member_function_>(this, j));
			if(run_ok) {
				++count;
				___water_test(dont_join(j));
				}
			
			// no thread + options
			___water_test(run_ok = run(*this, run_options()));
			if(run_ok) ++count;
			___water_test(run_ok = run<function_>(this, run_options()));
			if(run_ok) ++count;
			___water_test(run_ok = run<member_function_>(this, run_options()));
			if(run_ok) ++count;
			
			// thread + options
			___water_test(run_ok = run(*this, t, run_options()));
			if(run_ok) ++count;
			___water_test(run_ok = run<function_>(this, t, run_options()));
			if(run_ok) ++count;
			___water_test(run_ok = run<member_function_>(this, t, run_options()));
			if(run_ok) ++count;
			
			// join + options
			___water_test(run_ok = run(*this, j, run_options()));
			if(run_ok) {
				++count;
				___water_test(join(j));
				}
			___water_test(run_ok = run<function_>(this, j, run_options()));
			if(run_ok) {
				++count;
				___water_test(join(j));
				}
			___water_test(run_ok = run<member_function_>(this, j, run_options()));
			if(run_ok) {
				++count;
				___water_test(dont_join(j));
				}
			
			deadline d(10);
			auto l = lock_move(mylock);
			do {
				mycondition.wait(mylock, d);
				if(mycount == count)
					break;
				} while(d.left());
			___water_test(d.left());
			}
		void operator()() {
			auto l = lock_move(mylock);
			++mycount;
			mycondition.wake();
			}
		void function() {
			___water_test(this);
			(*this)();
			}
	};

}}}
#endif
