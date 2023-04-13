// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_TYPES1_NOTS_HPP
#define WATER_TYPES1_NOTS_HPP
#include <water/types1/ifel.hpp>
#include <water/types1/to_bool.hpp>
namespace water { namespace types1 {

/*

Result true or false. True if types1::to_bool<type_>::result is false for all types in list_

Stops evaluation at the first true, if that matters

This can be useful with a single type x, to get !types1::to_bool<x>::result

types1::ands is the opposite

nots<>::result is true

*/

template<typename ...list_>
struct nots;

template<typename at_, typename ...list_>
struct nots<at_, list_...> :
    ifel<to_bool<at_>::result, false_result, nots<list_...>>
{};

template<>
struct nots<> :
    true_result
{};

}}
#endif
