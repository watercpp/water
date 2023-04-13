// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_TYPES1_IF_NOT_VOID_HPP
#define WATER_TYPES1_IF_NOT_VOID_HPP
#include <water/types1/any_result.hpp>
namespace water { namespace types1 {

/*

If if_not_void_ is not void, result if_not_void_, otherwise:
- if the else_ template argument is specified, result else_
- if the else_ template argument is not specified, this has no result

if_not_void<int>::result is int
if_not_void<int, void>::result is int
if_not_void<void, int>::result is int
if_not_void<no_const<void const> > has no result
if_not_void<void, enum_result<123> >::result is 123
if_not_void<enum_result<123>, int>::result is 123

*/

namespace _ {

    template<typename a_, typename b_>
    struct do_if_not_void :
        any_result<a_>
    {};
    
    template<typename b_>
    struct do_if_not_void<void, b_> :
        any_result<b_>
    {};
    
    template<>
    struct do_if_not_void<void, nothing>
    {};
    
}

template<typename if_not_void_, typename else_ = nothing>
struct if_not_void :
    _::do_if_not_void<typename type<if_not_void_>::result, else_>
{};

}}
#endif
