// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_APPLE_MACH_HPP
#define WATER_THREADS_APPLE_MACH_HPP
#include <water/threads/apple/bits.hpp>
#include <mach/mach.h>
#include <mach/task.h>
#include <mach/mach_time.h>
#include <mach/sync_policy.h>
#include <mach/semaphore.h>
#include <water/threads/yield.hpp>
namespace water { namespace threads {

// mach/port.h mach_port_t is unsigned integer. this is an unsigned
// integer at least as large as mach_port_t that can be used with
// water::atomic
//
// 0 and MACH_PORT_DEAD (0xfff...) are not valid mach ports, because
// MACH_PORT_NULL, MACH_PORT_DEAD and MACH_PORT_VALID macros
//
using mach_t = types::if_not_void<atomic::uint_bits_at_least<numeric_limits<mach_port_t>::digits>, mach_port_t>::result;
using mach_time_t = uint64_t;

mach_t constexpr mach_dead = static_cast<mach_t>(MACH_PORT_DEAD); // 0xfff...

int constexpr
	mach_timeout = KERN_OPERATION_TIMED_OUT,
	mach_aborted = KERN_ABORTED,
	mach_terminated = KERN_TERMINATED;

inline mach_t mach_task() noexcept {
	auto r = mach_task_self();
	___water_assert(r);
	return r;
	}

inline mach_t mach_thread(::pthread_t a) noexcept {
	auto r = pthread_mach_thread_np(a);
	___water_assert(r);
	return r;
	}
	
inline mach_t mach_thread() noexcept {
	auto r = mach_thread(pthread_self());
	___water_assert(r);
	return r;
	// do not use mach_thread_self() because it leaks unless mach_port_deallocate()
	}

inline mach_time_t mach_time() noexcept {
	return mach_absolute_time();
	}

namespace _ {

	template<typename type_> struct
	 mach_global {
	 	static type_ global;
	 	};
	 	template<typename type_> type_ mach_global<type_>::global;
	
	struct mach_time_set_resolution {
		atomic::int_t set;
		double resolution;
		};

	}

inline double mach_time_resolution() noexcept {
	// returns
	// - 1 mach clock in seconds
	// - 0 if it failed (it probably cannot fail)
	//
	// in osfmk/kern/clock.c it looks like mach_timebase_info() cannot fail
	//
	// mach_timebase_info_data_t { uint32_t numer, denom; }
	//   nano = clock * numer / denom
	//   resolution = 1e-9 * numer / denom
	//   per second = 1e9 * denom / numer
	//   on intel, numer and denom is always 1?
	//
	auto* a = &_::mach_global<_::mach_time_set_resolution>::global;
	if(a->set || atomic::get<atomic::acquire>(a->set))
		return a->resolution;
	mach_timebase_info_data_t i{};
	if(!mach_timebase_info(&i) && i.numer && i.denom) {
		double r = (i.numer * 1e-9) / i.denom;
		if(r > 0) {
			// should not hurt if another thread writes the same data at the same time
			a->resolution = r;
			atomic::set(a->set, 1);
			___water_debug(trace()
				<< "water::threads::mach_time_resolution()\n"
				<< "  numer .... " << i.numer << '\n'
				<< "  denom .... " << i.denom << '\n'
				<< "  resolution " << a->resolution << '\n'
				<< "  per second " << ((i.denom * 1e9) / i.numer) << '\n';
				)
			return r;
			}
		}
	___water_assert(i.numer && i.denom && "mach_timebase_info() failed!");
	return 0;
	}

inline mach_time_t mach_time_from_seconds(double s) noexcept {
	// returns 0 if it fails or s <= 0.
	if(s > 0.)
		if(double r = mach_time_resolution())
			return static_cast<mach_time_t>(s / r);
	return 0;
	}

inline double mach_seconds_from_time(mach_time_t c) noexcept {
	// returns 0 if it fails, or c == 0
	return c * mach_time_resolution();
	}
	
inline mach_timespec_t mach_timespec_from_seconds(double seconds) noexcept {
	// struct mach_timespec_t {
	//   unsigned tv_sec;
	//   clock_res_t tv_nsec; // int
	//   };
	mach_timespec_t t{};
	if(seconds >= 1e-9) {
		t.tv_sec = static_cast<unsigned>(seconds);
		seconds -= t.tv_sec;
		t.tv_nsec = static_cast<int>(seconds * 1e9);
		if(t.tv_nsec < 0) t.tv_nsec = 0;
		if(t.tv_nsec > 999999999) t.tv_nsec = 999999999;
		}
	return t;
	}

unsigned constexpr semaphore_max = numeric_limits<int>::max();

inline mach_t semaphore_create(unsigned value = 0) noexcept {
	// returns 0 if failed
	//
	// pre
	// - value <= semaphore_max
	//
	// http://www.opensource.apple.com/source/xnu/xnu-2782.1.97/osfmk/kern/wait_queue.c
	// wait_queue_init (called by semaphore_create) will only work with lifo + fifo, used to work with priority on powerpc
	___water_assert(value <= semaphore_max);
	mach_port_t r;
	if(::semaphore_create(static_cast<mach_port_t>(mach_task()), &r, 0/*order 0 is fifo*/, static_cast<int>(value)))
		r = 0;
	___water_assert(r);
	return r;
	}

inline void semaphore_destroy(mach_t semaphore) noexcept {
	// if threads wait for it, semaphore_wait returns mach_terminated
	___water_debug(auto e =) ::semaphore_destroy(static_cast<mach_port_t>(mach_task()), static_cast<mach_port_t>(semaphore));
	___water_assert(!e);
	}

inline bool semaphore_signal(mach_t semaphore) noexcept {
	// add 1 to semaphore
	auto error = ::semaphore_signal(static_cast<mach_port_t>(semaphore));
	___water_assert(!error);
	return error == 0;
	}
	
inline int semaphore_wait(mach_t semaphore) noexcept {
	// returns
	// - 0 if success
	// - mach_aborted if interrupted by a signal
	// - mach_terminated if semaphore was destroyed
	auto error = ::semaphore_wait(static_cast<mach_port_t>(semaphore));
	___water_assert(!error || error == mach_aborted);
	return error;
	}
	
inline int semaphore_wait(mach_t semaphore, double seconds) noexcept {
	// returns
	// - what the other semaphore_wait does, plus...
	// - mach_timeout
	//
	// if seconds == 0 it will just try
	auto error = ::semaphore_timedwait(static_cast<mach_port_t>(semaphore), mach_timespec_from_seconds(seconds));
	___water_assert(!error || error == mach_aborted || error == mach_timeout);
	return error;
	}
	
inline int semaphore_wait_signal(mach_t wait, mach_t signal) noexcept {
	// wait for wait-semaphore and then signal signal-semaphore
	// returns what semaphore_wait does
	auto error = ::semaphore_wait_signal(static_cast<mach_port_t>(wait), static_cast<mach_port_t>(signal));
	___water_assert(!error || error == mach_aborted);
	return error;
	}

	
inline int semaphore_wait_signal(mach_t wait, mach_t signal, double seconds) noexcept {
	// wait for wait-semaphore and then signal signal-semaphore
	// returns what semaphore_wait does
	auto error = ::semaphore_timedwait_signal(static_cast<mach_port_t>(wait), static_cast<mach_port_t>(signal), mach_timespec_from_seconds(seconds));
	___water_assert(!error || error == mach_aborted || error == mach_timeout);
	return error;
	}

inline bool semaphore_wake(mach_t semaphore, mach_t thread = 0) noexcept {
	// wake a thread waiting for the semaphore
	// - if thread != 0, it wakes that thread only (use mach_thread)
	// - if thread == 0, it wakes any waiting thread
	//
	// returns true if it did wake someone 
	auto error = ::semaphore_signal_thread(static_cast<mach_port_t>(semaphore), static_cast<mach_port_t>(thread));
	___water_assert(!error || error == KERN_NOT_WAITING);
	return error == 0;
	}
	
inline bool semaphore_wake_all(mach_t semaphore) noexcept {
 	// wake all threads waiting for the semaphore
	//
	// returns true if there was no error (even if no one was waiting) 
	auto error = ::semaphore_signal_all(static_cast<mach_port_t>(semaphore));
	___water_assert(!error);
	return error == 0;
	}
	
inline bool semaphore_lock(mach_t semaphore) noexcept {
	int error;
	do error = semaphore_wait(semaphore); while(error == mach_aborted);
	___water_assert(!error);
	return error == 0;
	}

inline mach_t semaphore_atomic_create(mach_t& atomic, unsigned value) noexcept {
	mach_t a;
	while((a = atomic::get_compare_set<atomic::acquire>(atomic, 0, mach_dead)) == mach_dead)
		yield();
	if(!a) {
		a = semaphore_create(value);
		___water_assert(a != mach_dead);
		if(a == mach_dead) a = 0; // leak it
		atomic::set(atomic, a);
		}
	return a;
	}

inline mach_t semaphore_atomic_create(mach_t& atomic, unsigned value, mach_t& two, unsigned two_value) noexcept {
	mach_t a;
	while((a = atomic::get_compare_set<atomic::acquire>(atomic, 0, mach_dead)) == mach_dead)
		yield();
	if(!a) {
		mach_t a2 = semaphore_create(two_value);
		___water_assert(a2);
		if(a2) {
			a = semaphore_create(value);
			___water_assert(a != mach_dead);
			if(a == mach_dead) a = 0; // leak it
			if(!a) {
				semaphore_destroy(a2);
				a2 = 0;
				}
			}
		two = a2;
		atomic::set(atomic, a);
		}
	return a;
	}

inline mach_t semaphore_atomic_get(mach_t& atomic) noexcept {
	// useful together with semaphore_atomic_create, it returns 0 if mach_dead (locked)
	mach_t a = atomic::get<atomic::none>(atomic);
	if(a == mach_dead) a = 0;
	return a;
	}

}}
#endif
