// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_TYPES1_IS_FLOAT_HPP
#define WATER_TYPES1_IS_FLOAT_HPP
#include <water/types1/result.hpp>
namespace water { namespace types1 {
namespace _ {

    template<typename>
    struct do_is_float : false_result {};
    
    template<> struct do_is_float<float> : true_result {};
    template<> struct do_is_float<double> : true_result {};
    template<> struct do_is_float<long double> : true_result {};
    
}

template<typename type_>
struct is_float :
    _::do_is_float<typename type<type_>::result>
{};

}}
#endif
