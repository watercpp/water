// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_XML_ENCODING_HPP
#define WATER_XML_ENCODING_HPP
#include <water/xml/bits.hpp>
namespace water { namespace xml {

// try to figure out encoding from raw bytes
class encoding {
	bool
		my8 = false,
		my16 = false,
		my32 = false,
		mybig = false;
	unsigned
		mybom = 0,
		mybits = 8;
	char const
		*myname_begin = 0,
		*myname_end = 0;
	public:
		encoding(void const* begin, size_t bytes) {
			// byte order marks
			// 00 00 fe ff (utf 32 big endian)
			// ff fe 00 00 (utf 32 litte endian)
			// fe ff xx xx (utf 16 big endian)
			// ff fe xx xx (utf 16 little endian)
			// ef bb bf    (utf 8)
			//
			// no byte order mark (3c is <)
			// 00 00 00 3c (utf 32 big endian)
			// 3c 00 00 00 (utf 32 litte endian)
			// 00 3c xx xx (utf 16 big endian)
			// 3c 00 xx xx (utf 16 little endian)
			unsigned char const *b = static_cast<unsigned char const*>(begin);
			if(bytes < 4) {
				// 1 character utf16 is nor proper xml, and no room for <?xml encoding
				my8 = true;
				return;
				}
			else if(b[0] == 0xef && b[1] == 0xbb && b[2] == 0xbf) {
				my8 = true;
				mybom = 3;
				}
			else if(((b[0] == 0xfe && b[1] == 0xff) || (!b[0] && b[1] == '<')) && (b[2] || b[3])) {
				my16 = true;
				mybig = true;
				mybom = b[0] == 0xfe ? 2 : 0;
				mybits = 16;
				}
			else if(((b[1] == 0xfe && b[0] == 0xff) || (!b[1] && b[0] == '<')) && (b[2] || b[3])) {
				my16 = true;
				mybom = b[1] == 0xfe ? 2 : 0;
				mybits = 16;
				}
			else if(b[0] == '<' && b[1] == '?' && b[2] == 'x' && b[3] == 'm') {
				// this looks like an 8-bit encoding, look for encoding
				my8 = true; // until it's not true
				if(bytes > 8 && b[4] == 'l' && b[5] <= 0x20) {
					// find end of ?>
					unsigned char const
						*be = b + bytes,
						*x = b + 6;
					while(x != be - 1 && (x[0] != '?' || x[1] != '>')) ++x;
					if(x != be - 1) {
						b += 6;
						be = x;
						bool encoding = false;
						while(b < be - 8) {
							if(b[-1] <= 0x20 && equal("encoding", b, b + 8)) {
								encoding = true;
								b += 8;
								break;
								}
							++b;
							}
						if(encoding && b != be) {
							while(*b <= 0x20 && ++b != be);
							if(b != be && *b == '=') {
								while(++b != be && *b <= 0x20);
								if(b != be && (*b == '\'' || *b == '"')) {
									x = b;
									while(++b != be && *b != *x);
									if(b != be && ++x != b) {
										myname_begin = static_cast<char const*>(static_cast<void const*>(x));
										myname_end = static_cast<char const*>(static_cast<void const*>(b));
										if(
											myname_end - myname_begin != 5 ||
											!(myname_begin[0] == 'u' || myname_begin[0] == 'U') ||
											!(myname_begin[1] == 't' || myname_begin[1] == 'T') ||
											!(myname_begin[2] == 'f' || myname_begin[2] == 'F') ||
											myname_begin[3] != '-' ||
											myname_begin[4] != '8'
											)
												my8 = false;
										}
									}
								}
							}
						}
					}
				}
			else if(b[0] && b[1] && b[2] && b[3]) {
				// 8 bit without <?xml has to be utf-8
				my8 = true;
				}
			else if(((b[3] == 0xff && b[2] == 0xfe) || (b[3] == '<' && !b[2])) && !b[1] && !b[0]) {
				my32 = true;
				mybig = true;
				mybits = 32;
				mybom = b[3] == 0xff ? 4 : 0;
				}
			else if(((b[0] == 0xff && b[1] == 0xfe) || (b[0] == '<' && !b[1])) && !b[2] && !b[3]) {
				my32 = true;
				mybits = 32;
				mybom = b[0] == 0xff ? 4 : 0;
				}
			else
				mybits = 0;
			}
		unsigned bits() const {
			return mybits;
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
		bool big_endian() const {
			return mybig;
			}
		bool litte_endian() const {
			return !mybig;
			}
		unsigned byte_order_mark() const {
			// this many bytes are bom
			return mybom;
			}
		text<char const*> name() const {
			return { myname_begin, myname_end };
			}
	};

}}
#endif
