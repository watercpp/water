// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_TYPES_IS_POINTER_TO_MEMBER_FUNCTION_HPP
#define WATER_TYPES_IS_POINTER_TO_MEMBER_FUNCTION_HPP
#include <water/types/result.hpp>
namespace water { namespace types {
namespace _ {

    template<typename a_>
    struct do_is_pointer_to_member_function : false_result {};
    
    template<typename r_, typename c_>
    struct do_is_pointer_to_member_function<r_ (c_::*)()> : true_result {};
    
    template<typename r_, typename c_, typename ...a_>
    struct do_is_pointer_to_member_function<r_(c_::*)(a_...)> : true_result {};
    
    template<typename r_, typename c_, typename ...a_>
    struct do_is_pointer_to_member_function<r_(c_::*)(a_..., ...)> : true_result {};
    
}

template<typename type_>
struct is_pointer_to_member_function :
    _::do_is_pointer_to_member_function<typename type<type_>::result>
{};

}}
#endif
