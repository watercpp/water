// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_APPLE_MUTEX_SEMAPHORE_HPP
#define WATER_THREADS_APPLE_MUTEX_SEMAPHORE_HPP
#include <water/threads/pause.hpp>
#include <water/threads/apple/mach.hpp>
#include <water/threads/apple/deadline.hpp>
namespace water { namespace threads {

class mutex_semaphore {
	public:
		using needs = threads::needs<need_water, need_timeout, need_constexpr_constructor>;
	private:
		atomic::uint_t my = 0;
		mach_t mysemaphore = 0;
		___water_threads_statistics(threads::statistics::reference mystatistics;)
		___water_threads_statistics(using add_ = threads::statistics::add;)
	public:
		constexpr mutex_semaphore() noexcept = default;
		mutex_semaphore(mutex_semaphore const&) = delete;
		mutex_semaphore& operator=(mutex_semaphore const&) = delete;
		~mutex_semaphore() noexcept {
			if(auto s = semaphore_atomic_get(mysemaphore))
				semaphore_destroy(s);
			}
		void lock() noexcept {
			___water_threads_statistics(add_ add(mystatistics, this, "mutex_semaphore"); add.wait(true);)
			if(!atomic::get_add1<atomic::acquire>(my))
				return;
			___water_threads_statistics(add.wait(false));
			pause p = pause_wait();
			auto s = semaphore_atomic_get(mysemaphore);
			do {
				if(!s) {
					s = semaphore_atomic_create(mysemaphore, 1);
					___water_threads_statistics(add.wait_object_created(s));
					}
				if(!s)
					p();
				else
					semaphore_wait(s);
				} while(atomic::get_set<atomic::acquire>(my, 2));
			}
		bool lock(deadline d) noexcept {
			___water_threads_statistics(add_ add(mystatistics, this, "mutex_semaphore"); add.wait(true).timeout(true));
			if(!atomic::get_add1<atomic::acquire>(my))
				return true;
			___water_threads_statistics(add.wait(false).timeout(false));
			if(d.passed())
				return false;
			auto s = semaphore_atomic_get(mysemaphore);
			if(!s) {
				s = semaphore_atomic_create(mysemaphore, 1);
				___water_threads_statistics(add.wait_object_created(s));
				if(!s) return false;
				}
			int e;
			atomic::uint_t a;
			do {
				double left = d.left();
				e = left >= 1e-9 ? semaphore_wait(s, left) : mach_timeout;
				} while((a = atomic::get_set<atomic::acquire>(my, 2)) && (!e || e == mach_aborted));
			___water_threads_statistics(add.timeout(a == 0));
			return a == 0;
			}
		bool try_lock() noexcept {
			bool r = atomic::compare_set<atomic::acquire>(my, 0, 1);
			___water_threads_statistics(add_(mystatistics, this, "mutex_semaphore").wait(r));
			return r;
			}
		void unlock() noexcept {
			___water_threads_statistics(add_ add(mystatistics, this, "mutex_semaphore"); add.wake(true));
			if(atomic::get_set<atomic::release>(my, 0) > 1)
				if(auto s = semaphore_atomic_get(mysemaphore)) {
					semaphore_signal(s);
					___water_threads_statistics(add.wake(false));
					}
			}
		___water_threads_statistics(threads::statistics::data* statistics() noexcept { return get(mystatistics, this, "mutex_semaphore"); })
	};

}}
#endif
