// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_IS_NO_TO_IS_NO_TO_UNSIGNED_HPP
#define WATER_IS_NO_TO_IS_NO_TO_UNSIGNED_HPP
#include <water/is_no_to/bits.hpp>
namespace water { namespace is_no_to {


template<typename>    struct is_unsigned : result_false {};
template<typename a_> struct no_unsigned : type<a_> {};
template<typename a_> struct to_unsigned : type<a_> {};


template<bool if_, typename true_, typename false_>
struct if_unsigned : type<true_> {};

template<typename true_, typename false_>
struct if_unsigned<false, true_, false_> : type<false_> {};


template<> struct is_unsigned<char> : result_bool<(char(-1) >= char(0))> {};
template<> struct no_unsigned<char> : if_unsigned<char(-1) >= char(0), signed char, char> {};
template<> struct to_unsigned<char> : if_unsigned<char(-1) >= char(0), char, unsigned char> {};

template<> struct is_unsigned<unsigned char> : result_true {};
template<> struct no_unsigned<unsigned char> : type<signed char> {};
template<> struct to_unsigned<signed char> : type<unsigned char> {};

template<> struct is_unsigned<wchar_t> : result_bool<(wchar_t(-1) >= wchar_t(0))> {};

template<> struct is_unsigned<unsigned short> : result_true {};
template<> struct no_unsigned<unsigned short> : type<short> {};
template<> struct to_unsigned<short> : type<unsigned short> {};

template<> struct is_unsigned<unsigned int> : result_true {};
template<> struct no_unsigned<unsigned int> : type<int> {};
template<> struct to_unsigned<int> : type<unsigned int> {};

template<> struct is_unsigned<unsigned long> : result_true {};
template<> struct no_unsigned<unsigned long> : type<long> {};
template<> struct to_unsigned<long> : type<unsigned long> {};

template<> struct is_unsigned<unsigned long long> : result_true {};
template<> struct no_unsigned<unsigned long long> : type<long long> {};
template<> struct to_unsigned<long long> : type<unsigned long long> {};

template<> struct is_unsigned<char16_t> : result_true {};
template<> struct is_unsigned<char32_t> : result_true {};
template<> struct is_unsigned<char8_or_not> : result_true {};


template<typename a_> struct no_unsigned<a_&> : type<typename no_unsigned<a_>::result&> {};
template<typename a_> struct no_unsigned<a_&&> : type<typename no_unsigned<a_>::result&&> {};
template<typename a_> struct no_unsigned<a_ const>   : type<typename no_unsigned<a_>::result const> {};
template<typename a_> struct no_unsigned<a_ const&>  : type<typename no_unsigned<a_>::result const&> {};
template<typename a_> struct no_unsigned<a_ const&&> : type<typename no_unsigned<a_>::result const&&> {};
template<typename a_> struct no_unsigned<a_ volatile>   : type<typename no_unsigned<a_>::result volatile> {};
template<typename a_> struct no_unsigned<a_ volatile&>  : type<typename no_unsigned<a_>::result volatile&> {};
template<typename a_> struct no_unsigned<a_ volatile&&> : type<typename no_unsigned<a_>::result volatile&&> {};
template<typename a_> struct no_unsigned<a_ const volatile>   : type<typename no_unsigned<a_>::result const volatile> {};
template<typename a_> struct no_unsigned<a_ const volatile&>  : type<typename no_unsigned<a_>::result const volatile&> {};
template<typename a_> struct no_unsigned<a_ const volatile&&> : type<typename no_unsigned<a_>::result const volatile&&> {};


template<typename a_> struct to_unsigned<a_&> : type<typename to_unsigned<a_>::result&> {};
template<typename a_> struct to_unsigned<a_&&> : type<typename to_unsigned<a_>::result&&> {};
template<typename a_> struct to_unsigned<a_ const>   : type<typename to_unsigned<a_>::result const> {};
template<typename a_> struct to_unsigned<a_ const&>  : type<typename to_unsigned<a_>::result const&> {};
template<typename a_> struct to_unsigned<a_ const&&> : type<typename to_unsigned<a_>::result const&&> {};
template<typename a_> struct to_unsigned<a_ volatile>   : type<typename to_unsigned<a_>::result volatile> {};
template<typename a_> struct to_unsigned<a_ volatile&>  : type<typename to_unsigned<a_>::result volatile&> {};
template<typename a_> struct to_unsigned<a_ volatile&&> : type<typename to_unsigned<a_>::result volatile&&> {};
template<typename a_> struct to_unsigned<a_ const volatile>   : type<typename to_unsigned<a_>::result const volatile> {};
template<typename a_> struct to_unsigned<a_ const volatile&>  : type<typename to_unsigned<a_>::result const volatile&> {};
template<typename a_> struct to_unsigned<a_ const volatile&&> : type<typename to_unsigned<a_>::result const volatile&&> {};


}}
#endif
