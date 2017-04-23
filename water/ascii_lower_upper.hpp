// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_ASCII_LOWER_UPPER_HPP
#define WATER_ASCII_LOWER_UPPER_HPP
namespace water {

/*
ASCII
0x09 tab
0x0a line feed
0x0b tab vertical
0x0c form feed
0x0d carriage return
0x20 space
0x21 !
0x22 "
0x23 #
0x24 dollar
0x25 %
0x26 &
0x27 apostrophe
0x28 (
0x29 )
0x2a *
0x2b +
0x2c ,
0x2d -
0x2e .
0x2f /
0x30-0x39 0-9
0x3a :
0x3b ;
0x3c <
0x3d =
0x3e >
0x3f ?
0x40 at (a with elephant trunk)
0x41-0x5a A-Z
0x5b [
0x5c \
0x5d ]
0x5e ^
0x5f _
0x60 '
0x61-0x7a a-z
0x7b {
0x7c |
0x7d }
0x7e ~
0x7f delete
*/

template<typename char_> constexpr bool ascii_is_lower(char_ a) {
	return static_cast<char_>(0x61) <= a && a <= static_cast<char_>(0x7a);
	}

template<typename char_> constexpr bool ascii_is_upper(char_ a) {
	return static_cast<char_>(0x41) <= a && a <= static_cast<char_>(0x5a);
	}

template<typename char_> constexpr char_ ascii_to_lower(char_ a) {
	return ascii_is_upper(a) ? a + 0x20 : a;
	}

template<typename char_> constexpr char_ ascii_to_upper(char_ a) {
	return ascii_is_lower(a) ? a - 0x20 : a;
	}

}
#endif
