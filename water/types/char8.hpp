// Copyright 2021 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_TYPES_CHAR8_HPP
#define WATER_TYPES_CHAR8_HPP
#include <water/types/result.hpp>
namespace water { namespace types {

struct no_char8;

namespace _ {

    template<typename char8_ = decltype(*u8"")> struct char8_select;
    template<typename char8_> struct char8_select<char8_ const&> : type_plain<char8_> {};
    template<> struct char8_select<char const&> : type_plain<no_char8> {};

}

using char8_or_not = _::char8_select<>::result;

}}
#endif
