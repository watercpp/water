// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_TYPES_TESTS_LINK_HPP
#define WATER_TYPES_TESTS_LINK_HPP
#include <water/types/types.hpp>
namespace water { namespace types { namespace tests {
namespace link_hpp {

typedef insert<link<>, void> l1;
typedef insert<l1, int> l2;
typedef insert<l2, type_plain<l2>, size<l2>::result> l3;

template<typename a_, typename b_, int at_ = 0, int a_size_ = size<a_>::result, int b_size_ = size<b_>::result>
struct equal_all : false_result
{};

template<typename a_, typename b_, int size_>
struct equal_all<a_, b_, size_, size_, size_> : true_result
{};

template<typename a_, typename b_, int at_, int size_>
struct equal_all<a_, b_, at_, size_, size_> : ifel_type<
    equal<at<a_, at_>, at<b_, at_>>,
    equal_all<a_, b_, at_ + 1>,
    false_result
> {};


static_assert(value<equal<l1, void>>, "");

static_assert(value<equal<l2, int>>, "");

static_assert(value<equal<at<l2, 1>, void>>, "");

static_assert(value<equal<at<l3, 2>, type_plain<l2>>>, "");

static_assert(value<equal_plain<typename at<l3, 2>::result, l2> >, "");

static_assert(value<equal_plain<types::pull<l2>::result, link<int, link<void> > > >, "");

static_assert(value<equal_all<
    l3,
    link<int, link<void, link<type_plain<l2> > > >
>>, "");

static_assert(value<nots<enum_result<
    offset<at<insert<at<l3, 3>, long>, -3> >::result
> >>, "");

static_assert(value<equal<
    size<at<insert<at<l3, 3>, long>, -3> >,
    size<insert<l3, long, 3> >
>>, "");

static_assert(value<equal_all<
    at<insert<at<l3, 3>, long>, -3>,
    insert<l3, long, 3>
>>, "");

}
}}}
#endif
