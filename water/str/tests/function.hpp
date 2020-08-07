// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_STR_TESTS_FUNCTION_HPP
#define WATER_STR_TESTS_FUNCTION_HPP
#include <water/str/tests/bits.hpp>
namespace water { namespace str { namespace tests {

/*

test out_function + function

*/

template<typename char_>
void function_char() {
    char x[] = u8"hello world 1!\nhello world 2!";
    char16_t hello[] = u"hello";
    char_ c[256] {};
    char_
        *ci = c,
        *ce = c + 256;
    // one line, rvalue reference
    out_function([&ci, ce](char_ const* b, char_ const* e) { while(b != e && ci != ce) *ci++ = *b++; }) << (hello + 0) << u' ' << u8"world " << 1 << u'!' << el;
    // then multiline
    auto o = out_function([&ci, ce](char_ const* b, char_ const* e) { while(b != e && ci != ce) *ci++ = *b++; });
    o << (hello + 0) << u' ';
    o << u8"world " << 2 << u'!';
    ___water_test((ci - c) == sizeof(x) - 1);
    unsigned a = 0;
    while(a != sizeof(x) - 1 && c[a] == static_cast<char_>(x[a])) ++a;
    ___water_test(a == sizeof(x) - 1);
}

inline void function_all() {
    function_char<char>();
    function_char<char16_t>();
    function_char<char32_t>();
    function_char<wchar_t>();
}

}}}
#endif
