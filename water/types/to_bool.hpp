// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_TYPES_TO_BOOL_HPP
#define WATER_TYPES_TO_BOOL_HPP
#include <water/types/result.hpp>
#include <water/types/to_void.hpp>
namespace water { namespace types {

/*

result true or false
- if a_ is not a result, result false
- if a_ is a result, and the result_kind has to_bool<a_>, result that
- if a_ is a result, and the result_kind does not have to_bool, result true

types::integer_kind has to_bool, it is true if the result is non-zero
- to_bool<integer<int, 123> >::result is true
- to_bool<integer<int, 0> >::result is false
- to_bool<enum_result<123> >::result is true

types::type_kind does not have to_bool, it is true for any result
- to_bool<type<void> >::result is true
- to_bool<type_plain<int> >::result is true

if a_ is not a result, it is always false
- to_bool<int>::result is false

the result_kind should have

    struct some_kind {
        template<typename a_> struct to_bool;
    };

*/

namespace _ {

    template<typename a_, typename k_ = typename result_kind_of<a_>::result, typename x_ = void>
    struct do_to_bool : true_result
    {};

    template<typename a_>
    struct do_to_bool<a_, void, void> : false_result
    {};

    template<typename a_, typename k_>
    struct do_to_bool<a_, k_, to_void<typename k_::template to_bool<void> >> :
        k_::template to_bool<a_>
    {};

}

template<typename a_>
struct to_bool : _::do_to_bool<a_>
{};

}}
#endif
