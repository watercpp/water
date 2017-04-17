// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_JSON_ENCODING_HPP
#define WATER_JSON_ENCODING_HPP
#include <water/json/bits.hpp>
namespace water { namespace json {

class encoding {
	bool
		my8 = false,
		my16 = false,
		my32 = false,
		mylittle = false;
	unsigned mybom = 0;
	public:
		encoding(void const *begin, size_t size) {
			// size must be at least 4, unless the whole document is less than 4
			// 
			// byte order marks
			// 00 00 fe ff (utf 32 big endian)
			// ff fe 00 00 (utf 32 litte endian)
			// fe ff xx xx (utf 16 big endian)
			// ff fe xx xx (utf 16 little endian)
			// ef bb bf    (utf 8)
			//
			// minimal json
			// {}
			// []
			// 1
			//
			// all possible first characters are ascii
			// whitespace
			// {
			// [
			// -123
			// 123
			// "string
			// true
			// false
			// null
			auto *b = static_cast<unsigned char const*>(begin);
			if(!b || !size)
				return;
			if(size == 1 || size == 3)
				my8 = true;
			if(size == 2) {
				// just a bom is not a utf16 document
				// must be single digit utf16 or 2 ascii utf8 {} [] "" 12 -1
				bool
					b0 = is_ascii(b[0]),
					b1 = is_ascii(b[1]);
				if(b0 && b1)
					my8 = true;
				else if(b0 || b1) {
					my16 = true;
					mylittle = b0;
					}
				}
			else if(!b[0] && !b[1]) {
				my32 = true;
				mylittle = false;
				mybom = b[2] == 0xfe && b[3] == 0xff ? 4 : 0;
				}
			else if(!b[2] && !b[3]) {
				my32 = true;
				mylittle = true;
				mybom = b[0] == 0xff && b[1] == 0xfe ? 4 : 0;
				}
			else if((b[0] == 0xff && b[1] == 0xfe) || (b[0] == 0xfe && b[1] == 0xff)) {
				my16 = true;
				mylittle = b[0] == 0xff;
				mybom = 2;
				}
			else if(b[0] == 0xef && b[1] == 0xbb && b[2] == 0xbf) {
				my8 = true;
				mybom = 3;
				}
			else if(b[0] && b[1])
				my8 = true;
			else if(b[0])
				my16 = mylittle = true;
			else if(b[1])
				my16 = true;
			}
		explicit operator bool() const {
			return my8 || my16 || my32;
			}
		bool utf8() const {
			return my8;
			}
		bool utf16() const {
			return my16;
			}
		bool utf32() const {
			return my32;
			}
		unsigned byte_order_mark() const {
			return mybom;
			}
		bool little_endian() const {
			return (my16 || my32) && mylittle;
			}
		bool big_endian() const {
			return (my16 || my32) && !mylittle;
			}
	private:
		static bool is_ascii(unsigned char a) {
			return 0 < a && a <= 0x7f;
			}
	};

}}
#endif
