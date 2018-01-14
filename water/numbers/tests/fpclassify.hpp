// Copyright 2017-2018 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_NUMBERS_TESTS_FPCLASSIFY_HPP
#define WATER_NUMBERS_TESTS_FPCLASSIFY_HPP
#include <water/numbers/tests/bits.hpp>
namespace water { namespace numbers { namespace tests {

/*

test the fpclassify functions that may not work with some compiler optimization options

*/

template<typename float_> void fpclassify_type() {
	using limits = numeric_limits<float_>;
	float_ min = limits::min(); // avoid underflow warnings below
	___water_test(!isnormal(static_cast<float_>(0)));
	___water_test(isnormal(static_cast<float_>(1)));
	//___water_test(!isnormal(min / static_cast<float_>(2))); // this may not without strict IEEE compliance, like -ffast-math
	___water_test(!isnormal(limits::quiet_NaN()));
	___water_test(!isnormal(limits::infinity()));
	
	___water_test(isfinite(static_cast<float_>(0)));
	___water_test(isfinite(static_cast<float_>(1)));
	___water_test(isfinite(min / static_cast<float_>(2)));
	___water_test(!isfinite(limits::quiet_NaN()));
	___water_test(!isfinite(limits::infinity()));
	
	___water_test(!isinf(static_cast<float_>(0)));
	___water_test(!isinf(static_cast<float_>(1)));
	___water_test(!isinf(min / static_cast<float_>(2)));
	___water_test(!isinf(limits::quiet_NaN()));
	___water_test(isinf(limits::infinity()));

	___water_test(!isnan(static_cast<float_>(0)));
	___water_test(!isnan(static_cast<float_>(1)));
	___water_test(!isnan(min / static_cast<float_>(2)));
	___water_test(isnan(limits::quiet_NaN()));
	___water_test(!isnan(limits::infinity()));
	
	___water_test(!signbit(static_cast<float_>(0)));
	___water_test(!signbit(static_cast<float_>(1)));
	___water_test(!signbit(min / static_cast<float_>(2)));
	___water_test(!signbit(limits::quiet_NaN()));
	___water_test(!signbit(limits::infinity()));
	
	___water_test(signbit(-static_cast<float_>(1)));
	___water_test(signbit(-min / static_cast<float_>(2)));
	___water_test(signbit(-limits::quiet_NaN()));
	___water_test(signbit(-limits::infinity()));

	// test the _strict variants, if these break with -ffast-math there could be trouble

	___water_test(!isnormal_strict(static_cast<float_>(0)));
	___water_test(isnormal_strict(static_cast<float_>(1)));
	___water_test(!isnormal_strict(min / static_cast<float_>(2)));
	___water_test(!isnormal_strict(limits::quiet_NaN()));
	___water_test(!isnormal_strict(limits::infinity()));

	___water_test(isfinite_strict(static_cast<float_>(0)));
	___water_test(isfinite_strict(static_cast<float_>(1)));
	___water_test(isfinite_strict(min / static_cast<float_>(2)));
	___water_test(!isfinite_strict(limits::quiet_NaN()));
	___water_test(!isfinite_strict(limits::infinity()));

	___water_test(!isinf_strict(static_cast<float_>(0)));
	___water_test(!isinf_strict(static_cast<float_>(1)));
	___water_test(!isinf_strict(min / static_cast<float_>(2)));
	___water_test(!isinf_strict(limits::quiet_NaN()));
	___water_test(isinf_strict(limits::infinity()));

	___water_test(!isnan_strict(static_cast<float_>(0)));
	___water_test(!isnan_strict(static_cast<float_>(1)));
	___water_test(!isnan_strict(min / static_cast<float_>(2)));
	___water_test(isnan_strict(limits::quiet_NaN()));
	___water_test(!isnan_strict(limits::infinity()));
	}

inline void fpclassify_all() {
	fpclassify_type<float>();
	fpclassify_type<double>();
	fpclassify_type<long double>();
	}

}}}
#endif
