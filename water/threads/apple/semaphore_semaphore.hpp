// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_APPLE_SEMAPHORE_SEMAPHORE_HPP
#define WATER_THREADS_APPLE_SEMAPHORE_SEMAPHORE_HPP
#include <water/threads/apple/mach.hpp>
#include <water/threads/apple/deadline.hpp>
#include <water/threads/pause.hpp>
#include <water/threads/algorithms/semaphore.hpp>
namespace water { namespace threads {

class semaphore_semaphore {
	using algoritm = algorithms::semaphore<atomic::uint_t>;
	public:
		using needs = threads::needs<need_water, need_timeout, need_constexpr_constructor>;
	private:
		atomic::uint_t my;
		mach_t mysemaphore = 0;
		___water_threads_statistics(threads::statistics::reference mystatistics;)
		___water_threads_statistics(using add_ = threads::statistics::add;)
	public:
		constexpr semaphore_semaphore(unsigned value = 0) noexcept :
			my(value)
			{}
		semaphore_semaphore(semaphore_semaphore const&) = delete;
		semaphore_semaphore& operator=(semaphore_semaphore const&) = delete;
		~semaphore_semaphore() noexcept {
			if(auto s = semaphore_atomic_get(mysemaphore))
				semaphore_destroy(s);
			}
		explicit operator bool() const noexcept {
			return true;
			}
		bool down() noexcept {
			___water_threads_statistics(add_ add(mystatistics, this, "semaphore_semaphore"); add.wait(true));
			pause p;
			algoritm a(my);
			mach_t s = 0;
			while(!a.down()) {
				___water_threads_statistics(add.wait(false));
				if(!s) {
					if(!p.once())
						p = pause_wait();
					s = semaphore_atomic_get(mysemaphore);
					if(!s) {
						s = semaphore_atomic_create(mysemaphore, 0);
						___water_threads_statistics(add.wait_object_created(s));
						}
					if(!s)
						p();
					}
				else {
					int e = semaphore_wait(s);
					if(e && e != mach_aborted)
						p();
					}
				}
			return true;
			}
		bool down(deadline d) noexcept {
			___water_threads_statistics(add_ add(mystatistics, this, "semaphore_semaphore"); add.wait(true).timeout(true));
			algoritm a(my);
			if(a.try_down())
				return true;
			___water_threads_statistics(add.wait(false).timeout(false));
			if(d.passed())
				return false;
			mach_t s = semaphore_atomic_get(mysemaphore);
			if(!s) {
				s = semaphore_atomic_create(mysemaphore, 0);
				___water_threads_statistics(add.wait_object_created(s));
				}
			if(!s)
				return false;
			while(!a.down()) {
				int e = semaphore_wait(s, d.left());
				if(e == mach_timeout)
					return a.down_timeout();
				if(e && e != mach_aborted)
					return false;
				}
			___water_threads_statistics(add.timeout(true));
			return true;
			}
		bool try_down() noexcept {
			bool r = algoritm(my).try_down();
			___water_threads_statistics(add_(mystatistics, this, "semaphore_semaphore").wait(r));
			return r;
			}
		bool up() noexcept {
			___water_threads_statistics(add_ add(mystatistics, this, "semaphore_semaphore"); add.wake(true));
			if(algoritm(my).up())
				if(auto s = semaphore_atomic_get(mysemaphore)) {
					___water_threads_statistics(add.wake(false));
					return semaphore_signal(s);
					}
			return true;
			}
		___water_threads_statistics(threads::statistics::data* statistics() noexcept { return get(mystatistics, this, "semaphore_semaphore"); })
	};

}}
#endif
