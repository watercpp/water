// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#if !defined(WATER_ATOMIC_OLD_GET_SET_HPP) || defined(WATER_ATOMIC_OLD_INLINE)
#ifndef WATER_ATOMIC_OLD_INLINE
#define WATER_ATOMIC_OLD_GET_SET_HPP
#include <water/atomic_old/bits.hpp>
namespace water { namespace atomic_old {
#endif

namespace _ {

	has_t const get_set_exists =
		has_get_set |
		has_get_compare_set |
		has_compare_set;

	template<
		typename t_,
		barrier_t b_,
		has_t select_ =
			(underlying::atomic<t_, b_>::has & has_get_set) ? 0 :
			(underlying::atomic<t_, b_>::has & has_get_compare_set) ? has_get_compare_set :
			(underlying::atomic<t_, b_>::has & has_compare_set) ? has_compare_set :
			0
		> struct
	 get_set {
		static t_ do_it(t_* a, t_ b)
			{ return underlying::atomic<t_, b_>::get_set(a, b); }
		};
	template<typename t_, barrier_t b_> struct
	 get_set<t_, b_, has_get_compare_set> {
		static t_ do_it(t_* a, t_ b) {
			t_ a0 = *a, a1;
			while((a1 = underlying::atomic<t_, b_>::get_compare_set(a, a0, b)) != a0) a0 = a1;
			return a0;
			}
		};
	template<typename t_, barrier_t b_> struct
	 get_set<t_, b_, has_compare_set> {
		static t_ do_it(t_* a, t_ b) {
			t_ a0 = *a;
			while(!underlying::atomic<t_, b_>::compare_set(a, a0, b))
				a0 = *static_cast<typename volatile_if<t_>::result*>(a);
			return a0;
			}
		};

}

template<typename type_, barrier_t barrier_ = full> struct
 get_set_exists : _::exists<type_, barrier_, _::get_set_exists>
	{};
	
template<typename type_, barrier_t barrier_ = full> struct
 get_set_is_real : _::is_real<type_, barrier_, has_get_set>
	{};
		
template<typename type_, barrier_t barrier_ = full> struct
 get_set_barrier : _::barrier<type_, barrier_, _::get_set_exists>
	{};

template<barrier_t barrier_, typename type_> if_type<get_exists<type_, barrier_>, value_type_of<type_>>
 get_set(type_& atomic, value<type_> set) {
	// type_ get = atomic;
	// atomic = set;
	// return get;
	typedef typename underlying_type_from<type_>::result u_;
	return raw_cast<value<type_> >(
		_::get_set<u_, get_set_barrier<u_, barrier_>::result>::do_it(
			_::pointer_cast<u_>(atomic),
			raw_cast<u_>(set)
			)
		);
	}

template<typename type_> if_type<get_exists<type_, full>, value_type_of<type_>>
 get_set(type_& atomic, value<type_> set) {
	return get_set<full>(atomic, set);
	}

#ifndef WATER_ATOMIC_OLD_INLINE
}}
#endif
#endif
