// Copyright 2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_TYPES_TESTS_RESULT_HPP
#define WATER_TYPES_TESTS_RESULT_HPP
#include <water/types/types.hpp>
namespace water { namespace types { namespace tests {
namespace type_hpp {

// type.hpp
static_assert(value<equal<int, type<int>>>, "");
static_assert(value<equal<int, type<type<type<type<int>>>>>>, "");
static_assert(value<equal<int, type_plain<int>>>, "");

// nontype.hpp

static_assert(value<equal<nontype<int, 123>, nontype<int, 123>>>, "");
static_assert(!value<equal<nontype<int, 1>, nontype<int, 2>>>, "");
static_assert(value<equal<
    nontype<unsigned long long, static_cast<unsigned long long>(-1)>,
    nontype<nontype<nontype<type<unsigned long long>, static_cast<unsigned long long>(-1)>>>
>>, "");
static_assert(value<equal<enum_result<1>, nontype<int, 1>>>, "");
static_assert(!value<equal<enum_result<2>, nontype<int, 1>>>, "");

// to_bool.hpp
static_assert(!value<to_bool<int>>, "");
static_assert(value<to_bool<type<int>>>, "");
static_assert(value<to_bool<nontype<int, 123>>>, "");
static_assert(!value<to_bool<nontype<int, 0>>>, "");


// divide<0>::result is a compile time error
template<int divide_>
struct divide {
    static_assert(divide_ != 0, "");
    using result_tag = nontype_tag;
    using type = int;
    static type constexpr result = 1 / divide_;
};

//static_assert(value<is_result<divide<0>>>, "");
static_assert(value<to_bool<divide<1>>>, "");
//static_assert(value<to_bool<divide<0>>>, "");


struct bad_type {
    using result_tag = type_tag;
    // missing result
};

static_assert(value<is_result<bad_type>>, "");

// if divide<0> or bad_type is evaluated by ifel, ands, nots, ors below there is a compile time failure.


// ifel

static_assert(value<ifel<true, true_result>>, "");
static_assert(value<ifel<true, true_result, false_result>>, "");
static_assert(!value<ifel<false, true_result, false_result>>, "");
static_assert(!value<ifel<false, nothing, false_result>>, "");

static_assert(value<equal<short, ifel<true, short>>>, "");
static_assert(value<equal<short, ifel<true, any_result<short>, any_result<long>>>>, "");
static_assert(value<equal<long, ifel<false, any_result<short>, any_result<long>>>>, "");
static_assert(value<equal<long, ifel<false, nothing, long>>>, "");
static_assert(!value<is_result<ifel<false, int>>>, "");
static_assert(!value<is_result<ifel<true, nothing, int>>>, "");
static_assert(value<equal<
    typename ifel<true, int, ifel<false, bad_type, ifel<true, int, bad_type>>>::result,
    int
>>, "");

static_assert(value<ifel_type<true_result, true_result>>, "");
static_assert(value<ifel_type<true_result, true_result, false_result>>, "");
static_assert(!value<ifel_type<false_result, true_result, false_result>>, "");
static_assert(!value<ifel_type<false_result, nothing, false_result>>, "");

static_assert(value<ifel_type_not<false_result, true_result>>, "");
static_assert(value<ifel_type_not<false_result, true_result, false_result>>, "");
static_assert(!value<ifel_type_not<true_result, true_result, false_result>>, "");
static_assert(!value<ifel_type_not<true_result, nothing, false_result>>, "");

static_assert(value<equal<short, ifel_plain<true, short>>>, "");
static_assert(value<equal<short, ifel_plain<true, short, long>>>, "");
static_assert(value<equal<long, ifel_plain<false, short, long>>>, "");
static_assert(value<equal<long, ifel_plain<false, nothing, long>>>, "");


// if_not_void

static_assert(!value<is_result<if_not_void<void>>>, "");
static_assert(!value<is_result<if_not_void<type<void>>>>, "");
static_assert(!value<is_result<if_not_void<type<void>, type<void>>>>, "");
static_assert(value<if_not_void<true_result>>, "");
static_assert(value<if_not_void<void, true_result>>, "");

static_assert(value<if_not_void<true_result, bad_type>>, "");
static_assert(value<if_not_void<true_result, divide<0>>>, "");


// ands nots ors

static_assert(!value<ands<>>, "");
static_assert(!value<ands<true_result, true_result, false_result, divide<0>>>, "");
static_assert(!value<ands<true_result, true_result, int, divide<0>>>, "");
static_assert(value<ands<true_result, true_result, type<int>>>, "");

static_assert(value<nots<>>, "");
static_assert(!value<nots<false_result, false_result, true_result, divide<0>>>, "");
static_assert(value<nots<false_result, false_result, int>>, "");
static_assert(!value<nots<false_result, false_result, type<int>, divide<0>>>, "");

static_assert(!value<is_result<ors<>>>, "");
static_assert(!value<is_result<ors<int, int, false_result, false_result, int>>>, "");
static_assert(value<equal<void, ors<false_result, false_result, type<void>, divide<0>>>>, "");
static_assert(value<equal<divide<1>, is_result<ors<false_result, int, false_result, false_result, divide<1>>>>>, "");


// any_result

static_assert(value<equal<int, any_result<int>>>, "");
static_assert(value<equal<int, any_result<type<int>>>>, "");
static_assert(value<equal<nontype<int, 123>, any_result<nontype<int, 123>>>>, "");
static_assert(!value<is_result<any_result<>>>, "");


// any_type

static_assert(value<equal<int, any_type<int>>>, "");
static_assert(value<equal<int, any_type<type<int>>>>, "");
static_assert(value<equal<int, any_type<nontype<int, 123>>>>, "");


// result

static_assert(!value<is_result<int>>, "");
static_assert(value<is_result<type<int>>>, "");
static_assert(value<is_result<true_result>>, "");
static_assert(!value<is_result<any_result<>>>, "");

static_assert(value<equal<void, result_tag_of<int>>>, "");
static_assert(value<equal_plain<result_tag<type_kind>, typename result_tag_of<type<int>>::result>>, "");
static_assert(value<equal_plain<result_tag<nontype_kind>, typename result_tag_of<true_result>::result>>, "");

static_assert(value<equal<void, result_kind_of<int>>>, "");
static_assert(value<equal<type_kind, result_kind_of<type<int>>>>, "");




}
}}}
#endif
