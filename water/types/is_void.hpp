// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_TYPES_IS_VOID_HPP
#define WATER_TYPES_IS_VOID_HPP
#include <water/types/equal.hpp>
namespace water { namespace types {

template<typename type_>
struct is_void :
    equal_plain<typename type<type_>::result, void>
{};

}}
#endif
