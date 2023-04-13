// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_TYPES1_IS_NO_TO_VOLATILE_HPP
#define WATER_TYPES1_IS_NO_TO_VOLATILE_HPP
#include <water/types1/result.hpp>
namespace water { namespace types1 {
namespace _ {

    template<typename>
    struct do_is_volatile : false_result {};
    
    template<typename a_>
    struct do_is_volatile<a_ volatile> : true_result {};
    
    template<typename a_>
    struct do_is_volatile<a_ volatile&> : true_result {};
    
    template<typename a_>
    struct do_is_volatile<a_ volatile&&> : true_result {};
    
    
    template<typename a_>
    struct do_no_volatile : type_plain<a_> {};
    
    template<typename a_>
    struct do_no_volatile<a_ volatile> : type_plain<a_> {};
    
    template<typename a_>
    struct do_no_volatile<a_ volatile&> : type_plain<a_&> {};
    
    template<typename a_>
    struct do_no_volatile<a_ volatile&&> : type_plain<a_&> {};
    
    
    template<typename a_>
    struct do_to_volatile : type_plain<a_ volatile> {};
    
    template<typename a_>
    struct do_to_volatile<a_ volatile> : type_plain<a_ volatile> {};
    
    template<typename a_>
    struct do_to_volatile<a_&> : type_plain<a_ volatile&> {};
    
    template<typename a_>
    struct do_to_volatile<a_ volatile&> : type_plain<a_ volatile&> {};
    
    template<typename a_>
    struct do_to_volatile<a_&&> : type_plain<a_ volatile&&> {};
    
    template<typename a_>
    struct do_to_volatile<a_ volatile&&> : type_plain<a_ volatile&&> {};
    
}
// results true for
// - any_type volatile
// - any_type volatile&
template<typename type_>
struct is_volatile :
    _::do_is_volatile<typename type<type_>::result>
{};

// result type_ without volatile
// - no_volatile<int volatile>::result is int
// - no_volatile<int volatile&>::result is int&
// - no_volatile<int>::result is int
template<typename type_>
struct no_volatile :
    _::do_no_volatile<typename type<type_>::result>
{};

// result type_ with volatile
// - to_volatile<int>::result is int volatile
// - to_volatile<int&>::result is int volatile&
// - to_volatile<int volatile>::result is int volatile
template<typename type_>
struct to_volatile :
    _::do_to_volatile<typename type<type_>::result>
{};

}}
#endif
