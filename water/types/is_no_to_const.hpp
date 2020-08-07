// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_TYPES_IS_NO_TO_CONST_HPP
#define WATER_TYPES_IS_NO_TO_CONST_HPP
#include <water/types/result.hpp>
namespace water { namespace types {

namespace _ {

    template<typename>
    struct do_is_const : false_result {};
    
    template<typename a_>
    struct do_is_const<a_ const> : true_result {};
    
    template<typename a_>
    struct do_is_const<a_ const&> : true_result {};
    
    template<typename a_>
    struct do_is_const<a_ const&&> : true_result {};
    
    
    template<typename a_>
    struct do_no_const : type_plain<a_> {};
    
    template<typename a_>
    struct do_no_const<a_ const> : type_plain<a_> {};
    
    template<typename a_>
    struct do_no_const<a_ const&> : type_plain<a_&> {};
    
    template<typename a_>
    struct do_no_const<a_ const&&> : type_plain<a_&&> {};
    
    
    template<typename a_>
    struct do_to_const : type_plain<a_ const> {};
    
    template<typename a_>
    struct do_to_const<a_ const> : type_plain<a_ const> {};
    
    template<typename a_>
    struct do_to_const<a_&> : type_plain<a_ const&> {};
    
    template<typename a_>
    struct do_to_const<a_ const&> : type_plain<a_ const&> {};
    
    template<typename a_>
    struct do_to_const<a_&&> : type_plain<a_ const&&> {};
    
    template<typename a_>
    struct do_to_const<a_ const&&> : type_plain<a_ const&&> {};
    
}

// results true for
// - any_type const
// - any_type const&
template<typename type_>
struct is_const :
    _::do_is_const<typename type<type_>::result>
{};

// result type_ without const
// - no_const<int const>::result is int
// - no_const<int const&>::result is int&
// - no_const<int>::result is int
template<typename type_>
struct no_const :
    _::do_no_const<typename type<type_>::result>
{};

// result type_ with const
// - no_const<int>::result is int const
// - no_const<int&>::result is int const&
// - no_const<int const>::result is int const
template<typename type_>
struct to_const :
    _::do_to_const<typename type<type_>::result>
{};

}}
#endif
