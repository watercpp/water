// Copyright 2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_XTR_FOLD_HPP
#define WATER_XTR_FOLD_HPP
#include <water/xtr/base.hpp>
namespace water { namespace xtr {

/*

auto string = xtr::fold("hello", ' ', " world!", xtr::base<2>, 1234, '\n');

fputs(string, stdout);
std::vector<char> copy{string.begin(), string.end()};

fold_2014 enables up to 8 arguments on C++14.
On C++17 or later there is no limit.

*/

inline auto fold_2014() {
    return string(xtr::expression<>{});
}

template<typename a_>
auto fold_2014(a_&& a) {
    return string(xtr::expression<>{} << a);
}

template<typename a_, typename b_>
auto fold_2014(a_&& a, b_&& b) {
    return string(xtr::expression<>{} << a << b);
}

template<typename a_, typename b_, typename c_>
auto fold_2014(a_&& a, b_&& b, c_&& c) {
    return string(xtr::expression<>{} << a << b << c);
}

template<typename a_, typename b_, typename c_, typename d_>
auto fold_2014(a_&& a, b_&& b, c_&& c, d_&& d) {
    return string(xtr::expression<>{} << a << b << c << d);
}

template<typename a_, typename b_, typename c_, typename d_, typename e_>
auto fold_2014(a_&& a, b_&& b, c_&& c, d_&& d, e_&& e) {
    return string(xtr::expression<>{} << a << b << c << d << e);
}

template<typename a_, typename b_, typename c_, typename d_, typename e_, typename f_>
auto fold_2014(a_&& a, b_&& b, c_&& c, d_&& d, e_&& e, f_&& f) {
    return string(xtr::expression<>{} << a << b << c << d << e << f);
}

template<typename a_, typename b_, typename c_, typename d_, typename e_, typename f_, typename g_>
auto fold_2014(a_&& a, b_&& b, c_&& c, d_&& d, e_&& e, f_&& f, g_&& g) {
    return string(xtr::expression<>{} << a << b << c << d << e << f << g);
}

template<typename a_, typename b_, typename c_, typename d_, typename e_, typename f_, typename g_, typename h_>
auto fold_2014(a_&& a, b_&& b, c_&& c, d_&& d, e_&& e, f_&& f, g_&& g, h_&& h) {
    return string(xtr::expression<>{} << a << b << c << d << e << f << g << h);
}

#if __cpp_fold_expressions

template<typename... arguments_>
auto fold(arguments_&&... a) {
    return string((xtr::expression<>{} << ... << a));
}

#else

template<typename... arguments_>
auto fold(arguments_&&... a) {
    return fold_2014(static_cast<arguments_&&>(a)...);
}

#endif

}}
#endif
