// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#if !defined(WATER_ATOMIC_OLD_GET_ADD_HPP) || defined(WATER_ATOMIC_OLD_INLINE)
#ifndef WATER_ATOMIC_OLD_INLINE
#define WATER_ATOMIC_OLD_GET_ADD_HPP
#include <water/atomic_old/bits.hpp>
namespace water { namespace atomic_old {
#endif

namespace _ {

	has_t const
		get_add_is_real =
			has_get_add |
			has_add_get |
			has_get_subtract |
			has_subtract_get,
		get_add_exists =
			get_add_is_real |
			has_get_compare_set |
			has_compare_set;

	template<
		typename t_,
		barrier_t b_,
		has_t select_ =
			(underlying::atomic<t_, b_>::has & has_get_add) ? 0 :
			(underlying::atomic<t_, b_>::has & has_add_get) ? has_add_get :
			(underlying::atomic<t_, b_>::has & has_get_subtract) ? has_get_subtract :
			(underlying::atomic<t_, b_>::has & has_subtract_get) ? has_subtract_get :
			(underlying::atomic<t_, b_>::has & has_get_compare_set) ? has_get_compare_set :
			(underlying::atomic<t_, b_>::has & has_compare_set) ? has_compare_set :
			0
		> struct
	 get_add {
		static t_ do_it(t_* a, t_ b) { return underlying::atomic<t_, b_>::get_add(a, b); }
		};
	template<typename t_, barrier_t b_> struct
	 get_add<t_, b_, has_add_get> {
		static t_ do_it(t_* a, t_ b) { return underlying::atomic<t_, b_>::add_get(a, b) - b; }
		};
	template<typename t_, barrier_t b_> struct
	 get_add<t_, b_, has_get_subtract> {
		static t_ do_it(t_* a, t_ b) { return underlying::atomic<t_, b_>::get_subtract(a, static_cast<t_>(0) - b); }
		};
	template<typename t_, barrier_t b_> struct
	 get_add<t_, b_, has_subtract_get> {
		static t_ do_it(t_* a, t_ b) { return underlying::atomic<t_, b_>::subtract_get(a, static_cast<t_>(0) - b) - b; }
		};
	template<typename t_, barrier_t b_> struct
	 get_add<t_, b_, has_get_compare_set> {
		static t_ do_it(t_* a, t_ b) {
			t_ a0 = *a, a1;
			while((a1 = underlying::atomic<t_, b_>::get_compare_set(a, a0, a0 + b)) != a0) a0 = a1;
			return a0;
			}
		};
	template<typename t_, barrier_t b_> struct
	 get_add<t_, b_, has_compare_set> {
		static t_ do_it(t_* a, t_ b) {
			t_ a0 = *a;
			while(!underlying::atomic<t_, b_>::compare_set(a, a0, a0 + b)) a0 = *static_cast<t_ const volatile*>(a);
			return a0;
			}
		};

}

template<typename int_, barrier_t barrier_ = full> struct
 get_add_exists : _::exists_and_int<int_, barrier_, _::get_add_exists>
	{};
	
template<typename int_, barrier_t barrier_ = full> struct
 get_add_is_real : _::is_real_and_int<int_, barrier_, _::get_add_is_real>
	{};

template<typename int_, barrier_t barrier_ = full> struct
 get_add_barrier : _::barrier<int_, barrier_, _::get_add_exists>
	{};

template<barrier_t barrier_, typename int_> int_ get_add(int_& atomic, if_type<get_add_exists<int_, barrier_>, int_> add) {
	// int_ get = atomic;
	// atomic += add;
	// return get;
	typedef typename underlying_type_from<int_>::result u_;
	return static_cast<int_>(
		_::get_add<u_, get_add_barrier<u_, barrier_>::result>::do_it(
			static_cast<u_*>(static_cast<void*>(&atomic)),
			static_cast<u_>(add)
			)
		);
	}

template<typename int_> int_ get_add(int_& atomic, if_type<get_add_exists<int_, full>, int_> add) {
	return get_add<full>(atomic, add);
	}

#ifndef WATER_ATOMIC_OLD_INLINE
}}
#endif
#endif
