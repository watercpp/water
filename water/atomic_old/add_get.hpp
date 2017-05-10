// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#if !defined(WATER_ATOMIC_OLD_ADD_GET_HPP) || defined(WATER_ATOMIC_OLD_INLINE)
#ifndef WATER_ATOMIC_OLD_INLINE
#define WATER_ATOMIC_OLD_ADD_GET_HPP
#include <water/atomic_old/get_add.hpp>
namespace water { namespace atomic_old {
#endif

namespace _ {
	template<
		typename t_,
		barrier_t b_,
		int select_ =
			(underlying::atomic<t_, b_>::has & has_add_get) ? 0 :
			(underlying::atomic<t_, b_>::has & get_add_exists) ? has_get_add :
			0
		> struct
	 add_get {
		static t_ do_it(t_* a, t_ b) { return underlying::atomic<t_, b_>::add_get(a, b); }
		};
	template<typename t_, barrier_t b_> struct
	 add_get<t_, b_, has_get_add> {
		static t_ do_it(t_* a, t_ b) { return get_add<t_, b_>::do_it(a, b) + b; }
		};
	}

template<typename int_, barrier_t barrier_ = full> struct
 add_get_exists : get_add_exists<int_, barrier_>
	{};

template<typename int_, barrier_t barrier_ = full> struct
 add_get_is_real : get_add_is_real<int_, barrier_>
	{};

template<typename int_, barrier_t barrier_ = full> struct
 add_get_barrier : get_add_barrier<int_, barrier_>
	{};

template<barrier_t barrier_, typename int_> int_ add_get(int_& atomic, if_type<add_get_exists<int_, barrier_>, int_> add) noexcept {
	// atomic += add;
	// return atomic;
	typedef typename underlying_type_from<int_>::result u_;
	return static_cast<int_>(
		_::add_get<u_, add_get_barrier<u_, barrier_>::result>::do_it(
			static_cast<u_*>(static_cast<void*>(&atomic)),
			static_cast<u_>(add)
			)
		);
	}

template<typename int_> int_ add_get(int_& atomic, if_type<add_get_exists<int_, full>, int_> add) noexcept {
	return add_get<full>(atomic, add);
	}

#ifndef WATER_ATOMIC_OLD_INLINE
}}
#endif
#endif
