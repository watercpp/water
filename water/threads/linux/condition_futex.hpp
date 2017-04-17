// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_LINUX_CONDITION_FUTEX_HPP
#define WATER_THREADS_LINUX_CONDITION_FUTEX_HPP
#include <water/threads/linux/mutex_futex.hpp>
namespace water { namespace threads {
 	
template<bool exists_ = futex_exists> class
 condition_futex {
	public:
		using needs = threads::needs<need_water, need_constexpr_constructor, need_trivial_destructor, need_timeout>;
		using mutex = mutex_futex<>;
	private:
		futex_t my = 0;
		futex_t* mymutex = 0; // inited locked in wait
	public:
		constexpr condition_futex() noexcept = default;
		condition_futex(condition_futex const&) = delete;
		condition_futex& operator=(condition_futex const&) = delete;
		bool wait(mutex& m) noexcept {
			mymutex = &m.underlying();
			auto value = atomic::get(my);
			m.unlock();
			int e = futex_wait(my, value);
			m.lock();
			return e == 0;
			}
		bool wait(mutex& m, double seconds) noexcept {
			mymutex = &m.underlying();
			auto value = atomic::get(my);
			m.unlock();
			int e = futex_wait(my, value, seconds);
			m.lock();
			return e == 0;
			}
		bool wait(mutex& m, deadline_clock<clockid::realtime> d) noexcept {
			return wait(m, d.left());
			}
		bool wait(mutex& m, deadline_clock<clockid::monotonic_maybe> d) noexcept {
			return wait(m, d.left());
			}
		bool wake(unsigned n = 1) noexcept {
			if(!n || n > futex_max)
				n = futex_max;
			futex_t a = my, a1;
			while(!atomic::compare_set_else_non_atomic_get(my, a, a1 = (a + 1) & futex_max, a));
			// unless mutex is locked by this thread, other threads can read + start wait now
			if(!mymutex || n == 1)
				return futex_wake(my, n) >= 0;
			int e;
			while((e = futex_cmp_requeue(my, a1, 1, *mymutex, n - 1)) < 0) {
				if(e != futex_again)
					return false;
				a1 = my;
				}
			// if mutex is not 2 the requed threads will not be woken up.
			if(e > 1 && atomic::get(*mymutex) != 2)
				return futex_wake_all(*mymutex) >= 0;
			return true;
			}
		bool wake_all() noexcept {
			return wake(0);
			}
	};
template<> class
 condition_futex<false> {
	public:
		using needs = threads::needs<>;
 	};
 
}}
#endif
