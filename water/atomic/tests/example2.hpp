// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_ATOMIC_TESTS_EXAMPLE2_HPP
#define WATER_ATOMIC_TESTS_EXAMPLE2_HPP
#include <water/atomic/tests/bits.hpp>
#include <water/atomic/tests/namespace/outside.hpp>
#include <water/atomic/examples/example2.hpp>
namespace water_atomic { namespace example2 {
	namespace underlying = water::atomic::example2;
	#include <water/atomic/tests/namespace/inside.hpp>
	namespace tests {
		#define WATER_ATOMIC_INLINE
		#include <water/atomic/tests/test.hpp>
		#include <water/atomic/tests/int.hpp>
		#undef WATER_ATOMIC_INLINE
		}
}}
namespace water { namespace atomic { namespace tests {

	inline void example2_all() {
		water_atomic::example2::tests::test();
		}

}}}
#endif
