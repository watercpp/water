// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_ATOMIC_ATOMIC_OPS_TESTS_FAKE_HPP
#define WATER_ATOMIC_ATOMIC_OPS_TESTS_FAKE_HPP
#include <water/types/types.hpp>
#include <water/int.hpp>
#include <water/atomic/tests/fake/fake.hpp>
namespace water { namespace atomic { namespace atomic_ops {

namespace fake = atomic::tests::fake;

typedef types::ifel<
	sizeof(long) == sizeof(void*),
	unsigned long,
	types::if_not_void<
		atomic::uint_size<sizeof(void*)>,
		water::uint_size<sizeof(void*)>
		>
	>::result AO_t, AO_TS_t;
	
AO_TS_t const AO_TS_CLEAR = 0;

#define AO_HAVE_test_and_set_acquire

inline AO_TS_t
 AO_test_and_set_acquire(AO_TS_t *a) {
	return fake::get_set<acquire>(a, AO_TS_CLEAR + 1);
	}

inline void
 AO_CLEAR(AO_TS_t *a) {
	return fake::set<release>(a, AO_TS_CLEAR);
	}
	
#define AO_HAVE_nop_full

inline void
 AO_nop_full() {
 	fake::barrier();
	}
	
struct AO_double_t {
	AO_t AO_val1, AO_val2;
	bool operator==(AO_double_t a) const {
		return AO_val1 == a.AO_val1 && AO_val2 == a.AO_val2;
		}
	bool operator!=(AO_double_t a) const {
		return !this->operator==(a);
		}
	};

// compare_and_swap
/////////////////////////////////////////////////////////////////////

inline int AO_compare_double_and_swap_double(AO_double_t *a, AO_t b1, AO_t b2, AO_t c1, AO_t c2) {
	AO_double_t
		b = { b1, b2 },
		c = { c1, c2 };
	return fake::compare_set<full>(a, b, c) ? 1 : 0;
	}
inline int AO_compare_double_and_swap_double_acquire(AO_double_t *a, AO_t b1, AO_t b2, AO_t c1, AO_t c2)
	{ return AO_compare_double_and_swap_double(a, b1, b2, c1, c2); }
inline int AO_compare_double_and_swap_double_release(AO_double_t *a, AO_t b1, AO_t b2, AO_t c1, AO_t c2)
	{ return AO_compare_double_and_swap_double(a, b1, b2, c1, c2); }
inline int AO_compare_double_and_swap_double_full(AO_double_t *a, AO_t b1, AO_t b2, AO_t c1, AO_t c2)
	{ return AO_compare_double_and_swap_double(a, b1, b2, c1, c2); }

#define AO_HAVE_compare_double_and_swap_double
#define AO_HAVE_compare_double_and_swap_double_acquire
#define AO_HAVE_compare_double_and_swap_double_release
#define AO_HAVE_compare_double_and_swap_double_full

#define WATER_ATOMIC_ATOMIC_OPS_fake(prefix) \
	template<typename a_> int AO ## prefix ## compare_and_swap(a_ *a, a_ b, a_ c) \
		{ return fake::compare_set<none>(a, b, c) ? 1 : 0; } \
	template<typename a_> int AO ## prefix ## compare_and_swap_acquire(a_ *a, a_ b, a_ c) \
		{ return fake::compare_set<acquire>(a, b, c) ? 1 : 0; } \
	template<typename a_> int AO ## prefix ## compare_and_swap_release(a_ *a, a_ b, a_ c) \
		{ return fake::compare_set<release>(a, b, c) ? 1 : 0; } \
	template<typename a_> int AO ## prefix ## compare_and_swap_full(a_ *a, a_ b, a_ c) \
		{ return fake::compare_set<full>(a, b, c) ? 1 : 0; }

WATER_ATOMIC_ATOMIC_OPS_fake(_)
WATER_ATOMIC_ATOMIC_OPS_fake(_int_)
WATER_ATOMIC_ATOMIC_OPS_fake(_short_)
WATER_ATOMIC_ATOMIC_OPS_fake(_char_)
#undef WATER_ATOMIC_ATOMIC_OPS_fake

#define AO_HAVE_compare_and_swap
#define AO_HAVE_compare_and_swap_acquire
#define AO_HAVE_compare_and_swap_release
#define AO_HAVE_compare_and_swap_full
#define AO_HAVE_int_compare_and_swap
#define AO_HAVE_int_compare_and_swap_acquire
#define AO_HAVE_int_compare_and_swap_release
#define AO_HAVE_int_compare_and_swap_full
#define AO_HAVE_short_compare_and_swap
#define AO_HAVE_short_compare_and_swap_acquire
#define AO_HAVE_short_compare_and_swap_release
#define AO_HAVE_short_compare_and_swap_full
#define AO_HAVE_char_compare_and_swap
#define AO_HAVE_char_compare_and_swap_acquire
#define AO_HAVE_char_compare_and_swap_release
#define AO_HAVE_char_compare_and_swap_full

// fetch_and_add
/////////////////////////////////////////////////////////////////////
#define WATER_ATOMIC_ATOMIC_OPS_fake(prefix) \
	template<typename a_> a_ AO ## prefix ## fetch_and_add(a_ *a, a_ b) \
		{ return fake::get_add<none>(a, b); } \
	template<typename a_> a_ AO ## prefix ## fetch_and_add_acquire(a_ *a, a_ b) \
		{ return fake::get_add<acquire>(a, b); } \
	template<typename a_> a_ AO ## prefix ## fetch_and_add_release(a_ *a, a_ b) \
		{ return fake::get_add<release>(a, b); } \
	template<typename a_> a_ AO ## prefix ## fetch_and_add_full(a_ *a, a_ b) \
		{ return fake::get_add<full>(a, b); }
WATER_ATOMIC_ATOMIC_OPS_fake(_)
WATER_ATOMIC_ATOMIC_OPS_fake(_int_)
WATER_ATOMIC_ATOMIC_OPS_fake(_short_)
WATER_ATOMIC_ATOMIC_OPS_fake(_char_)
#undef WATER_ATOMIC_ATOMIC_OPS_fake

#define AO_HAVE_fetch_and_add
#define AO_HAVE_fetch_and_add_acquire
#define AO_HAVE_fetch_and_add_release
#define AO_HAVE_fetch_and_add_full
#define AO_HAVE_int_fetch_and_add
#define AO_HAVE_int_fetch_and_add_acquire
#define AO_HAVE_int_fetch_and_add_release
#define AO_HAVE_int_fetch_and_add_full
#define AO_HAVE_short_fetch_and_add
#define AO_HAVE_short_fetch_and_add_acquire
#define AO_HAVE_short_fetch_and_add_release
#define AO_HAVE_short_fetch_and_add_full
#define AO_HAVE_char_fetch_and_add
#define AO_HAVE_char_fetch_and_add_acquire
#define AO_HAVE_char_fetch_and_add_release
#define AO_HAVE_char_fetch_and_add_full

// fetch_and_add1
/////////////////////////////////////////////////////////////////////
#define WATER_ATOMIC_ATOMIC_OPS_fake(prefix) \
	template<typename a_> a_ AO ## prefix ## fetch_and_add1(a_ *a) \
		{ return fake::get_add1<none>(a); } \
	template<typename a_> a_ AO ## prefix ## fetch_and_add1_acquire(a_ *a) \
		{ return fake::get_add1<acquire>(a); } \
	template<typename a_> a_ AO ## prefix ## fetch_and_add1_release(a_ *a) \
		{ return fake::get_add1<release>(a); } \
	template<typename a_> a_ AO ## prefix ## fetch_and_add1_full(a_ *a) \
		{ return fake::get_add1<full>(a); }
WATER_ATOMIC_ATOMIC_OPS_fake(_)
WATER_ATOMIC_ATOMIC_OPS_fake(_int_)
WATER_ATOMIC_ATOMIC_OPS_fake(_short_)
WATER_ATOMIC_ATOMIC_OPS_fake(_char_)
#undef WATER_ATOMIC_ATOMIC_OPS_fake

#define AO_HAVE_fetch_and_add1
#define AO_HAVE_fetch_and_add1_acquire
#define AO_HAVE_fetch_and_add1_release
#define AO_HAVE_fetch_and_add1_full
#define AO_HAVE_int_fetch_and_add1
#define AO_HAVE_int_fetch_and_add1_acquire
#define AO_HAVE_int_fetch_and_add1_release
#define AO_HAVE_int_fetch_and_add1_full
#define AO_HAVE_short_fetch_and_add1
#define AO_HAVE_short_fetch_and_add1_acquire
#define AO_HAVE_short_fetch_and_add1_release
#define AO_HAVE_short_fetch_and_add1_full
#define AO_HAVE_char_fetch_and_add1
#define AO_HAVE_char_fetch_and_add1_acquire
#define AO_HAVE_char_fetch_and_add1_release
#define AO_HAVE_char_fetch_and_add1_full

// fetch_and_sub1
/////////////////////////////////////////////////////////////////////
#define WATER_ATOMIC_ATOMIC_OPS_fake(prefix) \
	template<typename a_> a_ AO ## prefix ## fetch_and_sub1(a_ *a) \
		{ return fake::get_subtract1<none>(a); } \
	template<typename a_> a_ AO ## prefix ## fetch_and_sub1_acquire(a_ *a) \
		{ return fake::get_subtract1<acquire>(a); } \
	template<typename a_> a_ AO ## prefix ## fetch_and_sub1_release(a_ *a) \
		{ return fake::get_subtract1<release>(a); } \
	template<typename a_> a_ AO ## prefix ## fetch_and_sub1_full(a_ *a) \
		{ return fake::get_subtract1<full>(a); }
WATER_ATOMIC_ATOMIC_OPS_fake(_)
WATER_ATOMIC_ATOMIC_OPS_fake(_int_)
WATER_ATOMIC_ATOMIC_OPS_fake(_short_)
WATER_ATOMIC_ATOMIC_OPS_fake(_char_)
#undef WATER_ATOMIC_ATOMIC_OPS_fake

#define AO_HAVE_fetch_and_sub1
#define AO_HAVE_fetch_and_sub1_acquire
#define AO_HAVE_fetch_and_sub1_release
#define AO_HAVE_fetch_and_sub1_full
#define AO_HAVE_int_fetch_and_sub1
#define AO_HAVE_int_fetch_and_sub1_acquire
#define AO_HAVE_int_fetch_and_sub1_release
#define AO_HAVE_int_fetch_and_sub1_full
#define AO_HAVE_short_fetch_and_sub1
#define AO_HAVE_short_fetch_and_sub1_acquire
#define AO_HAVE_short_fetch_and_sub1_release
#define AO_HAVE_short_fetch_and_sub1_full
#define AO_HAVE_char_fetch_and_sub1
#define AO_HAVE_char_fetch_and_sub1_acquire
#define AO_HAVE_char_fetch_and_sub1_release
#define AO_HAVE_char_fetch_and_sub1_full

// load
/////////////////////////////////////////////////////////////////////
#define WATER_ATOMIC_ATOMIC_OPS_fake(prefix) \
	template<typename a_> a_ AO ## prefix ## load(a_ *a) \
		{ return fake::get<none>(a); } \
	template<typename a_> a_ AO ## prefix ## load_acquire(a_ *a) \
		{ return fake::get<acquire>(a); } \
	template<typename a_> a_ AO ## prefix ## load_full(a_ *a) \
		{ return fake::get<full>(a); }
WATER_ATOMIC_ATOMIC_OPS_fake(_)
WATER_ATOMIC_ATOMIC_OPS_fake(_int_)
WATER_ATOMIC_ATOMIC_OPS_fake(_short_)
WATER_ATOMIC_ATOMIC_OPS_fake(_char_)
#undef WATER_ATOMIC_ATOMIC_OPS_fake

// no load_release
#define AO_HAVE_load
#define AO_HAVE_load_acquire
#define AO_HAVE_load_full
#define AO_HAVE_int_load
#define AO_HAVE_int_load_acquire
#define AO_HAVE_int_load_full
#define AO_HAVE_short_load
#define AO_HAVE_short_load_acquire
#define AO_HAVE_short_load_full
#define AO_HAVE_char_load
#define AO_HAVE_char_load_acquire
#define AO_HAVE_char_load_full

// store
/////////////////////////////////////////////////////////////////////
#define WATER_ATOMIC_ATOMIC_OPS_fake(prefix) \
	template<typename a_> void AO ## prefix ## store(a_ *a, a_ b) \
		{ fake::set<none>(a, b); } \
	template<typename a_> void AO ## prefix ## store_release(a_ *a, a_ b) \
		{ fake::set<release>(a, b); } \
	template<typename a_> void AO ## prefix ## store_full(a_ *a, a_ b) \
		{ fake::set<full>(a, b); }
WATER_ATOMIC_ATOMIC_OPS_fake(_)
WATER_ATOMIC_ATOMIC_OPS_fake(_int_)
WATER_ATOMIC_ATOMIC_OPS_fake(_short_)
WATER_ATOMIC_ATOMIC_OPS_fake(_char_)
#undef WATER_ATOMIC_ATOMIC_OPS_fake

// no store_acquire
#define AO_HAVE_store
#define AO_HAVE_store_release
#define AO_HAVE_store_full
#define AO_HAVE_int_store
#define AO_HAVE_int_store_release
#define AO_HAVE_int_store_full
#define AO_HAVE_short_store
#define AO_HAVE_short_store_release
#define AO_HAVE_short_store_full
#define AO_HAVE_char_store
#define AO_HAVE_char_store_release
#define AO_HAVE_char_store_full

}}}
#endif
