// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
/* 

a header file containing this...

	#include <water/atomic/atomic_ops/atomic_ops.hpp>
	#include <water/atomic/tests/namespace/outside.hpp>
	namespace space {
	namespace underlying = water::atomic::atomic_ops;
	#include <water/atomic/tests/namespace/inside.hpp>
	}

...makes all things avalible in water::atomic avalible in namespace space, using atomic_ops as the
underlying implementation. this could be useful for testing.

	space::int_t a;
	spaces::set<space::release>(a, 0);

*/

#define WATER_ATOMIC_INLINE
#include <water/atomic/bits.hpp>
#include <water/atomic/alias.hpp>
#include <water/atomic/int.hpp>
#include <water/atomic/barrier.hpp>

#include <water/atomic/get.hpp>
#include <water/atomic/get_set.hpp>
#include <water/atomic/set.hpp>

#include <water/atomic/compare_set.hpp>
#include <water/atomic/compare_set_else_get.hpp>
#include <water/atomic/compare_set_else_non_atomic_get.hpp>
#include <water/atomic/get_compare_set.hpp>

#include <water/atomic/get_add.hpp>
#include <water/atomic/add_get.hpp>
#include <water/atomic/add.hpp>

#include <water/atomic/get_add1.hpp>
#include <water/atomic/add1_get.hpp>
#include <water/atomic/add1.hpp>

#include <water/atomic/get_subtract.hpp>
#include <water/atomic/subtract_get.hpp>
#include <water/atomic/subtract.hpp>

#include <water/atomic/get_subtract1.hpp>
#include <water/atomic/subtract1_get.hpp>
#include <water/atomic/subtract1.hpp>
#undef WATER_ATOMIC_INLINE
