// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_XML_TESTS_ALL_HPP
#define WATER_XML_TESTS_ALL_HPP
#include <water/xml/tests/unicode.hpp>
namespace water { namespace xml { namespace tests {

inline void all() {
    unicode_test{}();
}

}}}
#endif
