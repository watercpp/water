// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_TYPES_TESTS_LIST_HPP
#define WATER_TYPES_TESTS_LIST_HPP
#include <water/types/types.hpp>
namespace water { namespace types { namespace tests {
namespace list_hpp {

struct at0{};
struct at1{};
struct at2{};
struct at3{};
struct at4{};
struct at5{};
struct at6{};
struct at7{};
struct at8{};
struct at9{};
struct at10{};
struct at11{};
struct at12{};
struct at13{};
struct at14{};
struct at15{};
struct at16{};
struct at17{};
struct at18{};
struct at19{};
struct at20{};
struct at21{};
struct at22{};
struct at23{};
struct at24{};
struct at25{};
struct at26{};
struct at27{};
struct at28{};
struct at29{};
struct at30{};
struct at31{};

using list32 = list<at0, at1, at2, at3, at4, at5, at6, at7, at8, at9, at10, at11, at12, at13, at14, at15, at16, at17, at18, at19, at20, at21, at22, at23, at24, at25, at26, at27, at28, at29, at30, at31>;


// at basic, so all specialziations of list_at work
static_assert(value<equal<at0, at<list32, 0>>>, "");
static_assert(value<equal<at1, at<list32, 1>>>, "");
static_assert(value<equal<at2, at<list32, 2>>>, "");
static_assert(value<equal<at3, at<list32, 3>>>, "");
static_assert(value<equal<at4, at<list32, 4>>>, "");
static_assert(value<equal<at5, at<list32, 5>>>, "");
static_assert(value<equal<at6, at<list32, 6>>>, "");
static_assert(value<equal<at7, at<list32, 7>>>, "");
static_assert(value<equal<at9, at<list32, 9>>>, "");
static_assert(value<equal<at10, at<list32, 10>>>, "");
static_assert(value<equal<at11, at<list32, 11>>>, "");
static_assert(value<equal<at12, at<list32, 12>>>, "");
static_assert(value<equal<at13, at<list32, 13>>>, "");
static_assert(value<equal<at14, at<list32, 14>>>, "");
static_assert(value<equal<at15, at<list32, 15>>>, "");

// at far
static_assert(value<equal<at19, at<list32, 19>>>, "");
static_assert(value<equal<at31, at<list32, 31>>>, "");

// at out of bounds
static_assert(value<nots<is_result<at<list32, 32>>>>, "");
static_assert(value<nots<is_result<at<list32, 1234>>>>, "");
static_assert(value<nots<is_result<at<list32, -1>>>>, "");
static_assert(value<nots<is_result<at<list32, -1234>>>>, "");

// at combo
static_assert(value<equal<
    at31,
    next<next<at<list32, 29>>>
>>, "");
static_assert(value<equal<
    at0,
    at<at<list32, 31>, -31>
>>, "");
static_assert(value<equal<
    at0,
    pull<at<at<list32, 31>, -31>>::result // this is the collection
>>, "");
static_assert(value<equal<
    at0,
    pull<at<at<list32, 31>, -31>>::result::result // this is at0
>>, "");

// offset + size
static_assert(value<equal<
    enum_result<5>,
    offset<at<list32, 5>>
>>, "");
static_assert(value<equal<
    enum_result<32 - 5>,
    size<at<list32, 5>>
>>, "");

// insert
static_assert(value<equal<
    int,
    at<insert<list32, int, 19>, 19>
>>, "");

// set
static_assert(value<equal<
    int,
    at<set<list32, int, 13>, 13>
>>, "");

// remove
static_assert(value<equal<
    at11,
    at<remove<list32, 10>, 10>
>>, "");


template<typename ...> struct not_list;

template<typename a_, typename b_, typename c_> struct three;


static_assert(value<equal<
    enum_result<3>,
    size<list_from<three<int, int, int>>>
>>, "");

static_assert(value<equal<
    enum_result<3>,
    size<list_from<not_list<int, int, int>>>
>>, "");;


}
}}}
#endif
