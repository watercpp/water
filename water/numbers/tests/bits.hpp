// Copyright 2017-2018 Johan Paulsson
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

template<typename to_, typename from_> to_ cast(from_ from) {
	// this is to avoid overflow warnings when casting static_cast<float>(-1.23456789012345678901234567890e50l)
	return static_cast<to_>(from);
	}

}}}
#endif
