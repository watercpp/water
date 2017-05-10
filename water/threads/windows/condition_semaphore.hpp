// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_WINDOWS_CONDITION_SEMAPHORE_HPP
#define WATER_THREADS_WINDOWS_CONDITION_SEMAPHORE_HPP
#include <water/threads/windows/deadline.hpp>
#include <water/threads/windows/name.hpp>
#include <water/threads/windows/mutex.hpp>
#include <water/threads/windows/hold_semaphore.hpp>
#include <water/threads/functions.hpp>
namespace water { namespace threads {

// condition variable that should work almost like a pthread_cond_t
// uses 1 semaphore and 1 atomic_old::uint_t. the semaphore is created when it is needed.
//
// a pthread_cond_t can be destroyed after pthread_cond_broadcast. this can not.
//
// this can be used with any kind of mutex
//
class condition_semaphore {
 	using uint_ = decltype(atomic_uint{}.load());
	static constexpr uint_
		shift = numeric_limits<uint_>::digits / 2,
		low = (static_cast<uint_>(1) << shift) - 1;
	public:
 		using needs = threads::needs<need_water, need_constexpr_constructor, need_timeout>;
 		using mutex = mutex_event;
 	private:
		atomic_uint my{0}; // low half = wait, hi half = wake
		void *mysemaphore = 0; // protected by lock in wait
	public:
		constexpr condition_semaphore() noexcept = default;
		condition_semaphore(condition_semaphore const&) = delete;
		condition_semaphore& operator=(condition_semaphore const&) = delete;
		~condition_semaphore() {
			my.load(memory_order_acquire); // barrier
			if(mysemaphore)
				CloseHandle(mysemaphore);
			}
		template<typename lock_>
		 bool wait(lock_& l) {
			return wait_do(l, 0);
			}
		template<typename lock_>
		 bool wait(lock_& l, deadline d) {
			return wait_do(l, &d);
			}
		bool wake() noexcept {
			return wake(1) != -1;
			}
		bool wake_all() noexcept {
			return wake(0) != -1;
			}
		int wake(int n) noexcept {
			// wake at most n threads. n <= 0 means wake all
			//
			// returns -1 if error, else the number of woken threads (0 or more)
			//
			// this can see the wrong number of waiting threads
			//
			// if threads time out or fail semaphore-wait, and have not yet --mywait, this still releases them. 2 things can happen:
			// - the timed out or failed thread consumes the wakeup when it --mywait
			// - a new thread, that called wait() after this was called, returns from semaphore_wait at once and consumes the wakeup before the other thread
			//   - the failed will still return from wait()
			//
			// if the external lock is not locked when this is called, or it is a read-lock, this will not see threads that have called wait() but not yet ++mywait
			// 
			if(n <= 0)
				n = numeric_limits<int>::max();
			int r = 0;
			uint_ a = my.load(memory_order_relaxed), a_wait, a_wake;
			do {
				a_wait = a & low;
				a_wake = a >> shift;
				if(a_wait > a_wake) {
					r = static_cast<int>(a_wait - a_wake);
					if(r > n) r = n;
					a_wake += static_cast<uint_>(r);
					}
				else
					r = 0;
				} while(!my.compare_exchange_weak(a, a_wait | (a_wake << shift)));
			if(r && (!mysemaphore || !ReleaseSemaphore(mysemaphore, r, 0)))
				r = -1;
			return r;
			}
	private:
		template<typename lock_>
		 bool wait_do(lock_& l, deadline *d) {
			if(d && d->passed())
				return false;
			if(!mysemaphore)
				mysemaphore = semaphore_create(0);
			if(!mysemaphore)
				return false;
			my.fetch_add(1); // now wake() see this as waiting
			unlock(l); // lock can be unlocked
			bool
				no_error = true,
				again = false;
			do {
				auto e = handle_wait(mysemaphore, d ? d->milli_left() : wait_forever);
				// wake() see the wrong wait-count now
				// if a new thread that calls WaitForSingleObject can steal a wakeup from an already waiting, that could happen here too
				uint_ a = my.load(memory_order_relaxed), a1;
				do {
					a1 = a;
					if(a1 >> shift) { // wake != 0
						a1 -= static_cast<uint_>(1) << shift; // wake
						again = false;
						no_error = true;
						}
					else
						again = no_error = e == 0;
					if(!again)
						--a1; // drop from wait
					} while(!my.compare_exchange_weak(a, a1));
				} while(again);
			lock(l); // could throw or fail
			return no_error;
			}
	};

}}
#endif
