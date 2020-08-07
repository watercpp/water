// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_TYPES_IS_INT_HPP
#define WATER_TYPES_IS_INT_HPP
#include <water/types/result.hpp>
namespace water { namespace types {

/*

result true if a type is a built-in integer or char type, but not bool

is_int<int>::result is true
is_int<int const>::result is false
is_int<bool>::result is false

*/

namespace _ {

    template<typename>
    struct do_is_int : false_result {};
    
    template<> struct do_is_int<char> : true_result {};
    template<> struct do_is_int<signed char> : true_result {};
    template<> struct do_is_int<unsigned char> : true_result {};
    template<> struct do_is_int<wchar_t> : true_result {};
    template<> struct do_is_int<short> : true_result {};
    template<> struct do_is_int<unsigned short> : true_result {};
    template<> struct do_is_int<int> : true_result {};
    template<> struct do_is_int<unsigned int> : true_result {};
    template<> struct do_is_int<long> : true_result {};
    template<> struct do_is_int<unsigned long> : true_result {};
    template<> struct do_is_int<long long> : true_result {};
    template<> struct do_is_int<unsigned long long> : true_result {};
    template<> struct do_is_int<char16_t> : true_result {};
    template<> struct do_is_int<char32_t> : true_result {};
    
}

template<typename type_>
struct is_int :
    _::do_is_int<typename type<type_>::result>
{};

}}
#endif
