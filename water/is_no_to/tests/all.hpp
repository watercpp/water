// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_IS_NO_TO_TESTS_ALL_HPP
#define WATER_IS_NO_TO_TESTS_ALL_HPP
#include <water/is_no_to.hpp>
#include <water/types.hpp>
namespace water { namespace is_no_to_tests {

/*

notice odd namespace to avoid using the things in water::is_no_to 

*/

class class_type;

union union_type { int i; float f; };

enum enum_basic { enum_basic_1 = 1 };

enum enum_type : unsigned long long { enum_type_1 = 1 };

enum class enum_class : unsigned long long { value = 1 };



// no_const_or_reference

static_assert(equal<no_const_or_reference<int>, int>, "");
static_assert(equal<no_const_or_reference<int&>, int>, "");
static_assert(equal<no_const_or_reference<int&&>, int>, "");
static_assert(equal<no_const_or_reference<int const>, int>, "");
static_assert(equal<no_const_or_reference<int const&>, int>, "");
static_assert(equal<no_const_or_reference<int const&&>, int>, "");
static_assert(equal<no_const_or_reference<int volatile>, int volatile>, "");



// const

static_assert(!is_const<int>, "");
static_assert(is_const<int const>, "");
static_assert(is_const<int const&>, "");
static_assert(is_const<int const&&>, "");

static_assert(equal<to_const<int>, int const>, "");
static_assert(equal<to_const<int&>, int const&>, "");
static_assert(equal<to_const<int&&>, int const&&>, "");
static_assert(equal<to_const<int const&>, int const&>, "");
static_assert(equal<to_const<int const&&>, int const&&>, "");

static_assert(equal<no_const<int>, int>, "");
static_assert(equal<no_const<int const>, int>, "");
static_assert(equal<no_const<int const&>, int&>, "");
static_assert(equal<no_const<int const&&>, int&&>, "");
static_assert(equal<no_const<int const volatile>, int volatile>, "");



// bool

static_assert(is_bool<bool const&>, "");
static_assert(!is_bool<bool*>, "");



// char

static_assert(is_char<char const&>, "");
static_assert(is_char<wchar_t&>, "");
static_assert(is_char<char32_t>, "");
static_assert(!is_char<int>, "");



// float

static_assert(is_float<long double const&>, "");
static_assert(is_float<float&&>, "");
static_assert(!is_float<int>, "");



// int

static_assert(is_int<int const&>, "");
static_assert(is_int<signed char&&>, "");
static_assert(is_int<unsigned long long>, "");
static_assert(!is_int<float>, "");



// unsigned

static_assert(!is_unsigned<int const&>, "");
static_assert(is_unsigned<unsigned int const&>, "");
static_assert(is_unsigned<unsigned int const volatile&&>, "");
static_assert(equal<no_unsigned<unsigned short>, short>, "");
static_assert(equal<no_unsigned<long>, long>, "");
static_assert(equal<no_unsigned<unsigned short const volatile&>, short const volatile&>, "");
static_assert(equal<to_unsigned<unsigned>, unsigned>, "");
static_assert(equal<to_unsigned<long>, unsigned long>, "");
static_assert(equal<to_unsigned<long const volatile&&>, unsigned long const volatile&&>, "");



// class struct union

static_assert(is_class_struct_union<class_type>, "");
static_assert(is_class_struct_union<union_type>, "");
static_assert(!is_class_struct_union<int>, "");
static_assert(!is_class_struct_union<enum_class>, "");
static_assert(is_class_struct_union<class_type&&>, "");
static_assert(is_class_struct_union<union_type const&>, "");



// enum

static_assert(is_enum<enum_basic>, "");
static_assert(is_enum<enum_type>, "");
static_assert(is_enum<enum_class>, "");
static_assert(is_enum<enum_class&&>, "");
static_assert(is_enum<enum_type const>, "");
static_assert(is_enum<enum_basic const volatile&>, "");
static_assert(!is_enum<void>, "");
static_assert(!is_enum<int>, "");
static_assert(!is_enum<float>, "");
static_assert(!is_enum<bool>, "");
static_assert(!is_enum<class_type>, "");
static_assert(!is_enum<int class_type::*>, "");
static_assert(!is_enum<int*>, "");



// function

static_assert(!is_function<void>, "");
static_assert(is_function<void()>, "");
static_assert(!is_function<int(*)()>, "");
static_assert(is_function<no_pointer<int(*)(int)>>, "");
static_assert(is_function<int(int, int, int, int, int, int, int, int, int, int, int, int, int, int, int)>, "");
static_assert(is_function<int(...)>, "");
static_assert(is_function<int(int, ...)>, "");
static_assert(is_function<int(int, int, int, int, int, int, int, int, int, int, int, int, int, int, ...)>, "");



// pointer

static_assert(is_pointer<int*>, "");
static_assert(is_pointer<int* const>, "");
static_assert(!is_pointer<int*&>, "");
static_assert(!is_pointer<int*&&>, "");
static_assert(!is_pointer<int*const&>, "");
static_assert(!is_pointer<int*&>, "");

static_assert(equal<no_pointer<int*>, int>, "");
static_assert(equal<no_pointer<int* const>, int>, "");
static_assert(equal<no_pointer<int*&>, int*&>, "");

static_assert(equal<to_pointer<int*>, int*>, "");
static_assert(equal<to_pointer<int* const>, int*>, "");
static_assert(equal<to_pointer<int*&>, int**>, "");
static_assert(equal<to_pointer<int[123]>, int(*)[123]>, "");



// pointer to member

static_assert(is_pointer_to_member<int class_type::*>, "");
static_assert(is_pointer_to_member<int class_type::*const>, "");
static_assert(is_pointer_to_member<int (class_type::*)()>, "");
static_assert(is_pointer_to_member<int (class_type::*const)()>, "");
static_assert(!is_pointer_to_member<int class_type::*&>, "");
static_assert(!is_pointer_to_member<int class_type::*const&>, "");
static_assert(!is_pointer_to_member<int (class_type::*&)()>, "");
static_assert(!is_pointer_to_member<int (class_type::*const&)()>, "");
static_assert(!is_pointer_to_member<int*>, "");
static_assert(is_pointer_to_member<int (class_type::*)(int)>, "");
static_assert(is_pointer_to_member<int (class_type::*)(int, ...)>, "");

static_assert(equal<no_pointer_to_member<int class_type::*>, int>, "");
static_assert(equal<no_pointer_to_member<int (class_type::*)(int)>, int(int)>, "");
static_assert(equal<no_pointer_to_member<int (class_type::*)(int, ...)>, int(int, ...)>, "");
static_assert(equal<no_pointer_to_member<int (class_type::*)(...)>, int(...)>, "");

static_assert(equal<pointer_to_member_of<int class_type::*>, class_type>, "");

static_assert(equal<to_pointer_to_member<int, class_type>, int class_type::*>, "");
static_assert(equal<to_pointer_to_member<int(int), class_type>, int (class_type::*)(int)>, "");
static_assert(equal<to_pointer_to_member<int(int, ...), class_type>, int (class_type::*)(int, ...)>, "");
static_assert(equal<to_pointer_to_member<int(union_type::*)(int), class_type>, int (class_type::*)(int)>, "");
static_assert(equal<to_pointer_to_member<int(union_type::*)(int, ...), class_type>, int (class_type::*)(int, ...)>, "");

static_assert(!is_pointer_to_member_function<void>, "");
static_assert(!is_pointer_to_member_function<void(*)()>, "");
static_assert(is_pointer_to_member_function<int (class_type::*)()>, "");
static_assert(is_pointer_to_member_function<int (class_type::*)(int)>, "");
static_assert(is_pointer_to_member_function<int (class_type::*)(int, int, int, int, int, int, int, int, int, int, int, int, int, int, int)>, "");
static_assert(is_pointer_to_member_function<int (class_type::*)(...)>, "");
static_assert(is_pointer_to_member_function<int (class_type::*)(int, int, int, int, int, int, int, int, int, int, int, int, int, int, ...)>, "");



// reference + rvalue_reference

static_assert(is_reference<int&>, "");
static_assert(is_reference<int&&>, "");
static_assert(!is_reference<int>, "");
static_assert(!is_reference<int*>, "");
static_assert(!is_rvalue_reference<int&>, "");
static_assert(is_rvalue_reference<int&&>, "");
static_assert(!is_rvalue_reference<int>, "");

static_assert(equal<no_reference<int&>, int>, "");
static_assert(equal<no_reference<int&&>, int>, "");
static_assert(equal<no_reference<int>, int>, "");

static_assert(equal<to_reference<int&>, int&>, "");
static_assert(equal<to_reference<int&&>, int&>, "");
static_assert(equal<to_reference<int>, int&>, "");

static_assert(equal<no_rvalue_reference<int&>, int&>, "");
static_assert(equal<no_rvalue_reference<int&&>, int>, "");
static_assert(equal<no_rvalue_reference<int>, int>, "");

static_assert(equal<to_rvalue_reference<int&>, int&&>, "");
static_assert(equal<to_rvalue_reference<int&&>, int&&>, "");
static_assert(equal<to_rvalue_reference<int>, int&&>, "");



// void

static_assert(is_void<void const>, "");
static_assert(!is_void<void*>, "");



// volatile

static_assert(!is_volatile<int>, "");
static_assert(is_volatile<int volatile>, "");
static_assert(is_volatile<int volatile&>, "");
static_assert(is_volatile<int volatile&&>, "");

static_assert(equal<to_volatile<int>, int volatile>, "");
static_assert(equal<to_volatile<int&>, int volatile&>, "");
static_assert(equal<to_volatile<int&&>, int volatile&&>, "");
static_assert(equal<to_volatile<int volatile&>, int volatile&>, "");
static_assert(equal<to_volatile<int volatile&&>, int volatile&&>, "");

static_assert(equal<no_volatile<int>, int>, "");
static_assert(equal<no_volatile<int volatile>, int>, "");
static_assert(equal<no_volatile<int volatile&>, int&>, "");
static_assert(equal<no_volatile<int volatile&&>, int&&>, "");
static_assert(equal<no_volatile<int volatile const>, int const>, "");



// array

static_assert(is_array<int[123]>, "");
static_assert(is_array<int const(&)[123]>, "");
static_assert(!is_array<int>, "");

static_assert(equal<no_array<int const(&)[123]>, int const&>, "");
static_assert(equal<no_array<int[123][456]>, int[456]>, "");
static_assert(equal<no_array<int>, int>, "");

static_assert(equal<to_array_with_size<int const(&)[123], 456>, int const(&)[456]>, "");
static_assert(equal<to_array_with_size<int[123][456], 789>, int[789][456]>, "");
static_assert(equal<to_array_with_size<int, 123>, int[123]>, "");


static_assert(array_size<int const(&)[123][456]>() == 123, "");
static_assert(array_size<int>() == 0, "");



}}
#endif
