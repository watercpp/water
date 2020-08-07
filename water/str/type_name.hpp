// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_STR_TYPE_NAME_HPP
#define WATER_STR_TYPE_NAME_HPP
#include <water/str/out.hpp>
#include <water/type_name.hpp>
namespace water { namespace str {

/*

out << type_name<int>();

*/

template<typename type_>
struct type_name_do : callback<type_name_do<type_>> {
    template<typename o_>
    void operator()(out<o_>& o) {
        type_name_to<type_>(o);
    }
};

template<typename type_>
type_name_do<type_> type_name() {
    return {};
}

template<typename type_>
type_name_do<type_> type_name(type_ const&) {
    return {};
}

}}
#endif
