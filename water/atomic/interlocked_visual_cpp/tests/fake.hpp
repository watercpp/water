// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_ATOMIC_INTERLOCKED_VISUAL_CPP_TESTS_FAKE_HPP
#define WATER_ATOMIC_INTERLOCKED_VISUAL_CPP_TESTS_FAKE_HPP
#include <water/types/types.hpp>
#include <water/int.hpp>
#include <water/atomic/tests/fake/fake.hpp>
namespace water { namespace atomic { namespace interlocked_visual_cpp {

#undef _MSC_VER
#define _MSC_VER 1500

#ifndef _M_X64
	#define _M_X64
	#endif

namespace fake = atomic::tests::fake;

typedef types::if_not_void<atomic::int_bits<16>, water::int_bits_at_least<16> >::result i16_t;
typedef types::if_not_void<atomic::int_bits<32>, water::int_bits_at_least<32> >::result i32_t;
typedef types::if_not_void<atomic::int_bits<64>, water::int_bits_at_least<64> >::result i64_t;

struct a128 {
	i64_t my_[2];
	bool operator==(a128 a) const {
		return my_[0] == a.my_[0] && my_[1] == a.my_[1];
		}
	bool operator!=(a128 a) const {
		return my_[0] != a.my_[0] || my_[1] != a.my_[1];
		}
	};
	
inline void _ReadWriteBarrier() { fake::barrier(); }

inline i16_t _InterlockedDecrement16    (i16_t* a) { return fake::subtract1_get<full>(a); }
inline i16_t _InterlockedDecrement16_acq(i16_t* a) { return fake::subtract1_get<acquire>(a); }
inline i16_t _InterlockedDecrement16_rel(i16_t* a) { return fake::subtract1_get<release>(a); }

inline i16_t _InterlockedCompareExchange16    (i16_t* a, i16_t e, i16_t c) { return fake::get_compare_set<full>(a, c, e); }
inline i16_t _InterlockedCompareExchange16_acq(i16_t* a, i16_t e, i16_t c) { return fake::get_compare_set<acquire>(a, c, e); }
inline i16_t _InterlockedCompareExchange16_rel(i16_t* a, i16_t e, i16_t c) { return fake::get_compare_set<release>(a, c, e); }

inline i16_t _InterlockedIncrement16    (i16_t* a) { return fake::add1_get<full>(a); }
inline i16_t _InterlockedIncrement16_acq(i16_t* a) { return fake::add1_get<acquire>(a); }
inline i16_t _InterlockedIncrement16_rel(i16_t* a) { return fake::add1_get<release>(a); }


inline i32_t _InterlockedDecrement    (i32_t* a) { return fake::subtract1_get<full>(a); }
inline i32_t _InterlockedDecrement_acq(i32_t* a) { return fake::subtract1_get<acquire>(a); }
inline i32_t _InterlockedDecrement_rel(i32_t* a) { return fake::subtract1_get<release>(a); }

inline i32_t _InterlockedCompareExchange    (i32_t* a, i32_t e, i32_t c) { return fake::get_compare_set<full>(a, c, e); }
inline i32_t _InterlockedCompareExchange_acq(i32_t* a, i32_t e, i32_t c) { return fake::get_compare_set<acquire>(a, c, e); }
inline i32_t _InterlockedCompareExchange_rel(i32_t* a, i32_t e, i32_t c) { return fake::get_compare_set<release>(a, c, e); }

inline i32_t _InterlockedExchange    (i32_t* a, i32_t e) { return fake::get_set<full>(a, e); }
inline i32_t _InterlockedExchange_acq(i32_t* a, i32_t e) { return fake::get_set<acquire>(a, e); }

inline i32_t _InterlockedExchangeAdd    (i32_t* a, i32_t b) { return fake::get_add<full>(a, b); }
inline i32_t _InterlockedExchangeAdd_acq(i32_t* a, i32_t b) { return fake::get_add<acquire>(a, b); }
inline i32_t _InterlockedExchangeAdd_rel(i32_t* a, i32_t b) { return fake::get_add<release>(a, b); }

inline i32_t _InterlockedIncrement    (i32_t* a) { return fake::add1_get<full>(a); }
inline i32_t _InterlockedIncrement_acq(i32_t* a) { return fake::add1_get<acquire>(a); }
inline i32_t _InterlockedIncrement_rel(i32_t* a) { return fake::add1_get<release>(a); }


inline i64_t _InterlockedDecrement64    (i64_t* a) { return fake::subtract1_get<full>(a); }
inline i64_t _InterlockedDecrement64_acq(i64_t* a) { return fake::subtract1_get<acquire>(a); }
inline i64_t _InterlockedDecrement64_rel(i64_t* a) { return fake::subtract1_get<release>(a); }

inline i64_t _InterlockedCompareExchange64    (i64_t* a, i64_t e, i64_t c) { return fake::get_compare_set<full>(a, c, e); }
inline i64_t _InterlockedCompareExchange64_acq(i64_t* a, i64_t e, i64_t c) { return fake::get_compare_set<acquire>(a, c, e); }
inline i64_t _InterlockedCompareExchange64_rel(i64_t* a, i64_t e, i64_t c) { return fake::get_compare_set<release>(a, c, e); }

inline i64_t _InterlockedExchange64    (i64_t* a, i64_t e) { return fake::get_set<full>(a, e); }
inline i64_t _InterlockedExchange64_acq(i64_t* a, i64_t e) { return fake::get_set<acquire>(a, e); }

inline i64_t _InterlockedExchangeAdd64    (i64_t* a, i64_t b) { return fake::get_add<full>(a, b); }
inline i64_t _InterlockedExchangeAdd64_acq(i64_t* a, i64_t b) { return fake::get_add<acquire>(a, b); }
inline i64_t _InterlockedExchangeAdd64_rel(i64_t* a, i64_t b) { return fake::get_add<release>(a, b); }

inline i64_t _InterlockedIncrement64    (i64_t* a) { return fake::add1_get<full>(a); }
inline i64_t _InterlockedIncrement64_acq(i64_t* a) { return fake::add1_get<acquire>(a); }
inline i64_t _InterlockedIncrement64_rel(i64_t* a) { return fake::add1_get<release>(a); }


inline unsigned char _InterlockedCompareExchange128(i64_t *a, i64_t e1, i64_t e0, i64_t *c) {
	fake::lock<> l;
	if(a[0] == c[0] && a[1] == c[1]) {
		a[0] = e0;
		a[1] = e1;
		return 1;
		}
	c[0] = a[0];
	c[1] = a[1];
	return 0;
	}

}}}
#endif
