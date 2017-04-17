// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_NUMBERS_TESTS_PARSE_HPP
#define WATER_NUMBERS_TESTS_PARSE_HPP
#include <water/numbers/tests/bits.hpp>
namespace water { namespace numbers { namespace tests {


/*

test parsing of strings, and compare it to raw digits, before its converted to integer/floatingpoint types

*/

inline begin_end<char const*> parse_cstring(char const* a) {
	auto e = a;
	if(e) while(*e) ++e;
	return {a, e};
	}

template<unsigned size_> bool parse_compare(parsed<size_> const& p, char const *c) {
	// o = overflow (must be first)
	// i = infinity (after sign)
	// n = nan (after sign)
	// digits, with point
	// o-n
	// o-123.456
	// 123
	auto x = parse_cstring(c);
	auto
		i = x.begin(),
		e = x.end();
	if(i == e)
		return !p.anything();
	if(*i == 'o') {
		if(!p.overflow()) return false;
		++i;
		}
	else if(p.overflow())
		return false;
	if(i != e && *i == '-') {
		if(!p.is_minus()) return false;
		++i;
		}
	else if(p.is_minus())
		return false;
	// infinity, nan
	if(i != e && (*i == 'i' || *i == 'n')) {
		if(*i == 'i' && !p.is_infinity()) return false;
		if(*i == 'n' && !p.is_nan()) return false;
		++i;
		}
	else if(p.is_infinity() || p.is_nan())
		return false;
	// leading_zeros + digits + point
	unsigned
		at = 0,
		leading_zeros = p.leading_zeros();
	while(leading_zeros) {
		if(i == e)
			return false;
		if(*i == '.') {
			if(p.point_at() != at)
				return false;
			}
		else if(*i != '0')
			return false;
		else {
			--leading_zeros;
			++at;
			}
		++i;
		}
	auto digits = p.digits();
	auto d = digits.begin();
	while(d != digits.end()) {
		if(i == e)
			return false;
		if(*i == '.') {
			if(p.point_at() != at)
				return false;
			}
		else {
			unsigned di = 999;
			if('0' <= *i && *i <= '9')
				di = static_cast<unsigned>(*i - '0');
			else if('a' <= *i && *i <= 'f')
				di = 10 + static_cast<unsigned>(*i - 'a');
			if(di != *d)
				return false;
			++at;
			++d;
			}
		++i;
		}
	return i == e;
	}

inline void parse_mantissa_exponent() {
	struct {
		numbers::parse parse;
		char const *from;
		unsigned base;
		char const *mantissa;
		char const *exponent;
		ptrdiff_t size;
		} array[] {
		{parse{}, "123.45e-6", 10, "123.45", "-6"},
		{parse{}.group(true).space(true), "123 456.789 012 E - 123 456 789", 10, "123456.789012", "-123456789"},
		{parse{}.group(true).space(true), "- 0X 123 ABC.789 def p - 123 456 789", 16, "-123abc.789def", "-123456789"},
		{parse{}.group(true).space(true), "- 123 456.789 p - 123 456 789", 10, "-123456789", 0},
		{parse{}.group(true).space(true).base(16), "- 123 456.789 p - 123 456 789", 16, "-123456789", "-123456789"},
		{parse{}, ".1", 10, "1", 0, 2},
		{parse{}, "1.", 10, "1", 0, 2},
		{parse{}, ".1e2", 10, ".1", "2", 4},
		{parse{}, "1.e2", 10, "1", "2", 4},
		{parse{}, "-.1", 10, "-.1", 0, 3},
		{parse{}, "0x.cafe", 16, ".cafe"},
		{parse{}.group(true).space(true), "Cafe.Dead 4 Beef 2 H P 1 2 3", 16, "cafe.dead4beef2", "123"},
		
		{parse{}, "nAn", 0, "n"},
		{parse{}, "iNfIniTy", 0, "i", 0, 8},
		{parse{}, "+inF", 0, "i"},
		{parse{}, "-nAn", 0, "-n"},
		{parse{}, "-iNfIniTy", 0, "-i", 0, 9},
		{parse{}, "-inF", 0, "-i"}
		
		// test the digit_before_point digit_after_point modes
		// test things that should not parse
		};
	parsed<32> mantissa, exponent;
	unsigned dummy = 0;
	for(auto a : array) {
		auto from = parse_cstring(a.from);
		auto from_end = a.parse.mantissa_exponent(mantissa, exponent, from.begin(), from.end());
		
		___water_test(!a.mantissa || from_end != from.begin());
		___water_test(!a.base || a.base == mantissa.base());
		___water_test(!a.size || (from_end - from.begin()) == a.size);
		___water_test(parse_compare(mantissa, a.mantissa));
		___water_test(parse_compare(exponent, a.exponent));
		++dummy; // to make breakpoint above stay in this scope
		}
	}

inline void parse_number() {
	struct {
		numbers::parse parse;
		char const *from;
		unsigned base;
		char const *number;
		ptrdiff_t size;
		} array[] {
		{parse{}, "123.45e-6", 10, "123.45"},
		{parse{}.group(true).space(true), "123 456.789 012", 10, "123456.789012"},
		{parse{}.group(true).space(true), "- 0X 123 ABC.789 def p - 123 456 789", 16, "-123abc.789def"},
		{parse{}.group(true).space(true), "- 123 456.789 p - 123 456 789", 10, "-123456789", 0},
		{parse{}, ".1", 10, "1", 2},
		{parse{}, "1.", 10, "1", 2},
		{parse{}, ".1e2", 10, ".1", 2},
		{parse{}, "1.e2", 10, "1", 2},
		{parse{}, "-.1", 10, "-.1", 3},
		{parse{}, "+0x.cafe", 16, ".cafe"},
		{parse{}.group(true).space(true), "Cafe.Dead 4 Beef 2 H P 1 2 3", 16, "cafe.dead4beef2"},
		{parse{}.group(true), "0b11111111 11111111 11111111 11111111", 2, "11111111111111111111111111111111"}
		};
	parsed<32> number;
	unsigned dummy = 0;
	for(auto a : array) {
		auto from = parse_cstring(a.from);
		auto from_end = a.parse(number, from.begin(), from.end());
		
		___water_test(!a.number || from_end != from.begin());
		___water_test(!a.base || a.base == number.base());
		___water_test(!a.size || (from_end - from.begin()) == a.size);
		___water_test(parse_compare(number, a.number));
		++dummy; // to make breakpoint above stay in this scope
		}
	}

inline void parse_all() {
	parse_mantissa_exponent();
	parse_number();
	}

}}}
#endif
