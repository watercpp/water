// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_TYPES_RESULT_HPP
#define WATER_TYPES_RESULT_HPP
#include <water/types/type.hpp>
#include <water/types/integer.hpp>
namespace water { namespace types {

/*

The result_tag is what makes primitives in water::types special. A type that has a result_tag that
is a typedef of types::result_tag is seen as a result. That enables nesting.

The kind_ template argument is like a class, it contains operations that work on types that have
that result_tag. Look at type_kind in type.hpp and integer_kind in integer.hpp. Also see the equal
template in equal.hpp that uses the kind's equal operation.

result_tag<> or result_tag<void> has a special purpose. It means "no result". The result_tag makes
it propagate through other templates. For example:

    ifel<false, int, ifel<false, long>>

The inner ifel<false, long> has no result, but it has result_tag<void>. This means the outer ifel
will detect that ifel<false, long> is a result and inherit that nested "no result". Otherwise the
expression would have a result of type ifel<false, long>

*/

template<typename kind_ /*= void*/>
struct result_tag :
    type_plain<kind_>
{};

namespace _ {
    
    template<typename x_ = void>
    struct do_result_tag_to_void {
        typedef void result;
    };
    
    template<template<typename> class to_, typename from_, typename x_ = void>
    struct do_result_tag_to :
        to_<void>
    {};
    
    template<template<typename> class to_, typename from_>
    struct do_result_tag_to<to_, from_, typename do_result_tag_to_void<typename from_::result_tag>::result> :
        to_<typename from_::result_tag>
    {};


    template<typename t_>
    struct do_is_result :
        false_result
    {};
    
    template<typename k_>
    struct do_is_result<result_tag<k_> > :
        true_result
    {};
    
    template<>
    struct do_is_result<result_tag<> > :
        false_result
    {};
    
    
    template<typename t_>
    struct do_result_tag_of :
        type_plain<void>
    {};
    
    template<typename k_>
    struct do_result_tag_of<result_tag<k_> > :
        type_plain<result_tag<k_> >
    {};
    
    
    template<typename t_>
    struct do_result_kind_of :
        type_plain<void>
    {};
    
    template<typename k_>
    struct do_result_kind_of<result_tag<k_> > :
        type_plain<k_>
    {};
    
    
    template<typename t_>
    struct do_result_kind
    {};
    
    template<typename k_>
    struct do_result_kind<result_tag<k_> > :
        k_
    {};
    
    template<>
    struct do_result_kind<result_tag<> >
    {};

}

// result true if a_ is a result, else false
// - is_result<int>::result is false
// - is_result<type<int> >::result is true
// - is_result<enum_result<123> >::result is true
// - is_result<is_result<int> >::result is true
template<typename a_>
struct is_result :
    _::do_result_tag_to<_::do_is_result, a_>
{};


// result a_::result_tag if it exsists and is a types::result_tag, else void
// - result_tag_of<type<int> >::result is types::result_tag<types::type_kind> >
// - result_tag_of<int>::result is void
template<typename a_>
struct result_tag_of :
    _::do_result_tag_to<_::do_result_tag_of, a_>
{};


// if it is a result, result the result kind, else void
// - result_kind_of<type<int> >::result is types::type_kind
// - result_kind_of<enum_result<123> >::result is types::integer_kind
// - result_kind_of<int>::result is void
template<typename a_>
struct result_kind_of :
    _::do_result_tag_to<_::do_result_kind_of, a_>
{};

// if it is a result, this inherits the result kind
// - result_kind<type<int> > inherits type::type_kind
// - result_kind<enum_result<123> > inherits types::integer_kind
// - result_kind<int> does not inherit anything
template<typename a_>
struct result_kind :
    _::do_result_tag_to<_::do_result_kind, a_>
{};

}}
#endif
