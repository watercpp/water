// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_TESTS_VECTORS_VALUE_SIMPLE_HPP
#define WATER_TESTS_VECTORS_VALUE_SIMPLE_HPP
#include <water/tests/vectors/bits.hpp>
namespace water { namespace tests { namespace vectors {

class value_simple {
    int i = 123;
    float f = 456.789f;
    void set(int a) {
        f = static_cast<float>(i = a);
    }
};

}}}
#endif
