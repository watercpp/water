// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_XML_THINGS_DECODE_HPP
#define WATER_XML_THINGS_DECODE_HPP
#include <water/xml/things/things.hpp>
#include <water/xml/things/utf.hpp>
namespace water { namespace xml { namespace things {

template<typename char_> char_*
 decode(char_ *begin, char_ *end) {
	// returns new end
	// this will leave any entity that cannot be decoded as it is, including &#0; or non-unicode chars like &#xffff;
	// char_ must be unsigned!
	//
	// linefeeds should become 0xa (line feed)
	// - 0xd (carriage return)
	// - 0xd 0xa
	// - 0x85 (nel, xml 1.1, utf-8 0xc2 0x85)
	// - 0xd 0x85
	// - 0x2028 (unicode, xml 1.1, utf-8 is 0xe2 0x80 0xa8)
	//
	char_
		*f = begin,
		*t = f;
	unsigned spaces = 0; // used to drop trailing spaces
	while(f != end) {
		if(*f == 0xd || (utf<char_>::utf16 && *f == 0x85) || (utf<char_>::utf8 && f[0] == 0xc2 && f + 1 < end && f[1] == 0x85)) {
			unsigned n = *f == 0xc2 ? 2 : 1;
			if(f + n != end && f[n] == 0xa)
				++f;
			f += n;
			if(t != begin) {
				*t++ = 0xa;
				++spaces;
				}
			}
		else if(utf<char_>::utf16 && *f == 0x2028) {
			++f;
			if(t != begin) {
				*t++ = 0xa;
				++spaces;
				}
			}
		else if(utf<char_>::utf8 && f[0] == 0xe2 && f + 2 < end && f[1] == 0x80 && f[2] == 0xa8) {
			f += 3;
			if(t != begin) {
				*t++ = 0xa;
				++spaces;
				}
			}
		else if(*f <= 0x20) {
			// t is not == begin now because of how document is parsed
			// drop anything that is not space, line feed, tab
			if(*f == 0x20 || *f == 0xa || *f == 0x9) {
				*t++ = *f;
				++spaces;
				}
			++f;
			}
		else {
			spaces = 0;
			bool copy = true;
			if(*f == '&') {
				// &lt; <
				// &gt; >
				// &amp; &
				// &apos; '
				// &quot; "
				// &#123; decimal
				// &#xa1b2; hex
				// &lalala; could be entity from <!ENTITY
				auto
					eb = f + 1,
					ee = eb;
				while(ee != end && *ee != ';' && *ee != '&') ++ee;
				if(ee != end && *ee == ';') {
					if(*eb != '#') {
						copy = false;
						if(equal("lt", eb, ee))
							*t = '<';
						else if(equal("gt", eb, ee))
							*t = '>';
						else if(equal("amp", eb, ee))
							*t = '&';
						else if(equal("apos", eb, ee))
							*t = '\'';
						else if(equal("quot", eb, ee))
							*t = '"';
						else
							copy = true;
						if(!copy) {
							++t;
							f = ee; // ++ below
							}
						}
					else {
						unsigned base = 10;
						if(eb[1] == 'x') { // eb[1] must exist because ee is ;
							base = 16;
							++eb; // eb[1] is safe again
							}
						unsigned long u = 0;
						while(++eb != ee) {
							unsigned d = 0;
							if('0' <= *eb && *eb <= '9')
								d = static_cast<unsigned>(*eb - '0');
							else if(base == 16 && 'a' <= *eb && *eb <= 'f')
								d = 10 + static_cast<unsigned>(*eb - 'a');
							else if(base == 16 && 'A' <= *eb && *eb <= 'F')
								d = 10 + static_cast<unsigned>(*eb - 'A');
							else {
								u = 0;
								break;
								}
							if(u >= static_cast<unsigned long>(-1) / base) { // overflow
								u = 0;
								break;
								}
							u = u * base + d;
							}
						if(
							u && u <= 0x10fffful &&
							u != 0xfffful && u != 0xfffeul && // bom
							(u < 0xd800ul || 0xdfff < u) // utf 16 pairs
							)
							{
							copy = false;
							t = utf<char_>::write(t, u); // this is safe because shortest &#1; is 4 chars
							f = ee;
							}
						}
					}
				}
			if(copy)
				*t++ = *f;
			++f;
			}
		}
	return t - spaces;
	}
	
template<typename char_> char_*
 decode_attribute(char_ *begin, char_ *end) {
	// decode() first, then replace all spaces with 1 space
	end = decode(begin, end);
	char_
		*f = begin,
		*t = begin;
	bool space = false;
	while(f != end) {
		if(*f <= 0x20 || (utf<char_>::utf16 && (*f == 0x85 || *f == 0x2028))) {
			space = true;
			++f;
			}
	else if(utf<char_>::utf8 && ((f[0] == 0xe2 && f + 2 < end && f[1] == 0x80 && f[2] == 0xa8) || (f[0] == 0xc2 && f + 1 < end && f[1] == 0x85))) {
			space = true;
			f += f[0] == 0xe2 ? 3 : 2;
			}
		else {
			if(t != begin && space)
				*t++ = 0x20;
			*t++ = *f++;
			space = false;
			}
		}
	return t;
	}

}}}
#endif
