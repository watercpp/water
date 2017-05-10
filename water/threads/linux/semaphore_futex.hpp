// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_LINUX_SEMAPHORE_FUTEX_HPP
#define WATER_THREADS_LINUX_SEMAPHORE_FUTEX_HPP
#include <water/threads/linux/futex.hpp>
#include <water/threads/pause.hpp>
namespace water { namespace threads {

/*

low half = value
hi half = waiting thread count, to avoid futex_wake in up when noone waits

*/

template<bool exists_ = futex_exists> class
 semaphore_futex {
	static constexpr unsigned
		shift = numeric_limits<int>::digits / 2,
		max = (1u << shift) - 1;
	public:
		using needs = threads::needs<need_water, need_constexpr_constructor, need_trivial_destructor, need_timeout>;
	private:
		futex_atomic my;
		___water_threads_statistics(threads::statistics::reference mystatistics;)
		___water_threads_statistics(using add_ = threads::statistics::add;)
	public:
		constexpr semaphore_futex(unsigned value = 0) noexcept :
			my(value)
			{}
		semaphore_futex(semaphore_futex const&) = delete;
		semaphore_futex& operator=(semaphore_futex const&) = delete;
		explicit operator bool() const noexcept {
			return true;
			}
		bool down() noexcept {
			___water_threads_statistics(add_ add(mystatistics, this, "semaphore_futex"); add.wait(true));
			unsigned now;
			if(down_do(now, 1))
				return true;
			___water_threads_statistics(add.wait(false));
			auto pause = pause_wait();
			do {
				int e = futex_wait(my, now);
				if(e && e != futex_again && e != futex_signal)
					pause();
				} while(!down_do(now, -1));
			return true;
			}
		bool down(deadline_clock<clockid::monotonic_maybe> d) noexcept {
			___water_threads_statistics(add_ add(mystatistics, this, "semaphore_futex"); add.wait(true).timeout(true));
			unsigned now;
			if(down_do(now, 1))
				return true;
			___water_threads_statistics(add.wait(false));
			double left;
			while((left = d.left()) >= 1e-9) {
				int e = futex_wait(my, 0, left);
				if(down_do(now, -1))
					return true;
				if(e && e != futex_again && e != futex_signal)
					break;
				}
			bool r = down_do(now, -2);
			___water_threads_statistics(add.timeout(r));
			return r;
			}
		bool try_down() noexcept {
			unsigned now;
			bool r = down_do(now, 0);
			___water_threads_statistics(add_(mystatistics, this, "semaphore_futex").wait(r));
			return r;
			}
		bool up(unsigned a = 1) noexcept {
			___water_threads_statistics(add_ add(mystatistics, this, "semaphore_futex"); add.wake(true));
			if(a && (my.fetch_add(a) >> shift)) {
				futex_wake(my, a);
				___water_threads_statistics(add.wake(false));
				}
			return true;
			}
		___water_threads_statistics(threads::statistics::data* statistics() noexcept { return get(mystatistics, this, "semaphore_futex"); })
	private:
		bool down_do(unsigned& now, int wait_change) noexcept {
			auto a = my.load(memory_order_relaxed);
			bool r;
			do {
				r = false;
				unsigned
					value = a & max,
					wait = a >> shift;
				if(value) {
					r = true;
					--value;
					if(wait_change < 0)
						--wait;
					}
				else if(wait_change > 0)
					++wait;
				else if(wait_change == -2)
					--wait;
				now = value | (wait << shift);
				} while(!my.compare_exchange_weak(a, now));
			return r;
			}
	};
template<> class
 semaphore_futex<false> {
	public:
		using needs = threads::needs<>;
 	};
 
}}
#endif
