// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_ATOMIC_OLD_TESTS_INTERLOCKED_WINDOWS_HPP
#define WATER_ATOMIC_OLD_TESTS_INTERLOCKED_WINDOWS_HPP
#include <water/atomic_old/tests/bits.hpp>
#include <water/atomic_old/tests/namespace/outside.hpp>
#ifndef WATER_ATOMIC_OLD_INTERLOCKED_WINDOWS_INTERLOCKED_WINDOWS_HPP
	#include <water/atomic_old/interlocked_windows/tests/fake.hpp>
	#include <water/atomic_old/interlocked_windows/interlocked_windows.hpp>
#endif
namespace water_atomic { namespace interlocked_windows {
	namespace underlying = water::atomic_old::interlocked_windows;
	#include <water/atomic_old/tests/namespace/inside.hpp>
	namespace tests {
		#define WATER_ATOMIC_OLD_INLINE
		#include <water/atomic_old/tests/test.hpp>
		#include <water/atomic_old/tests/int.hpp>
		#undef WATER_ATOMIC_OLD_INLINE
		}
}}
namespace water { namespace atomic_old { namespace tests {

	inline void interlocked_windows_all() {
		water_atomic::interlocked_windows::tests::test();
		}

}}}
#endif
