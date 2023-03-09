// Copyright 2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_XTR_TESTS_ZEROS_HPP
#define WATER_XTR_TESTS_ZEROS_HPP
#include <water/xtr/tests/bits.hpp>
namespace water { namespace xtr { namespace tests {

// test that writing strings stop at the first zero, but single chars can be 0

inline void zeros() {
    char const hello8[] = "hello\0world";
    char16_t const hello16[] = u"hello\0world";
    char32_t const hello32[] = U"hello\0world";
    
    char const e1[] = "hello";
    ___water_test(equal(xtr::no << hello8, e1));
    ___water_test(equal(xtr::no << hello16, e1));
    ___water_test(equal(xtr::no << hello32, e1));
    
    ___water_test(equal(xtr::no << begin_end_from(hello8), e1));
    ___water_test(equal(xtr::no << begin_end_from(hello16), e1));
    ___water_test(equal(xtr::no << begin_end_from(hello32), e1));
    
    char const e2[] = {'1', 0, '3'};
    ___water_test(equal(xtr::no << '1' << char(0) << '3', e2));
    ___water_test(equal(xtr::no << u'1' << char16_t(0) << u'3', e2));
    ___water_test(equal(xtr::no << U'1' << char32_t(0) << U'3', e2));
}

}}}
#endif
