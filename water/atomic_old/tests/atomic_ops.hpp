// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_ATOMIC_OLD_TESTS_ATOMIC_OPS_HPP
#define WATER_ATOMIC_OLD_TESTS_ATOMIC_OPS_HPP
#include <water/atomic_old/tests/bits.hpp>
#include <water/atomic_old/tests/namespace/outside.hpp>
#ifndef WATER_ATOMIC_OLD_ATOMIC_OPS_ATOMIC_OPS_HPP
	#include <water/atomic_old/atomic_ops/tests/fake.hpp>
	#include <water/atomic_old/atomic_ops/atomic_ops.hpp>
#endif
namespace water_atomic { namespace atomic_ops {
	namespace underlying = water::atomic_old::atomic_ops;
	#include <water/atomic_old/tests/namespace/inside.hpp>
	namespace tests {
		#define WATER_ATOMIC_OLD_INLINE
		#include <water/atomic_old/tests/test.hpp>
		#include <water/atomic_old/tests/int.hpp>
		#undef WATER_ATOMIC_OLD_INLINE
		}
}}
namespace water { namespace atomic_old { namespace tests {

	inline void atomic_ops_all() {
		water_atomic::atomic_ops::tests::test();
		}

}}}
#endif
