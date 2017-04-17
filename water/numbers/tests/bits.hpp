// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_NUMBERS_TESTS_BITS_HPP
#define WATER_NUMBERS_TESTS_BITS_HPP
#include <water/numbers/numbers.hpp>
#include <water/test.hpp>
#include <water/ministr/out.hpp>
#include <water/ministr/float.hpp>
namespace water { namespace numbers { namespace tests {

// long double math function can be a bit messy
bool constexpr test_long_double =
	#ifdef WATER_NUMBERS_TESTS_LONG_DOUBLE
	true;
	#else
	false;
	#endif

}}}
#endif
