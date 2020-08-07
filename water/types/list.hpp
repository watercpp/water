// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_TYPES_LIST_HPP
#define WATER_TYPES_LIST_HPP
#include <water/types/collection.hpp>
#include <water/types/any_result.hpp>
namespace water { namespace types {

/*

list<some_type, other_type, another_type, ...>

Collection using a template parameter pack. It works by splitting the parameter pack into two, one
previous and one current. The current position is at the start of the current parameter pack.

*/

template<typename ...types_>
struct list;

struct list_kind;

template<typename previous_, typename current_>
struct list_split;

template<typename ...previous_, typename at_, typename ...current_>
struct list_split<list<previous_...>, list<at_, current_...>> : any_result<at_> {
    using collection_tag = types::collection_tag<list_kind>;
    using previous = list<previous_...>;
    using current = list<at_, current_...>;
    using rewind = list<previous_..., at_, current_...>;
    // do not call these insert, set, remove because visual c++ will be confused in list_kind
    template<typename insert_> using insert_here = list_split<previous, list<insert_, at_, current_...>>;
    template<typename set_> using set_here = list_split<previous, list<set_, current_...>>;
    using remove_here = list_split<previous, list<current_...>>;
};

template<typename ...previous_>
struct list_split<list<previous_...>, list<>>  : any_result<> {
    using collection_tag = types::collection_tag<list_kind>;
    using previous = list<previous_...>;
    using current = list<>;
    using rewind = list<previous_...>;
    template<typename insert_> using insert_here = list_split<previous, list<insert_>>;
};

template<typename ...types_>
struct list : list_split<list<>, list<types_...>> {
    enum size_ { size = sizeof...(types_) };
};

template<typename from_>
struct list_from : list<>
{};

template<template<typename ...> class from_, typename ...types_>
struct list_from<from_<types_...>> : list<types_...>
{};


template<typename previous_, typename current_, int at_, bool = at_ >= 8>
struct list_at;
template<typename ...p_, typename c0_, typename c1_, typename c2_, typename c3_, typename c4_, typename c5_, typename c6_, typename c7_, typename ...c_, int at_>
struct list_at<list<p_...>, list<c0_, c1_, c2_, c3_, c4_, c5_, c6_, c7_, c_...>, at_, true> :
    list_at<list<p_..., c0_, c1_, c2_, c3_, c4_, c5_, c6_, c7_>, list<c_...>, at_ - 8>
{};

template<typename ...p_, typename c0_, typename c1_, typename c2_, typename c3_, typename c4_, typename c5_, typename c6_, typename ...c_>
struct list_at<list<p_...>, list<c0_, c1_, c2_, c3_, c4_, c5_, c6_, c_...>, 7, false> :
    type_plain<list_split<list<p_..., c0_, c1_, c2_, c3_, c4_, c5_, c6_>, list<c_...>>>
{};

template<typename ...p_, typename c0_, typename c1_, typename c2_, typename c3_, typename c4_, typename c5_, typename ...c_>
struct list_at<list<p_...>, list<c0_, c1_, c2_, c3_, c4_, c5_, c_...>, 6, false> :
    type_plain<list_split<list<p_..., c0_, c1_, c2_, c3_, c4_, c5_>, list<c_...>>>
{};

template<typename ...p_, typename c0_, typename c1_, typename c2_, typename c3_, typename c4_, typename ...c_>
struct list_at<list<p_...>, list<c0_, c1_, c2_, c3_, c4_, c_...>, 5, false> :
    type_plain<list_split<list<p_..., c0_, c1_, c2_, c3_, c4_>, list<c_...>>>
{};

template<typename ...p_, typename c0_, typename c1_, typename c2_, typename c3_, typename ...c_>
struct list_at<list<p_...>, list<c0_, c1_, c2_, c3_, c_...>, 4, false> :
    type_plain<list_split<list<p_..., c0_, c1_, c2_, c3_>, list<c_...>>>
{};

template<typename ...p_, typename c0_, typename c1_, typename c2_, typename ...c_>
struct list_at<list<p_...>, list<c0_, c1_, c2_, c_...>, 3, false> :
    type_plain<list_split<list<p_..., c0_, c1_, c2_>, list<c_...>>>
{};

template<typename ...p_, typename c0_, typename c1_, typename ...c_>
struct list_at<list<p_...>, list<c0_, c1_, c_...>, 2, false> :
    type_plain<list_split<list<p_..., c0_, c1_>, list<c_...>>>
{};

template<typename ...p_, typename c0_, typename ...c_>
struct list_at<list<p_...>, list<c0_, c_...>, 1, false> :
    type_plain<list_split<list<p_..., c0_>, list<c_...>>>
{};

template<typename ...p_, typename ...c_>
struct list_at<list<p_...>, list<c_...>, 0, false> :
    type_plain<list_split<list<p_...>, list<c_...>>>
{};


struct list_kind
{
    template<typename list_, int at_, bool = -list_::previous::size <= at_ && at_ <= list_::current::size>
    struct at : list_at<list<>, typename list_::rewind, list_::previous::size + at_>::result
    {};

    template<typename list_, int at_>
    struct at<list_, at_, false>
    {};

    template<typename list_>
    struct next : at<list_, 1>
    {};

    template<typename list_>
    struct pull : type_plain<list_split<typename list_::previous, typename list_::current>>
    {};

    template<typename list_>
    struct offset : integer<int, list_::previous::size>
    {};

    template<typename list_>
    struct size : integer<int, list_::current::size>
    {};

    template<typename list_, typename insert_, int at_>
    struct insert : list_split<typename list_::previous, typename list_at<list<>, typename list_::current, at_>::result::template insert_here<insert_>::rewind>
    {};

    template<typename list_, typename set_, int at_>
    struct set : list_split<typename list_::previous, typename list_at<list<>, typename list_::current, at_>::result::template set_here<set_>::rewind>
    {};

    template<typename list_, int at_>
    struct remove : list_split<typename list_::previous, typename list_at<list<>, typename list_::current, at_>::result::remove_here::rewind>
    {};

};

}}
#endif
