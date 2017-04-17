// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_NUMBERS_FORMATTED_MANTISSA_EXPONENT_HPP
#define WATER_NUMBERS_FORMATTED_MANTISSA_EXPONENT_HPP
#include <water/numbers/formatted.hpp>
#include <water/numbers/int_iterator.hpp>
#include <water/numbers/float_iterator.hpp>
namespace water { namespace numbers {

// a formatted floatingpoint number for writing as a string, returned by format_float

template<typename float_> class
 formatted_mantissa_exponent {
	formatted<float_iterator<float_>>  mymantissa;
	formatted<int_iterator<int>> myexponent;
	public:
		formatted_mantissa_exponent() = default;
		formatted_mantissa_exponent(formatted<float_iterator<float_>> const& mantissa, formatted<int_iterator<int>> const& exponent) :
			mymantissa(mantissa),
			myexponent(exponent)
			{}
		formatted<float_iterator<float_>> const& mantissa() const {
			return mymantissa;
			}
		formatted<int_iterator<int>> const& exponent() const {
			return myexponent;
			}
	};

}}
#endif
