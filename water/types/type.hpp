// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_TYPES_TYPE_HPP
#define WATER_TYPES_TYPE_HPP
#include <water/types/bits.hpp>
namespace water { namespace types {

/*

Type results have a result that is a type. The rules are:
- result is a type
- result_tag is a typedef of types::result_tag<types::type_kind>

void_result {
    using result = void;
    using result_tag = types::result_tag<types::type_kind>;
};

type<a_>::result is a_, unless a_ is a result of types::type_kind, then it is a_::result.
This is enables nesting of many results.
- type<int>::result is int
- type<type<int>>::result is int
- type<no_const<to_const<any_result<int>>>>::result is int
- type<type_plain<int>>::result is int

type_plain<a_>::result is always a_
- type_plain<int>::result is int
- type_plain<type<int>>::result is type<int>

Use types::type to make templates that allow nesting. Pass the template argument through types::type:

    template<typename a_>
    struct example {
        typedef typename types::type<a_>::result result;
        typedef types::result_tag<types::type_kind> result_tag;
    };

    example<example<example<int>>>::result i = 123;

*/

struct type_kind
{
    template<typename a_, typename b_>
    struct equal : equal_plain<typename a_::result, typename b_::result>
    {};

    template<typename a_>
    struct type : a_
    {};

};



namespace _ {

    template<typename r_, typename t_ = result_tag<type_kind>>
    struct do_type_result {
        typedef r_ result;
    };
    
    template<typename r_>
    struct do_type_result<r_, typename r_::result_tag> {
        typedef typename r_::result result;
    };
    
}

template<typename result_>
struct type {
    typedef typename _::do_type_result<result_>::result result;
    typedef types::result_tag<type_kind> result_tag;
};



template<typename result_>
struct type_plain {
    typedef result_ result;
    typedef types::result_tag<type_kind> result_tag;
};


}}
#endif
