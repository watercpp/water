// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
/* 

a header file containing this...

	#include <water/atomic_old/atomic_ops/atomic_ops.hpp>
	#include <water/atomic_old/tests/namespace/outside.hpp>
	namespace space {
	namespace underlying = water::atomic_old::atomic_ops;
	#include <water/atomic_old/tests/namespace/inside.hpp>
	}

...makes all things avalible in water::atomic_old avalible in namespace space, using atomic_ops as the
underlying implementation. this could be useful for testing.

	space::int_t a;
	spaces::set<space::release>(a, 0);

*/

#define WATER_ATOMIC_OLD_INLINE
#include <water/atomic_old/bits.hpp>
#include <water/atomic_old/alias.hpp>
#include <water/atomic_old/int.hpp>
#include <water/atomic_old/barrier.hpp>

#include <water/atomic_old/get.hpp>
#include <water/atomic_old/get_set.hpp>
#include <water/atomic_old/set.hpp>

#include <water/atomic_old/compare_set.hpp>
#include <water/atomic_old/compare_set_else_get.hpp>
#include <water/atomic_old/compare_set_else_non_atomic_get.hpp>
#include <water/atomic_old/get_compare_set.hpp>

#include <water/atomic_old/get_add.hpp>
#include <water/atomic_old/add_get.hpp>
#include <water/atomic_old/add.hpp>

#include <water/atomic_old/get_add1.hpp>
#include <water/atomic_old/add1_get.hpp>
#include <water/atomic_old/add1.hpp>

#include <water/atomic_old/get_subtract.hpp>
#include <water/atomic_old/subtract_get.hpp>
#include <water/atomic_old/subtract.hpp>

#include <water/atomic_old/get_subtract1.hpp>
#include <water/atomic_old/subtract1_get.hpp>
#include <water/atomic_old/subtract1.hpp>
#undef WATER_ATOMIC_OLD_INLINE
