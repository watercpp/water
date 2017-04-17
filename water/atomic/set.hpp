// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#if !defined(WATER_ATOMIC_SET_HPP) || defined(WATER_ATOMIC_INLINE)
#ifndef WATER_ATOMIC_INLINE
#define WATER_ATOMIC_SET_HPP
#include <water/atomic/get_set.hpp>
namespace water { namespace atomic {
#endif

namespace _ {
	template<
		typename t_,
		barrier_t b_,
		has_t select_ =
			(underlying::atomic<t_, b_>::has & has_set) ? 0 :
			(underlying::atomic<t_, b_>::has & get_set_exists) ? has_get_set :
			0
		> struct
	 set {
		static void do_it(t_* a, t_ b) { underlying::atomic<t_, b_>::set(a, b); }
		};
	template<typename t_, barrier_t b_> struct
	 set<t_, b_, has_get_set> { // this must return void because return in set below
		static void do_it(t_* a, t_ b) { get_set<t_, b_>::do_it(a, b); }
		};
	}

template<typename type_, barrier_t barrier_ = full> struct
 set_exists : _::exists<type_, barrier_, has_set | _::get_set_exists>
	{};

template<typename type_, barrier_t barrier_ = full> struct
 set_is_real : _::is_real<type_, barrier_, has_set | has_get_set>
	{};

template<typename type_, barrier_t barrier_ = full> struct
 set_barrier : _::barrier<type_, barrier_, has_set | _::get_set_exists>
	{};

template<barrier_t barrier_, typename type_> if_type<set_exists<type_, barrier_>, void> set(type_& atomic, value<type_> set) {
	// atomic = set;
	typedef typename underlying_type_from<type_>::result u_;
	_::set<u_, set_barrier<u_, barrier_>::result>::do_it(
		_::pointer_cast<u_>(atomic),
		raw_cast<u_>(set)
		);
	}

template<typename type_> if_type<set_exists<type_, full>, void> set(type_& atomic, value<type_> set_) {
	set<full>(atomic, set_);
	}

#ifndef WATER_ATOMIC_INLINE
}}
#endif
#endif
