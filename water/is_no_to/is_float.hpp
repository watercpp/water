// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_IS_NO_TO_IS_FLOAT_HPP
#define WATER_IS_NO_TO_IS_FLOAT_HPP
#include <water/is_no_to/bits.hpp>
namespace water { namespace is_no_to {


template<typename> struct is_float : result_false {};

template<> struct is_float<float> : result_true {};
template<> struct is_float<double> : result_true {};
template<> struct is_float<long double> : result_true {};


}}
#endif
