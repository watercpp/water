// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_TYPES_IS_CLASS_STRUCT_UNION_HPP
#define WATER_TYPES_IS_CLASS_STRUCT_UNION_HPP
#include <water/types/result.hpp>
namespace water { namespace types {
namespace _ {
	template<typename a_> true_size is_class_struct_union_test(int a_::*);
	template<typename a_> false_size is_class_struct_union_test(...);
	template<typename a_> struct
	 do_is_class_struct_union : bool_result<
		sizeof(_::is_class_struct_union_test<a_>(0)) == sizeof(true)
		> {};
	template<typename a_> struct
	 do_is_class_struct_union<a_&> : false_result
		{};
	template<typename a_> struct
	 do_is_class_struct_union<a_&&> : false_result
		{};
	template<typename a_> struct
	 do_is_class_struct_union<a_ const> : false_result
		{};
	template<typename a_> struct
	 do_is_class_struct_union<a_ const volatile> : false_result
		{};
	template<typename a_> struct
	 do_is_class_struct_union<a_ volatile> : false_result
		{};
	}

template<typename type_> struct
 is_class_struct_union :
	_::do_is_class_struct_union<typename type<type_>::result>
		{};

}}
#endif
