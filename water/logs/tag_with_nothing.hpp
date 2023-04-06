// Copyright 2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_LOGS_TAG_WITH_NOTHING_HPP
#define WATER_LOGS_TAG_WITH_NOTHING_HPP
#include <water/logs/bits.hpp>
#include <water/str/out.hpp>
namespace water { namespace logs {


struct tag_with_nothing {};


template<typename o_>
str::out<o_>& operator<<(str::out<o_>& o, tag_with_nothing) {
    return o;
}


}}
#endif
