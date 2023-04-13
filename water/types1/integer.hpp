// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_TYPES1_INTEGER_HPP
#define WATER_TYPES1_INTEGER_HPP
#include <water/types1/type.hpp>
namespace water { namespace types1 {

/*

Integer results are types that have a result that is a compile time integer constant. The rules are:
- result is a compile-time integer constant
- type is the type of the result
- result_tag is a typedef of types1::result_tag<types1::integer_kind>

struct zero {
    enum type { result = 0 };
    typedef types1::result_tag<types1::integer_kind> result_tag;
};

struct a {
    typedef char type;
    static char const result = 'a';
    typedef types1::result_tag<types1::integer_kind> result_tag;
};
char const a::result;

integer<int, 123>::result is int 123
integer<unsigned long long, 123>::result is unsigned long long 123
integer<integer<integer<char, 'a'>>>::result is char 'a', nesting works
integer<to_unsigned<int>, 123>::result is unsigned int 123, the type can be a type result

enum_result<123>::result an enum

bool_result<true || false>::result is a bool, with typedefs:
true_result
false_result


A special property of types1::integer is that it enables nesting using types1::int_if_integer. This is
not very useful for types1::integer by itself, but it can be useful for creating templates that accept
a single type argument that is an integer result, or two arguments type + itegral constant:

    template<
        typename type_or_result_,
        typename types1::int_if_integer<type_or_result_>::result result_ = 0
    >
    struct div2 :
        types1::integer<
            typename types1::integer<type_or_result_, result_>::type,
            types1::integer<type_or_result_, result_>::result / 2
        > {};

    int const two = div2<div2<div2<int, 16>>>::result; // (((16 / 2) / 2) / 2)

*/

template<bool result_> struct bool_result; // below

struct integer_kind {

    template<typename a_, typename b_>
    struct equal : bool_result<a_::result == b_::result>
    {};
    
    template<typename a_>
    struct type : type_plain<typename a_::type>
    {};
    
    template<typename a_>
    struct to_bool : bool_result<a_::result ? true : false>
    {};
};

namespace _ {

    template<
        typename a_,
        typename t_ = result_tag<integer_kind>,
        typename x_ = void,
        bool select_ = tag_overload_is_exact
    >
    struct do_int_if_number :
        type_plain<a_>
    {};
    
    template<typename a_>
    struct do_int_if_number<a_, typename a_::result_tag, void, true> :
        type_plain<int>
    {};
    
    template<typename a_>
    struct do_int_if_number<a_, typename a_::result_tag, void, false> :
        do_int_if_number<a_, void, typename a_::result_tag, false>
    {};
    
    template<typename a_>
    struct do_int_if_number<a_, void, result_tag<integer_kind>, false> :
        type_plain<int>
    {};
    
}

// if the types1::type<type_>::result is a types1::integer result int, else result types1::type<type_>
template<typename type_>
struct int_if_integer :
    _::do_int_if_number<typename type<type_>::result>
{};

namespace _ {

    template<typename t_, typename i_, i_ r_>
    struct do_integer :
        t_
    {};
    
    template<typename t_, t_ r_>
    struct do_integer<t_, t_, r_> {
        static t_ constexpr result = r_;
        typedef t_ type;
        typedef types1::result_tag<integer_kind> result_tag;
    };
    template<typename t_, t_ r_>
    t_ constexpr do_integer<t_, t_, r_>::result;
}

template<typename type_or_result_, typename int_if_integer<type_or_result_>::result result_ = 0>
struct integer :
    _::do_integer<
        typename type<type_or_result_>::result,
        typename int_if_integer<type_or_result_>::result,
        result_
    > {};


template<bool result_>
struct bool_result {
    static bool constexpr result = result_;
    typedef bool type;
    typedef types1::result_tag<integer_kind> result_tag;
};
template<bool result_>
bool constexpr bool_result<result_>::result;

typedef bool_result<false> false_result;
typedef bool_result<true> true_result;


template<int result_>
struct enum_result {
    enum type { result = result_ };
    typedef types1::result_tag<integer_kind> result_tag;
};

}}
#endif
