// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_NUMBERS_SPLIT_MANTISSA_EXPONENT_HPP
#define WATER_NUMBERS_SPLIT_MANTISSA_EXPONENT_HPP
#include <water/numbers/bits.hpp>
namespace water { namespace numbers {

// split into mantissa + exponent

template<typename float_> struct
 split_mantissa_exponent {
	bool minus = false;
	bool nan = false;
	bool infinity = false;
	float_ mantissa = 0;
	int exponent = 0;
	split_mantissa_exponent(float_ f, unsigned base) {
		// split to sign, mantissa and exponent
		//
		// if base == 10 and f != 0, 1 <= return.mantissa < 10
		// if base != 10 and f != 0, 1 <= return.mantissa < 2
		//
		// nan is 0, infinity is max, in case the thing writing the number cannot write "nan" or "infinity"
		//
		minus = signbit(f);
		nan = isnan(f);
		if(nan || f == 0)
			return;
		if(minus)
			f = -f;
		if(infinity = isinf(f))
			f = numeric_limits<float_>::max();
		if(base == 10) {
			int p = exponent = static_cast<int>(log10(f));
			// if subnormal, multiply until normal or pow could return 0.
			// is_iec559 || has_denorm becasue has_denorm was wrong on codewarrior many many years ago
			unsigned limit = 1000;
			if(p > 0 && (numeric_limits<float_>::is_iec559 || numeric_limits<float_>::has_denorm > 0) && f < numeric_limits<float_>::min())
				do ++p; while((f *= static_cast<float_>(10.)) < numeric_limits<float_>::min() && --limit);
			if(limit)
				mantissa = f / pow(static_cast<float_>(10.), p);
			else
				mantissa = 0;
			}
		else {
			mantissa = frexp(f, &exponent) * static_cast<float_>(2.); //returns 0.5 <= mantissa < 1
			if(exponent != numeric_limits<int>::min())
				--exponent;
			else
				mantissa = 0;
			}
		float_ const x = static_cast<float_>(base == 10 ? 10. : 2.);
		if(mantissa < static_cast<float_>(1.)) {
			if(exponent != numeric_limits<int>::min()) {
				mantissa *= x;
				--exponent;
				}
			else
				mantissa = 0;
			}
		else if(mantissa >= x) {
			mantissa /= x;
			if(exponent != numeric_limits<int>::max())
				++exponent;
			}
		if(!mantissa)
			exponent = 0;
		}
	};

}}
#endif
