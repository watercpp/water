// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_TYPES_IS_NO_TO_ADD_ARRAY_HPP
#define WATER_TYPES_IS_NO_TO_ADD_ARRAY_HPP
#include <water/types/result.hpp>
namespace water { namespace types {

namespace _ {
	// array rvalue references dont exist?
	
	template<typename> struct do_is_array : false_result {};
	template<typename a_, size_t b_> struct do_is_array<a_[b_]> : true_result {};
	template<typename a_, size_t b_> struct do_is_array<a_(&)[b_]> : true_result {};
	template<typename a_, size_t b_> struct do_is_array<a_(&&)[b_]> : true_result {};
	
	template<typename a_> struct do_no_array : type_plain<a_> {};
	template<typename a_, size_t b_> struct do_no_array<a_[b_]> : type_plain<a_> {};
	template<typename a_, size_t b_> struct do_no_array<a_(&)[b_]> : type_plain<a_&> {};
	template<typename a_, size_t b_> struct do_no_array<a_(&&)[b_]> : type_plain<a_&&> {};
	
	template<typename a_, size_t b_> struct do_to_array : type_plain<a_[b_]> {};
	template<typename a_, size_t b_> struct do_to_array<a_&, b_> : type_plain<a_(&)[b_]> {};
	template<typename a_, size_t b_> struct do_to_array<a_&&, b_> : type_plain<a_(&&)[b_]> {};
	template<typename a_, size_t b_, size_t c_> struct do_to_array<a_[b_], c_> : type_plain<a_[c_]> {};
	template<typename a_, size_t b_, size_t c_> struct do_to_array<a_(&)[b_], c_> : type_plain<a_(&)[c_]> {};
	template<typename a_, size_t b_, size_t c_> struct do_to_array<a_(&&)[b_], c_> : type_plain<a_(&&)[c_]> {};
	
	template<typename a_, size_t b_> struct do_add_array : type_plain<a_[b_]> {};
	template<typename a_, size_t b_> struct do_add_array<a_&, b_> : type_plain<a_(&)[b_]> {};
	template<typename a_, size_t b_> struct do_add_array<a_&&, b_> : type_plain<a_(&&)[b_]> {};
	
	template<typename> struct do_array_size : integer<size_t, 0> {};
	template<typename a_, size_t b_> struct do_array_size<a_[b_]> : integer<size_t, b_> {};
	template<typename a_, size_t b_> struct do_array_size<a_(&)[b_]> : integer<size_t, b_> {};
	template<typename a_, size_t b_> struct do_array_size<a_(&&)[b_]> : integer<size_t, b_> {};
	}

// results true for
// - any_type[123]
// - any_type(&)[123]
template<typename type_> struct
 is_array :
	_::do_is_array<typename type<type_>::result>
		{};

// - no_array<int[123]>::result is int
// - no_array<int(&)[123][456]>::result is int(&)[123]
// - no_array<int>::result is int
template<typename type_> struct
 no_array :
	_::do_no_array<typename type<type_>::result>
		{};

// makes a array type of a non-array type, or changes the size of an array
// - to_array<int, 123>::result is int[123]
// - to_array<int(&)[321], 123>::result is int(&)[123]
template<typename type_, size_t size_> struct
 to_array :
	_::do_to_array<typename type<type_>::result, size_>
		{};

// result type_[size_]
// - add_array<int, 123>::result is int[123]
// - add_array<int(&)[456], 123>::result is int(&)[123][456]
template<typename type_, size_t size_> struct
 add_array :
	_::do_add_array<typename type<type_>::result, size_>
		{};

// result number of elements in array (types::size_t),
// 0 if not types::is_array<type_>::result
// - array_size<int[123]>::result is 123
// - array_size<int(&)[123][456]>::result is 123
// - array_size<int>::result is 0
template<typename type_> struct
 array_size :
	_::do_array_size<typename type<type_>::result>
		{};

}}
#endif
