// Copyright 2017-2018 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_NUMBERS_TESTS_FORMAT_HPP
#define WATER_NUMBERS_TESTS_FORMAT_HPP
#include <water/numbers/tests/bits.hpp>
namespace water { namespace numbers { namespace tests {

/*

format numbers, and compare the output strings to expected strings

*/

struct format_one {
    numbers::settings settings;
    template<typename type_>
    void operator()(type_ t, char const *c) const {
        char x[4096] = {};
        write<type_>(t, settings)(x, sizeof(x) - 1);
        char const
            *xi = x,
            *ci = c;
        while(*xi && *ci && *xi == *ci) {
            ++xi;
            ++ci;
        }
        ___water_test(!*xi && !*ci);
    }
};

inline void format_all() {
    format_one f;
    
    f(0, "0");
    f(1, "1");
    f(-1, "-1");
    f(100, "100");
    f(-100, "-100");
    f(12345, "12345");
    f(-12345, "-12345");
    
    f(0u, "0");
    f(1u, "1");
    f(100u, "100");
    f(12345u, "12345");
    
    f(0.0, "0");
    f(1.0, "1");
    f(-1.0, "-1");
    f(100.0, "100");
    f(-100.0, "-100");
    f(12345.67, "12345.67");
    f(-12345.67, "-12345.67");
    
    f.settings = settings{}.exponent();
    f(1.2345e100, "1.2345E100");
    f(-1.2345e-100, "-1.2345E-100");
    f(1.2345e101, "1.2345E101");
    f(-1.2345e-101, "-1.2345E-101");
    f.settings.scientific(true);
    f(1.2345e100, "12.345E99");
    f(-1.2345e-100, "-123.45E-102");
    f(1.2345e101, "123.45E99");
    f(-1.2345e-101, "-12.345E-102");
    
    f.settings = settings{}.fraction_digits(2).no_exponent();
    f(123.456, "123.46");
    f(0.001, "0");
    f(0.005, "0.01");
    
    f.settings = settings{}.fraction_digits(0).no_exponent();
    f(123.456, "123");
    f(123.5, "124");
    f(0.49, "0");
    f(0.5, "1");
    f(0.05, "0");
    f(0.00005, "0");
    
    f.settings = settings{}.digits(32).group(true).base(2);
    f(0xfffffffful, "0b11111111 11111111 11111111 11111111");
    f(0xffffffffffffull, "0b11111111 11111111 11111111 11111111");
    f.settings.base(16).digits(10);
    f(0x123456789abcdefull, "0x67 89AB CDEF");
    f(-0x1, "-0x00 0000 0001");
}

}}}
#endif
