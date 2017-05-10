// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_ATOMIC_OLD_ATOMIC_OPS_ATOMIC_OPS_HPP
#define WATER_ATOMIC_OLD_ATOMIC_OPS_ATOMIC_OPS_HPP
#include <water/atomic_old/atomic_ops/compare_and_swap.hpp>
#include <water/atomic_old/atomic_ops/fetch_and_add.hpp>
#include <water/atomic_old/atomic_ops/fetch_and_add1.hpp>
#include <water/atomic_old/atomic_ops/fetch_and_sub1.hpp>
#include <water/atomic_old/atomic_ops/load.hpp>
#include <water/atomic_old/atomic_ops/store.hpp>
namespace water { namespace atomic_old { namespace atomic_ops {

/*

For the Boehm atomic_ops library
http://www.hboehm.info/gc/

this has been tested with version 1.2. it includes <atomic_ops.h>

PA-RISC will not work because
- the only atomic thing is AO_test_and_set_full(AO_TS_t*) and AO_CLEAR(AO_TS_t*)
- AO_TS_t is a struct with 4 ints, it must be macro-initialized

*/

template<typename type_, barrier_t barrier_, bool unsigned_ = types::is_unsigned<type_>::result> struct
 atomic {
	enum has_e { has = 0 };
	};
template<typename u_, barrier_t b_> struct
 atomic<u_, b_, true> {
	enum has_e { has =
		compare_and_swap<u_, b_>::result |
		fetch_and_add<u_, b_>::result |
		fetch_and_add1<u_, b_>::result |
		fetch_and_sub1<u_, b_>::result |
		load<u_, b_>::result |
		store<u_, b_>::result
		};
	static u_ get(u_ *a) {
		return load<u_, b_>::do_it(a);
		}
	static void set(u_ *a, u_ b) {
		store<u_, b_>::do_it(a, b);
		}
	static bool compare_set(u_ *a, u_ b, u_ c) {
		return compare_and_swap<u_, b_>::do_it(a, b, c) != 0;
		}
	static u_ get_add(u_ *a, u_ b) {
		return fetch_and_add<u_, b_>::do_it(a, b);
		}
	static u_ get_add1(u_ *a) {
		return fetch_and_add1<u_, b_>::do_it(a);
		}
	static u_ get_subtract1(u_ *a) {
		return fetch_and_sub1<u_, b_>::do_it(a);
		}
	};
template<barrier_t b_, bool unsigned_> struct
 atomic<ao_double_t, b_, unsigned_> {
	typedef ao_double_t u_;
	enum has_e { has =
		compare_and_swap<u_, b_>::result
		};
	static bool compare_set(u_ *a, u_ b, u_ c) {
		return compare_and_swap<u_, b_>::do_it(a, b, c) != 0;
		}
	};


bool const
	use_int = !types::equal<ao_t, unsigned int>::result && atomic<unsigned int, full>::has,
	use_short = !types::equal<ao_t, unsigned short>::result && atomic<unsigned short, full>::has,
	use_char = !types::equal<ao_t, unsigned char>::result && atomic<unsigned char, full>::has,
	use_double = atomic<ao_double_t, full>::has != 0;

template<typename t_, bool if_ = true> struct
 atomic_type_from :
	types::ifel<sizeof(t_) == sizeof(char) && use_char && (sizeof(ao_t) > sizeof(char)), unsigned char,
	types::ifel<sizeof(t_) <= sizeof(short) && use_short && (sizeof(ao_t) > sizeof(short)), unsigned short,
	types::ifel<sizeof(t_) <= sizeof(int) && use_int && (sizeof(ao_t) > sizeof(int)), unsigned int,
	types::ifel<sizeof(t_) <= sizeof(ao_t), ao_t,
	types::ifel<sizeof(t_) <= sizeof(short) && use_short && (sizeof(ao_t) < sizeof(short)), unsigned short,
	types::ifel<sizeof(t_) <= sizeof(int) && use_int && (sizeof(ao_t) < sizeof(int)), unsigned int,
	types::ifel<sizeof(t_) <= sizeof(ao_double_t) && use_double, ao_double_t,
	void
	> > > > > > > {};
template<> struct
 atomic_type_from<types::no_unsigned<ao_t>::result, true> : types::type_plain<ao_t>
	{};
template<> struct
 atomic_type_from<types::to_unsigned<ao_t>::result, true> : types::type_plain<ao_t>
	{};
template<> struct
 atomic_type_from<int, use_int> : types::type_plain<unsigned int>
	{};
template<> struct
 atomic_type_from<unsigned int, use_int> : types::type_plain<unsigned int>
	{};
template<> struct
 atomic_type_from<short, use_short> : types::type_plain<unsigned short>
	{};
template<> struct
 atomic_type_from<unsigned short, use_short> : types::type_plain<unsigned short>
	{};
template<> struct
 atomic_type_from<char, use_char> : types::type_plain<unsigned char>
	{};
template<> struct
 atomic_type_from<unsigned char, use_char> : types::type_plain<unsigned char>
	{};

inline void barrier() { AO_nop_full(); }

}}}
#endif
