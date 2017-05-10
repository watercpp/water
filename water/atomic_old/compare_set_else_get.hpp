// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#if !defined(WATER_ATOMIC_OLD_COMPARE_SET_ELSE_GET_HPP) || defined(WATER_ATOMIC_OLD_INLINE)
#ifndef WATER_ATOMIC_OLD_INLINE
#define WATER_ATOMIC_OLD_COMPARE_SET_ELSE_GET_HPP
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
	 compare_set_else_get {
		static bool do_it(t_* a, t_& b, t_ c) {
			t_ b0 = b;
			return (b = underlying::atomic<t_, b_>::get_compare_set(a, b0, c)) == b0;
			}
		};
	template<typename t_, barrier_t b_> struct
	 compare_set_else_get<t_, b_, has_compare_set> {
		static bool do_it(t_* a, t_& b, t_ c) {
			if(underlying::atomic<t_, b_>::compare_set(a, b, c))
				return true;
			b = get<t_, get_barrier<t_, none>::result>::do_it(a);
			return false;
			}
		};
	}
	
template<typename type_, barrier_t barrier_ = full> struct
 compare_set_else_get_exists : _::exists<type_, barrier_, has_compare_set | has_get_compare_set>
	{};

template<typename type_, barrier_t barrier_ = full> struct
 compare_set_else_get_is_real :
	types::bool_result<
		_::is_real<type_, barrier_, has_get_compare_set>::result ||
		(_::is_real<type_, barrier_, has_compare_set>::result && _::is_real<type_, none, _::get_is_real>::result)
		> {};

template<typename type_, barrier_t barrier_ = full> struct
 compare_set_else_get_barrier : _::barrier<type_, barrier_, has_compare_set | has_get_compare_set>
	{};

template<barrier_t barrier_, typename type_> if_type<compare_set_else_get_exists<type_, barrier_>, bool>
 compare_set_else_get(type_& atomic, value<type_> compare, value<type_> set, value<type_>& get) {
	//
	// if(atomic == compare) {
	//     atomic = set;
	//     return true;
	//     }
	// else {
	//     get = atomic;
	//     return false;
	//     }
	//
	// Notice the 4th argument to this function, get, is a non-const reference
	//
	// This is is made using atomic_old::get_compare_set, if it exists in the the underlying implementation.
	// If it does not, atomic_old::compare_set and atomic_old::get is used like this:
	//
	//   if(atomic_old::compare_set<barrier_>(atomic, compare, set))
	//       return true;
	//   get = atomic_old::get<atomic_old::none>(atomic);
	//   return false;
	//
	// This means it can be 2 atomic operations, although atomic_old::get is used with no barrier. 
	// It is possible that get == compare even after this function returns false, because of the 2 atomic operations.
	// atomic_old::compare_set_else_non_atomic_get can sometimes be used instead of this, and it could be more efficent.
	//
	// Example:
	//
	//   void atomic_add1(atomic_old::int_t& a) {
	//       atomic_old::int_t b = atomic_old::get<atomic_old::none>(a);
	//       while(!atomic_old::compare_set_else_get(a, b, b + 1, b));
	//       }
	//
	typedef typename underlying_type_from<type_>::result u_;
	u_ u = raw_cast<u_>(compare);
	if(_::compare_set_else_get<u_, compare_set_else_get_barrier<u_, barrier_>::result>::do_it(
		_::pointer_cast<u_>(atomic),
		u,
		raw_cast<u_>(set)
		))
			return true;
	get = raw_cast<value<type_> >(u);
	return false;
	}

template<typename type_> if_type<compare_set_else_get_exists<type_, full>, bool>
 compare_set_else_get(type_& atomic, value<type_> compare, value<type_> set, value<type_>& get) {
	return compare_set_else_get<full>(atomic, compare, set, get);
	}

#ifndef WATER_ATOMIC_OLD_INLINE
}}
#endif
#endif
