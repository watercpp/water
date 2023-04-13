// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_TYPES_LINK_HPP
#define WATER_TYPES_LINK_HPP
#include <water/types/collection.hpp>
#include <water/types/any_result.hpp>
#include <water/types/equal.hpp>
#include <water/types/ifel.hpp>
#include <water/types/nontype.hpp>
namespace water { namespace types {

/*

link is a collection. It is called link because it is a link in a linked list. To make a link
collection of more than one result, many links are joined:

    typedef link<> empty;
    typedef link<int> one; // contains int
    typedef link<int, link<float> > two; // contains int, float
    typedef link<int, link<float, link<enum_result<123> > > > three; // contains int, float, 123

    at<three, 0>::result i = at<three, 2>::result; // int i = 123
    three::result        j = at<three, 2>::result; // int j = 123

This was written in 2005 or earlier, it uses only C++ 1998. Back then types::list was implemented
with the preprocessor. That felt a bit "brute force" and it had a fixed maximum size of 100. This
was different.

Today types::list has a much simpler implementation and might be more useful.

*/

template<typename link_> struct link_kind;

template<typename result_ = nothing, typename next_ = void>
struct link :
    any_result<result_>
{
    typedef next_ next;
    typedef types::collection_tag<link_kind<link<result_, next_> > > collection_tag;
};

// Things below try to reduce template depth
// - They are specialized to jump up to 8 links at once
// - link_at can continue forwards from the current position

template<
    typename first_,
    int offset_,
    typename current_ // link at offset_, void if out of bounds
>
struct link_at_kind;


template<
    typename link0_, // first
    int offset_,
    typename current_ = link0_,
    int left_ = offset_, // left from current_ to reach offset_
    int jump_ =
        left_ <= 0 ? 0 :
        left_ >= 8 ? 8 :
        left_
>
struct link_at { // this is out of bounds
    typedef types::collection_tag<link_at_kind<link0_, offset_, void> > collection_tag;
};

template<typename l0_, int o_, template<typename, typename> class l_, typename a_, typename n_>
struct link_at<l0_, o_, l_<a_, n_>, 0, 0> : l_<a_, n_> { // this is not out of bounds
    typedef types::collection_tag<link_at_kind<l0_, o_, l_<a_, n_> > > collection_tag;
};

template<typename l0_, int o_, template<typename, typename> class l_, typename a_, typename n_, int i_>
struct link_at<l0_, o_, l_<a_, n_>, i_, 1> :
    link_at<l0_, o_, n_, i_ - 1>
{};

template<typename l0_, int o_, template<typename, typename> class l_, typename a_, typename b_, typename n_, int i_>
struct link_at<l0_, o_, l_<a_, l_<b_, n_> >, i_, 2> :
    link_at<l0_, o_, n_, i_ - 2>
{};

template<typename l0_, int o_, template<typename, typename> class l_, typename a_, typename b_, typename c_, typename n_, int i_>
struct link_at<l0_, o_, l_<a_, l_<b_, l_<c_, n_> > >, i_, 3> :
    link_at<l0_, o_, n_, i_ - 3>
{};

template<typename l0_, int o_, template<typename, typename> class l_, typename a_, typename b_, typename c_, typename d_, typename n_, int i_>
struct link_at<l0_, o_, l_<a_, l_<b_, l_<c_, l_<d_, n_> > > >, i_, 4> :
    link_at<l0_, o_, n_, i_ - 4>
{};

template<typename l0_, int o_, template<typename, typename> class l_, typename a_, typename b_, typename c_, typename d_, typename e_, typename n_, int i_>
struct link_at<l0_, o_, l_<a_, l_<b_, l_<c_, l_<d_, l_<e_, n_> > > > >, i_, 5> :
    link_at<l0_, o_, n_, i_ - 5>
{};

template<typename l0_, int o_, template<typename, typename> class l_, typename a_, typename b_, typename c_, typename d_, typename e_, typename f_, typename n_, int i_>
struct link_at<l0_, o_, l_<a_, l_<b_, l_<c_, l_<d_, l_<e_, l_<f_, n_> > > > > >, i_, 6> :
    link_at<l0_, o_, n_, i_ - 6>
{};

template<typename l0_, int o_, template<typename, typename> class l_, typename a_, typename b_, typename c_, typename d_, typename e_, typename f_, typename g_, typename n_, int i_>
struct link_at<l0_, o_, l_<a_, l_<b_, l_<c_, l_<d_, l_<e_, l_<f_, l_<g_, n_> > > > > > >, i_, 7> :
    link_at<l0_, o_, n_, i_ - 7>
{};

template<typename l0_, int o_, template<typename, typename> class l_, typename a_, typename b_, typename c_, typename d_, typename e_, typename f_, typename g_, typename h_, typename n_, int i_>
struct link_at<l0_, o_, l_<a_, l_<b_, l_<c_, l_<d_, l_<e_, l_<f_, l_<g_, l_<h_, n_> > > > > > > >, i_, 8> :
    link_at<l0_, o_, n_, i_ - 8>
{};


template<typename l_>
struct link_next
{};

template<template<typename, typename> class l_, typename a_, typename n_>
struct link_next<l_<a_, n_> >
{};

template<template<typename, typename> class l_, typename a_, typename b_, typename n_>
struct link_next<l_<a_, l_<b_, n_> > > :
    l_<b_, n_>
{};


// reverse_join
// Reverse r_ and join it with j_. r_(a, b, c) j_(x, y, z) becomes (c, b, a, x, y, z)

template<typename r_, typename j_>
struct link_reverse_join;
template<typename j_>
struct link_reverse_join<void, j_> :
    type_plain<j_>
{};

template<template<typename, typename> class l_, typename a_, typename n_, typename j_>
struct link_reverse_join<l_<a_, n_>, j_> :
    link_reverse_join<n_, l_<a_, j_> >
{};

template<template<typename, typename> class l_, typename a_, typename b_, typename n_, typename j_>
struct link_reverse_join<l_<a_, l_<b_, n_> >, j_> :
    link_reverse_join<n_, l_<b_, l_<a_, j_> > >
{};

template<template<typename, typename> class l_, typename a_, typename b_, typename c_, typename n_, typename j_>
struct link_reverse_join<l_<a_, l_<b_, l_<c_, n_> > >, j_> :
    link_reverse_join<n_, l_<c_, l_<b_, l_<a_, j_> > > >
{};

template<template<typename, typename> class l_, typename a_, typename b_, typename c_, typename d_, typename n_, typename j_>
struct link_reverse_join<l_<a_, l_<b_, l_<c_, l_<d_, n_> > > >, j_> :
    link_reverse_join<n_, l_<d_, l_<c_, l_<b_, l_<a_, j_> > > > >
{};

template<template<typename, typename> class l_, typename a_, typename b_, typename c_, typename d_, typename e_, typename n_, typename j_>
struct link_reverse_join<l_<a_, l_<b_, l_<c_, l_<d_, l_<e_, n_> > > > >, j_> :
    link_reverse_join<n_, l_<e_, l_<d_, l_<c_, l_<b_, l_<a_, j_> > > > > >
{};

template<template<typename, typename> class l_, typename a_, typename b_, typename c_, typename d_, typename e_, typename f_, typename n_, typename j_>
struct link_reverse_join<l_<a_, l_<b_, l_<c_, l_<d_, l_<e_, l_<f_, n_> > > > > >, j_> :
    link_reverse_join<n_, l_<f_, l_<e_, l_<d_, l_<c_, l_<b_, l_<a_, j_> > > > > > >
{};

template<template<typename, typename> class l_, typename a_, typename b_, typename c_, typename d_, typename e_, typename f_, typename g_, typename n_, typename j_>
struct link_reverse_join<l_<a_, l_<b_, l_<c_, l_<d_, l_<e_, l_<f_, l_<g_, n_> > > > > > >, j_> :
    link_reverse_join<n_, l_<g_, l_<f_, l_<e_, l_<d_, l_<c_, l_<b_, l_<a_, j_> > > > > > > >
{};

template<template<typename, typename> class l_, typename a_, typename b_, typename c_, typename d_, typename e_, typename f_, typename g_, typename h_, typename n_, typename j_>
struct link_reverse_join<l_<a_, l_<b_, l_<c_, l_<d_, l_<e_, l_<f_, l_<g_, l_<h_, n_> > > > > > > >, j_> :
    link_reverse_join<n_, l_<h_, l_<g_, l_<f_, l_<e_, l_<d_, l_<c_, l_<b_, l_<a_, j_> > > > > > > > >
{};


// link_insert_remove_set
// Make reverse chain of links untill i_, then insert/remove/set and link_reverse_join back the reversed links

template<typename a_> struct link_insert;
template<typename a_> struct link_set;

template<
    typename link_, // first
    int i_, // offset to insert/remove/set
    typename insert_set_ = void, // void is remove
    typename reverse_ = void, // reverse things before i_
    int jump_ =
        i_ <= 0 ? 0 :
        i_ >= 8 ? 8 :
        i_
>
struct link_insert_remove_set;

template<template<typename, typename> class l_, typename a_, typename n_, int i_, typename x_, typename r_>
struct link_insert_remove_set<l_<a_, n_>, i_, x_, r_, 1> :
    link_insert_remove_set<n_, i_ - 1, x_, l_<a_, r_> >
{};

template<template<typename, typename> class l_, typename a_, typename b_, typename n_, int i_, typename x_, typename r_>
struct link_insert_remove_set<l_<a_, l_<b_, n_> >, i_, x_, r_, 2> :
    link_insert_remove_set<n_, i_ - 2, x_, l_<b_, l_<a_, r_> > >
{};

template<template<typename, typename> class l_, typename a_, typename b_, typename c_, typename n_, int i_, typename x_, typename r_>
struct link_insert_remove_set<l_<a_, l_<b_, l_<c_, n_> > >, i_, x_, r_, 3> :
    link_insert_remove_set<n_, i_ - 3, x_, l_<c_, l_<b_, l_<a_, r_> > > >
{};

template<template<typename, typename> class l_, typename a_, typename b_, typename c_, typename d_, typename n_, int i_, typename x_, typename r_>
struct link_insert_remove_set<l_<a_, l_<b_, l_<c_, l_<d_, n_> > > >, i_, x_, r_, 4> :
    link_insert_remove_set<n_, i_ - 4, x_, l_<d_, l_<c_, l_<b_, l_<a_, r_> > > > >
{};

template<template<typename, typename> class l_, typename a_, typename b_, typename c_, typename d_, typename e_, typename n_, int i_, typename x_, typename r_>
struct link_insert_remove_set<l_<a_, l_<b_, l_<c_, l_<d_, l_<e_, n_> > > > >, i_, x_, r_, 5> :
    link_insert_remove_set<n_, i_ - 5, x_, l_<e_, l_<d_, l_<c_, l_<b_, l_<a_, r_> > > > > >
{};

template<template<typename, typename> class l_, typename a_, typename b_, typename c_, typename d_, typename e_, typename f_, typename n_, int i_, typename x_, typename r_>
struct link_insert_remove_set<l_<a_, l_<b_, l_<c_,  l_<d_, l_<e_, l_<f_, n_> > > > > >, i_, x_, r_, 6> :
    link_insert_remove_set<n_, i_ - 6, x_, l_<f_, l_<e_, l_<d_, l_<c_, l_<b_, l_<a_, r_> > > > > > >
{};

template<template<typename, typename> class l_, typename a_, typename b_, typename c_, typename d_, typename e_, typename f_, typename g_, typename n_, int i_, typename x_, typename r_>
struct link_insert_remove_set<l_<a_, l_<b_, l_<c_, l_<d_, l_<e_, l_<f_, l_<g_, n_> > > > > > >, i_, x_, r_, 7> :
    link_insert_remove_set<n_, i_ - 7, x_, l_<g_, l_<f_, l_<e_, l_<d_, l_<c_, l_<b_, l_<a_, r_> > > > > > > >
{};

template<template<typename, typename> class l_, typename a_, typename b_, typename c_, typename d_, typename e_, typename f_, typename g_, typename h_, typename n_, int i_, typename x_, typename r_>
struct link_insert_remove_set<l_<a_, l_<b_, l_<c_, l_<d_, l_<e_, l_<f_, l_<g_, l_<h_, n_> > > > > > > >, i_, x_, r_, 8> :
    link_insert_remove_set<n_, i_ - 8, x_, l_<h_, l_<g_, l_<f_, l_<e_, l_<d_, l_<c_, l_<b_, l_<a_, r_> > > > > > > > >
{};

template<template<typename, typename> class l_, typename a_, typename x_>
struct link_insert_remove_set<l_<a_, void>, 0, link_insert<x_>, void, 0> :
    type_plain<l_<x_, typename ifel_plain<equal_plain<nothing, a_>::result, void, l_<a_, void> >::result> >
{};
// The above ifel_plain because visual c++ 7.1 internal compiler error with:
//
// template<template<typename, typename> class l_, typename x_>
// struct link_insert_remove_set<l_<nothing, void>, 0, link_insert<x_>, void, 0> :
//    type_plain<l_<x_, void> >
// {};

template<template<typename, typename> class l_, typename a_, typename n_, typename x_, typename r_>
struct link_insert_remove_set<l_<a_, n_>, 0, link_insert<x_>, r_, 0> :
    link_reverse_join<r_, l_<x_, l_<a_, n_> > >
{};

template<typename x_, template<typename, typename> class l_, typename a_, typename n_> struct // when pushing the end
link_insert_remove_set<void, 0, link_insert<x_>, l_<a_, n_>, 0> :
    link_reverse_join<l_<a_, n_>, l_<x_, void> >
    {};

template<template<typename, typename> class l_, typename a_, typename n_, typename x_, typename r_>
struct link_insert_remove_set<l_<a_, n_>, 0, link_set<x_>, r_, 0> :
    link_reverse_join<r_, l_<x_, n_> >
{};

template<template<typename, typename> class l_, typename a_, typename n_, typename r_>
struct link_insert_remove_set<l_<a_, n_>, 0, void, r_, 0> :
    link_reverse_join<r_, n_>
{};


// link_size
// Do not specialize for link_size<l_<nothing, void>, s_> because visual c++ 7.1 internal compiler error. equal_plain below works

template<typename l_, unsigned s_ = 0>
struct link_size :
    integer<unsigned, s_>
{};

template<template<typename, typename> class l_, unsigned s_, typename a_>
struct link_size<l_<a_, void>, s_> :
    integer<unsigned, s_ + (equal_plain<nothing, a_>::result ? 0 : 1)>
{};

template<template<typename, typename> class l_, unsigned s_, typename a_, typename b_>
struct link_size<l_<a_, l_<b_, void> >, s_> :
    integer<unsigned, s_ + 2>
{};

template<template<typename, typename> class l_, unsigned s_, typename a_, typename b_, typename c_>
struct link_size<l_<a_, l_<b_, l_<c_, void> > >, s_> :
    integer<unsigned, s_ + 3>
{};

template<template<typename, typename> class l_, unsigned s_, typename a_, typename b_, typename c_, typename d_>
struct link_size<l_<a_, l_<b_, l_<c_, l_<d_, void> > > >, s_> :
    integer<unsigned, s_ + 4>
{};

template<template<typename, typename> class l_, unsigned s_, typename a_, typename b_, typename c_, typename d_, typename e_>
struct link_size<l_<a_, l_<b_, l_<c_, l_<d_, l_<e_, void> > > > >, s_> :
    integer<unsigned, s_ + 5>
{};

template<template<typename, typename> class l_, unsigned s_, typename a_, typename b_, typename c_, typename d_, typename e_, typename f_>
struct link_size<l_<a_, l_<b_, l_<c_, l_<d_, l_<e_, l_<f_, void> > > > > >, s_> :
    integer<unsigned, s_ + 6>
{};

template<template<typename, typename> class l_, unsigned s_, typename a_, typename b_, typename c_, typename d_, typename e_, typename f_, typename g_>
struct link_size<l_<a_, l_<b_, l_<c_, l_<d_, l_<e_, l_<f_, l_<g_, void> > > > > > >, s_> :
    integer<unsigned, s_ + 7>
{};

template<template<typename, typename> class l_, unsigned s_, typename a_, typename b_, typename c_, typename d_, typename e_, typename f_, typename g_, typename h_>
struct link_size<l_<a_, l_<b_, l_<c_, l_<d_, l_<e_, l_<f_, l_<g_, l_<h_, void> > > > > > > >, s_> :
    integer<unsigned, s_ + 8>
{};

template<template<typename, typename> class l_, unsigned s_, typename a_, typename b_, typename c_, typename d_, typename e_, typename f_, typename g_, typename h_, typename n_>
struct link_size<l_<a_, l_<b_, l_<c_, l_<d_, l_<e_, l_<f_, l_<g_, l_<h_, n_> > > > > > > >, s_> :
    link_size<n_, s_ + 8>
{};


template<typename link_>
struct link_kind
{
    template<typename l_, int at_>
    struct at : link_at<link_, at_>
    {};

    template<typename l_>
    struct at<l_, 0> : l_
    {};

    template<typename l_, typename a_, int i_>
    struct insert : link_insert_remove_set<link_, i_, link_insert<a_> >::result
    {};

    template<typename l_>
    struct next : link_next<link_>
    {};

    template<typename l_>
    struct offset : integer<int, 0>
    {};

    template<typename l_>
    struct pull : type_plain<link_>
    {};

    template<typename l_, int i_>
    struct remove : link_insert_remove_set<link_, i_>::result
    {};

    template<typename l_, typename a_, int i_>
    struct set : link_insert_remove_set<link_, i_, link_set<a_> >::result
    {};

    template<typename l_>
    struct size : link_size<link_>
    {};

};


template<typename first_, int offset_, typename current_>
struct link_at_kind
{
    template<typename l_, int i_>
    struct at :
        ifel_plain<
            (i_ < 0),
            link_at<first_, offset_ + i_>,
            link_at<first_, offset_ + i_, current_, i_> // continue from current
        >::result
    {};

    template<typename l_, typename a_, int i_>
    struct insert : link_at<typename link_insert_remove_set<first_, offset_ + i_, link_insert<a_> >::result, offset_>
    {};

    template<typename l_>
    struct next : link_next<current_>
    {};

    template<typename l_>
    struct offset : integer<int, offset_>
    {};

    template<typename l_>
    struct pull : ifel_plain<offset_ == 0, first_, link_at<first_, offset_, current_, 0> >
    {};

    template<typename l_, int i_>
    struct remove : link_at<typename link_insert_remove_set<first_, offset_ + i_>::result, offset_>
    {};

    template<typename l_, typename a_, int i_>
    struct set : link_at<typename link_insert_remove_set<first_, offset_ + i_, link_set<a_> >::result, offset_>
    {};

    template<typename l_>
    struct size : link_size<current_>
    {};

};


template<typename first_, int offset_>
struct link_at_kind<first_, offset_, void>
{
    template<typename l_, int i_>
    struct at : link_at<first_, offset_ + i_>
    {};

    template<typename l_, typename a_, int i_>
    struct insert : link_at<typename link_insert_remove_set<first_, offset_ + i_, link_insert<a_> >::result, offset_>
    {};

    template<typename l_>
    struct next : ifel_plain<offset_ == -1, first_, link_at<first_, offset_ + 1> >::result
    {};

    template<typename l_>
    struct offset : integer<int, offset_>
    {};

    template<typename l_>
    struct pull : ifel_plain<offset_ == 0, first_, link_at<first_, offset_> >
    {};

    template<typename l_, int i_>
    struct remove : link_at<typename link_insert_remove_set<first_, offset_ + i_>::result, offset_>
    {};

    template<typename l_, typename a_, int i_>
    struct set : link_at<typename link_insert_remove_set<first_, offset_ + i_, link_set<a_> >::result, offset_>
    {};

    template<typename l_>
    struct size : integer<unsigned, 0>
    {};

};

}}
#endif
