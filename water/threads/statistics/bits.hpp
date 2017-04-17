// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_STATISTICS_BITS_HPP
#define WATER_THREADS_STATISTICS_BITS_HPP
#include <water/threads/bits.hpp>
namespace water { namespace threads { namespace statistics {

using count_t = atomic::uint_t;

bool constexpr exists =
	#ifdef WATER_THREADS_STATISTICS
	atomic::any_exists<count_t>::result;
	#else
	false;
	#endif

template<bool = atomic::any_exists<count_t>::result> struct
 atomic_if {
	static count_t get(count_t& a) noexcept { return atomic::get<atomic::none>(a); }
	static void add(count_t& a) noexcept { atomic::add1<atomic::none>(a); }
	static void set(count_t& a, count_t b) noexcept { atomic::set<atomic::none>(a, b); }
	};
template<> struct
 atomic_if<false> {
	static count_t get(count_t&) noexcept { return 0; }
	static void add(count_t&) noexcept {}
	static void set(count_t&, count_t) noexcept {}
	};

}}}
#endif
