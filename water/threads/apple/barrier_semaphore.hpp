// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_APPLE_BARRIER_SEMAPHORE_HPP
#define WATER_THREADS_APPLE_BARRIER_SEMAPHORE_HPP
#include <water/threads/apple/mach.hpp>
#include <water/threads/pause.hpp>
namespace water { namespace threads {

class barrier_semaphore {
	public:
		using needs = threads::needs<need_water, need_timeout, need_constexpr_constructor>;
	private:
		atomic_uint my{0};
		unsigned mycount;
		mach_atomic mylock{0};
		mach_t mywait = 0;
		___water_threads_statistics(threads::statistics::reference mystatistics;)
	public:
		constexpr barrier_semaphore(unsigned count) noexcept :
			mycount(count)
			{}
		barrier_semaphore(barrier_semaphore const&) = delete;
		barrier_semaphore& operator=(barrier_semaphore const&) = delete;
		~barrier_semaphore() noexcept {
			if(auto l = mylock.load()) {
				semaphore_destroy(l);
				semaphore_destroy(mywait);
				}
			}
		bool reset(unsigned count) noexcept {
			mycount = count;
			return true;
			}
		explicit operator bool() const noexcept {
			return true;
			}
		bool operator()() noexcept {
			___water_threads_statistics(threads::statistics::add add(mystatistics, this, "barrier_semaphore"); add.wait(true));
			if(mycount <= 1)
				return true;
			pause p;
			auto now = my.load(memory_order_relaxed);
			mach_t l = semaphore_atomic_get(mylock);
			bool
				inside = false,
				last_out = false,
				locked = false;
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
						if(!locked && !--count) { // --count only after unlock
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
						if(!l)
							l = semaphore_atomic_get(mylock);
						if(l) {
							locked = semaphore_lock(l);
							semaphore_wake_all(mywait);
							}
						}
					if(locked)
						semaphore_signal(l);
					if(last_in || locked) {
						// now --count, this means last_out is truly the last to use the atomic/semaphores
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
					if(!l) {
						l = semaphore_atomic_create(mylock, 1, mywait, 0);
						___water_threads_statistics(add.wait_object_created(l));
						}
					if(!l)
						p();
					else if(!locked) {
						locked = semaphore_lock(l);
						if(!locked)
							p();
						}
					else {
						locked = false;
						if(semaphore_wait_signal(mywait, l))
							p();
						}
					}
				now = my; // re-read after wait
				}
			return last_out;
			}
		___water_threads_statistics(threads::statistics::data* statistics() noexcept { return get(mystatistics, this, "barrier_semaphore"); })
	};

}}
#endif
