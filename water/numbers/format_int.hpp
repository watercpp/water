// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_NUMBERS_FORMAT_INT_HPP
#define WATER_NUMBERS_FORMAT_INT_HPP
#include <water/numbers/int_iterator.hpp>
#include <water/numbers/formatted.hpp>
#include <water/numbers/settings.hpp>
namespace water { namespace numbers {

/*

123
-123
+123 (show base)
000123 (digits)
0xcafe
cafe (base hidden)
-0xcafe
+fe (show sign, base hidden, 2 digits)

*/

class format_int {
	public:
		static unsigned constexpr digits_max = 10000;
	private:
		unsigned mybase = 10;
		unsigned mydigits = 0;
		bool myplus = false;
		bool mybase_prefix_suffix = true;
		bool mybool_text = false;
	public:
		constexpr format_int() = default;
		format_int(settings a) :
			mybase{a.base() ? a.base() : 10},
			mydigits{a.digits() < digits_max ? a.digits() : digits_max},
			myplus{a.plus()},
			mybase_prefix_suffix{a.base_prefix_suffix()},
			mybool_text{a.bool_text()}
			{}
		format_int& base(unsigned a) {
			___water_assert(a != 1 && a <= 16);
			mybase = 2 <= a && a <= 16 ? a : 10;
			return *this;
			}
		format_int& base_prefix_suffix(bool a) {
			mybase_prefix_suffix = a;
			return *this;
			}
		format_int& digits(unsigned a) {
			mydigits = a < digits_max ? a : digits_max;
			return *this;
			}
		format_int& plus(bool a) {
			// show +
			myplus = a;
			return *this;
			}
		format_int& bool_text(bool a) {
			// ture/false instead of 1/0
			mybool_text = a;
			return *this;
			}
		template<typename int_>
		 formatted<int_iterator<int_>> operator()(int_ i) const {
			int_
				d = i,
				divide = 1; //  never ever 0. i / divide is 0 <= ... < base
			unsigned
				digits = 1,
				max_digits = mydigits ? mydigits : static_cast<unsigned>(-1);
			if(d < 0) {
				divide *= static_cast<int_>(-1); // * -1 not -divide for visual c++ unsigned warnings
				d /= static_cast<int_>(mybase) * -1;
				if(d && max_digits != 1) {
					divide *= static_cast<int_>(mybase);
					++digits;
					}
				}
			while(d >= static_cast<int_>(mybase) && digits != max_digits) {
				d /= static_cast<int_>(mybase);
				divide *= static_cast<int_>(mybase);
				++digits;
				}
			formatted<int_iterator<int_>> r(
				int_iterator<int_>(i, divide, mybase, mydigits > digits ? mydigits - digits : 0),
				mydigits ? mydigits : digits,
				mybase
				);
			r.base_prefix_suffix(mybase_prefix_suffix);
			r.sign(i < 0 ? -1 : myplus ? 1 : 0);
			return r;
			}
		formatted<int_iterator<unsigned>> operator()(bool i) const {
			auto r = operator()(i ? 1u : 0u);
			if(mybool_text)
				r.bool_set(i);
			return r;
			}
	};

}}
#endif
