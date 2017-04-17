// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_NUMBERS_ROUND_MANTISSA_EXPONENT_HPP
#define WATER_NUMBERS_ROUND_MANTISSA_EXPONENT_HPP
#include <water/numbers/float_iterator.hpp>
namespace water { namespace numbers {

// round floatingpoint

template<typename float_> class
 round_mantissa_exponent {
	unsigned mydigits = 1; // 1 digit, even if float is 0. except when rounding to 0 digits
	bool myup = false;
	public:
		unsigned digits() const {
			return mydigits;
			}
		bool up() const {
			return myup;
			}
		round_mantissa_exponent& operator()(float_& mantissa, int& exponent, unsigned base, unsigned digits) {
			// round to digits digits. return how many digits and if the last digit should be rounded up
			// in the 9.9999 case this will change mantissa = 1 and exponent += 1
			// round to 0 digits has special meaning. if the next digit is >= 5 it will change mantissa = 1 and exponent += 1
			//
			// post
			// - this->digits == 0 only if digits == 0 and no round
			// - this->digits >= 1 in all other cases
			// - exponent is unchanged or +1, or if mantissa <= 0 its 0
			// - mantissa is unchanged or 1, or if mantisssa <= 0 its 0
			___water_assert(base >= 2 && mantissa >= 0);
			mydigits = digits ? 1 : 0;
			myup = false;
			if(mantissa <= 0 || base < 2) {
				mantissa = 0;
				exponent = 0;
				return *this;
				}
			auto i = float_iterator<float_>(mantissa, base);
			unsigned
				nine = base - 1,
				nines = digits && (base != 10 || *i == nine) ? 1 : 0, // if base != 10 the first digit is 1 always
				zeros = 0;
			// can be trailing zeros
			// can be 5.9995 => 6
			// can be 9.9995 => 10
			if(digits) {
				unsigned at = 0;
				while(++at != digits) {
					++i;
					if(*i == nine) {
						++nines;
						zeros = 0;
						}
					else {
						nines = 0;
						if(*i)
							zeros = 0;
						else
							++zeros;
						}
					}
				// at the digit just after
				++i;
				}
			if(*i < base / 2 + (base & 1)) // dont round, 1.234 or 1.2000004
				mydigits = digits - zeros; // 0 if digits was 0
			else if(nines != digits) {
				myup = true;
				mydigits = digits - nines;
				}
			else if(exponent != numeric_limits<int>::max()) { // 9.9995, or round up when digits = 0
				mantissa = 1;
				if(!mydigits) mydigits = 1;
				++exponent;
				}
			else
				mydigits = digits; // 9.9995 but exponent is maxed
			return *this;
			}
	};

}}
#endif
