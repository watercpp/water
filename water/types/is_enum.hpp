// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_TYPES_IS_ENUM_HPP
#define WATER_TYPES_IS_ENUM_HPP
#include <water/types/is_bool_float_int.hpp>
#include <water/types/is_void.hpp>
#include <water/types/is_class_struct_union.hpp>
#include <water/types/to_void.hpp>
namespace water { namespace types {
namespace _ {
	template<typename a_, typename = void> struct
	 do_is_enum_test : false_result
	 	{};
	template<typename a_> struct
	 do_is_enum_test<a_, to_void<decltype(static_cast<int>(types::make<a_ const&>()))>> : true_result
	 	{};
	
	template<typename a_, bool select_ = is_bool_float_int<a_>::result || is_void<a_>::result || is_class_struct_union<a_>::result> struct
	 do_is_enum : false_result
		{};
	template<typename a_> struct
	 do_is_enum<a_, false> : do_is_enum_test<a_>
		{};
	template<typename a_> struct
	 do_is_enum<a_&, false> : false_result
		{};
	template<typename a_> struct
	 do_is_enum<a_&&, false> : false_result
		{};
	template<typename a_> struct
	 do_is_enum<a_ const, false> : false_result
		{};
	template<typename a_> struct
	 do_is_enum<a_ const volatile, false> : false_result
		{};
	template<typename a_> struct
	 do_is_enum<a_ volatile, false> : false_result
		{};
	}

template<typename type_> struct
 is_enum :
	_::do_is_enum<typename type<type_>::result>
		{};

}}
#endif
