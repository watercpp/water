// Copyright 2017-2018 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_TEMPORARY_TESTS_ALL_HPP
#define WATER_TEMPORARY_TESTS_ALL_HPP
#include <water/temporary/tests/allocator.hpp>
#include <water/temporary/tests/block.hpp>
#include <water/temporary/tests/vector.hpp>
namespace water { namespace temporary { namespace tests {

inline void all() {
	allocator_all();
	block_all();
	vector_all();
	}

}}}
#endif
