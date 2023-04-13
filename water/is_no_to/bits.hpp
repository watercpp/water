// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_IS_NO_TO_BITS_HPP
#define WATER_IS_NO_TO_BITS_HPP
#ifndef WATER_IS_NO_TO_HPP
    #error include <water/is_no_to.hpp> and do not use anything from the water::is_no_to namespace directly
#endif
namespace water { namespace is_no_to {

using size_t = decltype(sizeof(0));

template<typename result_>
struct type {
   using result = result_;
};

// do not..
// template<> bool constexpr is_something<int> = true;
// ...because duplicate symbols linker error

template<bool result_>
struct result_bool {
    static bool constexpr result = result_;
};

using result_true = result_bool<true>;
using result_false = result_bool<false>;

using true_size = bool;
using false_size = bool(&)[2];

struct no_char8;

template<typename char8_ = decltype(*u8"")> struct char8_select;
template<typename char8_> struct char8_select<char8_ const&> : type<char8_> {};
template<> struct char8_select<char const&> : type<no_char8> {};

using char8_or_not = char8_select<>::result;

}}
#endif
