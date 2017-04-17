// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#if !defined(WATER_ATOMIC_GET_ADD1_HPP) || defined(WATER_ATOMIC_INLINE)
#ifndef WATER_ATOMIC_INLINE
#define WATER_ATOMIC_GET_ADD1_HPP
#include <water/atomic/get_add.hpp>
namespace water { namespace atomic {
#endif

namespace _ {

	has_t const
		get_add1_is_real =
			has_get_add1 |
			has_add1_get |
			get_add_is_real,
		get_add1_exists =
			get_add1_is_real |
			get_add_exists;

	template<
		typename t_,
		barrier_t b_,
		has_t select_ =
			(underlying::atomic<t_, b_>::has & has_get_add1) ? 0 :
			(underlying::atomic<t_, b_>::has & has_add1_get) ? has_add1_get :
			(underlying::atomic<t_, b_>::has & get_add_exists) ? has_get_add :
			0
		> struct
	 get_add1 {
		static t_ do_it(t_* a) { return underlying::atomic<t_, b_>::get_add1(a); }
		};
	template<typename t_, barrier_t b_> struct
	 get_add1<t_, b_, has_add1_get> {
		static t_ do_it(t_* a) { return underlying::atomic<t_, b_>::add1_get(a) - static_cast<t_>(1); }
		};
	template<typename t_, barrier_t b_> struct
	 get_add1<t_, b_, has_get_add> {
		static t_ do_it(t_* a) { return get_add<t_, b_>::do_it(a, static_cast<t_>(1)); }
		};

}

template<typename int_, barrier_t barrier_ = full> struct
 get_add1_exists : _::exists_and_int<int_, barrier_, _::get_add1_exists>
	{};
	
template<typename int_, barrier_t barrier_ = full> struct
 get_add1_is_real : _::is_real_and_int<int_, barrier_, _::get_add1_is_real>
	{};

template<typename int_, barrier_t barrier_ = full> struct
 get_add1_barrier : _::barrier<int_, barrier_, _::get_add1_exists>
	{};

template<barrier_t barrier_, typename int_> if_type<get_add1_exists<int_, barrier_>, int_> get_add1(int_& atomic) {
	// int_ get = atomic;
	// atomic += 1;
	// return get;
	typedef typename underlying_type_from<int_>::result u_;
	return static_cast<int_>(
		_::get_add1<u_, get_add1_barrier<u_, barrier_>::result>::do_it(
			static_cast<u_*>(static_cast<void*>(&atomic))
			)
		);
	}

template<typename int_> if_type<get_add1_exists<int_, full>, int_> get_add1(int_& atomic) {
	return get_add1<full>(atomic);
	}

#ifndef WATER_ATOMIC_INLINE
}}
#endif
#endif
