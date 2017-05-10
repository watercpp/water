// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_ATOMIC_OLD_SYNC_SYNC_HPP
#define WATER_ATOMIC_OLD_SYNC_SYNC_HPP
#include <water/types/types.hpp>
#include <water/int.hpp>
#include <water/atomic_old/constants.hpp>
namespace water { namespace atomic_old { namespace sync {

/*

GCC builtin __sync_* functions originally from the Intel Itanium ABI

*/

unsigned constexpr has_size = 0
	#if defined(__GCC_HAVE_SYNC_COMPARE_AND_SWAP_1) && !defined(WATER_ATOMIC_OLD_SYNC_NO_1)
	| 1
	#endif
	#if defined(__GCC_HAVE_SYNC_COMPARE_AND_SWAP_2) && !defined(WATER_ATOMIC_OLD_SYNC_NO_2)
	| 2
	#endif
	#if defined(__GCC_HAVE_SYNC_COMPARE_AND_SWAP_4) && !defined(WATER_ATOMIC_OLD_SYNC_NO_4)
	| 4
	#endif
	#if defined(__GCC_HAVE_SYNC_COMPARE_AND_SWAP_8) && !defined(WATER_ATOMIC_OLD_SYNC_NO_8)
	| 8
	#endif
	#if defined(__GCC_HAVE_SYNC_COMPARE_AND_SWAP_16) && !defined(WATER_ATOMIC_OLD_SYNC_NO_16)
	| 16
	#endif
	#ifdef WATER_ATOMIC_OLD_SYNC_TESTS_FAKE_HPP
	| 1 | 2 | 4 | 8 | 16
	#endif
	;

using int_t = water::int_size<sizeof(void*)>;

struct size16 { unsigned char my[16]; };

using sync16_t =
	#if defined(__SIZEOF_INT128__) && __SIZEOF_INT128__ == 16 && defined(__GCC_HAVE_SYNC_COMPARE_AND_SWAP_16) && !defined(WATER_ATOMIC_OLD_SYNC_NO_16)
	__int128;
	#else
	types::if_not_void<water::uint_size<16>, size16>::result;
	#endif

static_assert(sizeof(sync16_t) == 16, "whoops");

inline void barrier() { __sync_synchronize(); }

template<typename type_> constexpr size_t sizeof_or_0() { return sizeof(type_); }
template<> constexpr size_t sizeof_or_0<void>() { return 0; }

// if a_ is pointer or int, and can be used, use it
// else select smallest int, if a_ is int use same sign as a_
template<typename a_, bool unsigned_ = types::is_unsigned<a_>::result> struct
 atomic_type_from :
	types::ifel<(types::is_pointer<a_>::result || types::is_int<a_>::result) && (has_size & sizeof(a_)), a_,
	types::ifel<unsigned_ && sizeof(a_) <= sizeof_or_0<water::uint_size<1>>() && (has_size & 1), water::uint_size<1>,
	types::ifel<unsigned_ && sizeof(a_) <= sizeof_or_0<water::uint_size<2>>() && (has_size & 2), water::uint_size<2>,
	types::ifel<unsigned_ && sizeof(a_) <= sizeof_or_0<water::uint_size<4>>() && (has_size & 4), water::uint_size<4>,
	types::ifel<unsigned_ && sizeof(a_) <= sizeof_or_0<water::uint_size<8>>() && (has_size & 8), water::uint_size<8>,
	types::ifel<!unsigned_ && sizeof(a_) <= sizeof_or_0<water::int_size<1>>() && (has_size & 1), water::int_size<1>,
	types::ifel<!unsigned_ && sizeof(a_) <= sizeof_or_0<water::int_size<2>>() && (has_size & 2), water::int_size<2>,
	types::ifel<!unsigned_ && sizeof(a_) <= sizeof_or_0<water::int_size<4>>() && (has_size & 4), water::int_size<4>,
	types::ifel<!unsigned_ && sizeof(a_) <= sizeof_or_0<water::int_size<8>>() && (has_size & 8), water::int_size<8>,
	types::ifel<sizeof(a_) <= 16 && (has_size & 16), sync16_t,
	void
	>>>>>>>>>> {};

template<typename type_, barrier_t barrier_, typename atomic_ = typename atomic_type_from<type_>::result, size_t size_ = sizeof(type_)> struct
 atomic {
	enum has_ { has = 0 };
	};

template<typename type_> struct have_add_subtract : types::true_result {};
template<typename type_> struct have_add_subtract<type_*> : types::false_result {};
template<> struct have_add_subtract<size16> : types::false_result {};

template<typename type_, size_t size_> struct
 atomic<type_, full, type_, size_> {
	static constexpr has_t has =
		#ifdef WATER_COMPILER_CLANG
		has_get_set |
		#endif
		has_compare_set |
		has_get_compare_set |
		(have_add_subtract<type_>::result ? (has_add_get | has_get_add | has_subtract_get | has_get_subtract) : 0);
	static type_ get_set        (type_ *a, type_ b)          { return __sync_swap(a, b); } // clang only
	static bool  compare_set    (type_ *a, type_ c, type_ s) { return __sync_bool_compare_and_swap(a, c, s); }
	static type_ get_compare_set(type_ *a, type_ c, type_ s) { return __sync_val_compare_and_swap(a, c, s); }
	static type_ add_get        (type_ *a, type_ b)          { return __sync_add_and_fetch(a, b); }
	static type_ get_add        (type_ *a, type_ b)          { return __sync_fetch_and_add(a, b); }
	static type_ subtract_get   (type_ *a, type_ b)          { return __sync_sub_and_fetch(a, b); }
	static type_ get_subtract   (type_ *a, type_ b)          { return __sync_fetch_and_sub(a, b); }
	};

// __sync_lock_test_and_set is supposed to be aquire, but maybe it will only set the atomic to 1? 
template<typename type_, size_t size_> struct
 atomic<type_, acquire, type_, size_> {
	static constexpr has_t has = has_get_set;
	static type_ get_set(type_ *a, type_ b) { return __sync_lock_test_and_set(a, b); }
	};

// assume get/set of pointer sized is atomic
template<typename type_> struct
 atomic<type_, none, type_, sizeof(void*)> {
	static constexpr has_t has = has_get | has_set;
	static type_ get(type_ volatile *a) { return *a; }
	static void  set(type_ volatile *a, type_ b) { *a = b; }
	};

}}}
#endif
