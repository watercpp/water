// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_IS_NO_TO_IS_NO_TO_CONST_HPP
#define WATER_IS_NO_TO_IS_NO_TO_CONST_HPP
#include <water/is_no_to/bits.hpp>
namespace water { namespace is_no_to {



template<typename>
bool constexpr is_const = false;

template<typename a_>
bool constexpr is_const<a_ const> = true;

template<typename a_>
bool constexpr is_const<a_ const&> = true;

template<typename a_>
bool constexpr is_const<a_ const&&> = true;



template<typename a_>
struct no_const : type<a_> {};

template<typename a_>
struct no_const<a_ const> : type<a_> {};

template<typename a_>
struct no_const<a_ const&> : type<a_&> {};

template<typename a_>
struct no_const<a_ const&&> : type<a_&&> {};



template<typename a_>
struct to_const : type<a_ const> {};

template<typename a_>
struct to_const<a_ const> : type<a_ const> {};

template<typename a_>
struct to_const<a_&> : type<a_ const&> {};

template<typename a_>
struct to_const<a_ const&> : type<a_ const&> {};

template<typename a_>
struct to_const<a_&&> : type<a_ const&&> {};

template<typename a_>
struct to_const<a_ const&&> : type<a_ const&&> {};
    

}}
#endif
