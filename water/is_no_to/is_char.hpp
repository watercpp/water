// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_IS_NO_TO_IS_CHAR_HPP
#define WATER_IS_NO_TO_IS_CHAR_HPP
#include <water/is_no_to/bits.hpp>
namespace water { namespace is_no_to {


template<typename> struct is_char : result_false {};

template<> struct is_char<char> : result_true {};
template<> struct is_char<signed char> : result_true {};
template<> struct is_char<unsigned char> : result_true {};
template<> struct is_char<wchar_t> : result_true {};
template<> struct is_char<char16_t> : result_true {};
template<> struct is_char<char32_t> : result_true {};
template<> struct is_char<char8_or_not> : result_true {};


}}
#endif
