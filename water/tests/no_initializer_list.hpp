// Copyright 2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_TESTS_NO_INITIALIZER_LIST_HPP
#define WATER_TESTS_NO_INITIALIZER_LIST_HPP
#include <water/test.hpp>
#include <water/types.hpp>
#include <water/initializer_list.hpp>
#include <water/no_initializer_list.hpp>
namespace water { namespace tests {

/*

Test that no_initializer_list is not dumber than it should be

*/

struct initializer_list_test_argument {};


template<typename type_>
class no_initializer_list_test_class
{
public:
    no_initializer_list_test_class(no_initializer_list<type_> = {}, initializer_list_test_argument const& = {}) {}
    no_initializer_list_test_class& operator=(no_initializer_list_test_class const&) = default;
    no_initializer_list_test_class& operator=(no_initializer_list_not_empty<type_>) { return *this; }
    void function(no_initializer_list<type_> const&) {}
};

using no_initializer_list_test_1 = to_void<
    decltype(no_initializer_list_test_class<int>{}),
    decltype(no_initializer_list_test_class<int>{{1}, initializer_list_test_argument{}}),
    decltype(no_initializer_list_test_class<int>{{1, 2}, initializer_list_test_argument{}}),
    decltype(no_initializer_list_test_class<int>{{1, 2, 3}}),
    decltype(make_type<no_initializer_list_test_class<int>&>() = {}),
    decltype(make_type<no_initializer_list_test_class<int>&>() = {1}),
    decltype(make_type<no_initializer_list_test_class<int>&>() = {1, 2}),
    decltype(make_type<no_initializer_list_test_class<int>&>() = {1, 2, 3}),
    decltype(make_type<no_initializer_list_test_class<int>&>().function({1, 2, 3})),
    decltype(make_type<no_initializer_list_test_class<int>&>().function({}))
>;


template<typename type_>
class initializer_list_test_class
{
public:
    initializer_list_test_class() = default;
    initializer_list_test_class(initializer_list<type_>, initializer_list_test_argument const& = {}) {}
    initializer_list_test_class& operator=(initializer_list_test_class const&) = default;
    initializer_list_test_class& operator=(initializer_list_std_or_not_empty<type_>) { return *this; }
    void function(initializer_list<type_> const&) {}
};

using initializer_list_test_1 = to_void<
    #ifndef WATER_NO_STD
    decltype(initializer_list_test_class<int>{}),
    decltype(initializer_list_test_class<int>{1, 2, 3}),
    decltype(initializer_list_test_class<int>{1, 2, 3, {}}), // 1 2 3 0
    #endif
    decltype(initializer_list_test_class<int>{{1}, initializer_list_test_argument{}}),
    decltype(initializer_list_test_class<int>{{1, 2, 3}}),
    decltype(make_type<initializer_list_test_class<int>&>() = {}),
    decltype(make_type<initializer_list_test_class<int>&>() = {1}),
    decltype(make_type<initializer_list_test_class<int>&>() = {1, 2}),
    decltype(make_type<initializer_list_test_class<int>&>() = {1, 2, 3}),
    decltype(make_type<initializer_list_test_class<int>&>().function({1, 2, 3}))
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

template<typename type_, template<typename> class list_>
struct std_initializer_list_do<type_, list_<int>> {
    using result = list_<type_>;
};

template<typename type_>
using std_initializer_list = typename std_initializer_list_do<type_>::result;

static_assert(equal<
    std_initializer_list<double>,
    std::initializer_list<double>
>, "");

#endif



class no_initializer_list_complicated
{
    bool myalive = true;

public:

    explicit no_initializer_list_complicated(initializer_list_test_argument const&)
    {}

    no_initializer_list_complicated(no_initializer_list_complicated const& a) :
        myalive{a.myalive}
    {}
    
    ~no_initializer_list_complicated() {
        myalive = false;
    }
    
    no_initializer_list_complicated& operator=(no_initializer_list_complicated const&) = delete;
    
    bool alive() const {
        return myalive;
    }
};


inline void no_initializer_list_test_complicated(no_initializer_list<no_initializer_list_complicated> list) {
    for(auto &a : list) {
        ___water_test(a.alive());
    }
}


inline void no_initializer_list_test_basic(no_initializer_list<int> list) {
    auto i = 0;
    for(auto a : list) {
        ___water_test(a == i);
        ++i;
    }
} 


inline void no_initializer_list_all() {
    no_initializer_list_test_complicated({});
    no_initializer_list_test_complicated({no_initializer_list_complicated{initializer_list_test_argument{}}});
    no_initializer_list_complicated c{initializer_list_test_argument{}};
    no_initializer_list_test_complicated({no_initializer_list_complicated{initializer_list_test_argument{}}, c,});
    no_initializer_list_test_complicated({no_initializer_list_complicated{initializer_list_test_argument{}}, c, c});
    no_initializer_list_test_complicated({no_initializer_list_complicated{initializer_list_test_argument{}}, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c});
    
    //auto const& list = no_initializer_list<no_initializer_list_complicated>{no_initializer_list_complicated{initializer_list_test_argument{}}, c, c};
    //___water_test(!list.begin()->alive());
    
    no_initializer_list_test_basic({});
    no_initializer_list_test_basic({0});
    no_initializer_list_test_basic({0, 1});
    no_initializer_list_test_basic({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21});
}


}}
#endif
