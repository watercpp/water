// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#if !defined(WATER_ATOMIC_OLD_GET_SUBTRACT1_HPP) || defined(WATER_ATOMIC_OLD_INLINE)
#ifndef WATER_ATOMIC_OLD_INLINE
#define WATER_ATOMIC_OLD_GET_SUBTRACT1_HPP
#include <water/atomic_old/get_subtract.hpp>
namespace water { namespace atomic_old {
#endif

namespace _ {

	has_t const
		get_subtract1_is_real =
			has_get_subtract1 |
			has_subtract1_get |
			get_subtract_is_real,
		get_subtract1_exists =
			get_subtract1_is_real |
			get_subtract_exists;

	template<
		typename t_,
		barrier_t b_,
		has_t select_ =
			(underlying::atomic<t_, b_>::has & has_get_subtract1) ? 0 :
			(underlying::atomic<t_, b_>::has & has_subtract1_get) ? has_subtract1_get :
			(underlying::atomic<t_, b_>::has & get_subtract_exists) ? has_get_subtract :
			0
		> struct
	 get_subtract1 {
		static t_ do_it(t_* a) { return underlying::atomic<t_, b_>::get_subtract1(a); }
		};
	template<typename t_, barrier_t b_> struct
	 get_subtract1<t_, b_, has_subtract1_get> {
		static t_ do_it(t_* a) { return underlying::atomic<t_, b_>::subtract1_get(a) + static_cast<t_>(1); }
		};
	template<typename t_, barrier_t b_> struct
	 get_subtract1<t_, b_, has_get_subtract> {
		static t_ do_it(t_* a) { return get_subtract<t_, b_>::do_it(a, static_cast<t_>(1)); }
		};

}

template<typename int_, barrier_t barrier_ = full> struct
 get_subtract1_exists : _::exists_and_int<int_, barrier_, _::get_subtract1_exists>
	{};
	
template<typename int_, barrier_t barrier_ = full> struct
 get_subtract1_is_real : _::is_real_and_int<int_, barrier_, _::get_subtract1_is_real>
	{};

template<typename int_, barrier_t barrier_ = full> struct
 get_subtract1_barrier : _::barrier<int_, barrier_, _::get_subtract1_exists>
	{};

template<barrier_t barrier_, typename int_> if_type<get_subtract1_exists<int_, barrier_>, int_> get_subtract1(int_& atomic) {
	// int_ get = atomic;
	// atomic -= 1;
	// return get;
	typedef typename underlying_type_from<int_>::result u_;
	return static_cast<int_>(
		_::get_subtract1<u_, get_subtract1_barrier<u_, barrier_>::result>::do_it(
			static_cast<u_*>(static_cast<void*>(&atomic))
			)
		);
	}

template<typename int_> if_type<get_subtract1_exists<int_, full>, int_> get_subtract1(int_& atomic) {
	return get_subtract1<full>(atomic);
	}

#ifndef WATER_ATOMIC_OLD_INLINE
}}
#endif
#endif
