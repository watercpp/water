// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#if !defined(WATER_ATOMIC_OLD_GET_COMPARE_SET_HPP) || defined(WATER_ATOMIC_OLD_INLINE)
#ifndef WATER_ATOMIC_OLD_INLINE
#define WATER_ATOMIC_OLD_GET_COMPARE_SET_HPP
#include <water/atomic_old/get.hpp>
namespace water { namespace atomic_old {
#endif

namespace _ {
	template<
		typename t_,
		barrier_t b_,
		has_t select_ =
			(underlying::atomic<t_, b_>::has & has_get_compare_set) ? 0 :
			(underlying::atomic<t_, b_>::has & has_compare_set) ? has_compare_set :
			0
		> struct
	 get_compare_set {
		static t_ do_it(t_* a, t_ b, t_ c)
			{ return underlying::atomic<t_, b_>::get_compare_set(a, b, c); }
		};
	template<typename t_, barrier_t b_> struct
	 get_compare_set<t_, b_, has_compare_set> {
		static t_ do_it(t_* a, t_ b, t_ c) {
			// atomic_old::get<none> can be compare_set in a loop too
			t_ r = b;
			while(
				!underlying::atomic<t_, b_>::compare_set(a, b, c) &&
				(r = get<t_, get_barrier<t_, none>::result>::do_it(a)) == b
				);
			return r;
			}
		};
	}

template<typename type_, barrier_t barrier_ = full> struct
 get_compare_set_exists : _::exists<type_, barrier_, has_compare_set | has_get_compare_set>
	{};

template<typename type_, barrier_t barrier_ = full> struct
 get_compare_set_is_real : _::is_real<type_, barrier_, has_get_compare_set>
	{};

template<typename type_, barrier_t barrier_ = full> struct
 get_compare_set_barrier : _::barrier<type_, barrier_, has_compare_set | has_get_compare_set>
	{};
	
template<barrier_t barrier_, typename type_> if_type<get_compare_set_exists<type_, barrier_>, value_type_of<type_>>
 get_compare_set(type_& atomic, value<type_> compare, value<type_> set) {
	// type_ get = atomic;
	// if(atomic == compare) atomic = set;
	// return get;
	typedef typename underlying_type_from<type_>::result u_;
	return raw_cast<value<type_> >(
		_::get_compare_set<u_, get_compare_set_barrier<u_, barrier_>::result>::do_it(
			_::pointer_cast<u_>(atomic),
			raw_cast<u_>(compare),
			raw_cast<u_>(set)
			)
		);
	}

template<typename type_> if_type<get_compare_set_exists<type_, full>, value_type_of<type_>>
 get_compare_set(type_& atomic, value<type_> compare, value<type_> set) {
	return get_compare_set<full>(atomic, compare, set);
	}

#ifndef WATER_ATOMIC_OLD_INLINE
}}
#endif
#endif
