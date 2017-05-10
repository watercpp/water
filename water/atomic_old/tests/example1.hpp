// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_ATOMIC_OLD_TESTS_EXAMPLE1_HPP
#define WATER_ATOMIC_OLD_TESTS_EXAMPLE1_HPP
#include <water/atomic_old/tests/bits.hpp>
#include <water/atomic_old/tests/namespace/outside.hpp>
#include <water/atomic_old/examples/example1.hpp>
namespace water_atomic { namespace example1 {
	namespace underlying = water::atomic_old::example1;
	#include <water/atomic_old/tests/namespace/inside.hpp>
	namespace tests {
		#define WATER_ATOMIC_OLD_INLINE
		#include <water/atomic_old/tests/test.hpp>
		#include <water/atomic_old/tests/int.hpp>
		#undef WATER_ATOMIC_OLD_INLINE
		}
}}
namespace water { namespace atomic_old { namespace tests {

	inline void example1_all() {
		water_atomic::example1::tests::test();
		}

}}}
#endif
