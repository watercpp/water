// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_WINDOWS_SYNCHRONIZATION_BARRIER_HPP
#define WATER_THREADS_WINDOWS_SYNCHRONIZATION_BARRIER_HPP
#include <water/threads/windows/bits.hpp>
namespace water { namespace threads {

#if WATER_WINDOWS_VERSION >= WATER_WINDOWS_8

class synchronization_barrier {
	public:
		using needs = threads::needs<need_system>;
	private:
		synchronization_barrier_t my;
		unsigned mycount = 0;
	public:
		synchronization_barrier(unsigned count) noexcept {
			if(!count) count = 1;
			if(InitializeSynchronizationBarrier(&my, count, 0))
				mycount = count;
			___water_assert(mycount);
			}
		~synchronization_barrier() noexcept {
			if(mycount) DeleteSynchronizationBarrier(&my);
			}
		synchronization_barrier(synchronization_barrier const&) = delete;
		synchronization_barrier& operator=(synchronization_barrier const&) = delete;
		bool reset(unsigned count) noexcept {
			if(!count) count = 1;
			if(count == mycount) return true;
			if(mycount) DeleteSynchronizationBarrier(&my);
			if(InitializeSynchronizationBarrier(&my, count, 0)) {
				mycount = count;
				return true;
				}
			mycount = 0;
			___water_assert(mycount);
			return false;
			}
		explicit operator bool() const noexcept {
			return mycount != 0;
			}
		bool operator()() noexcept {
			return EnterSynchronizationBarrier(&my, 0) != 0;
			}
		synchronization_barrier_t& underlying() noexcept {
			return my;
			}
	};

#else

class synchronization_barrier {
	public:
		using needs = threads::needs<>;
	};

#endif

}}
#endif
