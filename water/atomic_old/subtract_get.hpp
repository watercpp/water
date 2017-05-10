// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#if !defined(WATER_ATOMIC_OLD_SUBTRACT_GET_HPP) || defined(WATER_ATOMIC_OLD_INLINE)
#ifndef WATER_ATOMIC_OLD_INLINE
#define WATER_ATOMIC_OLD_SUBTRACT_GET_HPP
#include <water/atomic_old/get_subtract.hpp>
namespace water { namespace atomic_old {
#endif

namespace _ {
	template<
		typename t_,
		barrier_t b_,
		int select_ =
			(underlying::atomic<t_, b_>::has & has_subtract_get) ? 0 :
			(underlying::atomic<t_, b_>::has & get_subtract_exists) ? has_get_subtract :
			0
		> struct
	 subtract_get {
		static t_ do_it(t_* a, t_ b) { return underlying::atomic<t_, b_>::subtract_get(a, b); }
		};
	template<typename t_, barrier_t b_> struct
	 subtract_get<t_, b_, has_get_subtract> {
		static t_ do_it(t_* a, t_ b) { return get_subtract<t_, b_>::do_it(a, b) - b; }
		};
	}

template<typename int_, barrier_t barrier_ = full> struct
 subtract_get_exists : get_subtract_exists<int_, barrier_>
	{};

template<typename int_, barrier_t barrier_ = full> struct
 subtract_get_is_real : get_subtract_is_real<int_, barrier_>
	{};

template<typename int_, barrier_t barrier_ = full> struct
 subtract_get_barrier : get_subtract_barrier<int_, barrier_>
	{};

template<barrier_t barrier_, typename int_> int_ subtract_get(int_& atomic, if_type<subtract_get_exists<int_, barrier_>, int_> subtract) {
	// atomic -= subtract;
	// return atomic;
	typedef typename underlying_type_from<int_>::result u_;
	return static_cast<int_>(
		_::subtract_get<u_, subtract_get_barrier<u_, barrier_>::result>::do_it(
			static_cast<u_*>(static_cast<void*>(&atomic)),
			static_cast<u_>(subtract)
			)
		);
	}

template<typename int_> int_ subtract_get(int_& atomic, if_type<subtract_get_exists<int_, full>, int_> subtract) {
	return subtract_get<full>(atomic, subtract);
	}

#ifndef WATER_ATOMIC_OLD_INLINE
}}
#endif
#endif
