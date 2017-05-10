// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#if !defined(WATER_ATOMIC_OLD_GET_HPP) || defined(WATER_ATOMIC_OLD_INLINE)
#ifndef WATER_ATOMIC_OLD_INLINE
#define WATER_ATOMIC_OLD_GET_HPP
#include <water/atomic_old/bits.hpp>
namespace water { namespace atomic_old {
#endif

namespace _ {
	
	has_t const
		get_is_real =
			has_get |
			has_add_get |
			has_get_add |
			has_subtract_get |
			has_get_subtract |
			has_get_compare_set,
		get_exists =
			get_is_real |
			has_compare_set;
	
	template<
		typename t_,
		barrier_t b_,
		has_t select_ =
			(underlying::atomic<t_, b_>::has & has_get) ? 0 :
			(underlying::atomic<t_, b_>::has & has_add_get) ? has_add_get :
			(underlying::atomic<t_, b_>::has & has_get_add) ? has_get_add :
			(underlying::atomic<t_, b_>::has & has_subtract_get) ? has_subtract_get :
			(underlying::atomic<t_, b_>::has & has_get_subtract) ? has_get_subtract :
			(underlying::atomic<t_, b_>::has & has_get_compare_set) ? has_get_compare_set :
			(underlying::atomic<t_, b_>::has & has_compare_set) ? has_compare_set :
			0
		> struct
	 get {
		static t_ do_it(t_* a) { return underlying::atomic<t_, b_>::get(a); }
		};
	template<typename t_, barrier_t b_> struct
	 get<t_, b_, has_add_get> {
		static t_ do_it(t_* a) { return underlying::atomic<t_, b_>::add_get(a, static_cast<t_>(0)); }
		};
	template<typename t_, barrier_t b_> struct
	 get<t_, b_, has_get_add> {
		static t_ do_it(t_* a) { return underlying::atomic<t_, b_>::get_add(a, static_cast<t_>(0)); }
		};
	template<typename t_, barrier_t b_> struct
	 get<t_, b_, has_subtract_get> {
		static t_ do_it(t_* a) { return underlying::atomic<t_, b_>::subtract_get(a, static_cast<t_>(0)); }
		};
	template<typename t_, barrier_t b_> struct
	 get<t_, b_, has_get_subtract> {
		static t_ do_it(t_* a) { return underlying::atomic<t_, b_>::get_subtract(a, static_cast<t_>(0)); }
		};
	template<typename t_, barrier_t b_> struct
	 get<t_, b_, has_get_compare_set> {
		static t_ do_it(t_* a) {
			return underlying::atomic<t_, b_>::get_compare_set(a, t_(), t_()); // t_ maybe a struct
			}
		};
	template<typename t_, barrier_t b_> struct
	 get<t_, b_, has_compare_set> {
		static t_ do_it(t_* a) {
			t_ b = *a;
			while(!underlying::atomic<t_, b_>::compare_set(a, b, b))
				b = *static_cast<typename volatile_if<t_>::result*>(a);
			return b;
			}
		};

}

template<typename type_, barrier_t barrier_ = full> struct
 get_exists : _::exists<type_, barrier_, _::get_exists>
	{};
	
template<typename type_, barrier_t barrier_ = full> struct
 get_is_real : _::is_real<type_, barrier_, _::get_is_real>
	{};
		
template<typename type_, barrier_t barrier_ = full> struct
 get_barrier : _::barrier<type_, barrier_, _::get_exists>
	{};

template<barrier_t barrier_, typename type_> if_type<get_exists<type_, barrier_>, value_type_of<type_>> get(type_& atomic) {
	// return atomic;
	//
	// notice atomic is passed by non-const reference
	//
	typedef typename underlying_type_from<type_>::result u_;
	return raw_cast<value<type_> >(
		_::get<u_, get_barrier<u_, barrier_>::result>::do_it(
			_::pointer_cast<u_>(atomic)
			)
		);
	}

template<typename type_> if_type<get_exists<type_, full>, value_type_of<type_>> get(type_& atomic) {
	return get<full>(atomic);
	}

#ifndef WATER_ATOMIC_OLD_INLINE
}}
#endif
#endif
