// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_IS_NO_TO_IS_ENUM_HPP
#define WATER_IS_NO_TO_IS_ENUM_HPP
#include <water/is_no_to/is_class_struct_union.hpp>
namespace water { namespace is_no_to {

/*

static_cast<int>(enum) always works

operator= cannot be overloaded for enums

*/

template<typename a_, typename = void>
bool constexpr is_enum_2 = true;

template<typename a_>
bool constexpr is_enum_2<a_, to_void<decltype(make_type<a_&>() = 0)>> = false;

template<typename a_, bool = is_class_struct_union<a_>, typename = void>
bool constexpr is_enum = false;

template<typename a_>
bool constexpr is_enum<a_, false, to_void<decltype(static_cast<int>(make_type<a_>()))>> = is_enum_2<a_>;

}}
#endif
