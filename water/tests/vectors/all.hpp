// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_TESTS_VECTORS_ALL_HPP
#define WATER_TESTS_VECTORS_ALL_HPP
#include <water/tests/vectors/access.hpp>
#include <water/tests/vectors/construct.hpp>
#include <water/tests/vectors/modify.hpp>
#include <water/tests/vectors/more.hpp>
#include <water/tests/vectors/move.hpp>
namespace water { namespace tests { namespace vectors {

inline void all() {
	construct();
	access();
	modify();
	more();
	move();
	}

}}}
#endif
