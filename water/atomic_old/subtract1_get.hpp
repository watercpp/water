// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#if !defined(WATER_ATOMIC_OLD_SUBTRACT1_GET_HPP) || defined(WATER_ATOMIC_OLD_INLINE)
#ifndef WATER_ATOMIC_OLD_INLINE
#define WATER_ATOMIC_OLD_SUBTRACT1_GET_HPP
#include <water/atomic_old/get_subtract1.hpp>
namespace water { namespace atomic_old {
#endif

namespace _ {
	template<
		typename t_,
		barrier_t b_,
		int select_ =
			(underlying::atomic<t_, b_>::has & has_subtract1_get) ? 0 :
			(underlying::atomic<t_, b_>::has & get_subtract1_exists) ? has_get_subtract1 :
			0
		> struct
	 subtract1_get {
		static t_ do_it(t_* a) { return underlying::atomic<t_, b_>::subtract1_get(a); }
		};
	template<typename t_, barrier_t b_> struct
	 subtract1_get<t_, b_, has_get_subtract1> {
		static t_ do_it(t_* a) { return get_subtract1<t_, b_>::do_it(a) - static_cast<t_>(1); }
		};
	}

template<typename int_, barrier_t barrier_ = full> struct
 subtract1_get_exists : get_subtract1_exists<int_, barrier_>
	{};

template<typename int_, barrier_t barrier_ = full> struct
 subtract1_get_is_real : get_subtract1_is_real<int_, barrier_>
	{};

template<typename int_, barrier_t barrier_ = full> struct
 subtract1_get_barrier : get_subtract1_barrier<int_, barrier_>
	{};

template<barrier_t barrier_, typename int_> if_type<subtract1_get_exists<int_, barrier_>, int_> subtract1_get(int_& atomic) {
	// atomic -= 1;
	// return atomic;
	typedef typename underlying_type_from<int_>::result u_;
	return static_cast<int_>(
		_::subtract1_get<u_, subtract1_get_barrier<u_, barrier_>::result>::do_it(
			static_cast<u_*>(static_cast<void*>(&atomic))
			)
		);
	}

template<typename int_> if_type<subtract1_get_exists<int_, full>, int_> subtract1_get(int_& atomic) {
	return subtract1_get<full>(atomic);
	}

#ifndef WATER_ATOMIC_OLD_INLINE
}}
#endif
#endif
