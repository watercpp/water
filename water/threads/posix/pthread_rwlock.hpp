// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_POSIX_PTHREAD_RWLOCK_HPP
#define WATER_THREADS_POSIX_PTHREAD_RWLOCK_HPP
#include <water/threads/posix/bits.hpp>
#include <water/threads/posix/deadline_clock.hpp>
#include <water/threads/functions.hpp>
namespace water { namespace threads {


inline void lock(pthread_rwlock_t& a) {
	int e = pthread_rwlock_wrlock(&a);
	___water_assert(!e);
	panic_if(e);
	}

inline bool try_lock(pthread_rwlock_t& a) noexcept {
	return pthread_rwlock_trywrlock(&a) == 0;
	}

inline void unlock(pthread_rwlock_t& a) {
	int e = pthread_rwlock_unlock(&a);
	___water_assert(!e);
	panic_if(e);
	}

inline void read_lock(pthread_rwlock_t& a) {
	int e = pthread_rwlock_rdlock(&a);
	___water_assert(!e);
	panic_if(e);
	}

inline bool read_try_lock(pthread_rwlock_t& a) noexcept {
	return pthread_rwlock_tryrdlock(&a) == 0;
	}

inline void read_unlock(pthread_rwlock_t& a) {
	return unlock(a);
	}
	
#ifdef WATER_POSIX_TIMEOUTS

using pthread_rwlock_timeout = need_timeout;

inline bool lock(pthread_rwlock_t& a, deadline_clock<clockid::realtime> d) noexcept {
	int e = pthread_rwlock_timedwrlock(&a, &d.underlying());
	___water_assert(!e || e == ETIMEDOUT);
	return e == 0;
	}

inline bool read_lock(pthread_rwlock_t& a, deadline_clock<clockid::realtime> d) noexcept {
	int e = pthread_rwlock_timedrdlock(&a, &d.underlying());
	___water_assert(!e || e == ETIMEDOUT);
	return e == 0;
	}
	
#else

using pthread_rwlock_timeout = need_nothing;

#endif

class pthread_rwlock {
	public:
		using needs = threads::needs<need_system, need_constexpr_constructor, pthread_rwlock_timeout>;
	private:
		pthread_rwlock_t my = PTHREAD_RWLOCK_INITIALIZER; // this is standard only from posix issue 7
		___water_threads_statistics(threads::statistics::reference mystatistics;)
	public:
		constexpr pthread_rwlock() noexcept = default;
		pthread_rwlock(pthread_rwlock const&) = delete;
		pthread_rwlock& operator=(pthread_rwlock const&) = delete;
		~pthread_rwlock() noexcept {
			pthread_rwlock_destroy(&my);
			}
		void lock() {
			___water_threads_statistics(if(try_lock()) return;)
			threads::lock(my);
			}
		bool try_lock() noexcept {
			bool r = threads::try_lock(my);
			___water_threads_statistics(threads::statistics::add(mystatistics, this, "pthread_rwlock").wait(r));
			return r;
			}
		void unlock() {
			threads::unlock(my);
			}
		void read_lock() {
			___water_threads_statistics(if(read_try_lock()) return;)
			threads::read_lock(my);
			}
		bool read_try_lock() noexcept {
			bool r = threads::read_try_lock(my);
			___water_threads_statistics(threads::statistics::add(mystatistics, this, "pthread_rwlock").wait(r));
			return r;
			}
		void read_unlock() {
			threads::read_unlock(my);
			}
		#ifdef WATER_POSIX_TIMEOUTS
		bool lock(deadline_clock<clockid::realtime> d) noexcept {
			___water_threads_statistics(if(try_lock()) return true;)
			return threads::lock(my, d);
			}
		bool read_lock(deadline_clock<clockid::realtime> d) noexcept {
			___water_threads_statistics(if(read_try_lock()) return true;)
			return threads::read_lock(my, d);
			}
		#endif
		pthread_rwlock_t& underlying() noexcept {
			return my;
			}
		___water_threads_statistics(threads::statistics::data* statistics() noexcept { return get(mystatistics, this, "pthread_rwlock"); })
	};

}}
#endif
