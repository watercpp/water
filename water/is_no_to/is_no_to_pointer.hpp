// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_IS_NO_TO_IS_NO_TO_POINTER_HPP
#define WATER_IS_NO_TO_IS_NO_TO_POINTER_HPP
#include <water/is_no_to/bits.hpp>
namespace water { namespace is_no_to {



template<typename>
bool constexpr is_pointer = false;

template<typename a_>
bool constexpr is_pointer<a_*> = true;



template<typename a_>
struct no_pointer : type<a_> {};

template<typename a_>
struct no_pointer<a_*> : type<a_> {};



template<typename a_>
struct to_pointer : type<a_*> {};

template<typename a_>
struct to_pointer<a_*> : type<a_*> {};

template<typename a_>
struct to_pointer<a_ *const> : type<a_*> {};

template<typename a_>
struct to_pointer<a_ *volatile> : type<a_*> {};

template<typename a_>
struct to_pointer<a_ *const volatile> : type<a_*> {};

template<typename a_>
struct to_pointer<a_&> : type<a_*> {};

template<typename a_>
struct to_pointer<a_&&> : type<a_*> {};



}}
#endif
