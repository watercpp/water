// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_ATOMIC_TESTS_OSATOMIC_HPP
#define WATER_ATOMIC_TESTS_OSATOMIC_HPP
#include <water/atomic/tests/bits.hpp>
#include <water/atomic/tests/namespace/outside.hpp>
#ifndef WATER_ATOMIC_OSATOMIC_OSATOMIC_HPP
	#include <water/atomic/osatomic/tests/fake.hpp>
	#include <water/atomic/osatomic/osatomic.hpp>
#endif
namespace water_atomic { namespace osatomic {
	namespace underlying = water::atomic::osatomic;
	#include <water/atomic/tests/namespace/inside.hpp>
	namespace tests {
		#define WATER_ATOMIC_INLINE
		#include <water/atomic/tests/test.hpp>
		#include <water/atomic/tests/int.hpp>
		#undef WATER_ATOMIC_INLINE
		}
}}
namespace water { namespace atomic { namespace tests {

	inline void osatomic_all() {
		water_atomic::osatomic::tests::test();
		}

}}}
#endif
