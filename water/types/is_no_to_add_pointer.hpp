// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_TYPES_IS_NO_TO_ADD_POINTER_HPP
#define WATER_TYPES_IS_NO_TO_ADD_POINTER_HPP
#include <water/types/result.hpp>
namespace water { namespace types {

namespace _ {
	template<typename> struct do_is_pointer : false_result {};
	template<typename a_> struct do_is_pointer<a_*> : true_result {};
	
	template<typename a_> struct do_no_pointer : type_plain<a_> {};
	template<typename a_> struct do_no_pointer<a_*> : type_plain<a_> {};
	
	template<typename a_> struct do_to_pointer : type_plain<a_*> {};
	template<typename a_> struct do_to_pointer<a_*> : type_plain<a_*> {};
	template<typename a_> struct do_to_pointer<a_&> : type_plain<a_*> {};
	template<typename a_> struct do_to_pointer<a_&&> : type_plain<a_*> {};
	
	template<typename a_> struct do_add_pointer : type_plain<a_*> {};
	template<typename a_> struct do_add_pointer<a_&> : type_plain<a_*> {};
	template<typename a_> struct do_add_pointer<a_&&> : type_plain<a_*> {};
	}

// result true for any_type*
// - is_pointer<int>::result is false
// - is_pointer<int*>::result is true
// - is_pointer<int*&>::result is false
// - is_pointer<int (*)(int)>::result is true
// - is_pointer<int class_type::*>::result is false
// - is_pointer<int (class_type::*)(int)>::result is false
template<typename type_> struct
 is_pointer :
	_::do_is_pointer<typename type<type_>::result>
		{};

// if type_ is a pointer, result type_ without pointer, else type_
// - no_pointer<int*>::result is any_type
// - no_pointer<int*&>::result is any_type*&
// - no_pointer<int*const>::result is any_type*const
// - no_pointer<int**>::result is int*
// - no_pointer<int (*)(int)>::result is int(int)
template<typename type_> struct
 no_pointer :
	_::do_no_pointer<typename type<type_>::result>
		{};

// if type_ is not a pointer, result type_*, else result type_
// - to_pointer<int>::result is int*
// - to_pointer<int&>::result is int*
// - to_pointer<int*>::result is int*
// - to_pointer<int *const>::result is int *const*
template<typename type_> struct
 to_pointer :
	_::do_to_pointer<typename type<type_>::result>
		{};

// result type_*
// - add_pointer<int>::result is int*
// - add_pointer<int&>::result is int*
// - add_pointer<int*>::result is int**
template<typename type_> struct
 add_pointer :
	_::do_add_pointer<typename type<type_>::result>
		{};

}}
#endif
