// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_TYPES_ORS_HPP
#define WATER_TYPES_ORS_HPP
#include <water/types/ifel.hpp>
#include <water/types/to_bool.hpp>
namespace water { namespace types {

/*

Result the first in list_ where types::to_bool<x>::result is true.
If none is true, this has no result.

Does not evaluate anything after the first true

Use types::to_bool<types::ors<....>>::result to get a bool

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
