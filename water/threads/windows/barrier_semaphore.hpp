// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_WINDOWS_BARRIER_SEMAPHORE_HPP
#define WATER_THREADS_WINDOWS_BARRIER_SEMAPHORE_HPP
#include <water/threads/windows/bits.hpp>
namespace water { namespace threads {

/*

atomic::uint_t
- bit 0 is leave_bit, set if threads should leave
- the other bits are thread count

a thread cannot go inside the barrier if other threads are leaving (leave_bit is set)
those threads will pause while waiting. maybe bad.

when the function call operator return true it is safe to destroy this,
because it returns true for the last thread out, and the --count is the last thing it does.

*/

class barrier_semaphore {
 	using unsigned_ = atomic::uint_t;
 	public:
 		using needs = threads::needs<need_water, need_constexpr_constructor>;
 	private:
		unsigned_ my = 0;
		unsigned_ mycount;
		atomic::alias<void*> mysemaphore {};
		___water_threads_statistics(threads::statistics::reference mystatistics;)
	public:
		constexpr barrier_semaphore(unsigned count) noexcept :
			mycount(count)
			{}
		barrier_semaphore(barrier_semaphore const&) = delete;
		barrier_semaphore& operator=(barrier_semaphore const&) = delete;
		~barrier_semaphore() noexcept {
			if(void *h = atomic::get<atomic::acquire>(mysemaphore))
				CloseHandle(h);
			}
		bool reset(unsigned count) noexcept {
			atomic::set(mycount, count);
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
			unsigned_ now = my;
			bool
				inside = false,
				last_out = false;
			void *semaphore = 0;
			while(true) {
				last_out = false;
				unsigned_ count = now >> 1;
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
				if(!atomic::compare_set_else_non_atomic_get(my, now, (count << 1) | (leave_bit ? 1 : 0), now))
					continue; // retry
				if(hold) { // pause untill they have left
					if(!p.once())
						p = pause_wait();
					p();
					___water_threads_statistics(add.wait(false));
					}
				else if(leave) {
					if(last_in) {
						if(!semaphore)
							semaphore = atomic::get<atomic::none>(mysemaphore);
						if(semaphore && semaphore != handle_bad)
							ReleaseSemaphore(semaphore, static_cast<long_t>(mycount - 1), 0);
						// now --count, this means last_out is truly the last to use the atomic or semaphore
						do {
							count = now >> 1;
							last_out = --count == 0;
							} while(!atomic::compare_set_else_non_atomic_get(my, now, (count << 1) | (count ? 1 : 0), now));
						}
					break;
					}
				else {
					// wait, if semaphore is not inited, wait after next atomic
					inside = true;
					if(!p.once())
						p = pause_wait();
					if(!semaphore) {
						semaphore = atomic::get<atomic::none>(mysemaphore);
						if(!semaphore || semaphore == handle_bad) {
							semaphore = atomic_create(mysemaphore, []{ return semaphore_create(); });
							___water_threads_statistics(add.wait_object_created(semaphore));
							}
						if(!semaphore)
							p();
						}
					else if(handle_wait(semaphore))
						p();
					}
				now = my; // re-read after wait
				}
			return last_out;
			}
		___water_threads_statistics(threads::statistics::data* statistics() noexcept { return get(mystatistics, this, "barrier_semaphore"); })
	};

}}
#endif
