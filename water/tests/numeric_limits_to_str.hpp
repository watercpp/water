// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_TESTS_NUMERIC_LIMITS_TO_STR_HPP
#define WATER_TESTS_NUMERIC_LIMITS_TO_STR_HPP
#include <water/test.hpp>
#include <water/numeric_limits.hpp>
#include <water/no_numeric_limits.hpp>
#include <water/str/str.hpp>
#include <water/str/type_name.hpp>
#include <water/str/bytes.hpp>
namespace water { namespace tests {

/*

numeric_limits to str::out

*/

template<typename limits_, bool is_specialized_ = limits_::is_specialized>
struct numeric_limits_to_str_values {
    template<typename o_>
    static void do_it(str::out<o_>& o) {
        struct { char const *name; decltype(limits_::min()) value; } list[] {
            //  has_signaling_NaN
            {"  max() ........... ", limits_::max()},
            {"  min() ........... ", limits_::min()},
            {"  lowest() ........ ", limits_::lowest()},
            {"  denorm_min() .... ", limits_::denorm_min()},
            {"  epsilon() ....... ", limits_::epsilon()},
            {"  round_error() ... ", limits_::round_error()},
            {"  infinity() ...... ", limits_::infinity()},
            {"  quiet_NaN() ..... ", limits_::quiet_NaN()},
            {"  signaling_NaN()   ", limits_::signaling_NaN()}
        };
        for(auto l : list)
            o << l.name << str::number(l.value, str::settings().bool_text(true)) << '\n'
                << "                    " << str::number(l.value, str::settings().base(16)) << '\n'
                << "                    " << str::number(l.value, str::settings().base(2)) << '\n'
                << "                    " << str::bytes(l.value) << '\n';
    }
};
template<typename limits_>
struct numeric_limits_to_str_values<limits_, false> {
    template<typename o_>
    static void do_it(str::out<o_>&) {}
};


template<typename o_, typename limits_>
void numeric_limits_to_str(str::out<o_>& o, limits_) {
    char const *denorm = "unknown";
    switch(static_cast<int>(limits_::has_denorm)) {
        case -1: denorm = "denorm_indeterminate"; break;
        case 0: denorm = "denorm_absent"; break;
        case 1: denorm = "denorm_present"; break;
    }
    char const *round = "unknown";
    switch(static_cast<int>(limits_::round_style)) {
        case -1: round = "round_indeterminate"; break;
        case 0: round = "round_toward_zero"; break;
        case 1: round = "round_to_nearest"; break;
        case 2: round = "round_toward_infinity"; break;
        case 3: round = "round_toward_neg_infinity"; break;
    }
    o << str::type_name<limits_>() << '\n'
        << "  is_specialized .. " << limits_::is_specialized << '\n'
        << "  is_bounded ...... " << limits_::is_bounded << '\n'
        << "  is_iec559 ....... " << limits_::is_iec559 << '\n'
        << "  is_integer ...... " << limits_::is_integer << '\n'
        << "  is_signed ....... " << limits_::is_signed << '\n'
        << "  is_modulo ....... " << limits_::is_modulo << '\n'
        << "  is_exact ........ " << limits_::is_exact << '\n'
        << "  has_denorm_loss   " << limits_::has_denorm_loss << '\n'
        << "  has_infinity .... " << limits_::has_infinity << '\n'
        << "  has_quiet_NaN ... " << limits_::has_quiet_NaN << '\n'
        << "  has_signaling_NaN " << limits_::has_signaling_NaN << '\n'
        << "  tinyness_before   " << limits_::tinyness_before << '\n'
        << "  traps ........... " << limits_::traps << '\n'
        << "  digits .......... " << limits_::digits << '\n'
        << "  digits10 ........ " << limits_::digits10 << '\n'
        << "  max_digits10 .... " << limits_::max_digits10 << '\n'
        << "  max_exponent .... " << limits_::max_exponent << '\n'
        << "  min_exponent .... " << limits_::min_exponent << '\n'
        << "  max_exponent10 .. " << limits_::max_exponent10 << '\n'
        << "  min_exponent10 .. " << limits_::min_exponent10 << '\n'
        << "  radix ........... " << limits_::radix << '\n'
        << "  has_denorm ...... " << static_cast<int>(limits_::has_denorm) << ' ' << denorm << '\n'
        << "  round_style ..... " << static_cast<int>(limits_::round_style) << ' ' << round << '\n';
    numeric_limits_to_str_values<limits_>::do_it(o);
    o << str::flush;
}

struct numeric_limits_to_str_t {};

template<template<typename> class make_, typename o_>
void numeric_limits_to_str(str::out<o_>& o) {
    numeric_limits_to_str(o, typename make_<bool>::result{});
    numeric_limits_to_str(o, typename make_<char>::result{});
    numeric_limits_to_str(o, typename make_<signed char>::result{});
    numeric_limits_to_str(o, typename make_<unsigned char>::result{});
    numeric_limits_to_str(o, typename make_<short>::result{});
    numeric_limits_to_str(o, typename make_<unsigned short>::result{});
    numeric_limits_to_str(o, typename make_<int>::result{});
    numeric_limits_to_str(o, typename make_<unsigned int>::result{});
    numeric_limits_to_str(o, typename make_<long>::result{});
    numeric_limits_to_str(o, typename make_<unsigned long>::result{});
    numeric_limits_to_str(o, typename make_<long long>::result{});
    numeric_limits_to_str(o, typename make_<unsigned long long>::result{});
    numeric_limits_to_str(o, typename make_<float>::result{});
    numeric_limits_to_str(o, typename make_<double>::result{});
    numeric_limits_to_str(o, typename make_<long double>::result{});
    numeric_limits_to_str(o, typename make_<char16_t>::result{});
    numeric_limits_to_str(o, typename make_<char32_t>::result{});
    numeric_limits_to_str(o, typename make_<wchar_t>::result{});
    numeric_limits_to_str(o, typename make_<void*>::result{});
    numeric_limits_to_str(o, typename make_<numeric_limits_to_str_t>::result{});
}

template<typename type_>
struct numeric_limits_to_str_make_std {
    using result = numeric_limits<type_>;
};

template<typename type_>
struct numeric_limits_to_str_make_water {
    using result = no_numeric_limits<type_>;
};

template<typename o_>
void numeric_limits_to_str_std(str::out<o_>& o) {
    numeric_limits_to_str<numeric_limits_to_str_make_std>(o);
}

template<typename o_>
void numeric_limits_to_str_water(str::out<o_>& o) {
    numeric_limits_to_str<numeric_limits_to_str_make_water>(o);
}

}}
#endif
