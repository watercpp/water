// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_IS_NO_TO_IS_FUNCTION_HPP
#define WATER_IS_NO_TO_IS_FUNCTION_HPP
#include <water/is_no_to/bits.hpp>
namespace water { namespace is_no_to {


template<typename a_>
bool constexpr is_function = false;

template<typename r_>
bool constexpr is_function<r_()> = true;

template<typename r_, typename ...a_>
bool constexpr is_function<r_(a_...)> = true;

template<typename r_, typename ...a_>
bool constexpr is_function<r_(a_..., ...)> = true;


}}
#endif
