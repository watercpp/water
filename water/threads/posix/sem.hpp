// Copyright 2017-2018 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_POSIX_SEM_HPP
#define WATER_THREADS_POSIX_SEM_HPP
#include <water/threads/posix/bits.hpp>
#include <water/threads/posix/deadline_clock.hpp>
#ifdef WATER_POSIX_SEMAPHORES
	#include <semaphore.h>
#endif
namespace water { namespace threads {

#ifdef WATER_POSIX_SEMAPHORES

inline bool up(sem_t& a) noexcept {
	int e = sem_post(&a);
	___water_debug(
		if(e) {
			e = errno;
			___water_assert(!e);
		}
		)
	return e == 0;
	}

inline bool down(sem_t& a) noexcept {
	while(sem_wait(&a)) {
		int e = errno;
		___water_assert(e == EINTR);
		if(e != EINTR) // keep waiting if signal
			return false;
		}
	return true;
	}

inline bool try_down(sem_t& a) noexcept {
	return sem_trywait(&a) == 0;
	}

#ifdef WATER_POSIX_TIMEOUTS

inline bool down(sem_t& a, deadline_clock<clockid::realtime> d) noexcept {
	while(sem_timedwait(&a, &d.underlying())) {
		int e = errno;
		___water_assert(e == EINTR || e == ETIMEDOUT);
		if(e != EINTR)
			return false;
		}
	return true;
	}

#endif

class sem {
	public:
		#ifdef WATER_POSIX_TIMEOUTS
		using needs = threads::needs<need_system, need_timeout>;
		#else
		using needs = threads::needs<need_system>;
		#endif
	private:
		sem_t my;
		bool myok = false;
		___water_threads_statistics(threads::statistics::reference mystatistics;)
	public:
		sem(unsigned value = 0) noexcept {
			myok = sem_init(&my, 0, value) == 0;
			___water_debug(
				if(!myok) {
					int e = errno;
					___water_assert(myok && !e);
					}
				)
			}
		sem(sem const&) = delete;
		sem& operator=(sem const&) = delete;
		~sem() noexcept {
			if(myok) sem_destroy(&my);
			}
		explicit operator bool() const noexcept {
			return myok;
			}
		bool up() noexcept {
			return threads::up(my);
			}
		bool down() noexcept {
			___water_threads_statistics(if(try_down()) return true;)
			return threads::down(my);
			}
		bool try_down() noexcept {
			bool r = threads::try_down(my);
			___water_threads_statistics(threads::statistics::add(mystatistics, this, "sem").wait(r));
			return r;
			}
		#ifdef WATER_POSIX_TIMEOUTS
		bool down(deadline_clock<clockid::realtime> d) noexcept {
			___water_threads_statistics(if(try_down()) return true;)
			return threads::down(my, d);
			}
		#endif
		___water_threads_statistics(threads::statistics::data* statistics() noexcept { return get(mystatistics, this, "sem"); })
	};

#else

class sem {
	public:
		using needs = threads::needs<>;
	};

#endif

}}
#endif
