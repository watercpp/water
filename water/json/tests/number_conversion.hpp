// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_JSON_TESTS_NUMBER_CONVERSION_HPP
#define WATER_JSON_TESTS_NUMBER_CONVERSION_HPP
#include <water/json/tests/bits.hpp>
namespace water { namespace json { namespace tests {

/*

test conversion to/from json::number int64_t double

*/

inline bool almost_equal(double a, double b) {
    double const e = 1. + numeric_limits<double>::epsilon();
    return a < 0 ?
        (a / e >= b && b >= a * e) :
        (a / e <= b && b <= a * e);
}


struct number_double_t {
    int64_t integer, exponent;
};

inline number_double_t number_round(number a, int64_t ten /*100 will round to 3 digis*/) {
    number_double_t r {a.integer(), a.exponent()};
    if(!r.integer)
        return r;
    if(a.integer() < 0)
        r.integer = -r.integer;
    while(r.integer < ten) {
        r.integer *= 10;
        --r.exponent;
    }
    while(r.integer >= ten * 100) {
        r.integer /= 10;
        ++r.exponent;
    }
    if(r.integer >= ten * 10) {
        r.integer = r.integer / 10 + (r.integer % 10) / 5;
        ++r.exponent;
    }
    if(a.integer() < 0)
        r.integer = -r.integer;
    return r;
}

inline void number_cast() {
    number n = 1;
    if(n)
        n = 2;
    n = 1l;
    n = 1ll;
    n = 1.f;
    n = 1.l;
    n = 'a';
    n = 1u;
    n = 1ull;
    auto x =
        static_cast<int>(n) +
        static_cast<unsigned>(n) +
        static_cast<float>(n) +
        static_cast<long double>(n) +
        static_cast<char>(n) +
        static_cast<unsigned char>(n);
    if(x)
        n = 3;
}

inline void number_conversion() {
    
    number_cast();
    
    // any comparation to a.integer + a.exponent is unsafe
    number n = 1.2345;
    double d = static_cast<double>(n);
    int64_t i = static_cast<int64_t>(n);
    auto r = number_round(n, 10000);
    ___water_test(r.integer == 12345 && r.exponent == -4 && !n.imprecise() && i == 1);
    ___water_test(almost_equal(d, 1.2345));
    
    n = -1.2345;
    d = static_cast<double>(n);
    r = number_round(n, 10000);
    i = static_cast<int64_t>(n);
    ___water_test(r.integer == -12345 && r.exponent == -4 && !n.imprecise() && i == -1);
    ___water_test(almost_equal(d, -1.2345));
    
    n = 1.2345e123;
    d = static_cast<double>(n);
    r = number_round(n, 10000);
    i = static_cast<int64_t>(n);
    ___water_test(r.integer == 12345 && r.exponent == (123-4) && !n.imprecise() && !i);
    ___water_test(almost_equal(d, 1.2345e123));
    
    n = -1.2345e123;
    d = static_cast<double>(n);
    r = number_round(n, 10000);
    i = static_cast<int64_t>(n);
    ___water_test(r.integer == -12345 && r.exponent == (123-4) && !n.imprecise() && !i);
    ___water_test(almost_equal(d, -1.2345e123));
    
    
    n = 1.5;
    d = static_cast<double>(n);
    r = number_round(n, 10000);
    i = static_cast<int64_t>(n);
    ___water_test(r.integer == 15000 && r.exponent == -4 && !n.imprecise() && i == 2);
    ___water_test(almost_equal(d, 1.5));
    
    n = -1.5;
    d = static_cast<double>(n);
    r = number_round(n, 10000);
    i = static_cast<int64_t>(n);
    ___water_test(r.integer == -15000 && r.exponent == -4 && !n.imprecise() && i == -2);
    ___water_test(almost_equal(d, -1.5));
    
    
    n = 1.2345e-123;
    d = static_cast<double>(n);
    r = number_round(n, 10000);
    i = static_cast<int64_t>(n);
    ___water_test(r.integer == 12345 && r.exponent == (-123-4) && !n.imprecise() && !i);
    ___water_test(almost_equal(d, 1.2345e-123));
    
    double l = numeric_limits<double>::max();
    n = l;
    d = static_cast<double>(n);
    i = static_cast<int64_t>(n);
    ___water_test(almost_equal(d, l) && !i);
    
    l = -numeric_limits<double>::max();
    n = l;
    d = static_cast<double>(n);
    i = static_cast<int64_t>(n);
    ___water_test(almost_equal(d, l) && !i);
    
    l = numeric_limits<double>::min();
    n = l;
    d = static_cast<double>(n);
    i = static_cast<int64_t>(n);
    ___water_test(almost_equal(d, l) && !i);
    
    l = numeric_limits<double>::epsilon();
    n = l;
    d = static_cast<double>(n);
    i = static_cast<int64_t>(n);
    ___water_test(almost_equal(d, l) && !i);
    
    l = numeric_limits<double>::denorm_min();
    n = l;
    d = static_cast<double>(n); // can be converted to 0 because subnormal
    i = static_cast<int64_t>(n);
    ___water_test(d <= l && n.exponent() != 0 && n.integer() != 0 && !i);
    
    l = numeric_limits<double>::quiet_NaN();
    n = l;
    d = static_cast<double>(n);
    i = static_cast<int64_t>(n);
    ___water_test(d == 0 && n.imprecise() && !n.exponent() && !n.integer() && !i);
    
    l = numeric_limits<double>::infinity();
    n = l;
    d = static_cast<double>(n);
    i = static_cast<int64_t>(n);
    ___water_test(d == 0 && n.imprecise() && !n.exponent() && !n.integer() && !i);
    
    n = {numeric_limits<int64_t>::max() / 2, numeric_limits<int32_t>::max() / 2};
    d = static_cast<double>(n);
    i = static_cast<int64_t>(n);
    ___water_test(d == 0 && i == 0);
    
    n = {numeric_limits<int64_t>::max() / -2, numeric_limits<int32_t>::max() / -2};
    d = static_cast<double>(n);
    i = static_cast<int64_t>(n);
    ___water_test(d == 0 && i == 0);
}

}}}
#endif
