// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_TYPES1_TYPE_HPP
#define WATER_TYPES1_TYPE_HPP
#include <water/types1/bits.hpp>
namespace water { namespace types1 {

/*

Type results are type that have a result that is a type. The rules are:
- result is a type
- result_tag is a typedef of types1::result_tag<types1::type_kind>

void_result {
    typedef void result;
    typedef types1::result_tag<types1::type_kind> result_tag;
};

type<a_>::result is a_, unless a_ is a result of types1::type_kind, then it is a_::result.
This is enables nesting of many results.
- type<int>::result is int
- type<type<int>>::result is int
- type<no_const<to_const<any_result<int>>>>::result is int
- type<type_plain<int>>::result is int

type_plain<a_>::result is always a_
- type_plain<int>::result is int
- type_plain<type<int>>::result is type<int>

Use types1::type to make templates that allow nesting. Pass the template argument through types1::type:

    template<typename a_>
    struct example {
        typedef typename types1::type<a_>::result result;
        typedef types1::result_tag<types1::type_kind> result_tag;
    };

    example<example<example<int>>>::result i = 123;

*/

namespace _ {

    // better than moving bool_result + equal_plain to bits.hpp?
    template<bool a_>
    struct type_kind_bool {
        static bool constexpr result = a_;
        typedef bool type;
        typedef types1::result_tag<integer_kind> result_tag;
    };
    template<bool a_>
    bool constexpr type_kind_bool<a_>::result;
    
    template<typename a_, typename b_>
    struct type_kind_equal : type_kind_bool<false>
    {};

    template<typename a_>
    struct type_kind_equal<a_, a_> : type_kind_bool<true>
    {};

}

struct type_kind
{
    template<typename a_, typename b_>
    struct equal : _::type_kind_equal<typename a_::result, typename b_::result>
    {};

    template<typename a_>
    struct type : a_
    {};

};

namespace _ {

    template<
        typename r_,
        typename t_ = result_tag<type_kind>,
        typename x_ = void,
        bool select_ = tag_overload_is_exact
    >
    struct do_type_result {
        typedef r_ result;
    };
    
    template<typename r_>
    struct do_type_result<r_, typename r_::result_tag, void, true> {
        typedef typename r_::result result;
    };
    
    template<typename r_>
    struct do_type_result<r_, typename r_::result_tag, void, false> :
        do_type_result<r_, void, typename r_::result_tag, false>
    {};

    template<typename r_>
    struct do_type_result<r_, void, result_tag<type_kind>, false> {
        typedef typename r_::result result;
    };
    
}

template<typename result_>
struct type {
    typedef typename _::do_type_result<result_>::result result;
    typedef types1::result_tag<type_kind> result_tag;
};

template<typename result_>
struct type_plain {
    typedef result_ result;
    typedef types1::result_tag<type_kind> result_tag;
};

}}
#endif
