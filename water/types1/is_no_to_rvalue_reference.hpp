// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_TYPES1_IS_NO_TO_RVALUE_REFERENCE_HPP
#define WATER_TYPES1_IS_NO_TO_RVALUE_REFERENCE_HPP
#include <water/types1/result.hpp>
namespace water { namespace types1 {

namespace _ {

    template<typename>
    struct do_is_rvalue_reference : false_result {};
    
    template<typename a_>
    struct do_is_rvalue_reference<a_&&> : true_result {};
    
    
    template<typename a_>
    struct do_no_rvalue_reference : type_plain<a_> {};
    
    template<typename a_>
    struct do_no_rvalue_reference<a_&&> : type_plain<a_> {};
    
    
    template<typename a_>
    struct do_to_rvalue_reference : type_plain<a_&&> {};
    
    template<typename a_>
    struct do_to_rvalue_reference<a_&> : type_plain<a_&&> {};
    
    template<typename a_>
    struct do_to_rvalue_reference<a_&&> : type_plain<a_&&> {};
    
}

// result true if type_ is a rvalue reference
// - is_rvalue_reference<int&&>::result is true
// - is_rvalue_reference<int>::result is false
template<typename type_>
struct is_rvalue_reference :
    _::do_is_rvalue_reference<typename type<type_>::result>
{};

// if type_ is a rvalue reference, result type_ without reference, else type_
// - no_reference<int&&>::result is int
// - no_reference<int>::result is int
template<typename type_>
struct no_rvalue_reference :
    _::do_no_rvalue_reference<typename type<type_>::result>
{};

// if type_ is not a rvalue reference, result type_&&, else type_
// - to_reference<int&>::result is int&&
// - to_reference<int>::result is int&&
template<typename type_>
struct to_rvalue_reference :
    _::do_to_rvalue_reference<typename type<type_>::result>
{};

}}
#endif
