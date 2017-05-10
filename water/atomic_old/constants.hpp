// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_ATOMIC_OLD_HAS_HPP
#define WATER_ATOMIC_OLD_HAS_HPP
#include <water/int.hpp>
namespace water { namespace atomic_old {

// when writing a new underlying implementation, just..
//   using namespace water::atomic_old::constants;
// ...to bring the types and constants in this file into another namespace

enum barrier_t {
	none,
	acquire,
	release,
	full
	};

namespace constants {

	typedef water::uint_bits_at_least<17> has_t;

	has_t const
		has_get = static_cast<has_t>(1) << 0,
		has_set = static_cast<has_t>(1) << 1,
		has_get_set = static_cast<has_t>(1) << 2,
		has_compare_set = static_cast<has_t>(1) << 3,
		has_get_compare_set = static_cast<has_t>(1) << 4,
		has_add = static_cast<has_t>(1) << 5,
		has_add_get = static_cast<has_t>(1) << 6,
		has_get_add = static_cast<has_t>(1) << 7,
		has_add1 = static_cast<has_t>(1) << 8,
		has_add1_get = static_cast<has_t>(1) << 9,
		has_get_add1 = static_cast<has_t>(1) << 10,
		has_subtract = static_cast<has_t>(1) << 11,
		has_subtract_get = static_cast<has_t>(1) << 12,
		has_get_subtract = static_cast<has_t>(1) << 13,
		has_subtract1 = static_cast<has_t>(1) << 14,
		has_subtract1_get = static_cast<has_t>(1) << 15,
		has_get_subtract1 = static_cast<has_t>(1) << 16,
		has_everything = (has_get_subtract1 - 1) | has_get_subtract1;
	
	}

using namespace constants;
	
namespace constants {
	using atomic_old::barrier_t;
	using atomic_old::none;
	using atomic_old::acquire;
	using atomic_old::release;
	using atomic_old::full;
	}

}}
#endif
