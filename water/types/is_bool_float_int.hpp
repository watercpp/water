// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_TYPES_IS_BOOL_FLOAT_INT_HPP
#define WATER_TYPES_IS_BOOL_FLOAT_INT_HPP
#include <water/types/is_bool.hpp>
#include <water/types/is_float.hpp>
#include <water/types/is_int.hpp>
namespace water { namespace types {

template<typename type_> struct
 is_bool_float_int :
	bool_result<
		is_bool<type_>::result ||
		is_float<type_>::result ||
		is_int<type_>::result
		> {};

}}
#endif
