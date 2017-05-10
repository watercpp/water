// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#if !defined(WATER_ATOMIC_OLD_SUBTRACT_HPP) || defined(WATER_ATOMIC_OLD_INLINE)
#ifndef WATER_ATOMIC_OLD_INLINE
#define WATER_ATOMIC_OLD_SUBTRACT_HPP
#include <water/atomic_old/get_subtract.hpp>
namespace water { namespace atomic_old {
#endif

namespace _ {
	template<
		typename t_,
		barrier_t b_,
		int select_ =
			(underlying::atomic<t_, b_>::has & has_subtract) ? 0 :
			(underlying::atomic<t_, b_>::has & (has_get_subtract | has_subtract_get)) ? has_get_subtract :
			(underlying::atomic<t_, b_>::has & has_add) ? has_add :
			(underlying::atomic<t_, b_>::has & get_subtract_exists) ? has_get_subtract :
			0
		> struct
	 subtract {
		static void do_it(t_* a, t_ b) { underlying::atomic<t_, b_>::subtract(a, b); }
		};
	template<typename t_, barrier_t b_> struct
	 subtract<t_, b_, has_add> {
		static void do_it(t_* a, t_ b) { underlying::atomic<t_, b_>::add(a, static_cast<t_>(0) - b); }
		};
	template<typename t_, barrier_t b_> struct
	 subtract<t_, b_, has_get_subtract> :
	 	get_subtract<t_, b_>
			{};
	}

template<typename int_, barrier_t barrier_ = full> struct
 subtract_exists : _::exists_and_int<int_, barrier_, has_subtract | has_add | _::get_subtract_exists>
	{};
		
template<typename int_, barrier_t barrier_ = full> struct
 subtract_is_real : _::is_real_and_int<int_, barrier_, has_subtract | has_add | _::get_subtract_is_real>
	{};

template<typename int_, barrier_t barrier_ = full> struct
 subtract_barrier : _::barrier<int_, barrier_, has_subtract | has_add | _::get_subtract_exists>
	{};

template<barrier_t barrier_, typename int_> void subtract(int_& atomic, if_type<subtract_exists<int_, barrier_>, int_> subtract_) {
	// atomic -= subtract;
	typedef typename underlying_type_from<int_>::result u_;
	_::subtract<u_, subtract_barrier<u_, barrier_>::result>::do_it(
		static_cast<u_*>(static_cast<void*>(&atomic)),
		static_cast<u_>(subtract_)
		);
	}

template<typename int_> void subtract(int_& atomic, if_type<subtract_exists<int_, full>, int_> subtract_) {
	subtract<full>(atomic, subtract_);
	}

#ifndef WATER_ATOMIC_OLD_INLINE
}}
#endif
#endif
