// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_TYPES_TESTS_LIST_HPP
#define WATER_TYPES_TESTS_LIST_HPP
#include <water/types/list.hpp>
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

int constexpr assert1 =
    // at basic, so all specialziations of list_at work
    type_assert<equal<at0, at<list32, 0>>>::assert +
    type_assert<equal<at1, at<list32, 1>>>::assert +
    type_assert<equal<at2, at<list32, 2>>>::assert +
    type_assert<equal<at3, at<list32, 3>>>::assert +
    type_assert<equal<at4, at<list32, 4>>>::assert +
    type_assert<equal<at5, at<list32, 5>>>::assert +
    type_assert<equal<at6, at<list32, 6>>>::assert +
    type_assert<equal<at7, at<list32, 7>>>::assert +
    type_assert<equal<at9, at<list32, 9>>>::assert +
    type_assert<equal<at10, at<list32, 10>>>::assert +
    type_assert<equal<at11, at<list32, 11>>>::assert +
    type_assert<equal<at12, at<list32, 12>>>::assert +
    type_assert<equal<at13, at<list32, 13>>>::assert +
    type_assert<equal<at14, at<list32, 14>>>::assert +
    type_assert<equal<at15, at<list32, 15>>>::assert +
    
    // at far
    type_assert<equal<at19, at<list32, 19>>>::assert +
    type_assert<equal<at31, at<list32, 31>>>::assert +
    
    // at out of bounds
    type_assert<nots<is_result<at<list32, 32>>>>::assert +
    type_assert<nots<is_result<at<list32, 1234>>>>::assert +
    type_assert<nots<is_result<at<list32, -1>>>>::assert +
    type_assert<nots<is_result<at<list32, -1234>>>>::assert +
    
    // at combo
    type_assert<equal<
        at31,
        next<next<at<list32, 29>>>
    >>::assert +
    type_assert<equal<
        at0,
        at<at<list32, 31>, -31>
    >>::assert +
    type_assert<equal<
        at0,
        pull<at<at<list32, 31>, -31>>::result // this is the collection
    >>::assert +
    type_assert<equal<
        at0,
        pull<at<at<list32, 31>, -31>>::result::result // this is at0
    >>::assert +
    
    // offset + size
    type_assert<equal<
        enum_result<5>,
        offset<at<list32, 5>>
    >>::assert +
    type_assert<equal<
        enum_result<32 - 5>,
        size<at<list32, 5>>
    >>::assert +
    
    // insert
    type_assert<equal<
        int,
        at<insert<list32, int, 19>, 19>
    >>::assert +
    
    // set
    type_assert<equal<
        int,
        at<set<list32, int, 13>, 13>
    >>::assert +
    
    // remove
    type_assert<equal<
        at11,
        at<remove<list32, 10>, 10>
    >>::assert
    ;

template<typename ...> struct not_list;

template<typename a_, typename b_, typename c_> struct three;

int constexpr assert2 =
    type_assert<equal<
        enum_result<3>,
        size<list_from<three<int, int, int>>>
    >>::assert +
    type_assert<equal<
        enum_result<3>,
        size<list_from<not_list<int, int, int>>>
    >>::assert;
}
}}}
#endif
