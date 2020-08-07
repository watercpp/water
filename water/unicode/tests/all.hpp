// Copyright 2018 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_UNICODE_TESTS_ALL_HPP
#define WATER_UNICODE_TESTS_ALL_HPP
#include <water/unicode/tests/conversion.hpp>
#include <water/unicode/tests/adjust_end.hpp>
namespace water { namespace unicode { namespace tests {

inline void all() {
    conversion{};
    adjust_end();
}

}}}
#endif
