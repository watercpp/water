// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_TYPES_ANY_TYPE_HPP
#define WATER_TYPES_ANY_TYPE_HPP
#include <water/types/any_result.hpp>
namespace water { namespace types {

/*

If type_ is a result of some kind, result kind::type<type_>, else results type_

any_type<int>::result is int
any_type<nontype<int, 123>>::result is int

The result_kind must have:

    struct some_kind {
        template<typename a_> struct type;
    };

*/

template<typename type_>
struct any_type :
    result_kind<any_result<type_> >::template type<any_result<type_> >
{};

}}
#endif
