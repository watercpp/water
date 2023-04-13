// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_TYPES1_ORS_HPP
#define WATER_TYPES1_ORS_HPP
#include <water/types1/ifel.hpp>
#include <water/types1/to_bool.hpp>
namespace water { namespace types1 {

/*

Result the first in list_ where types1::to_bool<x>::result is true.
If none is true, this has no result.

Does not evaluate anything after the first true

Use types1::to_bool<types1::ors<....>>::result to get a bool

*/

template<typename ...list_>
struct ors;

template<typename at_, typename ...list_>
struct ors<at_, list_...> :
    ifel<to_bool<at_>::result, at_, ors<list_...>>
{};

template<>
struct ors<> :
    any_result<>
{};

}}
#endif
