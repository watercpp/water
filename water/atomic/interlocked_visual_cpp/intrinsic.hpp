// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_ATOMIC_INTERLOCKED_VISUAL_CPP_INTRINSIC_HPP
#define WATER_ATOMIC_INTERLOCKED_VISUAL_CPP_INTRINSIC_HPP
#ifndef WATER_ATOMIC_INTERLOCKED_VISUAL_CPP_TESTS_FAKE_HPP
#include <intrin.h>

// use #pragma intrinsic(_InterlockedFunction) or link error
// unless intrinsic-option is set in compiler
	
#pragma intrinsic(_ReadWriteBarrier)

#pragma intrinsic(_InterlockedDecrement16)
#pragma intrinsic(_InterlockedCompareExchange16)
#pragma intrinsic(_InterlockedIncrement16)

#pragma intrinsic(_InterlockedDecrement)
#pragma intrinsic(_InterlockedCompareExchange)
#pragma intrinsic(_InterlockedExchange)
#pragma intrinsic(_InterlockedExchangeAdd)
#pragma intrinsic(_InterlockedIncrement)

#pragma intrinsic(_InterlockedCompareExchange64)
#if defined(_M_X64) || defined(_M_AMD64)
	#pragma intrinsic(_InterlockedDecrement64)
	#pragma intrinsic(_InterlockedExchange64)
	#pragma intrinsic(_InterlockedExchangeAdd64)
	#pragma intrinsic(_InterlockedIncrement64)
	#endif

#if (defined(_M_X64) || defined(_M_AMD64)) && _MSC_VER >= 1500
	#pragma intrinsic(_InterlockedCompareExchange128)
	// unsigned char _InterlockedCompareExchange128(
	//	 i64_t *a,
	//   i64_t exchange1, // sets a[1]
	//   i64_t exchange0, // sets a[0]
	//   i64_t *compare // a is copied into this
	//   )
	// returns 0 if failed
	#endif

#endif
#endif
