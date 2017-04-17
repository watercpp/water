// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_NUMBERS_PARSED_HPP
#define WATER_NUMBERS_PARSED_HPP
#include <water/numbers/bits.hpp>
namespace water { namespace numbers {

/*

intermediate format of a number parsed from a string but not yet converted to a numeric type

this will remove leading zeros, if they are before the decimal point
0.123 will have point_at() 1, leading_zeros() 1 and digits() 123
0.0123 will have point_at() 1 and leading_zeros() 2 and digits() 123

all unsigned numbers are limited to int max. it is safe to cast to int

*/

template<unsigned max_digits_> class
 parsed {
 	static unsigned constexpr max = static_cast<unsigned>(numeric_limits<int>::max());
 	public:
 		static unsigned constexpr max_digits = max_digits_;
 	private:
		unsigned char my[max_digits];
		unsigned myleading0 = 0;
		unsigned mydigits = 0; // number of digits read after myleading0. can be more than max_digits
		int mypoint_at = -1; // 0 is ok number can be .123
		unsigned mybase = 0;
		bool myminus = false;
		bool mynan = false;
		bool myinfinity = false;
		bool myoverflow = false; // if there was more digits than max_digits and they were not 0
	public:
		void digit(unsigned a) {
			if(mydigits || a) {
				if(mypoint_at == -1) // drop leading 0 if no point yet
					myleading0 = 0;
				if(mydigits < max_digits)
					my[mydigits] = static_cast<unsigned char>(a);
				else if(a)
					myoverflow = true;
				if(mydigits != max)
					++mydigits;
				}
			else if(mypoint_at != -1) {
				if(myleading0 != max)
					++myleading0;
				}
			else if(!myleading0)
				myleading0 = 1;
			}
		void point() {
			mypoint_at = static_cast<int>(total_digits()); // limited to max
			}
		void base(unsigned base) {
			mybase = base;
			}
		void minus() {
			myminus = true;
			}
		void nan() {
			mynan = true;
			}
		void infinity() {
			myinfinity = true;
			}
		void clear() {
			myleading0 = 0;
			mydigits = 0;
			mybase = 0;
			mypoint_at = -1;
			myminus = false;
			mynan = false;
			myinfinity = false;
			myoverflow = false;
			}
	public:
		bool error() const {
			// too many digits
			return total_digits() == max;
			}
		bool anything() const {
			// return true if anything was found
			return myleading0 || mydigits || mynan || myinfinity;
			}
		unsigned leading_zeros() const {
			// zeros after decimal point, before non-zero digits(). 3 for 0.0001
			return myleading0;
			}
		begin_end<unsigned char const*> digits() const {
			// the first is always != 0. if this is not empty, at least 1 non-zero digit was read
			return {my, my + digits_size()};
			}
		unsigned digits_size() const {
			return mydigits < max_digits ? mydigits : max_digits;
			}
		unsigned total_digits() const {
			// total number digits, leading zeros and digits beyond max_digits 
			if(max - myleading0 < mydigits)
				return max;
			return myleading0 + mydigits;
			}
		bool overflow() const {
			// true when more non-0 digits than max_digits was read
			return myoverflow;
			}
		unsigned base() const {
			return mybase;
			}
		unsigned point_at() const {
			// 0 <= return <= total_digits()
			return mypoint_at < 0 ? total_digits() : static_cast<unsigned>(mypoint_at);
			}
		bool is_minus() const {
			return myminus;
			}
		bool is_nan() const {
			return mynan;
			}
		bool is_infinity() const {
			return myinfinity;
			}
	};

}}
#endif
