// Copyright 2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_XTR_TESTS_CHAR_AND_STRINGS_HPP
#define WATER_XTR_TESTS_CHAR_AND_STRINGS_HPP
#include <water/xtr/tests/bits.hpp>
namespace water { namespace xtr { namespace tests {

// test that writing chars and char-strings work

inline void char_and_strings() {
    
    char constexpr constant[] = "hello world";
    ___water_test(equal(no << constant, constant));
    ___water_test(equal(no << static_cast<char const*>(constant), constant));
    ___water_test(equal(no << cstring(constant), constant));
    
    char array[] = "hello world";
    ___water_test(equal(no << array, array));
    ___water_test(equal(no << static_cast<char*>(array), array));
    ___water_test(equal(no << begin_end_from(array), array));
    
    char8_or_char constexpr constant8[] = u8"hej världen";
    ___water_test(equal(no << constant8, constant8));
    ___water_test(equal(no << static_cast<char8_or_char const*>(constant8), constant8));
    ___water_test(equal(no << cstring(constant8), constant8));
    
    char8_or_char array8[] = u8"hej världen";
    ___water_test(equal(no << array8, array8));
    ___water_test(equal(no << static_cast<char8_or_char*>(array8), array8));
    ___water_test(equal(no << begin_end_from(array8), array8));
    
    ___water_test(equal(no << 'h' << constant[1] << array[2], "hel"));
    
    ___water_test(equal(no << char8_or_char{'h'} << constant8[1] << array8[2], "hej"));
    
    // reserve<123>
    
    char longer[1024];
    for(unsigned a = 0; a != sizeof(longer); ++a)
        longer[a] = '0' + (a % 10);
    longer[sizeof(longer) - 1] = 0;
    ___water_test(equal(no << reserve<1024> << static_cast<char const*>(longer), longer));
    ___water_test(equal(no << reserve<1024> << cstring(longer), longer));
    
    // bytes
    unsigned char constexpr byte_array[] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 0xDE, 0xAD, 0xBE, 0xEF, 0xCA, 0xFE};
    ___water_test(equal(no << bytes(byte_array), "01234567 89ABCDEF DEADBEEF CAFE"));
    ___water_test(equal(no << lowercase << bytes(static_cast<void const*>(byte_array), sizeof(byte_array)), "01234567 89abcdef deadbeef cafe"));
}

}}}
#endif
