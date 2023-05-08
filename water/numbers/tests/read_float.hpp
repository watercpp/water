// Copyright 2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_NUMBERS_TESTS_READ_FLOAT_HPP
#define WATER_NUMBERS_TESTS_READ_FLOAT_HPP
#include <water/numbers/tests/bits.hpp>
#include <water/cstring.hpp>
namespace water { namespace numbers { namespace tests {

/*

compare how close numbers::read<float> gets to the compilers float parser

*/

template<typename float_> constexpr char const* read_float_name();
template<> constexpr char const* read_float_name<float>() { return "float"; }
template<> constexpr char const* read_float_name<double>() { return "double"; }
template<> constexpr char const* read_float_name<long double>() { return "long double"; }



template<typename float_>
struct read_float_and_compare
{
    bool error = false;
    bool exact = false;
    bool similar = false;

    read_float_and_compare(float_ const compiler, char const* text) {
        auto text_end = cstring(text).end();
        read<float_> r;
        auto e = r(text, text_end);
        ___water_test(e == text_end && "did not read the whole text");
        ___water_test(!r.infinity() && !r.nan());
        if(e != text_end || r.infinity() || r.nan()) {
            error = true;
            return;
        }
        auto value = r.value();
        auto const epsilon = static_cast<float_>(numeric_limits<ifel<equal<long double, float_>, double, float_>>::epsilon());
        auto almost1 = static_cast<float_>(1) + epsilon;
        exact = compiler == value;
        similar =
            (value / almost1 <= compiler && compiler <= value * almost1) ||
            (value / almost1 >= compiler && compiler >= value * almost1) ||
            (compiler / almost1 <= value && value <= compiler * almost1) ||
            (compiler / almost1 >= value && value >= compiler * almost1);
        if(!similar) {
            trace << "water::numbers::tests::read_float_and_compare failed for " << read_float_name<float_>() << '\n'
                << "text     " << text << '\n' << xtr::digits<numeric_limits<float_>::digits10 + 2>
                << "compiler " << compiler << '\n'
                << "read     " << value << '\n' << xtr::digits<0> << xtr::hex
                << "compiler " << compiler << '\n'
                << "read     " << value;
        }
        ___water_test(similar);
    }
};



template<typename to_>
to_ read_float_list_cast(ifel<water::equal<to_, float>, double, to_> a) { // avoiud float truncation warnings
    return static_cast<to_>(a);
}



template<typename float_>
void read_float_list() {
    struct value_text {
        float_ value;
        char const* text;
        bool skip_long_double;
    };
    #define WATER_NUMBERS_TESTS_READ_FLOAT(a) {read_float_list_cast<float_>(a), #a, false}
    #define WATER_NUMBERS_TESTS_READ_FLOAT_NO_LONG_DOUBLE(a) {read_float_list_cast<float_>(a), #a, true}
    value_text const array[] = {
        WATER_NUMBERS_TESTS_READ_FLOAT(0),
        WATER_NUMBERS_TESTS_READ_FLOAT(1),
        WATER_NUMBERS_TESTS_READ_FLOAT(-1),
        WATER_NUMBERS_TESTS_READ_FLOAT(10),
        WATER_NUMBERS_TESTS_READ_FLOAT(-10),
        WATER_NUMBERS_TESTS_READ_FLOAT(-1.23456789012345678901234567890e-50),
        WATER_NUMBERS_TESTS_READ_FLOAT(+1.23456789012345678901234567890e-40),
        WATER_NUMBERS_TESTS_READ_FLOAT(-1.23456789012345678901234567890e-30),
        WATER_NUMBERS_TESTS_READ_FLOAT(+1.23456789012345678901234567890e-20),
        WATER_NUMBERS_TESTS_READ_FLOAT(-1.23456789012345678901234567890e-10),
        WATER_NUMBERS_TESTS_READ_FLOAT(+1.23456789012345678901234567890e-9),
        WATER_NUMBERS_TESTS_READ_FLOAT(-1.23456789012345678901234567890e-8),
        WATER_NUMBERS_TESTS_READ_FLOAT(+1.23456789012345678901234567890e-7),
        WATER_NUMBERS_TESTS_READ_FLOAT(-1.23456789012345678901234567890e-6),
        WATER_NUMBERS_TESTS_READ_FLOAT(+1.23456789012345678901234567890e-5),
        WATER_NUMBERS_TESTS_READ_FLOAT(-1.23456789012345678901234567890e-4),
        WATER_NUMBERS_TESTS_READ_FLOAT(+1.23456789012345678901234567890e-3),
        WATER_NUMBERS_TESTS_READ_FLOAT(-1.23456789012345678901234567890e-2),
        WATER_NUMBERS_TESTS_READ_FLOAT(+1.23456789012345678901234567890e-1),
        WATER_NUMBERS_TESTS_READ_FLOAT(-1.23456789012345678901234567890e0),
        WATER_NUMBERS_TESTS_READ_FLOAT(+1.23456789012345678901234567890e1),
        WATER_NUMBERS_TESTS_READ_FLOAT(-1.23456789012345678901234567890e2),
        WATER_NUMBERS_TESTS_READ_FLOAT(+1.23456789012345678901234567890e3),
        WATER_NUMBERS_TESTS_READ_FLOAT(-1.23456789012345678901234567890e4),
        WATER_NUMBERS_TESTS_READ_FLOAT(+1.23456789012345678901234567890e5),
        WATER_NUMBERS_TESTS_READ_FLOAT(-1.23456789012345678901234567890e6),
        WATER_NUMBERS_TESTS_READ_FLOAT(+1.23456789012345678901234567890e7),
        WATER_NUMBERS_TESTS_READ_FLOAT(-1.23456789012345678901234567890e8),
        WATER_NUMBERS_TESTS_READ_FLOAT(+1.23456789012345678901234567890e9),
        WATER_NUMBERS_TESTS_READ_FLOAT(-1.23456789012345678901234567890e10),
        WATER_NUMBERS_TESTS_READ_FLOAT(+1.23456789012345678901234567890e20),
        WATER_NUMBERS_TESTS_READ_FLOAT(-1.23456789012345678901234567890e30),
        WATER_NUMBERS_TESTS_READ_FLOAT(+1.23456789012345678901234567890e40),
        WATER_NUMBERS_TESTS_READ_FLOAT(-1.23456789012345678901234567890e50),
        WATER_NUMBERS_TESTS_READ_FLOAT(9.99995),
        WATER_NUMBERS_TESTS_READ_FLOAT(1.2345e100),
        WATER_NUMBERS_TESTS_READ_FLOAT(-1.2345e-100),
        WATER_NUMBERS_TESTS_READ_FLOAT(1.2345e101),
        WATER_NUMBERS_TESTS_READ_FLOAT(-1.2345e-101),
        WATER_NUMBERS_TESTS_READ_FLOAT(1.2345678901234567890123456789001234567890),
        WATER_NUMBERS_TESTS_READ_FLOAT(1.23456e89),
        WATER_NUMBERS_TESTS_READ_FLOAT(-9.876543E-21),
        WATER_NUMBERS_TESTS_READ_FLOAT(-9.7654321),
        WATER_NUMBERS_TESTS_READ_FLOAT(9.999),
        WATER_NUMBERS_TESTS_READ_FLOAT(123.45678e9),
        WATER_NUMBERS_TESTS_READ_FLOAT(1.234e10),
        WATER_NUMBERS_TESTS_READ_FLOAT(-9.876e-5),
        WATER_NUMBERS_TESTS_READ_FLOAT(-65536.0),
        WATER_NUMBERS_TESTS_READ_FLOAT(1e20),
        WATER_NUMBERS_TESTS_READ_FLOAT(1e21),
        WATER_NUMBERS_TESTS_READ_FLOAT(1e-20),
        WATER_NUMBERS_TESTS_READ_FLOAT(1e-21),
        WATER_NUMBERS_TESTS_READ_FLOAT(-1.2345e10),
        WATER_NUMBERS_TESTS_READ_FLOAT(-1.2345e11),
        WATER_NUMBERS_TESTS_READ_FLOAT(-1.2345e-10),
        WATER_NUMBERS_TESTS_READ_FLOAT(-1.2345e-11),
        WATER_NUMBERS_TESTS_READ_FLOAT(1234.5),
        WATER_NUMBERS_TESTS_READ_FLOAT(0.303808909303808),
        WATER_NUMBERS_TESTS_READ_FLOAT(808909.303808909303808),
        WATER_NUMBERS_TESTS_READ_FLOAT(909303.808909303808),
        WATER_NUMBERS_TESTS_READ_FLOAT(0.303808909),
        WATER_NUMBERS_TESTS_READ_FLOAT(1.41421356237309504880168872420969807856967187537694807317667973799),
        WATER_NUMBERS_TESTS_READ_FLOAT(3.14159265358979323846264338327950288419716939937510),
        
        WATER_NUMBERS_TESTS_READ_FLOAT(4.9406564584124654e-324), // double denorm min
        WATER_NUMBERS_TESTS_READ_FLOAT(2.2204460492503131e-16), // double epsilon
        WATER_NUMBERS_TESTS_READ_FLOAT(1.7976931348623157e+308), // double max
        WATER_NUMBERS_TESTS_READ_FLOAT(2.2250738585072014e-308), // double min
        WATER_NUMBERS_TESTS_READ_FLOAT_NO_LONG_DOUBLE(-1.2345e-323), // denormal (on android and linux the compiler reads it as -9.88...e-324 for long double, so skipping this)
        WATER_NUMBERS_TESTS_READ_FLOAT(-1.77e+308),
        WATER_NUMBERS_TESTS_READ_FLOAT_NO_LONG_DOUBLE(-1e-309), // denormal (on android water::numbers reads this as -9.99...e-310 for long double, so skipping)
        
        WATER_NUMBERS_TESTS_READ_FLOAT(1.40129846e-45), // float denorm min
        WATER_NUMBERS_TESTS_READ_FLOAT(1.19209290e-7), // float epsilon
        WATER_NUMBERS_TESTS_READ_FLOAT(3.40282347e+38), // float max
        WATER_NUMBERS_TESTS_READ_FLOAT(1.17549435e-38), // float min
        WATER_NUMBERS_TESTS_READ_FLOAT(-1.2345e-44), // denormal
        WATER_NUMBERS_TESTS_READ_FLOAT(-1.77e+38),
        WATER_NUMBERS_TESTS_READ_FLOAT(-1e-40) // denormal
        
    };
    #undef WATER_NUMBERS_TESTS_READ_FLOAT
    #undef WATER_NUMBERS_TESTS_READ_FLOAT_NO_LONG_DOUBLE
    size_t
        exact = 0,
        tests = 0;
    for(auto a : array) {
        if(!isinf_strict(a.value) && (!a.skip_long_double || !water::equal<float_, long double>)) { // some float values will be infinity
            ++tests;
            read_float_and_compare<float_> c{a.value, a.text};
            if(c.exact)
                ++exact;
        }
    }
    trace << "water::numbers::tests::read_float_list<" << read_float_name<float_>() << "> tests=" << tests << " exact=" << exact;
}



inline void read_float_all() {
    read_float_list<double>();
    read_float_list<float>();
    read_float_list<long double>();
}



}}}
#endif
