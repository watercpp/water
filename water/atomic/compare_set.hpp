// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#if !defined(WATER_ATOMIC_COMPARE_SET_HPP) || defined(WATER_ATOMIC_INLINE)
#ifndef WATER_ATOMIC_INLINE
#define WATER_ATOMIC_COMPARE_SET_HPP
#include <water/atomic/bits.hpp>
namespace water { namespace atomic {
#endif

namespace _ {
	template<
		typename t_,
		barrier_t b_,
		has_t select_ =
			(underlying::atomic<t_, b_>::has & has_compare_set) ? 0 :
			(underlying::atomic<t_, b_>::has & has_get_compare_set) ? has_get_compare_set :
			0
		> struct
	 compare_set {
		static bool do_it(t_* a, t_ b, t_ c)
			{ return underlying::atomic<t_, b_>::compare_set(a, b, c); }
		};
	template<typename t_, barrier_t b_> struct
	 compare_set<t_, b_, has_get_compare_set> {
		static bool do_it(t_* a, t_ b, t_ c)
			{ return underlying::atomic<t_, b_>::get_compare_set(a, b, c) == b; }
		};
	}

template<typename type_, barrier_t barrier_ = full> struct
 compare_set_exists : _::exists<type_, barrier_, has_compare_set | has_get_compare_set>
	{};
	
template<typename type_, barrier_t barrier_ = full> struct
 compare_set_is_real : _::is_real<type_, barrier_, has_compare_set | has_get_compare_set>
	{};
		
template<typename type_, barrier_t barrier_ = full> struct
 compare_set_barrier : _::barrier<type_, barrier_, has_compare_set | has_get_compare_set>
	{};

template<barrier_t barrier_, typename type_> if_type<compare_set_exists<type_, barrier_>, bool>
 compare_set(type_& atomic, value<type_> compare, value<type_> set) {
	// if(atomic == compare) {
	//     atomic = set;
	//     return true;
	//     }
	// return false;
	typedef typename underlying_type_from<type_>::result u_;
	return _::compare_set<u_, compare_set_barrier<u_, barrier_>::result>::do_it(
		_::pointer_cast<u_>(atomic),
		raw_cast<u_>(compare),
		raw_cast<u_>(set)
		);
	}

template<typename type_> if_type<compare_set_exists<type_, full>, bool>
 compare_set(type_& atomic, value<type_> compare, value<type_> set) {
	return compare_set<full>(atomic, compare, set);
	}

#ifndef WATER_ATOMIC_INLINE
}}
#endif
#endif
