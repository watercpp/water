// Copyright 2017-2018 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_JSON_READ_NUMBER_HPP
#define WATER_JSON_READ_NUMBER_HPP
#include <water/json/number.hpp>
namespace water { namespace json {

/*
number
-0
-0.123
1.0
cannot have leading 0
decimal point must be preceeded and followed by a digit

exponent
e or E
sign can be +-
can have leading 0

0.0e1234 has to be possible??

how is 0.0000000123 parsed???

this does not preserve negative 0
*/

inline void read_digit(int64_t& integer, bool& imprecise, int32_t& overflow, char8_t digit, bool minus) {
	auto d = static_cast<int64_t>(digit - '0');
	if(!overflow) {
		if(minus) {
			if(integer >= (numeric_limits<int64_t>::min() + d) / 10)
				integer = integer * 10 - d;
			else {
				if(d >= 5 && integer != numeric_limits<int64_t>::min()) // round overflow
					--integer;
				overflow = 1;
				imprecise = d != 0;
				}
			}
		else {
			if(integer <= (numeric_limits<int64_t>::max() - d) / 10)
				integer = integer * 10 + d;
			else {
				if(d >= 5 && integer != numeric_limits<int64_t>::max()) // round overflow
					++integer;
				overflow = 1;
				imprecise = d != 0;
				}
			}
		}
	else if(overflow != numeric_limits<int32_t>::max()) {
		++overflow;
		imprecise = imprecise || d;
		}
	}

template<typename iterator_> bool read_number(number& to, iterator_& from, iterator_ end) {
	to = {};
	int64_t integer = 0;
	int32_t
		exponent = 0,
		overflow = 0,
		point = 0; // exponent for decimalpoint, 1.23 == 123e-2
	bool
		imprecise = false,
		minus = *from == '-';
	if(minus)
		++from;
	auto first_digit = from;
	while(from != end && ('0' <= *from && *from <= '9')) {
		read_digit(integer, imprecise, overflow, static_cast<char8_t>(*from), minus);
		++from;
		}
	if(from == first_digit)
		return false;
	if(from != first_digit + 1 && *first_digit == '0') // no leading 0
		return false;
	if(from != end && *from == '.') {
		auto decimal_point = from++;
		while(from != end && ('0' <= *from && *from <= '9')) {
			read_digit(integer, imprecise, overflow, static_cast<char8_t>(*from), minus);
			if(point != numeric_limits<int32_t>::min())
				--point;
			++from;
			}
		if(from == decimal_point + 1) // digits must follow decimal point
			return false;
		}
	// exponent
	if(from != end && (*from == 'e' || *from == 'E')) {
		bool exponent_minus = false;
		if(++from != end && ((exponent_minus = *from == '-') != 0 || *from == '+'))
			++from;
		first_digit = from;
		while(from != end && ('0' <= *from && *from <= '9')) {
			int32_t d = static_cast<int32_t>(*from - '0');
			if(exponent_minus) {
				if(exponent < (numeric_limits<int32_t>::min() + d) / 10)
					exponent = numeric_limits<int32_t>::min();
				else
					exponent = exponent * 10 - d;
				}
			else {
				if(exponent > (numeric_limits<int32_t>::max() - d) / 10)
					exponent = numeric_limits<int32_t>::max();
				else
					exponent = exponent * 10 + d;
				}
			++from;
			}
		if(from == first_digit)
			return false;
		}
	if(!integer) { // exponent does not matter
		exponent = 0;
		return true;
		}
	bool
		mini = false,
		huge = false;
	if(point == numeric_limits<int32_t>::min() || exponent == numeric_limits<int32_t>::min())
		mini = true;
	else if(overflow == numeric_limits<int32_t>::max() || exponent == numeric_limits<int32_t>::max())
		huge = true;
	else if(overflow += point) { // try to add overflow + point to exponent. overflow is positive and point negative += will work
		if(overflow < 0 && exponent < numeric_limits<int32_t>::min() - overflow)
			mini = true;
		else if(overflow >= 0 && exponent > numeric_limits<int32_t>::max() - overflow)
			huge = true;
		else
			exponent += overflow;
		}
	if(mini)
		to = {0, 0, true};
	else if(huge)
		to = {integer < 0 ? numeric_limits<int64_t>::min() : numeric_limits<int64_t>::max(), numeric_limits<int32_t>::max(), true};
	else
		to = {integer, exponent, imprecise};
	return true;
	}

}}
#endif
