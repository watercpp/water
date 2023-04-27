// Copyright 2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_TESTS_SIZE_FROM_HPP
#define WATER_TESTS_SIZE_FROM_HPP
#include <water/water.hpp>
#include <water/test.hpp>
#include <water/size_from.hpp>
#include <water/begin_end.hpp>
#include <water/downgrade_iterators/downgrade_iterators.hpp>
namespace water { namespace tests {

/*

basic tests for water::size_from

*/

template<typename a_, typename = void>
bool constexpr size_from_exists = false;

template<typename a_>
bool constexpr size_from_exists<a_, to_void<decltype(size_from(make_type<a_&>()))>> = true;

template<typename>
bool constexpr size_from_test = true;

template<typename size_>
struct size_from_test_type {
    size_ size();
};

template<typename type_>
struct size_from_test_begin_end {
    type_ begin();
    type_ end();
};

static_assert(size_from_exists<size_from_test_type<signed>>, "");
static_assert(size_from_exists<size_from_test_type<unsigned>>, "");
static_assert(size_from_exists<int const[123]>, "");
static_assert(size_from_exists<begin_end<int*>>, "");
static_assert(size_from_exists<begin_end<downgrade_iterators::forward<int*>>>, "");

static_assert(!size_from_exists<int>, "");
static_assert(!size_from_exists<int*>, "");
static_assert(!size_from_exists<size_from_test_type<int*>>, "");
static_assert(!size_from_exists<size_from_test_type<void>>, "");
static_assert(!size_from_exists<size_from_test_begin_end<void>>, "");
//static_assert(!size_from_exists<size_from_test_begin_end<void*>>, ""); // loophole
static_assert(!size_from_exists<size_from_test_begin_end<int>>, "");
static_assert(!size_from_exists<size_from_test_begin_end<size_from_test_begin_end<void>>>, "");

static_assert(size_from_test<decltype(size_from(size_from_test_type<signed>{}))>, "");
static_assert(size_from_test<decltype(size_from(size_from_test_type<unsigned>{}))>, "");
static_assert(size_from_test<decltype(size_from(make_type<int const(&)[123]>()))>, "");
static_assert(size_from_test<decltype(size_from(make_type<begin_end<int*>>()))>, "");
static_assert(size_from_test<decltype(size_from(make_type<begin_end<downgrade_iterators::forward<int*>>>()))>, "");

}}
#endif
