// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_TYPES1_IS_NO_TO_UNSIGNED_HPP
#define WATER_TYPES1_IS_NO_TO_UNSIGNED_HPP
#include <water/types1/ifel.hpp>
#include <water/types1/char8.hpp>
namespace water { namespace types1 {
namespace _ {

    template<typename>
    struct do_is_unsigned : false_result {};
    
    template<typename a_>
    struct do_no_unsigned : type_plain<a_> {};
    
    template<typename a_>
    struct do_to_unsigned : type_plain<a_> {};
    
    template<> struct do_is_unsigned<char> : bool_result<char(-1) >= char(0)> {};
    template<> struct do_no_unsigned<char> : ifel<char(-1) >= char(0), signed char, char> {};
    template<> struct do_to_unsigned<char> : ifel<char(-1) >= char(0), char, unsigned char> {};
    template<> struct do_is_unsigned<unsigned char> : true_result {};
    template<> struct do_no_unsigned<unsigned char> : type_plain<signed char> {};
    template<> struct do_to_unsigned<signed char> : type_plain<unsigned char> {};
    
    template<> struct do_is_unsigned<wchar_t> : bool_result<wchar_t(-1) >= wchar_t(0)> {};
    
    template<> struct do_is_unsigned<unsigned short> : true_result {};
    template<> struct do_no_unsigned<unsigned short> : type_plain<short> {};
    template<> struct do_to_unsigned<short> : type_plain<unsigned short> {};
    
    template<> struct do_is_unsigned<unsigned int> : true_result {};
    template<> struct do_no_unsigned<unsigned int> : type_plain<int> {};
    template<> struct do_to_unsigned<int> : type_plain<unsigned int> {};
    
    template<> struct do_is_unsigned<unsigned long> : true_result {};
    template<> struct do_no_unsigned<unsigned long> : type_plain<long> {};
    template<> struct do_to_unsigned<long> : type_plain<unsigned long> {};
    
    template<> struct do_is_unsigned<unsigned long long> : true_result {};
    template<> struct do_no_unsigned<unsigned long long> : type_plain<long long> {};
    template<> struct do_to_unsigned<long long> : type_plain<unsigned long long> {};
    
    template<> struct do_is_unsigned<char16_t> : true_result {};
    template<> struct do_is_unsigned<char32_t> : true_result {};
    template<> struct do_is_unsigned<char8_or_not> : true_result {};
    
}

// result true if type_ is a built-in unsigned integer type (not bool)
// - is_unsigned<unsigned int>::result is true
// - is_unsigned<int>::result is false
// - is_unsigned<char>::result is true if char is unsigned, else false
// - is_unsigned<wchar_t>::result is true if wchar_t is unsigned, else false
//
// note:
// - char, signed char and unsigned char are 3 different types
//   (unlike int, signed int, unsigned int where int and signed int
//   are the same type)
// - there is no "unsigned wchar_t", but wchar_t has the exact same
//   representation as one of the other built-in integer types, there
//   is always an unsigned or signed type with the same number of
//   bits as wchar_t

template<typename type_>
struct is_unsigned :
    _::do_is_unsigned<typename type<type_>::result>
{};

// If type_ is a built-in unsigned integer type, and a signed version of the same type exists, result
// that signed type. else result is type_
// - no_unsigned<unsigned int>::result is int
// - no_unsigned<int>::result is int
// - no_unisgned<char>::result is...
//   - signed char if char is unsigned
//   - char if char is signed
// - no_unsigned<wchar_t>::result is wchar_t. there is no "unsigned wchar_t"

template<typename type_>
struct no_unsigned :
    _::do_no_unsigned<typename type<type_>::result>
{};

// If type_ is a built-in signed integer type, and a unsigned version of the same type exists, result
// that unsigned type. Else result is type_
// - to_unsigned<unsigned int>::result is unsigned int
// - to_unsigned<int>::result is unsigned int
// - to_unisgned<char>::result is...
//   - unsigned char if char is signed
//   - char if char is unsigned
// - to_unsigned<wchar_t>::result is wchar_t. there is no "unsigned wchar_t"

template<typename type_>
struct to_unsigned :
    _::do_to_unsigned<typename type<type_>::result>
{};

}}
#endif
