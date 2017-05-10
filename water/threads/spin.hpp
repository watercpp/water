// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_SPIN_HPP
#define WATER_THREADS_SPIN_HPP
#include <water/threads/yield.hpp>
#include <water/threads/bits.hpp>
#ifdef WATER_THREADS_STATISTICS
	#include <water/threads/statistics/statistics.hpp>
#endif
namespace water { namespace threads {

bool constexpr spin_exists = atomic_exists;

class spin {
	unsigned my = 0;
	unsigned myspin;
	public:
		constexpr spin(unsigned times = 0) noexcept :
			myspin{times}
			{}
		void operator()() noexcept {
			if(my < myspin)
				++my;
			else {
				my = myspin + 1; // so it will not be 0
				yield();
				}
			}
		bool once() const noexcept {
			return my != 0;
			}
		bool spinning() const noexcept {
			// return true if this was spinning the last time 
			return my < myspin;
			}
		spin& times(unsigned a) noexcept {
			myspin = a;
			return *this;
			}
	};

inline unsigned spin_times() noexcept {
	#ifdef WATER_THREADS_SPIN_TIMES
	return WATER_THREADS_SPIN_TIMES;
	#else
	return 1000;
	#endif
	}

}}
#endif
