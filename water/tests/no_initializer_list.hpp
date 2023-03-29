// Copyright 2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_TESTS_NO_INITIALIZER_LIST_HPP
#define WATER_TESTS_NO_INITIALIZER_LIST_HPP
#include <water/test.hpp>
#include <water/types/type.hpp>
#include <water/initializer_list.hpp>
namespace water { namespace tests {

/*

Test that no_initializer_list is not dumber than it should be

*/

struct initializer_list_test_argument {};


template<typename type_>
class no_initializer_list_test_class
{
public:
    no_initializer_list_test_class() = default;
    no_initializer_list_test_class(no_initializer_list<type_> const&, initializer_list_test_argument const& = {}) {}
    no_initializer_list_test_class& operator=(no_initializer_list_test_class const&) = default;
    no_initializer_list_test_class& operator=(no_initializer_list<type_> const&) { return *this; }
    void function(no_initializer_list<type_> const&) {}
};

using no_initializer_list_test_1 = types::list<
    decltype(no_initializer_list_test_class<int>{}),
    decltype(no_initializer_list_test_class<int>{{1}, initializer_list_test_argument{}}),
    decltype(no_initializer_list_test_class<int>{{1, 2, 3}}),
    decltype(types::make<no_initializer_list_test_class<int>&>() = {1, 2, 3}),
    decltype(types::make<no_initializer_list_test_class<int>&>().function({1, 2, 3}))
>;


template<typename type_>
class initializer_list_test_class
{
public:
    initializer_list_test_class(initializer_list<type_> const&, initializer_list_test_argument const& = {}) {}
    initializer_list_test_class& operator=(initializer_list_test_class const&) = default;
    initializer_list_test_class& operator=(initializer_list<type_> const&) { return *this; }
    void function(initializer_list<type_> const&) {}
};

using initializer_list_test_1 = types::list<
    #ifndef WATER_NO_STD
    decltype(initializer_list_test_class<int>{}),
    decltype(initializer_list_test_class<int>{1, 2, 3}),
    decltype(initializer_list_test_class<int>{1, 2, 3, {}}), // 1 2 3 0
    #endif
    decltype(initializer_list_test_class<int>{{1}, initializer_list_test_argument{}}),
    decltype(initializer_list_test_class<int>{{1, 2, 3}}),
    decltype(types::make<initializer_list_test_class<int>&>() = {1, 2, 3}),
    decltype(types::make<initializer_list_test_class<int>&>().function({1, 2, 3}))
>;



#ifndef WATER_NO_STD

/*

This code figures out what the std::initializer_list is without the <initializer_list> header, but
it will fail to compile without including <initializer_list>.

*/

template<typename>
struct std_initializer_list_auto {
    static constexpr auto result = {0};
};

template<typename type_, typename = decltype(std_initializer_list_auto<void>::result)>
struct std_initializer_list_do;

template<typename type_, typename list_>
struct std_initializer_list_do<type_, list_ const> :
    std_initializer_list_do<type_, list_>
{};

template<typename type_, template<typename> typename list_>
struct std_initializer_list_do<type_, list_<int>> {
    using result = list_<type_>;
};

template<typename type_>
using std_initializer_list = typename std_initializer_list_do<type_>::result;

static_assert(types::type_assert<types::equal<
    std_initializer_list<double>,
    std::initializer_list<double>
>>::assert, "");

#endif



}}
#endif
