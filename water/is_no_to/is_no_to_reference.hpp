// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_IS_NO_TO_IS_NO_TO_REFERENCE_HPP
#define WATER_IS_NO_TO_IS_NO_TO_REFERENCE_HPP
#include <water/is_no_to/bits.hpp>
namespace water { namespace is_no_to {



template<typename>
bool constexpr is_reference = false;

template<typename a_>
bool constexpr is_reference<a_&> = true;

template<typename a_>
bool constexpr is_reference<a_&&> = true;


template<typename a_>
struct no_reference : type<a_> {};

template<typename a_>
struct no_reference<a_&> : type<a_> {};

template<typename a_>
struct no_reference<a_&&> : type<a_> {};


template<typename a_>
struct to_reference : type<a_&> {};

template<typename a_>
struct to_reference<a_&> : type<a_&> {};

template<typename a_>
struct to_reference<a_&&> : type<a_&> {};



template<typename>
bool constexpr is_rvalue_reference = false;

template<typename a_>
bool constexpr is_rvalue_reference<a_&&> = true;


template<typename a_>
struct no_rvalue_reference : type<a_> {};

template<typename a_>
struct no_rvalue_reference<a_&&> : type<a_> {};


template<typename a_>
struct to_rvalue_reference : type<a_&&> {};

template<typename a_>
struct to_rvalue_reference<a_&> : type<a_&&> {};

template<typename a_>
struct to_rvalue_reference<a_&&> : type<a_&&> {};



}}
#endif
