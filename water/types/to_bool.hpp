// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_TYPES_TO_BOOL_HPP
#define WATER_TYPES_TO_BOOL_HPP
#include <water/types/result.hpp>
namespace water { namespace types {

/*

Result true or false
- if a_ is not a result, result false
- if a_ is a result, and the result_kind has to_bool<a_>, result that
- if a_ is a result, and the result_kind does not have to_bool, result true

to_bool<void>::result is false (void is not a result)
to_bool<type<void> >::result is true
to_bool<nontype<int, 0>> is false, nontypes values will be converted to bool 

If the result_kind does not have to_bool, this results true. Otherwise its result is result_kind::to_bool

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
