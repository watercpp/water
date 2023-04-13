// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_TYPES1_ANDS_HPP
#define WATER_TYPES1_ANDS_HPP
#include <water/types1/ifel.hpp>
#include <water/types1/to_bool.hpp>
namespace water { namespace types1 {

/*

Result true or false. True if types1::to_bool<type_>::result is true for all types in list_

Stops evaluation at the first false, if that matters

types1::nots is the opposite

ands<>::result is false

*/

template<typename ...list_>
struct ands;

template<typename at_, typename ...list_>
struct ands<at_, list_...> :
    ifel<to_bool<at_>::result, ands<list_...>, false_result>
{};

template<typename at_>
struct ands<at_> :
    to_bool<at_>
{};

template<>
struct ands<> :
    false_result
{};

}}
#endif
