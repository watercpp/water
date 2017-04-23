// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_MINISTR_TESTS_UTF_HPP
#define WATER_MINISTR_TESTS_UTF_HPP
#include <water/ministr/tests/bits.hpp>
#include <water/ministr/utf.hpp>
#include <water/trace.hpp>
namespace water { namespace ministr { namespace tests {

char constexpr
	utf8_korean[] = u8"작은 문자열",
	utf8_arabic[] = u8"سلسلة صغيرة";

char16_t constexpr
	utf16_korean[] = u"작은 문자열",
	utf16_arabic[] = u"سلسلة صغيرة";

char32_t constexpr
	utf32_korean[] = U"작은 문자열",
	utf32_arabic[] = U"سلسلة صغيرة";

wchar_t constexpr whcar_english[] = L"small string";
	
template<typename char_> char_ const * utf_pointer(char_ const *a) { return a; }

inline void utf() {
	auto o = out_line_function([](char const* b, char const*) { trace(b); });
	
	o << "utf-8  array " << utf8_korean;
	o << "utf-8  array " << utf8_arabic;
	o << "utf-16 array " << utf16_korean;
	o << "utf-16 array " << utf16_arabic;
	o << "utf-32 array " << utf32_korean;
	o << "utf-32 array " << utf32_arabic;
	
	o << "utf-8  pointer " << utf_pointer(utf8_korean );
	o << "utf-8  pointer " << utf_pointer(utf8_arabic );
	o << "utf-16 pointer " << utf_pointer(utf16_korean);
	o << "utf-16 pointer " << utf_pointer(utf16_arabic);
	o << "utf-32 pointer " << utf_pointer(utf32_korean);
	o << "utf-32 pointer " << utf_pointer(utf32_arabic);
	
	o << "utf-8  range " << unicode::utf_range_from_cstring<8 >(utf8_korean );
	o << "utf-8  range " << unicode::utf_range_from_cstring<8 >(utf8_arabic );
	//___breakpoint();
	o << "utf-16 range " << unicode::utf_range_from_cstring<16>(utf16_korean);
	o << "utf-16 range " << unicode::utf_range_from_cstring<16>(utf16_arabic);
	o << "utf-32 range " << unicode::utf_range_from_cstring<32>(utf32_korean);
	o << "utf-32 range " << unicode::utf_range_from_cstring<32>(utf32_arabic);
	
	o << "char16 A " << u'A';
	o << "char32 A " << U'A';
	o << "wchar  A " << L'A';
	
	char16_t c16 = 0xc5f4; // u'열' visual c++ complains with plain unicode
	char32_t c32 = 0x1f640; // U'🙀'
	o << "char16 korean char " << c16 << base<16> << " codepoint " << static_cast<unsigned>(c16);
	o << "char32 emoji char  " << c32 << base<16> << " codepoint " << static_cast<unsigned>(c32); // emojis are over 0xffff
	
	o << "utf8  bytes " << bytes(utf8_korean);
	o << "utf8  bytes " << bytes(utf8_arabic, sizeof(utf8_arabic));
	o << "utf16 bytes " << bytes(utf16_korean);
	o << "utf16 bytes " << bytes<sizeof(utf16_arabic)>(utf16_arabic);
	o << "utf32 bytes " << bytes(utf32_korean);
	o << "utf32 bytes " << bytes<sizeof(utf32_korean)>(utf16_arabic, sizeof(utf32_korean));
	}

}}}
#endif
