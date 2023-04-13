// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_TYPES1_IS_BOOL_HPP
#define WATER_TYPES1_IS_BOOL_HPP
#include <water/types1/equal.hpp>
namespace water { namespace types1 {

template<typename type_>
struct is_bool :
    equal_plain<typename type<type_>::result, bool>
    {};

}}
#endif
