// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_ATOMIC_TESTS_ALL_FAKE_HPP
#define WATER_ATOMIC_TESTS_ALL_FAKE_HPP
#include <water/atomic/tests/atomic_ops.hpp>
#include <water/atomic/tests/example1.hpp>
#include <water/atomic/tests/example2.hpp>
#include <water/atomic/tests/example3.hpp>
#include <water/atomic/tests/interlocked_visual_cpp.hpp>
#include <water/atomic/tests/interlocked_windows.hpp>
#include <water/atomic/tests/osatomic.hpp>
#include <water/atomic/tests/sync.hpp>
namespace water { namespace atomic { namespace tests {

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
