// Copyright 2018 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_UNICODE_TESTS_ADJUST_END_HPP
#define WATER_UNICODE_TESTS_ADJUST_END_HPP
#include <water/unicode/unicode.hpp>
#include <water/test.hpp>
namespace water { namespace unicode { namespace tests {

template<typename char_> void adjust_end_test(char32_t codepoint) {
	unsigned constexpr
		utf = utf_from_char<char_>::result,
		size = 8 * 3; // multiple of 1,2,3,4
	char_ array[size];
	unsigned n = utf_encode<utf>(array, codepoint);
	___water_test(1 <= n && n <= 4);
	unsigned at = n;
	do array[at] = array[at % n]; while(++at != size);
	do {
		auto end = utf_adjust_end<utf>(array, array + at);
		unsigned adjusted = static_cast<unsigned>(end - array);
		___water_test(!(adjusted % n));
		___water_test(at < adjusted + n);
		auto verify = utf_length<utf>(array, adjusted);
		___water_test(verify);
		} while(--at);
	}

inline void adjust_end() {
	char32_t codepoints[] {
		'a',
		0xabc,
		0xabcd,
		0x10abcd
		};
	for(auto codepoint : codepoints) {
		adjust_end_test<char>(codepoint);
		adjust_end_test<char16_t>(codepoint);
		adjust_end_test<char32_t>(codepoint);
		}
	}

}}}
#endif
