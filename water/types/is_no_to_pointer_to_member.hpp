// Copyright 2017-2018 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_TYPES_IS_NO_TO_POINTER_TO_MEMBER_HPP
#define WATER_TYPES_IS_NO_TO_POINTER_TO_MEMBER_HPP
#include <water/types/result.hpp>
namespace water { namespace types {

namespace _ {
	// visual c++ 2015 building 32-bit x86, not 64-bit or 2017, needs the specializations for functions with ...
	// probably related to error C2217: '...' requires '__cdecl'

	template<typename> struct do_is_pointer_to_member : false_result {};
	template<typename a_, typename b_> struct do_is_pointer_to_member<a_ b_::*> : true_result {};
	//template<typename a_, typename b_, typename ...c_> struct do_is_pointer_to_member<a_ (b_::*)(c_...)> : true_result {};
	template<typename a_, typename b_, typename ...c_> struct do_is_pointer_to_member<a_ (b_::*)(c_..., ...)> : true_result {};
	
	template<typename a_> struct do_no_pointer_to_member : type_plain<a_> {};
	template<typename a_, typename b_> struct do_no_pointer_to_member<a_ b_::*> : type_plain<a_> {};
	//template<typename a_, typename b_, typename ...c_> struct do_no_pointer_to_member<a_ (b_::*)(c_...)> : type_plain<a_(c_...)> {};
	template<typename a_, typename b_, typename ...c_> struct do_no_pointer_to_member<a_ (b_::*)(c_..., ...)> : type_plain<a_(c_..., ...)> {};
	
	template<typename a_, typename b_> struct do_to_pointer_to_member : type_plain<a_ b_::*> {};
	template<typename a_, typename b_, typename ...c_> struct do_to_pointer_to_member<a_(c_..., ...), b_> : type_plain<a_ (b_::*)(c_..., ...)> {};
	template<typename a_, typename b_, typename c_> struct do_to_pointer_to_member<a_ b_::*, c_> : type_plain<a_ c_::*> {};
	template<typename a_, typename b_, typename c_, typename ...d_> struct do_to_pointer_to_member<a_ (b_::*)(d_..., ...), c_> : type_plain<a_ (c_::*)(d_..., ...)> {};
	
	template<typename a_> struct do_pointer_to_member_of : type_plain<void> {};
	template<typename a_, typename b_> struct do_pointer_to_member_of<a_ b_::*> : type_plain<b_> {};
	//template<typename a_, typename b_, typename ...c_> struct do_pointer_to_member_of<a_ (b_::*)(c_...)> : type_plain<b_> {};
	template<typename a_, typename b_, typename ...c_> struct do_pointer_to_member_of<a_ (b_::*)(c_..., ...)> : type_plain<b_> {};
	}
//
// result true if type_ is a pointer to member
// - is_pointer_to_member<int class_type::*>::result is true
// - is_pointer_to_member<int (class_type::*)(int)>::result is true
// - is_pointer_to_member<int>::result is false
//
// use types::is_pointer_to_member_function to find out if it's a member
// function pointer or not
template<typename type_> struct
 is_pointer_to_member :
	_::do_is_pointer_to_member<typename type<type_>::result>
		{};

// if type_ is a pointer to member, result what it points to
// - no_pointer_to_member<int class_type::*>::result is int
// - no_pointer_to_member<int (class_type::*)(int)>::result is int(int)
// - no_pointer_to_member<int>::result is int
template<typename type_> struct
 no_pointer_to_member :
	_::do_no_pointer_to_member<typename type<type_>::result>
		{};

// if type_ is...
// - not a pointer to member, result type_ member_of_::*
// - a pointer to member, change it to be a member of member_of_
//
// example
// - to_pointer_to_member<int, class_type>::result is int class_type::*
// - to_pointer_to_member<int(int), class_type>::result is int (class_type::*)(int)
// - to_pointer_to_member<int other::*, class_type>::result is int class_type::*
// 
template<typename type_, typename member_of_> struct
 to_pointer_to_member :
	_::do_to_pointer_to_member<
		typename type<type_>::result,
		typename type<member_of_>::result
		> {};
	
// if type_ is a pointer to member, result what it is a member of. else void
// - pointer_to_member_of<int class_type::*>::result is class_type
// - pointer_to_member_of<int (class_type::*)(int)>::result is class_type
// - pointer_to_member_of<int>::result is void
template<typename type_> struct
 pointer_to_member_of :
	_::do_pointer_to_member_of<typename type<type_>::result>
		{};

}}
#endif

