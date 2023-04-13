// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_TYPES_NONTYPE_HPP
#define WATER_TYPES_NONTYPE_HPP
#include <water/types/type.hpp>
namespace water { namespace types {





struct nontype_kind {

    template<typename a_, typename b_, typename = void>
    struct equal :
        equal_plain<typename a_::type, typename b_::type>
    {};
    
    template<typename a_, typename b_>
    struct equal<a_, b_, to_void<decltype(a_::result == b_::result)>> :
        bool_result<a_::result == b_::result>
    {};
    
    template<typename a_>
    struct type :
        type_plain<typename a_::type>
    {};
    
    template<typename a_, typename = void>
    struct to_bool : 
        bool_result<true>
    {};
    
    template<typename a_>
    struct to_bool<a_, to_void<decltype(a_::result ? true : false)>> :
        bool_result<a_::result ? true : false>
    {};
    
};



namespace _ {

    template<typename a_, typename t_ = result_tag<nontype_kind>>
    struct do_int_if_nontype :
        type_plain<a_>
    {};
    
    template<typename a_>
    struct do_int_if_nontype<a_, typename a_::result_tag> :
        type_plain<int>
    {};
    
}

// if the types::type<type_>::result is a types::nontype result int, else result types::type<type_>
template<typename type_>
struct int_if_nontype :
    _::do_int_if_nontype<typename type<type_>::result>
{};



namespace _ {

    template<typename t_, typename i_, i_ r_>
    struct nontype_do :
        t_
    {};
    
    template<typename t_, t_ r_>
    struct nontype_do<t_, t_, r_> {
        static t_ constexpr result = r_;
        typedef t_ type;
        typedef types::result_tag<nontype_kind> result_tag;
    };

}

template<typename type_or_result_, typename int_if_nontype<type_or_result_>::result result_ = 0>
struct nontype :
    _::nontype_do<
        typename type<type_or_result_>::result,
        typename int_if_nontype<type_or_result_>::result,
        result_
    > {};




template<int result_>
struct enum_result {
    enum type { result = result_ };
    typedef types::result_tag<nontype_kind> result_tag;
};




// nontype was called integer in the past, when it was used only for integral constants.
// this is for backwards compatibility. 

using integer_kind = nontype_kind;

template<typename type_>
using int_if_integer = int_if_nontype<type_>;

template<typename type_or_result_, typename int_if_nontype<type_or_result_>::result result_ = 0>
using integer = nontype<type_or_result_, result_>;



}}
#endif
