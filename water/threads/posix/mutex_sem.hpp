// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_POSIX_MUTEX_SEM_HPP
#define WATER_THREADS_POSIX_MUTEX_SEM_HPP
#include <water/threads/posix/sem.hpp>
#include <water/threads/posix/deadline_clock.hpp>
#include <water/threads/functions.hpp>
#include <water/threads/pause.hpp>
#ifdef WATER_THREADS_STATISTICS
	#include <water/threads/statistics/statistics.hpp>
#endif
namespace water { namespace threads {

template<bool exists_ = atomic::any_exists<atomic::uint_t>::result && sem::needs::need> class
 mutex_sem;

template<> class
 mutex_sem<false> {
 	public:
		using needs = threads::needs<>;
 	};

#ifdef WATER_POSIX_SEMAPHORES

template<bool exists_> class
 mutex_sem {
	public:
		#ifdef WATER_POSIX_TIMEOUTS
		using needs = threads::needs<need_water, need_constexpr_constructor, need_timeout>;
		#else
		using needs = threads::needs<need_water, need_constexpr_constructor>;
		#endif
	private:
		atomic::uint_t my = 0;
		atomic::uint_t myinit = 0;
		sem_t mysem;
		___water_threads_statistics(threads::statistics::reference mystatistics;)
		___water_threads_statistics(using add_ = threads::statistics::add;)
	public:
		constexpr mutex_sem() noexcept = default;
		mutex_sem(mutex_sem const&) = delete;
		mutex_sem& operator=(mutex_sem const&) = delete;
		~mutex_sem() noexcept {
			if(atomic::get<atomic::none>(myinit))	
				sem_destroy(&mysem);
			}
		void lock() noexcept {
			___water_threads_statistics(add_ add(mystatistics, this, "mutex_sem"); add.wait(true);)
			if(!atomic::get_add1<atomic::acquire>(my))
				return;
			___water_threads_statistics(add.wait(false));
			pause p = pause_wait();
			bool inited = false;
			do {
				if(!inited)
					inited = init();
				if(!inited) 
					p();
				else
					down(mysem);
				} while(atomic::get_set<atomic::acquire>(my, 2));
			}
		#ifdef WATER_POSIX_TIMEOUTS
		bool lock(deadline_clock<clockid::realtime> d) noexcept {
			___water_threads_statistics(add_ add(mystatistics, this, "mutex_sem"); add.wait(true).timeout(true);)
			if(!atomic::get_add1<atomic::acquire>(my))
				return true;
			___water_threads_statistics(add.wait(false).timeout(false));
			if(!d.left() || !init())
				return false;
			while(atomic::get_set<atomic::acquire>(my, 2)) {
				if(!down(mysem, d))
					return false;
				}
			___water_threads_statistics(add.timeout(true));
			return true;
			}
		#endif
		bool try_lock() noexcept {
			bool r = atomic::compare_set<atomic::acquire>(my, 0, 1);
			___water_threads_statistics(add_(mystatistics, this, "mutex_sem").wait(r));
			return r;
			}
		void unlock() noexcept {
			___water_threads_statistics(add_ add(mystatistics, this, "mutex_sem"); add.wake(true));
			if(atomic::get_set<atomic::release>(my, 0) > 1)
				if(atomic::get<atomic::none>(myinit) == 1) {
					up(mysem);
					___water_threads_statistics(add.wake(false));
					}
			}
		___water_threads_statistics(threads::statistics::data* statistics() noexcept { return get(mystatistics, this, "mutex_sem"); })
	private:
		bool init() noexcept {
			auto c = atomic::get<atomic::none>(myinit);
			if(c == 1)
				return true;
			if(!atomic::compare_set<atomic::acquire>(myinit, 0, 2))
				return false;
			bool r = sem_init(&mysem, 0, 1) == 0;
			atomic::set<atomic::release>(myinit, r ? 1 : 0);
			___water_threads_statistics(add_(mystatistics, this, "mutex_sem").wait_object_created(r));
			return r;
			}
	};

#endif

}}
#endif
