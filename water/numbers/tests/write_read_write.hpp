// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_NUMBERS_TESTS_WRITE_READ_WRITE_HPP
#define WATER_NUMBERS_TESTS_WRITE_READ_WRITE_HPP
#include <water/numbers/tests/bits.hpp>
namespace water { namespace numbers { namespace tests {

/*

1 write number
2 read it
3 write the read number
compare the strings from 1 and 3

*/

template<typename type_>
inline type_ write_read_write_divide_4_maybe(type_ a) {
    bool has_infinity = numeric_limits<type_>::has_infinity; // avoid warning C4127: conditional expression is constant
    if(has_infinity && (a == numeric_limits<type_>::max() || a == static_cast<type_>(numeric_limits<type_>::lowest()))) {
        // when precision is small this causes max to be rounded up, and reading it back will be infinity
        return a / 4;
    }
    return a;
}

inline bool write_read_write_divide_4_maybe(bool a) {  // visual c++ warnings when bool / 4
    return a;
}

template<typename type_>
auto write_read_write_one_denorm_or_0(type_ t) -> ifel<is_float<type_>, bool> {
    return !t || !isnormal_strict(t);
}

template<typename type_>
auto write_read_write_one_denorm_or_0(type_) -> ifel<!is_float<type_>, bool> {
    return false;
}

class write_read_write_one
{
    settings s;

public:
    write_read_write_one(settings s) :
        s{s}
    {}

    template<typename type_>
    void operator()(type_ t) const {
        char
            a[8000] {},
            b[8000] {};
        
        t = write_read_write_divide_4_maybe(t);

        write<type_> w(t, s);
        size_t ws1 = w.template size<char>();
        char *ae = w(a, sizeof(a));
        size_t as = static_cast<size_t>(ae - a);
        ___water_test(as);
        ___water_test(ws1 == as);
        ___water_test(ae < (a + sizeof(a)) && !*ae);
        //trace << a;

        read<type_> r(s);
        char *re = r(a, a + sizeof(a));
        size_t ru = r.used();
        size_t rs = static_cast<size_t>(re - a);
        ___water_test(ru);
        ___water_test(ru == rs);
        ___water_test(ru == as);

        type_ tr = r;

        w = {tr, s};
        size_t ws2 = w.template size<char>();
        char *be = w(b, b + sizeof(b)); // begin + end to test that
        size_t bs = static_cast<size_t>(be - b);
        ___water_test(bs);
        ___water_test(ws2 == bs);
        ___water_test(be < (b + sizeof(b)) && !*be);
        
        // let denormals differ
        bool denormals = write_read_write_one_denorm_or_0(tr) && write_read_write_one_denorm_or_0(t);

        ___water_test((ws1 == ws2 || denormals) && "wrote same number of characters twice");

        bool equal = as == bs;
        if(equal) {
            auto
                ai = a,
                bi = b;
            while(ai != ae && (equal = (*ai == *bi)) == true) {
                ++ai;
                ++bi;
            }
        }
        ___water_test((equal || denormals) && "wrote equal strings twice");
    }
};

template<typename float_>
void write_read_write_float(settings s) {
    float_ const array[] = {
        0,
        1,
        -1,
        10,
        -10,
        cast<float_>(-1.23456789012345678901234567890e-50l),
        cast<float_>(+1.23456789012345678901234567890e-40l),
        cast<float_>(-1.23456789012345678901234567890e-30l),
        cast<float_>(+1.23456789012345678901234567890e-20l),
        cast<float_>(-1.23456789012345678901234567890e-10l),
        cast<float_>(+1.23456789012345678901234567890e-9l),
        cast<float_>(-1.23456789012345678901234567890e-8l),
        cast<float_>(+1.23456789012345678901234567890e-7l),
        cast<float_>(-1.23456789012345678901234567890e-6l),
        cast<float_>(+1.23456789012345678901234567890e-5l),
        cast<float_>(-1.23456789012345678901234567890e-4l),
        cast<float_>(+1.23456789012345678901234567890e-3l),
        cast<float_>(-1.23456789012345678901234567890e-2l),
        cast<float_>(+1.23456789012345678901234567890e-1l),
        cast<float_>(-1.23456789012345678901234567890e0l),
        cast<float_>(+1.23456789012345678901234567890e1l),
        cast<float_>(-1.23456789012345678901234567890e2l),
        cast<float_>(+1.23456789012345678901234567890e3l),
        cast<float_>(-1.23456789012345678901234567890e4l),
        cast<float_>(+1.23456789012345678901234567890e5l),
        cast<float_>(-1.23456789012345678901234567890e6l),
        cast<float_>(+1.23456789012345678901234567890e7l),
        cast<float_>(-1.23456789012345678901234567890e8l),
        cast<float_>(+1.23456789012345678901234567890e9l),
        cast<float_>(-1.23456789012345678901234567890e10l),
        cast<float_>(+1.23456789012345678901234567890e20l),
        cast<float_>(-1.23456789012345678901234567890e30l),
        cast<float_>(+1.23456789012345678901234567890e40l),
        cast<float_>(-1.23456789012345678901234567890e50l),
        //static_cast<float_>(9.99995), // float become 949 with this
        cast<float_>(1.2345e100), // scientific mode test
        cast<float_>(-1.2345e-100), // scientific mode test
        cast<float_>(1.2345e101), // scientific mode test
        cast<float_>(-1.2345e-101), // scientific mode test
        numeric_limits<float_>::min(),
        numeric_limits<float_>::max(),
        numeric_limits<float_>::infinity(),
        numeric_limits<float_>::quiet_NaN(),
        numeric_limits<float_>::epsilon(),
        numeric_limits<float_>::denorm_min(),
        -numeric_limits<float_>::min(),
        -numeric_limits<float_>::max(),
        -numeric_limits<float_>::infinity(),
        -numeric_limits<float_>::quiet_NaN(),
        -numeric_limits<float_>::epsilon(),
        -numeric_limits<float_>::denorm_min()
    };
    for(auto a : array)
        write_read_write_one{s}(a);
}

template<typename int_>
void write_read_write_signed(settings s) {
    int
        i12345 = 12345, // avoid warning "static_cast truncation of constant value" in static cast below
        cafe = 0xcafe,
        dead = -0xdead;
    int_ const array[] = {
        0,
        1,
        -1,
        10,
        -10,
        static_cast<int_>(i12345),
        static_cast<int_>(-i12345),
        static_cast<int_>(cafe),
        static_cast<int_>(-dead),
        numeric_limits<int_>::min(),
        numeric_limits<int_>::max(),
        static_cast<int_>(numeric_limits<int_>::min() + 1),
        static_cast<int_>(numeric_limits<int_>::max() - 1),
        static_cast<int_>(numeric_limits<int_>::min() / 2),
        static_cast<int_>(numeric_limits<int_>::max() / 2),
        static_cast<int_>(numeric_limits<int_>::min() / 3),
        static_cast<int_>(numeric_limits<int_>::max() / 3)
    };
    for(auto a : array)
        write_read_write_one{s}(a);
}

template<typename int_>
void write_read_write_unsigned(settings s) {
    unsigned
        i12345 = 12345, // avoid warning "static_cast truncation of constant value" in static cast below
        cafe = 0xcafe,
        dead = 0xdead;
    int_ const array[] = {
        0,
        1,
        10,
        static_cast<int_>(i12345),
        static_cast<int_>(cafe),
        static_cast<int_>(dead),
        numeric_limits<int_>::min(),
        numeric_limits<int_>::max(),
        static_cast<int_>(numeric_limits<int_>::max() - 1),
        static_cast<int_>(numeric_limits<int_>::max() / 2),
        static_cast<int_>(numeric_limits<int_>::max() / 3)
    };
    for(auto a : array)
        write_read_write_one{s}(a);
}

inline void write_read_write_bool(settings s) {
    write_read_write_one{s}(false);
    write_read_write_one{s}(true);
}

inline void write_read_write_all() {
    settings settings_float[] {
        settings{},
        settings{}.base(2),
        settings{}.base(8).plus(true),
        settings{}.base(10),
        settings{}.base(16).plus(true),
        settings{}.group(true).space(true),
        settings{}.base(2).group(true).space(true).plus(true),
        settings{}.base(8).group(true).space(true),
        settings{}.base(10).group(true).space(true).plus(true),
        settings{}.base(16).group(true).space(true),
        settings{}.base(10).no_exponent().trailing_zeros(true).digits(255),
        settings{}.base(10).scientific(true).exponent(),
        settings{}.base(10).scientific(false).exponent(),
        settings{}.base(10).scientific(true).exponent().group(true).space(true).plus(true),
        settings{}.base(10).scientific(false).exponent().group(true).space(true).plus(true),
        settings{}.base(2).base_prefix_suffix(false),
        settings{}.base(8).base_prefix_suffix(false),
        settings{}.base(16).base_prefix_suffix(false),
        settings{}.base(2).precision(3),
        settings{}.base(8).precision(3),
        settings{}.base(10).precision(3),
        settings{}.base(16).precision(3)
    };
    for(auto s : settings_float) {
        write_read_write_float<float>(s);
        write_read_write_float<double>(s);
        if(!s.precision() || s.precision() >= max_digits<double>(10))
            s.precision(max_digits<double>(10) - 1);
        if(test_long_double)
            write_read_write_float<long double>(s);
    }
    settings settings_int[] {
        settings{},
        settings{}.bool_text(true),
        settings{}.digits(255).plus(true),
        settings{}.digits(255).plus(true).group(true),
        settings{}.base(2).base_prefix_suffix(false),
        settings{}.base(8).base_prefix_suffix(false),
        settings{}.base(16).base_prefix_suffix(false),
        settings{}.base(2).base_prefix_suffix(false).group(true).space(true),
        settings{}.base(8).base_prefix_suffix(false).group(true).space(true),
        settings{}.base(16).base_prefix_suffix(false).group(true).space(true),
        settings{}.base(2).digits(255).plus(true),
        settings{}.base(2).digits(255).plus(true).group(true),
        settings{}.base(8).digits(255).plus(true),
        settings{}.base(8).digits(255).plus(true).group(true),
        settings{}.base(16).digits(255).plus(true),
        settings{}.base(16).digits(255).plus(true).group(true),
        settings{}.digits(3)
    };
    for(auto s : settings_int) {
        write_read_write_bool(s);
        write_read_write_signed<signed char>(s);
        write_read_write_signed<signed short>(s);
        write_read_write_signed<signed int>(s);
        write_read_write_signed<signed long>(s);
        write_read_write_signed<signed long long>(s);
        write_read_write_unsigned<unsigned char>(s);
        write_read_write_unsigned<unsigned short>(s);
        write_read_write_unsigned<unsigned int>(s);
        write_read_write_unsigned<unsigned long>(s);
        write_read_write_unsigned<unsigned long long>(s);
    }
}

}}}
#endif
