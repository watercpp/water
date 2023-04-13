// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_IS_NO_TO_IS_INT_HPP
#define WATER_IS_NO_TO_IS_INT_HPP
#include <water/is_no_to/bits.hpp>
namespace water { namespace is_no_to {


template<typename> struct is_int : result_false {};

template<> struct is_int<char> : result_true {};
template<> struct is_int<signed char> : result_true {};
template<> struct is_int<unsigned char> : result_true {};
template<> struct is_int<wchar_t> : result_true {};
template<> struct is_int<short> : result_true {};
template<> struct is_int<unsigned short> : result_true {};
template<> struct is_int<int> : result_true {};
template<> struct is_int<unsigned int> : result_true {};
template<> struct is_int<long> : result_true {};
template<> struct is_int<unsigned long> : result_true {};
template<> struct is_int<long long> : result_true {};
template<> struct is_int<unsigned long long> : result_true {};
template<> struct is_int<char16_t> : result_true {};
template<> struct is_int<char32_t> : result_true {};
template<> struct is_int<char8_or_not> : result_true {};


}}
#endif
