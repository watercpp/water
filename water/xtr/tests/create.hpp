// Copyright 2024 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_XTR_TESTS_CREATE_HPP
#define WATER_XTR_TESTS_CREATE_HPP
#include <water/xtr/tests/bits.hpp>
namespace water { namespace xtr { namespace tests {

/*

Visual C++ 2022 has a problem with operators that look like the one for create_a below.

Using xtr::expression_return instead of decltype works, like the one for create_b.

Compilation failed for "complex" expression << operators that are templates.
Like the one detecting begin() and end(), used here via begin_end_from.

Reproducing the error is not easy, it happened in some places in a program, but the same expression
somewhere else in the same program worked. This test has code identical to what failed, but it will
probably not fail on Visual C++ 2022 anyway.


*/

struct create_a {};
struct create_b {};

template<typename type_>
auto operator<<(create_a, type_&& a) -> decltype(expression<>{} << a) {
    return expression<>{} << a;
}

template<typename type_>
auto operator<<(create_b, type_&& a) -> expression_return<expression<>, type_&&> {
    return expression<>{} << a;
}

inline void create_test() {

    auto hello = begin_end_from("hello");

    no << hello << "" << 1;
    no << begin_end_from("hello") << "" << 1;

    to_function([](char const*, char const*) {}) << hello << "" << 1;
    to_function([](char const*, char const*) {}) << begin_end_from("hello") << "" << 1;

    char x[128];
    to_begin_end(x) << hello << "" << 1;
    to_begin_end(x) << begin_end_from("hello") << "" << 1;

    // these may or may not work on visual c++ 2022
    //create_a{} << hello << "" << 1;
    //create_a{} << begin_end_from("hello") << "" << 1;
    
    create_b{} << hello << "" << 1;
    create_b{} << begin_end_from("hello") << "" << 1;
}

}}}
#endif
