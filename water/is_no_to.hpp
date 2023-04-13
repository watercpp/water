// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_IS_NO_TO_HPP
#define WATER_IS_NO_TO_HPP
#include <water/types.hpp>
#include <water/is_no_to/is_char.hpp>
#include <water/is_no_to/is_class_struct_union.hpp>
#include <water/is_no_to/is_enum.hpp>
#include <water/is_no_to/is_float.hpp>
#include <water/is_no_to/is_function.hpp>
#include <water/is_no_to/is_int.hpp>
#include <water/is_no_to/is_no_to_array.hpp>
#include <water/is_no_to/is_no_to_const.hpp>
#include <water/is_no_to/is_no_to_pointer.hpp>
#include <water/is_no_to/is_no_to_pointer_to_member.hpp>
#include <water/is_no_to/is_no_to_reference.hpp>
#include <water/is_no_to/is_no_to_unsigned.hpp>
#include <water/is_no_to/is_no_to_volatile.hpp>
namespace water {

/*

These do not depend on the standard <type_traits> or anything else, because they need to work if
WATER_NO_STD is defined and there is no standard library.

You probably want to use the standard ones instead of these.

is_something<type> will be true even if it type is const, volatile or a reference:

    is_bool<bool const&> is true
    is_const<bool const&> is true
    is_reference<bool const&> is true

is_pointer is true for const and volatile, but not references:

    is_pointer<int*> is true
    is_pointer<int *const> is true
    is_pointer<int*&> is false, its a reference

to match this, no_pointer and to_pointer will drop const and volatile from int *const volatile:
    
    no_pointer<int *const> is int
    to_pointer<int *const volatile> is int*
    to_pointer<int&> is int*
    to_pointer<int&&> is int*

is/no/to_pointer_to_member and is/no/to_pointer_to_member_function behave like is/no/to_pointer

to_pointer for array types does not decay the array to a pointer:

    to_pointer<int[123]> is int(*)[123]
    to_pointer<int const(&)[123]> is int const(*)[123]

*/

using size_t = decltype(sizeof(0));


template<typename type_>
bool constexpr is_const = is_no_to::is_const<type_>;

// no_const<int const> is int
// no_const<int const&> is int&
template<typename type_>
using no_const = typename is_no_to::no_const<type_>::result;

// to_const<int> is int const
// to_const<int&> is int const&
template<typename type_>
using to_const = typename is_no_to::to_const<type_>::result;



template<typename type_>
bool constexpr is_volatile = is_no_to::is_volatile<type_>;

// no_volatile<int volatile> is int
// no_volatile<int volatile&> is int&
template<typename type_>
using no_volatile = typename is_no_to::no_volatile<type_>::result;

// to_volatile<int> is int volatile
// to_volatile<int&> is int volatile&
template<typename type_>
using to_volatile = typename is_no_to::to_volatile<type_>::result;



// is_referece<int&> is true
// is_referece<int&&> is true
template<typename type_>
bool constexpr is_reference = is_no_to::is_reference<type_>;

// no_reference<int&> is int
// no_reference<int&&> is int
template<typename type_>
using no_reference = typename is_no_to::no_reference<type_>::result;

// to_reference<int> is int&
// to_reference<int&&> is int&
template<typename type_>
using to_reference = typename is_no_to::to_reference<type_>::result;



// is_rvalue_referece<int&&> is true
template<typename type_>
bool constexpr is_rvalue_reference = is_no_to::is_rvalue_reference<type_>;

// no_rvalue_reference<int&> is int&
// no_rvalue_reference<int&&> is int
template<typename type_>
using no_rvalue_reference = typename is_no_to::no_rvalue_reference<type_>::result;

// to_rvalue_reference<int> is int&&
// to_rvalue_reference<int&> is int&&
template<typename type_>
using to_rvalue_reference = typename is_no_to::to_rvalue_reference<type_>::result;



template<typename type_>
using no_const_or_reference = no_const<no_reference<type_>>;


// its long but shorter than water::no_const<water::no_volatile<water::no_reference<x>>>
template<typename type_>
using no_const_or_volatile_or_reference = no_const<no_volatile<no_reference<type_>>>;



template<typename type_>
using no_const_or_volatile = no_const<no_volatile<type_>>;



// type_ = bool is true
template<typename type_>
bool constexpr is_bool = equal<bool, no_const_or_volatile_or_reference<type_>>;

// true for all built in character types
template<typename type_>
bool constexpr is_char = is_no_to::is_char<no_const_or_volatile_or_reference<type_>>::result;

template<typename type_>
bool constexpr is_class_struct_union = is_no_to::is_class_struct_union<no_const_or_volatile_or_reference<type_>>;

template<typename type_>
bool constexpr is_enum = is_no_to::is_enum<no_const_or_volatile_or_reference<type_>>;

// true for float, double, long double
template<typename type_>
bool constexpr is_float = is_no_to::is_float<no_const_or_volatile_or_reference<type_>>::result;

// void(int) is true
// void(*)(int) is false (use is_function<no_pointer<...>>)
template<typename type_>
bool constexpr is_function = is_no_to::is_function<no_const_or_volatile_or_reference<type_>>;

// true for all built in integer types, including character types, but not bool
template<typename type_>
bool constexpr is_int = is_no_to::is_int<no_const_or_volatile_or_reference<type_>>::result;

template<typename type_>
bool constexpr is_void = equal<void, no_const_or_volatile_or_reference<type_>>;



// true for anything[123]
template<typename type_>
bool constexpr is_array = is_no_to::is_array<no_const_or_volatile_or_reference<type_>>;

// no_array<int[123]> is int
// no_array<int(&)[123][456]> is int(&)[456]
// no_array<int> is int
template<typename type_>
using no_array = typename is_no_to::no_array<type_>::result;

// to_array_with_size<int, 123> is int[123]
// to_array_with_size<int(&)[321], 123> is int(&)[123]
template<typename type_, size_t size_>
using to_array_with_size = typename is_no_to::to_array_with_size<type_, size_>::result;



// result true for any_type*
// is_pointer<int> is false
// is_pointer<int*> is true
// is_pointer<int *const> is true
// is_pointer<int*&> is false
// is_pointer<int (*)(int)> is true
// is_pointer<int class_type::*> is false
// is_pointer<int (class_type::*)(int)> is false
template<typename type_>
bool constexpr is_pointer = is_no_to::is_pointer<no_const_or_volatile<type_>>;

// if type_ is a pointer, result type_ without pointer, else type_
// no_pointer<int*> is any_type
// no_pointer<int*&> is any_type*&
// no_pointer<int*const> is any_type*
// no_pointer<int**> is int*
// no_pointer<int (*)(int)> is int(int)
template<typename type_>
using no_pointer = ifel<
    is_pointer<type_>,
    typename is_no_to::no_pointer<no_const_or_volatile<type_>>::result,
    type_
>;

// if type_ is not a pointer, result type_*, else result type_
// to_pointer<int> is int*
// to_pointer<int&> is int*
// to_pointer<int*> is int*
// to_pointer<int *const> is int* (to match is_pointer and no_pointer)
// to_pointer<int[123]> is int(*)[123] (it does not decay arrays to pointers)
template<typename type_>
using to_pointer = typename is_no_to::to_pointer<type_>::result;



// is_pointer_to_member<int class_type::*> is true
// is_pointer_to_member<int (class_type::*)(int)> is true
// is_pointer_to_member<int> is false
template<typename type_>
bool constexpr is_pointer_to_member = is_no_to::is_pointer_to_member<no_const_or_volatile<type_>>;


template<typename type_>
bool constexpr is_pointer_to_member_function = is_no_to::is_pointer_to_member_function<no_const_or_volatile<type_>>;


// if type_ is a pointer to member, result what it points to
// no_pointer_to_member<int class_type::*> is int
// no_pointer_to_member<int (class_type::*)(int)> is int(int)
// no_pointer_to_member<int> is int
template<typename type_>
using no_pointer_to_member = ifel<
    is_pointer_to_member<type_>,
    typename is_no_to::no_pointer_to_member<no_const_or_volatile<type_>>::result,
    type_
>;


// if type_ is...
// - not a pointer to member, result type_ member_of_::*
// - a pointer to member, change it to be a member of member_of_
//
// example
// - to_pointer_to_member<int, class_type> is int class_type::*
// - to_pointer_to_member<int(int), class_type> is int (class_type::*)(int)
// - to_pointer_to_member<int other::*, class_type> is int class_type::*
//
template<typename type_, typename member_of_>
using to_pointer_to_member = typename is_no_to::to_pointer_to_member<type_, member_of_>::result;


// if type_ is a pointer to member, result what it is a member of. else void
// pointer_to_member_of<int class_type::*> is class_type
// pointer_to_member_of<int (class_type::*)(int)> is class_type
// pointer_to_member_of<int> is void
template<typename type_>
using pointer_to_member_of = typename is_no_to::pointer_to_member_of<no_const_or_volatile<type_>>::result;






// true if type_ is a built in unsigned integer or character type (not bool)
//
// remember that char, signed char and unsigned char are 3 different types (unlike signed int and int)
template<typename type_>
bool constexpr is_unsigned = is_no_to::is_unsigned<no_const_or_volatile_or_reference<type_>>::result;


// if is_unsigned<type_> is true, and there is a signed version of the same type, this is that type.
// otherwise its type_ unchanged
// no_unsigned<unsigned short> is short
// no_unsigned<wchar_t> is wchar_t even if wchar_t is unsigned
// no_unsigned<float> is float
template<typename type_>
using no_unsigned = typename is_no_to::no_unsigned<type_>::result;


// if is_unsigned<type_> is false, and there is a unsigned version of the same type, this is that type.
// otherwise its type_ unchanged
// no_unsigned<short> is unsigned short
// no_unsigned<wchar_t> is wchar_t even if wchar_t is signed
// no_unsigned<float> is float
template<typename type_>
using to_unsigned = typename is_no_to::to_unsigned<type_>::result;



// array_size<int[123]>() is 123
// array_size<int(&)[123][456]>() is 123
// array_size<int>() is 0

template<typename type_>
constexpr size_t array_size() {
    return is_no_to::array_size<no_reference<type_>>;
}

template<typename type_>
constexpr size_t array_size(type_ const&) {
    return array_size<type_>();
}


}
#endif
