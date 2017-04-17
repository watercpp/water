// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#if !defined(WATER_ATOMIC_SUBTRACT1_HPP) || defined(WATER_ATOMIC_INLINE)
#ifndef WATER_ATOMIC_INLINE
#define WATER_ATOMIC_SUBTRACT1_HPP
#include <water/atomic/get_subtract1.hpp>
namespace water { namespace atomic {
#endif

namespace _ {
	template<
		typename t_,
		barrier_t b_,
		int select_ =
			(underlying::atomic<t_, b_>::has & has_subtract1) ? 0 :
			(underlying::atomic<t_, b_>::has & get_subtract1_exists) ? has_get_subtract1 :
			0
		> struct
	 subtract1 {
		static void do_it(t_* a) { underlying::atomic<t_, b_>::subtract1(a); }
		};
	template<typename t_, barrier_t b_> struct
	 subtract1<t_, b_, has_get_subtract1> {
		static void do_it(t_* a) { get_subtract1<t_, b_>::do_it(a); }
		};
	}

template<typename int_, barrier_t barrier_ = full> struct
 subtract1_exists : _::exists_and_int<int_, barrier_, has_subtract1 | _::get_subtract1_exists>
	{};
		
template<typename int_, barrier_t barrier_ = full> struct
 subtract1_is_real : _::is_real_and_int<int_, barrier_, has_subtract1 | _::get_subtract1_is_real>
	{};

template<typename int_, barrier_t barrier_ = full> struct
 subtract1_barrier : _::barrier<int_, barrier_, has_subtract1 | _::get_subtract1_exists>
	{};

template<barrier_t barrier_, typename int_> if_type<subtract1_exists<int_, barrier_>, void> subtract1(int_& atomic) {
	// atomic -= 1;
	typedef typename underlying_type_from<int_>::result u_;
	return _::subtract1<u_, subtract1_barrier<u_, barrier_>::result>::do_it(
		static_cast<u_*>(static_cast<void*>(&atomic))
		);
	}

template<typename int_> if_type<subtract1_exists<int_, full>, void> subtract1(int_& atomic) {
	return subtract1<full>(atomic);
	}

#ifndef WATER_ATOMIC_INLINE
}}
#endif
#endif
