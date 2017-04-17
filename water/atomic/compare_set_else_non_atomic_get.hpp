// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#if !defined(WATER_ATOMIC_COMPARE_SET_ELSE_NON_ATOMIC_GET_HPP) || defined(WATER_ATOMIC_INLINE)
#ifndef WATER_ATOMIC_INLINE
#define WATER_ATOMIC_COMPARE_SET_ELSE_NON_ATOMIC_GET_HPP
#include <water/atomic/bits.hpp>
namespace water { namespace atomic {
#endif

namespace _ {
	template<
		typename t_,
		barrier_t b_,
		has_t select_ =
			(underlying::atomic<t_, b_>::has & has_get_compare_set) ? has_get_compare_set :
			0
		> struct
	 compare_set_else_non_atomic_get {
		static bool do_it(t_* a, t_& b, t_ c) {
			if(underlying::atomic<t_, b_>::compare_set(a, b, c))
				return true;
			b = *static_cast<typename volatile_if<t_>::result*>(a);
			return false;
			}
		};
	template<typename t_, barrier_t b_> struct
	 compare_set_else_non_atomic_get<t_, b_, has_get_compare_set> {
		static bool do_it(t_* a, t_& b, t_ c) {
			t_ b0 = b;
			return (b = underlying::atomic<t_, b_>::get_compare_set(a, b0, c)) == b0;
			}
		};
	}
	
template<typename type_, barrier_t barrier_ = full> struct
 compare_set_else_non_atomic_get_exists : _::exists<type_, barrier_, has_compare_set | has_get_compare_set>
	{};

template<typename type_, barrier_t barrier_ = full> struct
 compare_set_else_non_atomic_get_is_real : _::is_real<type_, barrier_, has_compare_set | has_get_compare_set>
	{};

template<typename type_, barrier_t barrier_ = full> struct
 compare_set_else_non_atomic_get_barrier : _::barrier<type_, barrier_, has_compare_set | has_get_compare_set>
	{};

template<barrier_t barrier_, typename type_> if_type<compare_set_else_non_atomic_get_exists<type_, barrier_>, bool>
 compare_set_else_non_atomic_get(type_& atomic, value<type_> compare, value<type_> set, value<type_>& get) {
	// This is exactly like atomic::compare_set_else_get, except when it is implemented using atomic::compare_set:
	//
	//   if(atomic::compare_set<barrier_>(atomic, compare, set))
	//       return true;
	//   get = static_cast<volatile type_&>(atomic); // maybe not atomic!
	//   return false;
	//
	// In some cases this could be more efficient than atomic::compare_set_else_get but use it only
	// if all possible bit-patterns of a value of type_ are valid.
	//
	// Example
	//
	//   void atomic_add1(atomic::int_t& a) {
	//       atomic::int_t b = a;
	//       while(!atomic::compare_set_else_non_atomic_get(a, b, b + 1, b));
	//       }
	//
	typedef typename underlying_type_from<type_>::result u_;
	u_ u = raw_cast<u_>(compare);
	if(_::compare_set_else_non_atomic_get<u_, compare_set_else_non_atomic_get_barrier<u_, barrier_>::result>::do_it(
		_::pointer_cast<u_>(atomic),
		u,
		raw_cast<u_>(set)
		))
			return true;
	get = raw_cast<value<type_> >(u);
	return false;
	}

template<typename type_> if_type<compare_set_else_non_atomic_get_exists<type_, full>, bool>
 compare_set_else_non_atomic_get(type_& atomic, value<type_> compare, value<type_> set, value<type_>& get) {
	return compare_set_else_non_atomic_get<full>(atomic, compare, set, get);
	}

#ifndef WATER_ATOMIC_INLINE
}}
#endif
#endif
