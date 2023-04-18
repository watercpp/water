// Copyright 2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_XTR_TESTS_TEMPLATE_DEDUCTION_HPP
#define WATER_XTR_TESTS_TEMPLATE_DEDUCTION_HPP
#include <water/xtr/tests/bits.hpp>
namespace water { namespace xtr { namespace tests {

// test that char_array template deduction guides work

inline void template_deduction() {

    #ifdef __cpp_deduction_guides

    char const constant[] = "hello world";
    
    auto ca1 = string(no << constant);
    char_array ca2{no << constant};
    auto make_char_array = [](auto&& x) { return char_array{x}; }; // reference constructor
    auto ca3 = make_char_array(no << constant);
    static_assert(ca1.capacity() == ca2.capacity(), "");
    static_assert(ca1.capacity() == ca3.capacity(), "");
    ___water_test(equal(ca1.begin(), ca1.end(), ca2.begin(), ca2.end()));
    ___water_test(equal(ca1.begin(), ca1.end(), ca3.begin(), ca3.end()));

    #endif
}

}}}
#endif
