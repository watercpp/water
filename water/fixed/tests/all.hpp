// Copyright 2018 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_FIXED_TESTS_ALL_HPP
#define WATER_FIXED_TESTS_ALL_HPP
#include <water/fixed/tests/lookup.hpp>
#include <water/fixed/tests/memory_functions.hpp>
namespace water { namespace fixed { namespace tests {

inline void all() {
    lookup();
    memory_functions();
}

}}}
#endif
