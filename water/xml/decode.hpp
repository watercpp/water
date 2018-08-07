// Copyright 2017-2018 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_XML_DECODE_HPP
#define WATER_XML_DECODE_HPP
#include <water/xml/bits.hpp>
namespace water { namespace xml {

namespace _ {
	template<unsigned utf_, typename char_> typename types::ifel<utf_ == 8, bool>::result
	 decode_is_0x85(char_ *f, char_ *end) {	
		return f[0] == 0xc2 && f + 1 < end && f[1] == 0x85;
		}
	template<unsigned utf_, typename char_> typename types::ifel<utf_ != 8, bool>::result
	 decode_is_0x85(char_ *f, char_ *) {	
		return *f == 0x85;
		}

	template<unsigned utf_, typename char_> typename types::ifel<utf_ == 8, bool>::result
	 decode_is_0x2028(char_ *f, char_ *end) {	
		return f[0] == 0xe2 && f + 2 < end && f[1] == 0x80 && f[2] == 0xa8;
		}
	template<unsigned utf_, typename char_> typename types::ifel<utf_ != 8, bool>::result
	 decode_is_0x2028(char_ *f, char_ *) {	
		return *f == 0x2028;
		}
	}

template<typename char_> char_* decode(char_ *begin, char_ *end) {
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
	unsigned constexpr utf = unicode::utf_from_char<char_>::result;
	char_
		*f = begin,
		*t = f;
	unsigned spaces = 0; // used to drop trailing spaces
	while(f != end) {
		if(*f == 0xd || _::decode_is_0x85<utf>(f, end)) {
			unsigned n = *f == 0xc2 ? 2 : 1;
			if(f + n != end && f[n] == 0xa)
				++f;
			f += n;
			if(t != begin) {
				*t++ = 0xa;
				++spaces;
				}
			}
		else if(_::decode_is_0x2028<utf>(f, end)) {
			f += utf == 8 ? 3 : 1;
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
							t += unicode::utf_encode<utf>(t, static_cast<char32_t>(u)); // this is safe because shortest &#1; is 4 chars
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

namespace _ {
	template<unsigned utf_, typename char_> typename types::ifel<utf_ == 8, unsigned>::result
	 decode_attribute_is_space(char_ *f, char_ *end) {	
		if(*f <= 0x20)
			return 1;
		if(f[0] == 0xc2 && f + 1 < end && f[1] == 0x85)
			return 2;
		if(f[0] == 0xe2 && f + 2 < end && f[1] == 0x80 && f[2] == 0xa8)
			return 3;
		return 0;
		}
	template<unsigned utf_, typename char_> typename types::ifel<utf_ != 8, unsigned>::result
	 decode_attribute_is_space(char_ *f, char_ *) {	
		if(*f <= 0x20 || *f == 0x85 || *f == 0x2028)
			return 1;
		return 0;
		}
	}
	
template<typename char_> char_* decode_attribute(char_ *begin, char_ *end) {
	// decode() first, then replace all spaces with 1 space
	end = decode(begin, end);
	char_
		*f = begin,
		*t = begin;
	bool space = false;
	while(f != end) {
		if(unsigned n = _::decode_attribute_is_space<unicode::utf_from_char<char_>::result>(f, end)) {
			space = true;
			f += n;
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

}}
#endif
