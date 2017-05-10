// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_ATOMIC_OLD_TESTS_ALL_FAKE_HPP
#define WATER_ATOMIC_OLD_TESTS_ALL_FAKE_HPP
#include <water/atomic_old/tests/atomic_ops.hpp>
#include <water/atomic_old/tests/example1.hpp>
#include <water/atomic_old/tests/example2.hpp>
#include <water/atomic_old/tests/example3.hpp>
#include <water/atomic_old/tests/interlocked_visual_cpp.hpp>
#include <water/atomic_old/tests/interlocked_windows.hpp>
#include <water/atomic_old/tests/osatomic.hpp>
#include <water/atomic_old/tests/sync.hpp>
namespace water { namespace atomic_old { namespace tests {

inline void all_fake() {
	atomic_ops_all();
	example1_all();
	example2_all();
	example3_all();
	interlocked_visual_cpp_all();
	interlocked_windows_all();
	osatomic_all();
	sync_all();
	}

}}}
#endif
