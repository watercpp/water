// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_POSIX_PTHREAD_BARRIER_HPP
#define WATER_THREADS_POSIX_PTHREAD_BARRIER_HPP
#include <water/threads/posix/bits.hpp>
namespace water { namespace threads {

#ifdef WATER_POSIX_BARRIERS

class pthread_barrier {
	public:
		using needs = threads::needs<need_system>;
	private:
		pthread_barrier_t my;
		unsigned mycount = 0;
	public:
		pthread_barrier(unsigned count) noexcept {
			reset(count);
			}
		pthread_barrier(pthread_barrier const&) = delete;
		pthread_barrier& operator=(pthread_barrier const&) = delete;
		~pthread_barrier() noexcept {
			if(mycount) pthread_barrier_destroy(&my);
			}
		bool reset(unsigned count) noexcept {
			if(!count) count = 1;
			if(count == mycount) return true;
			if(mycount) pthread_barrier_destroy(&my);
			int e = pthread_barrier_init(&my, 0, count);
			___water_assert(!e);
			if(!e) mycount = count;
			return e == 0;
			}
		explicit operator bool() const noexcept {
			return mycount != 0;
			}
		bool operator()() {
			int e = pthread_barrier_wait(&my);
			if(e == PTHREAD_BARRIER_SERIAL_THREAD)
				return true;
			___water_assert(!e);
			panic_if(e);
			return false;
			}
		pthread_barrier_t& underlying() noexcept {
			return my;
			}
	};

#else

class pthread_barrier {
	public:
		using needs = threads::needs<>;
	};

#endif

}}
#endif
