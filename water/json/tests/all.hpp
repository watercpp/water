// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_JSON_TESTS_ALL_HPP
#define WATER_JSON_TESTS_ALL_HPP
#include <water/json/tests/create.hpp>
#include <water/json/tests/number_conversion.hpp>
#include <water/json/tests/utf.hpp>
namespace water { namespace json { namespace tests {

inline void all() {
    create();
    number_conversion();
    utf();
}

}}}
#endif
