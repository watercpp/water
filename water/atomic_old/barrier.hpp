// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#if !defined(WATER_ATOMIC_OLD_BARRIER_HPP) || defined(WATER_ATOMIC_OLD_INLINE)
#ifndef WATER_ATOMIC_OLD_INLINE
#define WATER_ATOMIC_OLD_BARRIER_HPP
#include <water/atomic_old/bits.hpp>
namespace water { namespace atomic_old {
#endif

inline void barrier() {
	// atomic_old::full barrier without an atomic operation
	underlying::barrier();
	}

#ifndef WATER_ATOMIC_OLD_INLINE
}}
#endif
#endif
