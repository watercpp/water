// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_LINUX_BARRIER_FUTEX_HPP
#define WATER_THREADS_LINUX_BARRIER_FUTEX_HPP
#include <water/threads/linux/futex.hpp>
#include <water/threads/pause.hpp>
namespace water { namespace threads {

/*

maybe add 1 extra bit for hold threads
then last_out can wake_all if hold bit was set
and hold threads can wait on the same futex

*/

template<bool exists_ = futex_exists> class
 barrier_futex {
	public:
		using needs = threads::needs<need_water, need_constexpr_constructor, need_trivial_destructor>;
	private:
		futex_atomic my{0};
		unsigned mycount;
		___water_threads_statistics(threads::statistics::reference mystatistics;)
	public:
		constexpr barrier_futex(unsigned count) noexcept :
			mycount(count)
			{}
		barrier_futex(barrier_futex const&) = delete;
		barrier_futex& operator=(barrier_futex const&) = delete;
		explicit operator bool() const noexcept {
			return true;
			}
		bool reset(unsigned count) noexcept {
			mycount = count;
			return true;
			}
		bool operator()() noexcept {
			___water_threads_statistics(threads::statistics::add add(mystatistics, this, "barrier_futex"); add.wait(true));
			if(mycount <= 1)
				return true;
			pause p;
			auto now = my.load(memory_order_relaxed);
			bool
				inside = false,
				last_out = false;
			while(true) {
				last_out = false;
				auto count = now >> 1;
				bool
					last_in = false,
					leave_bit = (now & 1) != 0,
					hold = leave_bit && !inside, // can enter only if all previous threads have left, leave_set == false
					leave = false;
				if(!hold) {
					if(!inside && ++count == mycount)
						last_in = leave = leave_bit = true; // --count after using semaphore
					else if(inside && leave_bit) {
						leave = true;
						if(!--count) {
							last_out = true;
							leave_bit = false;
							}
						}
					}
				if(!my.compare_exchange_weak(now, (count << 1) | (leave_bit ? 1 : 0)))
					continue; // retry
				if(hold) {
					if(!p.once())
						p = pause_wait();
					p();
					___water_threads_statistics(add.wait(false));
					}
				else if(leave) {
					if(last_in) {
						futex_wake_all(my);
						// now --count, this means last_out is truly the last to use the atomic/futex
						do {
							count = now >> 1;
							last_out = --count == 0;
							} while(!my.compare_exchange_weak(now, (count << 1) | (count ? 1 : 0)));
						}
					break;
					}
				else {
					// wait
					inside = true;
					if(!p.once())
						p = pause_wait();
					int e = futex_wait(my, now);
					if(e && e != futex_again && e != futex_signal)
						p();
					}
				now = my; // re-read after wait
				}
			return last_out;
			}
		___water_threads_statistics(threads::statistics::data* statistics() noexcept { return get(mystatistics, this, "barrier_futex"); })
	};
template<> class
 barrier_futex<false> {
	public:
		using needs = threads::needs<>;
 	};
 
}}
#endif
