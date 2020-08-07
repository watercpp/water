// Copyright 2017-2018 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_MINISTR_TESTS_TEST_HPP
#define WATER_MINISTR_TESTS_TEST_HPP
#include <water/ministr/tests/bits.hpp>
#include <water/trace.hpp>
#include <water/int.hpp>
namespace water { namespace ministr { namespace tests {

struct test_trace {
    void operator()(char const *b, char const *e) const {
        ___water_test(b);
        ___water_test(e);
        ___water_test(b < e);
        ___water_test(*b);
        ___water_test(!*e);
        trace(b);
    }
};

template<typename to_, typename from_>
to_ cast(from_ from) {
    // this is to avoid overflow warnings when casting static_cast<float>(-1.23456789012345678901234567890e50l)
    // warning C4756: overflow in constant arithmetic
    from_ copy{};
    auto *c = static_cast<unsigned char*>(static_cast<void*>(&copy)), *ce = c + sizeof(copy);
    auto *f = static_cast<unsigned char const*>(static_cast<void const*>(&from));
    do *c++ = *f++; while(c != ce);
    return static_cast<to_>(copy);
}

template<typename float_, int min_, int max_, typename out_>
void test_exponent_min_max(out_ o, char const *type) {
    // this used to be 1 large expression, split to avoid warning C4503 decorated name length exceeded, name was truncated
    struct list_ { char const* name; float_ value; } list[] {
        {"-50 ", cast<float_>(-1.23456789012345678901234567890e-50)},
        {"-40 ", cast<float_>(+1.23456789012345678901234567890e-40)},
        {"-30 ", cast<float_>(-1.23456789012345678901234567890e-30)},
        {"-20 ", cast<float_>(+1.23456789012345678901234567890e-20)},
        {"-10 ", cast<float_>(-1.23456789012345678901234567890e-10)},
        {" -9 ", cast<float_>(+1.23456789012345678901234567890e-9)},
        {" -8 ", cast<float_>(-1.23456789012345678901234567890e-8)},
        {" -7 ", cast<float_>(+1.23456789012345678901234567890e-7)},
        {" -6 ", cast<float_>(-1.23456789012345678901234567890e-6)},
        {" -5 ", cast<float_>(+1.23456789012345678901234567890e-5)},
        {" -4 ", cast<float_>(-1.23456789012345678901234567890e-4)},
        {" -3 ", cast<float_>(+1.23456789012345678901234567890e-3)},
        {" -2 ", cast<float_>(-1.23456789012345678901234567890e-2)},
        {" -1 ", cast<float_>(+1.23456789012345678901234567890e-1)},
        {"  0 ", cast<float_>(-1.23456789012345678901234567890e0)},
        {"  1 ", cast<float_>(+1.23456789012345678901234567890e1)},
        {"  2 ", cast<float_>(-1.23456789012345678901234567890e2)},
        {"  3 ", cast<float_>(+1.23456789012345678901234567890e3)},
        {"  4 ", cast<float_>(-1.23456789012345678901234567890e4)},
        {"  5 ", cast<float_>(+1.23456789012345678901234567890e5)},
        {"  6 ", cast<float_>(-1.23456789012345678901234567890e6)},
        {"  7 ", cast<float_>(+1.23456789012345678901234567890e7)},
        {"  8 ", cast<float_>(-1.23456789012345678901234567890e8)},
        {"  9 ", cast<float_>(+1.23456789012345678901234567890e9)},
        {" 10 ", cast<float_>(-1.23456789012345678901234567890e10)},
        {" 20 ", cast<float_>(+1.23456789012345678901234567890e20)},
        {" 30 ", cast<float_>(-1.23456789012345678901234567890e30)},
        {" 40 ", cast<float_>(+1.23456789012345678901234567890e40)},
        {" 50 ", cast<float_>(-1.23456789012345678901234567890e50)}
    };
    o << "no_exponent_min_max<" << min_ << ", " << max_ << "> " << type << "\n";
    for(auto l : list)
        o << no_exponent_min_max<min_, max_> << l.name << l.value << '\n';
}

struct range {
    char const* begin() const {
        return "hello world";
    }
    char const* end() const {
        return begin() + 5;
    }
};

struct test {
    test() {
        out<test_trace>
            o1,
            o2 = test_trace(),
            o3{test_trace()},
            o4 = o3;
        o1 = o2 = out<test_trace>{o4};
        char not_const[] = "hello";
        out<test_trace>() << not_const << ' ' << "world" << 123 << 456u << '\n';
        o3 << "hello" << static_cast<char const*>(" world ") << this << '\n';
        o3 << range() << " world" << '\n';
        o3 << "float " << 1.2345 << base<16> << " hex " << 1.2345 << '\n';
        
        o3 << "bytes " << bytes(o3) << '\n';
        
        auto c1 = (out<>() << "(expression).cstring() float " << 1.2345 << base<16> << " hex " << 1.2345 << '\n').cstring();
        auto c2 = cstring(out<>() << "cstring(expression) float " << 1.2345 << base<16> << " hex " << 1.2345 << '\n');
        trace(c1);
        trace(c2);
        trace((out<>() << "(expression).cstring() float " << 1.2345 << base<16> << " hex " << 1.2345 << '\n').cstring());
        trace(cstring(out<>() << "cstring(expression) float " << 1.2345 << base<16> << " hex " << 1.2345 << '\n'));
        
        o3 << true << ' ' << false << ' ' << 0 << -1 << 0x5 << '\n';
        o3 << size<5> << base<2>  << 123 << ' ' << -123 << ' ' << 123123u << ' ' << 0xfff << '\n';
        o3 << size<5> << base<8>  << 123 << ' ' << -123 << ' ' << 123123u << ' ' << 0xfff << '\n';
        o3 << size<5> << base<16> << 123 << ' ' << -123 << ' ' << 123123u << ' ' << 0xfff << '\n';
        
        o3 << uppercase << lowercase << uppercase << base<2>  << 123 << ' ' << -123 << ' ' << 123123u << ' ' << 0xfff << '\n';
        o3 << uppercase << lowercase << uppercase << base<8>  << 123 << ' ' << -123 << ' ' << 123123u << ' ' << 0xfff << '\n';
        o3 << uppercase << lowercase << uppercase << base<16> << 123 << ' ' << -123 << ' ' << 123123u << ' ' << 0xfff << '\n';
        
        o3 << uppercase << mixedcase << lowercase << base<2>  << 123 << ' ' << -123 << ' ' << 123123u << ' ' << 0xfff << '\n';
        o3 << uppercase << mixedcase << lowercase << base<8>  << 123 << ' ' << -123 << ' ' << 123123u << ' ' << 0xfff << '\n';
        o3 << uppercase << mixedcase << lowercase << base<16> << 123 << ' ' << -123 << ' ' << 123123u << ' ' << 0xfff << '\n';
        
        o3 << "signed char        " << numeric_limits<signed   char>::min()      <<  ' ' << numeric_limits<signed   char>::max()      << '\n';
        o3 << "unsigned char      " << numeric_limits<unsigned char>::min()      <<  ' ' << numeric_limits<unsigned char>::max()      << '\n';
        o3 << "signed short       " << numeric_limits<signed   short>::min()     <<  ' ' << numeric_limits<signed   short>::max()     << '\n';
        o3 << "unsigned short     " << numeric_limits<unsigned short>::min()     <<  ' ' << numeric_limits<unsigned short>::max()     << '\n';
        o3 << "signed int         " << numeric_limits<signed   int>::min()       <<  ' ' << numeric_limits<signed   int>::max()       << '\n';
        o3 << "unsigned int       " << numeric_limits<unsigned int>::min()       <<  ' ' << numeric_limits<unsigned int>::max()       << '\n';
        o3 << "signed long        " << numeric_limits<signed   long>::min()      <<  ' ' << numeric_limits<signed   long>::max()      << '\n';
        o3 << "unsigned long      " << numeric_limits<unsigned long>::min()      <<  ' ' << numeric_limits<unsigned long>::max()      << '\n';
        o3 << "signed long long   " << numeric_limits<signed   long long>::min() <<  ' ' << numeric_limits<signed   long long>::max() << '\n';
        o3 << "unsigned long long " << numeric_limits<unsigned long long>::min() <<  ' ' << numeric_limits<unsigned long long>::max() << '\n';
        
        test_exponent_min_max<float, -100, 100>(o1, "float");
        test_exponent_min_max<double, -100, 100>(o1, "dobule");
        test_exponent_min_max<long double, -100, 100>(o1, "long dobule");
        
        test_exponent_min_max<float, -3, -1>(o1, "float");
        test_exponent_min_max<double, -3, -1>(o1, "dobule");
        test_exponent_min_max<long double, -3, -1>(o1, "long dobule");
        
        test_exponent_min_max<float, 0, 0>(o1, "float");
        test_exponent_min_max<double, 0, 0>(o1 << uppercase, "dobule");
        test_exponent_min_max<long double, 0, 0>(o1 << lowercase, "long dobule");
        
        test_exponent_min_max<float, -100, 100>(o1 << base<16>, "float");
        test_exponent_min_max<double, -100, 100>(o1 << base<16> << uppercase, "dobule");
        test_exponent_min_max<long double, -100, 100>(o1 << base<16> << lowercase, "long dobule");
        
        test_exponent_min_max<float, -100, 100>(o1 << base<8>, "float");
        test_exponent_min_max<double, -100, 100>(o1 << base<8> << uppercase, "dobule");
        test_exponent_min_max<long double, -100, 100>(o1 << base<8> << lowercase, "long dobule");
        
        test_exponent_min_max<float, -100, 100>(o1 << base<2>, "float");
        test_exponent_min_max<double, -100, 100>(o1 << base<2> << uppercase, "dobule");
        test_exponent_min_max<long double, -100, 100>(o1 << base<2> << lowercase, "long dobule");
    }
};

}}}
#endif
