// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_TYPES1_IS_ENUM_HPP
#define WATER_TYPES1_IS_ENUM_HPP
#include <water/types1/is_bool_float_int.hpp>
#include <water/types1/is_void.hpp>
#include <water/types1/is_class_struct_union.hpp>
#include <water/types1/to_void.hpp>
namespace water { namespace types1 {
namespace _ {
    
    /*
    template<typename a_, typename = void>
    struct do_is_enum_test : false_result
    {};
    
    template<typename a_>
    struct do_is_enum_test<a_, to_void<decltype(static_cast<int>(types1::make<a_>()))>> : true_result
    {};
    */
    
    // intel c++ did not like the above, below works
    
    template<typename a_> false_size do_is_enum_test(...);
    template<typename a_> true_size do_is_enum_test(a_*, decltype(static_cast<int>(types1::make<a_>())));

    template<typename a_, bool select_ = is_bool_float_int<a_>::result || is_void<a_>::result || is_class_struct_union<a_>::result>
    struct do_is_enum : false_result
    {};
    
    template<typename a_>
    struct do_is_enum<a_, false> : bool_result<sizeof(do_is_enum_test<a_>(0, 0)) == sizeof(true)>
    {};
    
    /*template<typename a_>
    struct do_is_enum<a_, false> : do_is_enum_test<a_>
    {};*/
    
    template<typename a_>
    struct do_is_enum<a_&, false> : false_result
    {};
    
    template<typename a_>
    struct do_is_enum<a_&&, false> : false_result
    {};
    
    template<typename a_>
    struct do_is_enum<a_ const, false> : false_result
    {};
    
    template<typename a_>
    struct do_is_enum<a_ const volatile, false> : false_result
    {};
    
    template<typename a_>
    struct do_is_enum<a_ volatile, false> : false_result
    {};
}

template<typename type_>
struct is_enum :
    _::do_is_enum<typename type<type_>::result>
{};

}}
#endif
