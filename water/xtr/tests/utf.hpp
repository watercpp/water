// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_XTR_TESTS_UTF_HPP
#define WATER_XTR_TESTS_UTF_HPP
#include <water/xtr/tests/bits.hpp>
namespace water { namespace xtr { namespace tests {

// test that char8_t, char16_t, char32_t, wchar_t and utf-conversion works

inline void utf() {

    char8_or_char constexpr
        utf8_korean[] = u8"작은 문자열",
        utf8_arabic[] = u8"سلسلة صغيرة";

    char16_t constexpr
        utf16_korean[] = u"작은 문자열",
        utf16_arabic[] = u"سلسلة صغيرة";

    char32_t constexpr
        utf32_korean[] = U"작은 문자열",
        utf32_arabic[] = U"سلسلة صغيرة";

    wchar_t constexpr wchar_english[] = L"small string";    
    
    ___water_test(equal(no << utf8_korean, utf8_korean));
    ___water_test(equal(no << utf8_arabic, utf8_arabic));
    ___water_test(equal(no << utf16_korean, utf8_korean));
    ___water_test(equal(no << utf16_arabic, utf8_arabic));
    ___water_test(equal(no << utf32_korean, utf8_korean));
    ___water_test(equal(no << utf32_arabic, utf8_arabic));
    ___water_test(equal(no << wchar_english, wchar_english));
    
    ___water_test(equal(no << (utf8_korean + 0), utf8_korean));
    ___water_test(equal(no << (utf8_arabic + 0), utf8_arabic));
    ___water_test(equal(no << (utf16_korean + 0), utf8_korean));
    ___water_test(equal(no << (utf16_arabic + 0), utf8_arabic));
    ___water_test(equal(no << (utf32_korean + 0), utf8_korean));
    ___water_test(equal(no << (utf32_arabic + 0), utf8_arabic));
    ___water_test(equal(no << (wchar_english + 0), wchar_english));
    
    ___water_test(equal(no << unicode::utf_range_from_cstring<8>(utf8_korean), utf8_korean));
    ___water_test(equal(no << unicode::utf_range_from_cstring<8>(utf8_arabic), utf8_arabic));
    ___water_test(equal(no << unicode::utf_range_from_cstring<16>(utf16_korean), utf8_korean));
    ___water_test(equal(no << unicode::utf_range_from_cstring<16>(utf16_arabic), utf8_arabic));
    ___water_test(equal(no << unicode::utf_range_from_cstring<32>(utf32_korean), utf8_korean));
    ___water_test(equal(no << unicode::utf_range_from_cstring<32>(utf32_arabic), utf8_arabic));
    ___water_test(equal(no << cstring(wchar_english), wchar_english));
    
    ___water_test(equal(no << u'a' << U'b' << L'c', "abc"));
    
    char16_t constexpr c16 = 0xc5f4; // u'열' visual c++ complains with plain unicode
    char32_t constexpr c32 = 0x1f640; // U'🙀'
    
    ___water_test(equal(no << c16, unicode::utf_range_from<8>(&c16, 1)));
    ___water_test(equal(no << c32, unicode::utf_range_from<8>(&c32, 1)));
}

}}}
#endif
