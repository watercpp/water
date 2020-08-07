// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_LOGS_BITS_HPP
#define WATER_LOGS_BITS_HPP
#include <water/water.hpp>
#include <water/types/types.hpp>
namespace water { namespace logs {

namespace _ {

    template<typename range_, typename result_, typename = decltype(types::make<range_&>().begin() == types::make<range_&>().end())>
    struct if_range_do :
        types::type_plain<result_>
    {};

}

template<typename range_, typename result_>
using if_range = typename _::if_range_do<range_, result_>::result;

}}
#endif
