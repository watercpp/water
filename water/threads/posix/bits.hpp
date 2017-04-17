// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_POSIX_BITS_HPP
#define WATER_THREADS_POSIX_BITS_HPP
#include <water/threads/bits.hpp>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h> // gettimeofday

//
// posix configuration macros
// they do not seem to work, probably needs some manual defines
//
// single unix version 7 has moved some things into base, like _POSIX_TIMEOUTS
//
// manual configuration:
// - #define WATER_POSIX_SOMETHING to always use
// - #define WATER_POSIX_NO_SOMETHING to never use (for testing)
//
// linux thread priority needs special permissions?
//

#if \
!defined(WATER_POSIX_BARRIERS) && \
!defined(WATER_POSIX_NO_BARRIERS) && \
((defined(_POSIX_BARRIERS) && _POSIX_BARRIERS > 0) || defined(PTHREAD_BARRIER_SERIAL_THREAD))
	#define WATER_POSIX_BARRIERS
#endif

#if \
!defined(WATER_POSIX_PRIORITY_SCHEDULING) && \
!defined(WATER_POSIX_NO_PRIORITY_SCHEDULING) && \
(defined(_POSIX_PRIORITY_SCHEDULING) && _POSIX_PRIORITY_SCHEDULING > 0)
	#define WATER_POSIX_PRIORITY_SCHEDULING
#endif

#if \
!defined(WATER_POSIX_SEMAPHORES) && \
!defined(WATER_POSIX_NO_SEMAPHORES) && \
((defined(_POSIX_SEMAPHORES) && _POSIX_SEMAPHORES > 0) || defined(WATER_SYSTEM_LINUX))
	#define WATER_POSIX_SEMAPHORES
	// apple has them, but they are deprecated
#endif

#if \
!defined(WATER_POSIX_TIMEOUTS) && \
!defined(WATER_POSIX_NO_TIMEOUTS) && \
((defined(_POSIX_TIMEOUTS) && _POSIX_TIMEOUTS > 0) || defined(WATER_SYSTEM_LINUX))
	#define WATER_POSIX_TIMEOUTS
#endif

#if \
!defined(WATER_POSIX_NANOSLEEP) && \
!defined(WATER_POSIX_NO_NANOSLEEP) && \
((defined(_POSIX_TIMERS) && _POSIX_TIMERS > 0) || defined(WATER_SYSTEM_LINUX) || defined(WATER_SYSTEM_APPLE))
	#define WATER_POSIX_NANOSLEEP
#endif

#if \
!defined(WATER_POSIX_CLOCK_NANOSLEEP) && \
!defined(WATER_POSIX_NO_CLOCK_NANOSLEEP) && \
((defined(_POSIX_CLOCK_SELECTION) && _POSIX_CLOCK_SELECTION > 0) || defined(WATER_SYSTEM_LINUX))
	#define WATER_POSIX_CLOCK_NANOSLEEP
#endif
	
#if \
!defined(WATER_POSIX_THREAD_PRIORITY_SCHEDULING) && \
!defined(WATER_POSIX_NO_THREAD_PRIORITY_SCHEDULING) && \
((defined(_POSIX_THREAD_PRIORITY_SCHEDULING) && _POSIX_THREAD_PRIORITY_SCHEDULING > 0) || defined(WATER_SYSTEM_APPLE))
	#define WATER_POSIX_THREAD_PRIORITY_SCHEDULING
#endif

#if \
!defined(WATER_POSIX_THREAD_ATTR_STACKSIZE) && \
!defined(WATER_POSIX_NO_THREAD_ATTR_STACKSIZE) && \
((defined(_POSIX_THREAD_ATTR_STACKSIZE) && _POSIX_THREAD_ATTR_STACKSIZE > 0) || defined(WATER_SYSTEM_LINUX) || defined(WATER_SYSTEM_APPLE))
	#define WATER_POSIX_THREAD_ATTR_STACKSIZE
#endif

#if \
!defined(WATER_POSIX_MUTEX_RECURSIVE) && \
!defined(WATER_POSIX_NO_MUTEX_RECURSIVE) && \
(defined(_XOPEN_VERSION) || defined(PTHREAD_MUTEX_RECURSIVE))
	#define WATER_POSIX_MUTEX_RECURSIVE
#endif
	
#if \
!defined(WATER_POSIX_CLOCK_REALTIME) && \
!defined(WATER_POSIX_NO_CLOCK_REALTIME) && \
defined(CLOCK_REALTIME)
	#define WATER_POSIX_CLOCK_REALTIME
#endif

#if \
!defined(WATER_POSIX_CLOCK_MONOTONIC) && \
!defined(WATER_POSIX_NO_CLOCK_MONOTONIC) && \
defined(CLOCK_MONOTONIC)
	#define WATER_POSIX_CLOCK_MONOTONIC
#endif

#ifdef WATER_THREADS_STATISTICS
	#include <water/threads/statistics/statistics.hpp>
#endif
namespace water { namespace threads {

inline void timespec_add(timespec& t, double seconds) {
	constexpr long nanosecond = 1000000000l;
	long
		s = static_cast<long>(seconds),
		n = static_cast<long>((seconds - s) * 1e9);
	if(s < 0) s = 0;
	if(n < 0) n = 0;
	else if(n >= nanosecond) n = nanosecond - 1;
	t.tv_nsec += n;
	if(t.tv_nsec >= nanosecond) {
		++s;
		t.tv_nsec -= nanosecond;
		}
	t.tv_sec += s;
	}

}}

#endif
