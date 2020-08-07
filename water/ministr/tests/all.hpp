// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_MINISTR_TESTS_ALL_HPP
#define WATER_MINISTR_TESTS_ALL_HPP
#include <water/ministr/tests/char_pointer.hpp>
#include <water/ministr/tests/test.hpp>
#include <water/ministr/tests/utf.hpp>
namespace water { namespace ministr { namespace tests {

inline void all() {
    char_pointer();
    test();
    utf();
}

}}}
#endif
