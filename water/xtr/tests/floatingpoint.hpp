// Copyright 2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_XTR_TESTS_FLOATINGPOINT_HPP
#define WATER_XTR_TESTS_FLOATINGPOINT_HPP
#include <water/xtr/tests/bits.hpp>
#include <water/numbers/numbers.hpp>
namespace water { namespace xtr { namespace tests {

// Hexfloat constants was added in C++17, but this is made to work on C++11

inline double float_from_text(char const* from) {
    auto f = cstring(from);
    numbers::read<double> r;
    r(f.begin(), f.end());
    return r.value();
}

inline void floatingpoint() {
    
    ___water_test(equal(
        no << 1.23456e89 << ' ' << -9.876543e-21 << ' ' << -numeric_limits<double>::infinity() << ' ' << numeric_limits<double>::quiet_NaN(),
        "1.23456E89 -9.876543E-21 -infinity NaN"
    ));
    
    ___water_test(equal(
        no << 1.234e10f << ' ' << -9.876e-5f << ' ' << -numeric_limits<float>::infinity() << ' ' << numeric_limits<float>::quiet_NaN(),
        "1.234E10 -9.876E-5 -infinity NaN"
    ));
    
    // digits
    
    ___water_test(equal(
        no << digits<3> << -9.7654321 << ' ' << 9.999 << ' ' << 123.45678e9,
        "-9.77 10 1.23E11"
    ));
    
    // base
    
    double const
        hex1 = float_from_text("-0x1.cafep123"),
        hex2 = float_from_text("0x1.55aap-99");
    
    ___water_test(equal(
        no << base<16> << hex1 << ' ' << 1.0 << ' ' << -65536.0 << ' ' << hex2,
        "-0x1.CAFEp123 0x1p0 -0x1p16 0x1.55AAp-99"
    ));
    
    ___water_test(equal(
        no << base<8> << hex1 << ' ' << 1.0 << ' ' << -65536.0 << ' ' << hex2,
        "-0o1.62577p123 0o1p0 -0o1p16 0o1.25325p-99"
    ));
    
    ___water_test(equal(
        no << base<2> << hex1 << ' ' << 1.0 << ' ' << -65536.0 << ' ' << hex2,
        "-0b1.110010101111111p123 0b1p0 -0b1p16 0b1.010101011010101p-99"
    ));
    
    // uppercase lowercase show_plus hide_plus
    
    ___water_test(equal(
        no << uppercase << show_plus << 1.23456e89 << ' ' << base<16> << hex1 << ' ' << 1.0 << ' ' << -65536.0 << ' ' << hex2,
        "+1.23456E+89 -0X1.CAFEP+123 +0X1P+0 -0X1P+16 +0X1.55AAP-99"
    ));
    
    ___water_test(equal(
        no << show_plus << hide_plus << lowercase << 1.23456e89 << ' ' << base<16> << hex1 << ' ' << 1.0 << ' ' << -65536.0 << ' ' << hex2,
        "1.23456e89 -0x1.cafep123 0x1p0 -0x1p16 0x1.55aap-99"
    ));
    
    // hide_base
    
    ___water_test(equal(
        no << hide_base << base<16> << hex1 << ' ' << 1.0 << ' ' << -65536.0 << ' ' << hex2,
        "-1.CAFEp123 1p0 -1p16 1.55AAp-99"
    ));
    
    ___water_test(equal(
        no << hide_base << base<8> << hex1 << ' ' << 1.0 << ' ' << -65536.0 << ' ' << hex2,
        "-1.62577p123 1p0 -1p16 1.25325p-99"
    ));
    
    ___water_test(equal(
        no << hide_base <<base<2> << hex1 << ' ' << 1.0 << ' ' << -65536.0 << ' ' << hex2,
        "-1.110010101111111p123 1p0 -1p16 1.010101011010101p-99"
    ));
    
    // no_exponent_min_max
    
    ___water_test(equal(
        no << no_exponent_min_max<-20, 20> << 1e20 << ' ' << 1e21 << ' ' << 1e-20 << ' ' << 1e-21,
        "100000000000000000000 1E21 0.00000000000000000001 1E-21"
    ));
    
    ___water_test(equal(
        no << no_exponent_min_max<-10, 10> << -1.2345e10 << ' ' << -1.2345e11 << ' ' << -1.2345e-10 << ' ' << -1.2345e-11,
        "-12345000000 -1.2345E11 -0.00000000012345 -1.2345E-11"
    ));
    
    ___water_test(equal(
        no << exponent << -1.2345e10 << ' ' << -1.2345e11 << ' ' << -1.2345e-10 << ' ' << -1.2345e-11 << ' ' << 1.0 << ' ' << -1.0,
        "-1.2345E10 -1.2345E11 -1.2345E-10 -1.2345E-11 1E0 -1E0"
    ));
    
    ___water_test(equal(
        no << 1234.5 << ' ' << exponent << 1234.5 << ' ' << exponent_auto << 1234.5,
        "1234.5 1.2345E3 1234.5"
    ));
    
}

}}}
#endif
