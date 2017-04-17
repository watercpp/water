// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#if !defined(WATER_ATOMIC_GET_SUBTRACT_HPP) || defined(WATER_ATOMIC_INLINE)
#ifndef WATER_ATOMIC_INLINE
#define WATER_ATOMIC_GET_SUBTRACT_HPP
#include <water/atomic/bits.hpp>
namespace water { namespace atomic {
#endif

namespace _ {

	has_t const
		get_subtract_is_real =
			has_get_subtract |
			has_subtract_get |
			has_get_add |
			has_add_get,
		get_subtract_exists =
			get_subtract_is_real |
			has_get_compare_set |
			has_compare_set;

	template<
		typename t_,
		barrier_t b_,
		has_t select_ =
			(underlying::atomic<t_, b_>::has & has_get_subtract) ? 0 :
			(underlying::atomic<t_, b_>::has & has_subtract_get) ? has_subtract_get :
			(underlying::atomic<t_, b_>::has & has_get_add) ? has_get_add :
			(underlying::atomic<t_, b_>::has & has_add_get) ? has_add_get :
			(underlying::atomic<t_, b_>::has & has_get_compare_set) ? has_get_compare_set :
			(underlying::atomic<t_, b_>::has & has_compare_set) ? has_compare_set :
			0
		> struct
	 get_subtract {
		static t_ do_it(t_* a, t_ b) { return underlying::atomic<t_, b_>::get_subtract(a, b); }
		};
	template<typename t_, barrier_t b_> struct
	 get_subtract<t_, b_, has_subtract_get> {
		static t_ do_it(t_* a, t_ b) { return underlying::atomic<t_, b_>::subtract_get(a, b) + b; }
		};
	template<typename t_, barrier_t b_> struct
	 get_subtract<t_, b_, has_get_add> {
		static t_ do_it(t_* a, t_ b) { return underlying::atomic<t_, b_>::get_add(a, static_cast<t_>(0) - b); }
		};
	template<typename t_, barrier_t b_> struct
	 get_subtract<t_, b_, has_add_get> {
		static t_ do_it(t_* a, t_ b) { return underlying::atomic<t_, b_>::add_get(a, static_cast<t_>(0) - b) + b; }
		};
	template<typename t_, barrier_t b_> struct
	 get_subtract<t_, b_, has_get_compare_set> {
		static t_ do_it(t_* a, t_ b) {
			t_ a0 = *a, a1;
			while((a1 = underlying::atomic<t_, b_>::get_compare_set(a, a0, a0 - b)) != a0) a0 = a1;
			return a0;
			}
		};
	template<typename t_, barrier_t b_> struct
	 get_subtract<t_, b_, has_compare_set> {
		static t_ do_it(t_* a, t_ b) {
			t_ a0 = *a;
			while(!underlying::atomic<t_, b_>::compare_set(a, a0, a0 - b)) a0 = *static_cast<t_ const volatile*>(a);
			return a0;
			}
		};

}

template<typename int_, barrier_t barrier_ = full> struct
 get_subtract_exists : _::exists_and_int<int_, barrier_, _::get_subtract_exists>
	{};
	
template<typename int_, barrier_t barrier_ = full> struct
 get_subtract_is_real : _::is_real_and_int<int_, barrier_, _::get_subtract_is_real>
	{};

template<typename int_, barrier_t barrier_ = full> struct
 get_subtract_barrier : _::barrier<int_, barrier_, _::get_subtract_exists>
	{};

template<barrier_t barrier_, typename int_> int_ get_subtract(int_& atomic, if_type<get_subtract_exists<int_, barrier_>, int_> subtract) {
	// int_ get = atomic;
	// atomic -= subtract;
	// return get;
	typedef typename underlying_type_from<int_>::result u_;
	return static_cast<int_>(
		_::get_subtract<u_, get_subtract_barrier<u_, barrier_>::result>::do_it(
			static_cast<u_*>(static_cast<void*>(&atomic)),
			static_cast<u_>(subtract)
			)
		);
	}

template<typename int_> int_ get_subtract(int_& atomic, if_type<get_subtract_exists<int_, full>, int_> subtract) {
	return get_subtract<full>(atomic, subtract);
	}

#ifndef WATER_ATOMIC_INLINE
}}
#endif
#endif
