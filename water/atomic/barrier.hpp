// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#if !defined(WATER_ATOMIC_BARRIER_HPP) || defined(WATER_ATOMIC_INLINE)
#ifndef WATER_ATOMIC_INLINE
#define WATER_ATOMIC_BARRIER_HPP
#include <water/atomic/bits.hpp>
namespace water { namespace atomic {
#endif

inline void barrier() {
	// atomic::full barrier without an atomic operation
	underlying::barrier();
	}

#ifndef WATER_ATOMIC_INLINE
}}
#endif
#endif
