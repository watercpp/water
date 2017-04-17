// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_XML_THINGS_UTF_HPP
#define WATER_XML_THINGS_UTF_HPP
#include <water/xml/bits.hpp>
namespace water { namespace xml { namespace things {

template<typename char_, unsigned = static_cast<char_>(0xffffu) == 0xffffu ? 16 : 8> struct
 utf {
	constexpr static bool utf8 = true;
	constexpr static bool utf16 = false;
	using char_type = char_;
	static char_* write(char_ *to, unsigned long from) {
		if(from <= 0x7f) {
			*to = static_cast<char_>(from);
			return to + 1;
			}
		if(from <= 0x7ff) {
			to[0] = static_cast<char_>(0xc0 | (from >> 6));
			to[1] = static_cast<char_>(0x80 | (from & 0x3f));
			return to + 2;
			}
		if(from <= 0xffff) {
			to[0] = static_cast<char_>(0xe0 | (from >> 12));
			to[1] = static_cast<char_>(0x80 | ((from >> 6) & 0x3f));
			to[2] = static_cast<char_>(0x80 | (from & 0x3f));
			return to + 3;
			}
		to[0] = static_cast<char_>(0xf0 | (from >> 18));
		to[1] = static_cast<char_>(0x80 | ((from >> 12) & 0x3f));
		to[2] = static_cast<char_>(0x80 | ((from >> 6) & 0x3f));
		to[3] = static_cast<char_>(0x80 | (from & 0x3f));
		return to + 4;
		}
	};
template<typename char_> struct
 utf<char_, 16> {
	constexpr static bool utf8 = false;
	constexpr static bool utf16 = true;
	using char_type = char_;
	static char_* write(char_ *to, unsigned long from) {
		if(from <= 0xfffful) {
			*to = static_cast<char_>(from);
			return to + 1;
			}
		from -= 0x10000ul;
		to[0] = static_cast<char_>(0xd800u | ((from >> 10) & 0x3ffu));
		to[1] = static_cast<char_>(0xdc00u | (from & 0x3ffu));
		return to + 2;
		}
	};
template<> struct utf<char, 8> : utf<unsigned char, 8> {};
template<> struct utf<char, 16> : utf<unsigned char, 8> {};
template<> struct utf<unsigned char, 16> : utf<unsigned char, 8> {};
template<> struct utf<signed char, 8> : utf<unsigned char, 8> {};
template<> struct utf<signed char, 16> : utf<unsigned char, 8> {};

unsigned char const utf8_lookup_1[0x100] = {
	// 1  2  3  4  5  6  7  8  9  a  b  c  d  e  f
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 0
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 1
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 2
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 3
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 4
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 5
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 6
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 7
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 8
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 9
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // a
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // b
	0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, // c
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, // d
	3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, // e
	4, 4, 4, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // f
	};

// utf8/16_length will return 0 if not valid utf, or if a 0 char was encoded

template<bool utf8_> size_t
 utf16_length(void const* begin, size_t bytes, bool big_endian) {
	if(!bytes || bytes & 1 || !begin)
		return false;
	unsigned char const
		*b = static_cast<unsigned char const*>(begin),
		*e = b + bytes;
	size_t r = utf8_ ? 0 : bytes / 2;
	do {
		char16_t c = (static_cast<char16_t>(b[!big_endian]) << 8) | b[big_endian];
		if(!c || c > 0xffff)
			return 0;
		b += 2;
		if((c & 0xf800) != 0xd800) {// 1 of 1
			if(utf8_)
				r += 1 + (c > 0x7f) + (c > 0x7ff);
			continue;
			}
		if((c & 0xfc00) != 0xd800 || b == e) // not 1 of 2
			return 0;
		c = (static_cast<char16_t>(b[!big_endian]) << 8) | b[big_endian];
		b += 2;
		if(c < 0xdc00 || 0xdfff < c) // not 2 of 2
			return 0;
		if(utf8_)
			r += 4;
		} while(b != e);
	return r;
	}

template<bool utf16_> size_t
 utf8_length(void const* begin, size_t bytes) {
	if(!bytes || !begin)
		return 0;
	unsigned char const *c = static_cast<unsigned char const*>(begin);
	size_t r = utf16_ ? 0 : bytes;
	do {
		if(!c[0] || c[0] > 0xff /*please optimize away*/)
			return false;
		unsigned n = utf8_lookup_1[c[0]];
		if(!n || n > bytes)
			return 0;
		bool ok = true;
		switch(n) {
			case 2:
				ok = 0x80 <= c[1] && c[1] <= 0xbf;
				break;
			case 3:
				ok = (
					c[0] == 0xe0 ? 0xa0 <= c[1] && c[1] <= 0xbf :
					c[0] == 0xed ? 0x80 <= c[1] && c[1] <= 0x9f :
					0x80 <= c[1] && c[1] <= 0xbf
					) &&
					0x80 <= c[2] && c[2] <= 0xbf;
				break;
			case 4:
				ok = (
					c[0] == 0xf0 ? 0x90 <= c[1] && c[1] <= 0xbf :
					c[0] == 0xf4 ? 0x80 <= c[1] && c[1] <= 0x8f :
					0x80 <= c[1] && c[1] <= 0xbf
					) &&
					0x80 <= c[2] && c[2] <= 0xbf &&
					0x80 <= c[3] && c[3] <= 0xbf;
			}
		if(!ok)
			return 0;
		if(utf16_)
			r += n == 4 ? 2 : 1;
		c += n;
		bytes -= n;
		} while(bytes);
	return r;
	}

template<typename char_> char_*
 utf8_convert(char_ *to, void const* begin, size_t bytes) {
	// use utf8_length<true> first or die
	unsigned char const
		*c = static_cast<unsigned char const*>(begin),
		*ce = c + bytes;
	do {
		switch(utf8_lookup_1[*c]) {
			case 2:
				*to++ =
					(static_cast<char_>(c[0] & 0x1f) << 6) |
					(c[1] & 0x3f);
				c += 2;
				break;
			case 3:
				*to++ =
					(static_cast<char_>(c[0] & 0x0f) << 12) |
					(static_cast<char_>(c[1] & 0x3f) << 6) |
					(c[2] & 0x3f);
				c += 3;
				break;
			case 4: {
				char32_t u =
					(static_cast<char32_t>(c[0] & 0x7) << 18) |
					(static_cast<char32_t>(c[1] & 0x3f) << 12) |
					(static_cast<char32_t>(c[2] & 0x3f) << 6) |
					(c[3] & 0x3f);
				u -= 0x10000;
				*to++ = static_cast<char_>(0xd800 | ((u >> 10) & 0x3ff));
				*to++ = static_cast<char_>(0xdc00 | (u & 0x3ff));
				c += 4;
				break;
				}
			default:
				*to++ = *c++;
			}
		} while(c < ce);
	return to;
	}

template<typename char_> char_*
 utf16_convert(char_ *to, void const* begin, size_t bytes, bool big_endian) {
	// use utf16_length<true> first or die
	unsigned char const
		*b = static_cast<unsigned char const*>(begin),
		*be = b + bytes;
	do {
		char16_t c = (static_cast<char16_t>(b[!big_endian]) << 8) | b[big_endian];
		b += 2;
		if(c <= 0x7f)
			*to++ = static_cast<char_>(c);
		else if(c <= 0x7ff) {
			*to++ = static_cast<char_>(0xc0 | (c >> 6));
			*to++ = static_cast<char_>(0x80 | (c & 0x3f));
			}
		else if((c & 0xf800) != 0xd800) {
			*to++ = static_cast<char_>(0xe0 | (c >> 12));
			*to++ = static_cast<char_>(0x80 | ((c >> 6) & 0x3f));
			*to++ = static_cast<char_>(0x80 | (c & 0x3f));
			}
		else {
			// pack 4 from 2
			char16_t c1 = (static_cast<char16_t>(b[!big_endian]) << 8) | b[big_endian];
			b += 2;
			char32_t u = 0x10000 + ((static_cast<char32_t>(c & 0x3ff) << 10) | (c1 & 0x3ff));
			*to++ = static_cast<char_>(0xf0 | (u >> 18));
			*to++ = static_cast<char_>(0x80 | ((u >> 12) & 0x3f));
			*to++ = static_cast<char_>(0x80 | ((u >> 6) & 0x3f));
			*to++ = static_cast<char_>(0x80 | (u & 0x3f));
			}
		} while(b < be);
	return to;
	}

}}}
#endif
