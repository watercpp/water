// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_ATOMIC_TESTS_SYNC_HPP
#define WATER_ATOMIC_TESTS_SYNC_HPP
#include <water/atomic/tests/bits.hpp>
#include <water/atomic/tests/namespace/outside.hpp>
#ifndef WATER_ATOMIC_SYNC_SYNC_HPP
	#include <water/atomic/sync/tests/fake.hpp>
	#include <water/atomic/sync/sync.hpp>
#endif
namespace water_atomic { namespace sync {
	namespace underlying = water::atomic::sync;
	#include <water/atomic/tests/namespace/inside.hpp>
	namespace tests {
		#define WATER_ATOMIC_INLINE
		#include <water/atomic/tests/test.hpp>
		#include <water/atomic/tests/int.hpp>
		#undef WATER_ATOMIC_INLINE
		}
}}
namespace water { namespace atomic { namespace tests {

	inline void sync_all() {
		water_atomic::sync::tests::test();
		}

}}}
#endif
