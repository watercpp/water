// Copyright 2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_TESTS_FLOAT_TO_STRING_HPP
#define WATER_TESTS_FLOAT_TO_STRING_HPP
#include <water/str/str.hpp>
#include <water/str/out_trace.hpp>
#include <water/str/type_name.hpp>
#include <water/xtr/xtr.hpp>
#if !defined(WATER_NO_STD) && defined(__cpp_lib_to_chars) && __cpp_lib_to_chars
    #include <charconv>
#endif
#ifndef WATER_NO_CHEADERS
    #include <stdio.h>
#else
    #include <cstdio>
#endif
namespace water { namespace tests {

/*

sketch to compare different float-to-string conversion methods.

by looking at the text, it's not automatic.

use 
float_to_string(to, 1.234e56, "1.234e56");

*/



class float_to_string_methods
{
    char my[64];
    unsigned mysize = 0;
    int myexponent = 0;

public:
    
    template<typename type_>
    float_to_string_methods(type_ a, unsigned method, unsigned digits = 32, unsigned prepare_method = 1) :
        mysize{!digits || digits > sizeof(my) ? static_cast<unsigned>(sizeof(my)) : digits}
    {
        bool minus = false;
        a = prepare(a, minus, prepare_method);
        if(digits > sizeof(my))
            digits = static_cast<unsigned>(sizeof(my));
        switch(method) {
            case 1: method1(a); break;
            case 2: method2(a); break;
            case 3: method34(a, 3); break;
            case 4: method34(a, 4); break;
        }
        if(mysize != sizeof(my))
            ++mysize;
        auto s = mysize;
        while(--s > 1)
            my[s] = my[s - 1];
        my[1] = '.';
        if(minus) {
            if(mysize != sizeof(my))
                ++mysize;
            s = mysize;
            while(--s)
                my[s] = my[s - 1];
            my[0] = '-';
        }
    }
    
    int exponent() const {
        return myexponent;
    }
    
    char const* begin() const {
        return my;
    }
    
    char const* end() const {
        return my + mysize;
    }

private:

    template<typename type_>
    void method1(type_ m) {
        auto *to = my;
        unsigned digits = mysize;
        while(digits--) {
            unsigned d = static_cast<unsigned>(m);
            *to++ = static_cast<char>(u'0' + d);
            m = fmod(m, static_cast<type_>(1.)) * 10;
        };
    }
    
    template<typename type_>
    void method2(type_ m) {
        using uint = uint_bits_at_least<64>;
        auto *to = my;
        auto *end = my + mysize;
        unsigned constexpr digits_max = numeric_limits<type_>::digits10 + 1;
        while(to != end) {
            unsigned d = digits_max;
            uint divide = 1;
            while(d-- && divide < static_cast<uint>(-1) / (10 * 10))
                divide *= 10;
            m *= divide;
            if(to != my) // its 1.23 the first time, then 0.123
                divide /= 10;
            auto u = static_cast<uint>(m);
            m -= u;
            do {
                *to++ = static_cast<char>(u'0' + ((u / divide) % 10));
            } while(to != end && (divide /= 10));
        }
        mysize = static_cast<unsigned>(to - my);
    }
    
    template<typename type_>
    void method34(type_ m, unsigned method) {
        using uint = uint_bits_at_least<64>;
        struct lookup_ { type_ f; uint u; };
        lookup_ constexpr lookup[] {
            {static_cast<type_>(1e0),  static_cast<uint>(1ull)},
            {static_cast<type_>(1e1),  static_cast<uint>(10ull)},
            {static_cast<type_>(1e2),  static_cast<uint>(100ull)},
            {static_cast<type_>(1e3),  static_cast<uint>(1000ull)},
            {static_cast<type_>(1e4),  static_cast<uint>(10000ull)},
            {static_cast<type_>(1e5),  static_cast<uint>(100000ull)},
            {static_cast<type_>(1e6),  static_cast<uint>(1000000ull)},
            {static_cast<type_>(1e7),  static_cast<uint>(10000000ull)},
            {static_cast<type_>(1e8),  static_cast<uint>(100000000ull)},
            {static_cast<type_>(1e9),  static_cast<uint>(1000000000ull)},
            {static_cast<type_>(1e10), static_cast<uint>(10000000000ull)},
            {static_cast<type_>(1e11), static_cast<uint>(100000000000ull)},
            {static_cast<type_>(1e12), static_cast<uint>(1000000000000ull)},
            {static_cast<type_>(1e13), static_cast<uint>(10000000000000ull)},
            {static_cast<type_>(1e14), static_cast<uint>(100000000000000ull)},
            {static_cast<type_>(1e15), static_cast<uint>(1000000000000000ull)},
            {static_cast<type_>(1e16), static_cast<uint>(10000000000000000ull)},
            {static_cast<type_>(1e17), static_cast<uint>(100000000000000000ull)},
            {static_cast<type_>(1e18), static_cast<uint>(1000000000000000000ull)},
            {static_cast<type_>(1e19), static_cast<uint>(10000000000000000000ull)}
        };
        bool use_lookup = method == 3;
        auto *to = my;
        unsigned constexpr digits_max = numeric_limits<type_>::digits10 + 1 < 19 ? numeric_limits<type_>::digits10 + 1 : 19;
        unsigned digits = mysize;
        while(digits) {
            unsigned d = digits > digits_max ? digits_max : digits;
            digits -= d;
            // on android pow is worse
            if(use_lookup)
                m *= lookup[to == my ? d - 1 : d].f; // first is 1.234, reset is fmod 1, 0.123
            else
                m *= pow(static_cast<type_>(10), static_cast<type_>(to == my ? d - 1 : d));
            auto mu = static_cast<uint>(m);
            auto mx = lookup[d - 1].u;
            //fmod makes no difference
            m -= mu;
            //m = fmod(m, static_cast<type_>(1.));
            do {
                *to++ = static_cast<char>(u'0' + ((mu / mx) % 10));
            } while(mx /= 10);
        }
    }
    
    template<typename type_>
    type_ prepare(type_ a, bool& minus, unsigned method) {
        // normalize to 0...9
        minus = signbit(a);
        type_ m = minus ? -a : a;
        if(!a)
            return 0;
        myexponent = static_cast<int>(log10(m));
        int p = myexponent;
        if((numeric_limits<type_>::is_iec559 || numeric_limits<type_>::has_denorm > 0) && m < numeric_limits<type_>::min())
            do ++p; while((m *= static_cast<type_>(10)) < numeric_limits<type_>::min());
        if(method == 1)
            m /= pow(static_cast<type_>(10), static_cast<type_>(p));
        else {
            // lookup instead of pow
            type_ constexpr pow10[] {
                static_cast<type_>(1e0),
                static_cast<type_>(1e1),
                static_cast<type_>(1e2),
                static_cast<type_>(1e3),
                static_cast<type_>(1e4),
                static_cast<type_>(1e5),
                static_cast<type_>(1e6),
                static_cast<type_>(1e7),
                static_cast<type_>(1e8),
                static_cast<type_>(1e9),
                static_cast<type_>(1e10),
                static_cast<type_>(1e11),
                static_cast<type_>(1e12),
                static_cast<type_>(1e13),
                static_cast<type_>(1e14),
                static_cast<type_>(1e15),
                static_cast<type_>(1e16),
                static_cast<type_>(1e17),
                static_cast<type_>(1e18),
                static_cast<type_>(1e19),
                static_cast<type_>(1e20),
                static_cast<type_>(1e21),
                static_cast<type_>(1e22),
                static_cast<type_>(1e23),
                static_cast<type_>(1e24),
                static_cast<type_>(1e25),
                static_cast<type_>(1e26),
                static_cast<type_>(1e27),
                static_cast<type_>(1e28),
                static_cast<type_>(1e29),
                static_cast<type_>(1e30),
                static_cast<type_>(1e31),
                static_cast<type_>(1e32)
            };
            type_ constexpr pow10_minus[] {
                static_cast<type_>(1e-0),
                static_cast<type_>(1e-1),
                static_cast<type_>(1e-2),
                static_cast<type_>(1e-3),
                static_cast<type_>(1e-4),
                static_cast<type_>(1e-5),
                static_cast<type_>(1e-6),
                static_cast<type_>(1e-7),
                static_cast<type_>(1e-8),
                static_cast<type_>(1e-9),
                static_cast<type_>(1e-10),
                static_cast<type_>(1e-11),
                static_cast<type_>(1e-12),
                static_cast<type_>(1e-13),
                static_cast<type_>(1e-14),
                static_cast<type_>(1e-15),
                static_cast<type_>(1e-16),
                static_cast<type_>(1e-17),
                static_cast<type_>(1e-18),
                static_cast<type_>(1e-19),
                static_cast<type_>(1e-20),
                static_cast<type_>(1e-21),
                static_cast<type_>(1e-22),
                static_cast<type_>(1e-23),
                static_cast<type_>(1e-24),
                static_cast<type_>(1e-25),
                static_cast<type_>(1e-26),
                static_cast<type_>(1e-27),
                static_cast<type_>(1e-28),
                static_cast<type_>(1e-29),
                static_cast<type_>(1e-30),
                static_cast<type_>(1e-31),
                static_cast<type_>(1e-32)
            };
            unsigned constexpr pow10_max = 32;
            if(myexponent < 0) {
                unsigned e = static_cast<unsigned>(-myexponent);
                do {
                    unsigned e0 = e > pow10_max ? pow10_max : e;
                    m *= pow10[e0];
                    e -= e0;
                } while(e);
            }
            if(myexponent > 0) {
                unsigned e = static_cast<unsigned>(myexponent);
                do {
                    unsigned e0 = e > pow10_max ? pow10_max : e;
                    m *= pow10_minus[e0];
                    e -= e0;
                } while(e);
            }
        }
        if(m < 1) {
            m *= 10;
            --myexponent;
        }
        else if(m >= 10) {
            m /= 10;
            ++myexponent;
        }
        return m;
    }

};



template<typename o_, typename type_>
void float_to_string(str::out<o_>& to, type_ value, char const* text) {
    to << str::type_name<type_>() << '\n';
    to << "text ... " << text << '\n';    
    float_to_string_methods
        m1{value, 1},
        m2{value, 2},
        m3{value, 3},
        m4{value, 4};
    
    char buffer[128];
    
    //    "to_chars "
    //    "snprintf "
    to << "method 1 " << m1 << " E" << m1.exponent() << '\n';
    to << "method 2 " << m2 << " E" << m2.exponent() << '\n';
    to << "method 3 " << m3 << " E" << m3.exponent() << '\n';
    to << "method 4 " << m4 << " E" << m4.exponent() << '\n';
    to << "str .... " << (str::to_begin_end(buffer) << value) << '\n';
    to << "xtr .... " << string(xtr::no << value) << '\n';
    
    #if !defined(WATER_NO_STD) && defined(__cpp_lib_to_chars) && __cpp_lib_to_chars
    auto to_chars = begin_end_from(
        buffer,
        std::to_chars(buffer, buffer + sizeof(buffer), value, std::chars_format::scientific, 32).ptr
    );
    to << "to_chars " << to_chars << '\n';
    #endif
    
    if(equal<type_, double>) {
        #ifndef WATER_NO_CHEADERS
        using ::snprintf;
        #else
        using std::snprintf;
        #endif
        snprintf(buffer, sizeof(buffer), "%.32E", static_cast<double>(value));
        to << "snprintf " << buffer << '\n';
    }
    to << '\n';
}



template<typename type_, typename o_>
void float_to_string_all_for_type(str::out<o_>& to) {
    struct list_ {
        type_ value;
        char const *text;
    };
    #define ___water_list(a) {static_cast<type_>(a), #a}
    list_ list[] {
        ___water_list(0),
        ___water_list(1),
        ___water_list(1.2345678901234567890123456789001234567890),
        ___water_list(1.23456e89),
        ___water_list(-9.876543E-21),
        ___water_list(-9.7654321),
        ___water_list(9.999),
        ___water_list(123.45678e9),
        ___water_list(1.234e10),
        ___water_list(-9.876e-5),
        ___water_list(-65536.0),
        ___water_list(1e20),
        ___water_list(1e21),
        ___water_list(1e-20),
        ___water_list(1e-21),
        ___water_list(-1.2345e10),
        ___water_list(-1.2345e11),
        ___water_list(-1.2345e-10),
        ___water_list(-1.2345e-11),
        ___water_list(1234.5),
        ___water_list(numeric_limits<type_>::max()),
        ___water_list(-numeric_limits<type_>::max() * static_cast<type_>(0.303808909303808)),
        ___water_list(numeric_limits<type_>::min() * static_cast<type_>(808909.303808909303808)),
        ___water_list(-numeric_limits<type_>::min()),
        ___water_list(numeric_limits<type_>::denorm_min()),
        ___water_list(-numeric_limits<type_>::denorm_min() * static_cast<type_>(909303.808909303808)),
        ___water_list(numeric_limits<type_>::epsilon()),
        ___water_list(-numeric_limits<type_>::epsilon()),
        ___water_list(sqrt(numeric_limits<type_>::max() * static_cast<type_>(0.303808909) - static_cast<type_>(1.234e5))),
        ___water_list(sqrt(static_cast<type_>(2))),
        ___water_list(static_cast<type_>(1.41421356237309504880168872420969807856967187537694807317667973799)),
        ___water_list(static_cast<type_>(3.14159265358979323846264338327950288419716939937510)),
        ___water_list(pow(sqrt(static_cast<type_>(2)), static_cast<type_>(numeric_limits<type_>::max_exponent10))),
        ___water_list(pow(sqrt(static_cast<type_>(2)), static_cast<type_>(numeric_limits<type_>::min_exponent10))),
        ___water_list(pow(static_cast<type_>(1.2345678901234567890), static_cast<type_>(numeric_limits<type_>::max_exponent10 / 3))),
        ___water_list(pow(static_cast<type_>(1.2345678901234567890), static_cast<type_>(numeric_limits<type_>::min_exponent10 / 3))),
    };
    #undef ___water_list
    for(auto l : list)
        float_to_string(to, l.value, l.text);
}



inline void float_to_string_all() {
    str::out_trace to;
    float_to_string_all_for_type<double>(to);
    float_to_string_all_for_type<float>(to);
    float_to_string_all_for_type<long double>(to);
}



}}
#endif
