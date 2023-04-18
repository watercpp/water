// Copyright 2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_UNICODE_TESTS_TEMPLATE_DEDUCTION_HPP
#define WATER_UNICODE_TESTS_TEMPLATE_DEDUCTION_HPP
#include <water/unicode/unicode.hpp>
#include <water/downgrade_iterators/downgrade_iterators.hpp>
#include <water/test.hpp>
namespace water { namespace unicode { namespace tests {

/*

test template argument deduction

*/

template<typename char_>
void template_deduction_for_char() {
    #ifdef __cpp_deduction_guides

    char_ const c[2] = {'a', 'a'};

    auto verify = [](char32_t) { return true; };

    using expect = utf_length<utf_from_char<char_>>;

    utf_length l1{c, 2};
    utf_length l2{c, c + 2};
    utf_length l3{c, 2, verify};
    utf_length l4{c, c + 2, verify};

    utf_length l5{downgrade_iterators::forward_from(c), 2};
    utf_length l6{downgrade_iterators::forward_from(c), downgrade_iterators::forward_from(c + 2)};
    utf_length l7{downgrade_iterators::forward_from(c), 2, verify};
    utf_length l8{downgrade_iterators::forward_from(c), downgrade_iterators::forward_from(c + 2), verify};

    static_assert(equal<expect, decltype(l1)>, "");
    static_assert(equal<expect, decltype(l2)>, "");
    static_assert(equal<expect, decltype(l3)>, "");
    static_assert(equal<expect, decltype(l4)>, "");
    static_assert(equal<expect, decltype(l5)>, "");
    static_assert(equal<expect, decltype(l6)>, "");
    static_assert(equal<expect, decltype(l7)>, "");
    static_assert(equal<expect, decltype(l8)>, "");

    #endif
}

inline void template_deduction() {
    #ifdef __cpp_deduction_guides
    
    template_deduction_for_char<char>();
    template_deduction_for_char<unsigned char>();
    template_deduction_for_char<char16_t>();
    template_deduction_for_char<char32_t>();

    #endif
}


}}}
#endif
