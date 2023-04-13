// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_TYPES1_IS_CHAR_HPP
#define WATER_TYPES1_IS_CHAR_HPP
#include <water/types1/result.hpp>
#include <water/types1/char8.hpp>
namespace water { namespace types1 {
namespace _ {

    template<typename>
    struct do_is_char : false_result {};
    
    template<> struct do_is_char<char> : true_result {};
    template<> struct do_is_char<signed char> : true_result {};
    template<> struct do_is_char<unsigned char> : true_result {};
    template<> struct do_is_char<wchar_t> : true_result {};
    template<> struct do_is_char<char16_t> : true_result {};
    template<> struct do_is_char<char32_t> : true_result {};
    template<> struct do_is_char<char8_or_not> : true_result {};
    
}

template<typename type_>
struct is_char :
    _::do_is_char<typename type<type_>::result>
{};

}}
#endif
