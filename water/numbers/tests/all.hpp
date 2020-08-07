// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_NUMBERS_TESTS_ALL_HPP
#define WATER_NUMBERS_TESTS_ALL_HPP
#include <water/numbers/tests/format.hpp>
#include <water/numbers/tests/fpclassify.hpp>
#include <water/numbers/tests/locale_words.hpp>
#include <water/numbers/tests/max_digits.hpp>
#include <water/numbers/tests/parse.hpp>
#include <water/numbers/tests/write_read.hpp>
#include <water/numbers/tests/write_read_write.hpp>
namespace water { namespace numbers { namespace tests {

inline void all() {
    fpclassify_all();
    format_all();
    locale_words_all();
    parse_all();
    write_read_all();
    write_read_write_all();
    max_digits_all();
}

}}}
#endif
