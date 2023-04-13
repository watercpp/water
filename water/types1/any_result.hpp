// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_TYPES1_ANY_RESULT_HPP
#define WATER_TYPES1_ANY_RESULT_HPP
#include <water/types1/result.hpp>
namespace water { namespace types1 {

/*

any_result<a_>
- if a_ is a result, result that
- if a_ is types1::nothing, no result
- if a_ has result_tag that is a typedef of result_tag<void>, no result
- else result a_

example
- any_result<int>::result is int
- any_result<type<int> >::result is int
- any_result<enum_result<123> >::result is 123
- any_result<> has no result
- any_result<any_result<>> has no result

*/

namespace _ {

    template<typename t_>
    struct do_any_result_to_void;
    
    template<typename k_>
    struct do_any_result_to_void<result_tag<k_> > {
        typedef void result;
    };
    
    template<typename r_, typename x_ = void>
    struct do_any_result :
        type_plain<r_>
    {};
    
    template<typename r_>
    struct do_any_result<r_, typename do_any_result_to_void<typename r_::result_tag>::result> :
        r_
    {};
    
}

template<typename a_ = nothing>
struct any_result :
    _::do_any_result<a_>
{};

template<>
struct any_result<> {
    typedef types1::result_tag<> result_tag;
};

}}
#endif
