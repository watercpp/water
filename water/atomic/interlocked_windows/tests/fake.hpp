// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_ATOMIC_INTERLOCKED_WINDOWS_TESTS_FAKE_HPP
#define WATER_ATOMIC_INTERLOCKED_WINDOWS_TESTS_FAKE_HPP
#include <water/types/types.hpp>
#include <water/int.hpp>
#include <water/atomic/tests/fake/fake.hpp>
namespace water { namespace atomic { namespace interlocked_windows {

#ifndef WATER_WINDOWS_XP
	#define WATER_WINDOWS_XP 0
	#endif

#ifndef WATER_WINDOWS_2003
	#define WATER_WINDOWS_2003 (WATER_WINDOWS_XP + 1)
	#endif

#ifndef WATER_WINDOWS_VISTA
	#define WATER_WINDOWS_VISTA (WATER_WINDOWS_2003 + 1)
	#endif

#ifndef WATER_WINDOWS_VERSION
	#define WATER_WINDOWS_VERSION WATER_WINDOWS_VISTA
	#endif

namespace fake = atomic::tests::fake;

size_t const bits32 = 32
	#ifdef WATER_ATOMIC_INTERLOCKED_TESTS_FAKE_64
	- 16
	#endif
	;

typedef types::if_not_void<atomic::int_bits<bits32>, water::int_bits_at_least<bits32> >::result a32_t, LONG;
typedef types::if_not_void<atomic::int_bits<bits32 * 2>, water::uint_bits_at_least<bits32 * 2> >::result a64_t, LONGLONG;

#undef InterlockedDecrement
#undef InterlockedDecrementAcquire
#undef InterlockedDecrementRelease
#undef InterlockedCompareExchange
#undef InterlockedCompareExchangeAcquire
#undef InterlockedCompareExchangeRelease
#undef InterlockedExchange
#undef InterlockedExchangeAcquire
#undef InterlockedExchangeAdd
#undef InterlockedExchangeAddAcquire
#undef InterlockedExchangeAddRelease
#undef InterlockedIncrement
#undef InterlockedIncrementAcquire
#undef InterlockedIncrementRelease
#undef InterlockedDecrement64
#undef InterlockedDecrementAcquire64
#undef InterlockedDecrementRelease64
#undef InterlockedCompareExchange64
#undef InterlockedCompareExchangeAcquire64
#undef InterlockedCompareExchangeRelease64
#undef InterlockedExchange64
#undef InterlockedExchangeAcquire64
#undef InterlockedExchangeAdd64
#undef InterlockedExchangeAddAcquire64
#undef InterlockedExchangeAddRelease64
#undef InterlockedIncrement64
#undef InterlockedIncrementAcquire64
#undef InterlockedIncrementRelease64
#undef InterlockedCompareExchangePointer
#undef InterlockedExchangePointer

inline a32_t InterlockedDecrement    (a32_t* a) { return fake::subtract1_get<full>(a); }
inline a32_t InterlockedDecrementAcquire(a32_t* a) { return fake::subtract1_get<acquire>(a); }
inline a32_t InterlockedDecrementRelease(a32_t* a) { return fake::subtract1_get<release>(a); }

inline a32_t InterlockedCompareExchange       (a32_t* a, a32_t e, a32_t c) { return fake::get_compare_set<full>(a, c, e); }
inline a32_t InterlockedCompareExchangeAcquire(a32_t* a, a32_t e, a32_t c) { return fake::get_compare_set<acquire>(a, c, e); }
inline a32_t InterlockedCompareExchangeRelease(a32_t* a, a32_t e, a32_t c) { return fake::get_compare_set<release>(a, c, e); }

inline a32_t InterlockedExchange       (a32_t* a, a32_t e) { return fake::get_set<full>(a, e); }
inline a32_t InterlockedExchangeAcquire(a32_t* a, a32_t e) { return fake::get_set<acquire>(a, e); }

inline a32_t InterlockedExchangeAdd       (a32_t* a, a32_t b) { return fake::get_add<full>(a, b); }
inline a32_t InterlockedExchangeAddAcquire(a32_t* a, a32_t b) { return fake::get_add<acquire>(a, b); }
inline a32_t InterlockedExchangeAddRelease(a32_t* a, a32_t b) { return fake::get_add<release>(a, b); }

inline a32_t InterlockedIncrement       (a32_t* a) { return fake::add1_get<full>(a); }
inline a32_t InterlockedIncrementAcquire(a32_t* a) { return fake::add1_get<acquire>(a); }
inline a32_t InterlockedIncrementRelease(a32_t* a) { return fake::add1_get<release>(a); }


inline a64_t InterlockedDecrement64       (a64_t* a) { return fake::subtract1_get<full>(a); }
inline a64_t InterlockedDecrementAcquire64(a64_t* a) { return fake::subtract1_get<acquire>(a); }
inline a64_t InterlockedDecrementRelease64(a64_t* a) { return fake::subtract1_get<release>(a); }

inline a64_t InterlockedCompareExchange64       (a64_t* a, a64_t e, a64_t c) { return fake::get_compare_set<full>(a, c, e); }
inline a64_t InterlockedCompareExchangeAcquire64(a64_t* a, a64_t e, a64_t c) { return fake::get_compare_set<acquire>(a, c, e); }
inline a64_t InterlockedCompareExchangeRelease64(a64_t* a, a64_t e, a64_t c) { return fake::get_compare_set<release>(a, c, e); }

inline a64_t InterlockedExchange64       (a64_t* a, a64_t e) { return fake::get_set<full>(a, e); }
inline a64_t InterlockedExchangeAcquire64(a64_t* a, a64_t e) { return fake::get_set<acquire>(a, e); }

inline a64_t InterlockedExchangeAdd64       (a64_t* a, a64_t b) { return fake::get_add<full>(a, b); }
inline a64_t InterlockedExchangeAddAcquire64(a64_t* a, a64_t b) { return fake::get_add<acquire>(a, b); }
inline a64_t InterlockedExchangeAddRelease64(a64_t* a, a64_t b) { return fake::get_add<release>(a, b); }

inline a64_t InterlockedIncrement64       (a64_t* a) { return fake::add1_get<full>(a); }
inline a64_t InterlockedIncrementAcquire64(a64_t* a) { return fake::add1_get<acquire>(a); }
inline a64_t InterlockedIncrementRelease64(a64_t* a) { return fake::add1_get<release>(a); }


inline void* InterlockedCompareExchangePointer(void** a, void* e, void* c) { return fake::get_compare_set<full>(a, c, e); }
inline void* InterlockedExchangePointer       (void** a, void* e) { return fake::get_set<full>(a, e); }

}}}
#endif
