// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#if !defined(WATER_ATOMIC_ADD_HPP) || defined(WATER_ATOMIC_INLINE)
#ifndef WATER_ATOMIC_INLINE
#define WATER_ATOMIC_ADD_HPP
#include <water/atomic/get_add.hpp>
namespace water { namespace atomic {
#endif

namespace _ {
	template<
		typename t_,
		barrier_t b_,
		int select_ =
			(underlying::atomic<t_, b_>::has & has_add) ? 0 :
			(underlying::atomic<t_, b_>::has & (has_get_add | has_add_get)) ? has_get_add :
			(underlying::atomic<t_, b_>::has & has_subtract) ? has_subtract :
			(underlying::atomic<t_, b_>::has & get_add_exists) ? has_get_add :
			0
		> struct
	 add {
		static void do_it(t_* a, t_ b) { underlying::atomic<t_, b_>::add(a, b); }
		};
	template<typename t_, barrier_t b_> struct
	 add<t_, b_, has_subtract> {
		static void do_it(t_* a, t_ b) { underlying::atomic<t_, b_>::subtract(a, static_cast<t_>(0) - b); }
		};
	template<typename t_, barrier_t b_> struct
	 add<t_, b_, has_get_add> :
	 	get_add<t_, b_>
			{};
	}

template<typename int_, barrier_t barrier_ = full> struct
 add_exists : _::exists_and_int<int_, barrier_, has_add | has_subtract | _::get_add_exists>
	{};
		
template<typename int_, barrier_t barrier_ = full> struct
 add_is_real : _::is_real_and_int<int_, barrier_, has_add | has_subtract | _::get_add_is_real>
	{};

template<typename int_, barrier_t barrier_ = full> struct
 add_barrier : _::barrier<int_, barrier_, has_add | has_subtract | _::get_add_exists>
	{};

template<barrier_t barrier_, typename int_> void add(int_& atomic, if_type<add_exists<int_, barrier_>, int_> add) {
	// atomic += add;
	typedef typename underlying_type_from<int_>::result u_;
	_::add<u_, add_barrier<u_, barrier_>::result>::do_it(
		static_cast<u_*>(static_cast<void*>(&atomic)),
		static_cast<u_>(add)
		);
	}

template<typename int_> void add(int_& atomic, if_type<add_exists<int_, full>, int_> add_) {
	add<full>(atomic, add_);
	}

#ifndef WATER_ATOMIC_INLINE
}}
#endif
#endif
