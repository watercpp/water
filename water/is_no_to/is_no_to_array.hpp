// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_IS_NO_TO_IS_NO_TO_ARRAY_HPP
#define WATER_IS_NO_TO_IS_NO_TO_ARRAY_HPP
#include <water/is_no_to/bits.hpp>
namespace water { namespace is_no_to {

// array rvalue references dont exist?



template<typename>
bool constexpr is_array = false;

template<typename a_, size_t b_>
bool constexpr is_array<a_[b_]> = true;

template<typename a_>
bool constexpr is_array<a_[]> = true;



template<typename a_>
struct no_array : type<a_> {};

template<typename a_, size_t b_>
struct no_array<a_[b_]> : type<a_> {};

template<typename a_, size_t b_>
struct no_array<a_(&)[b_]> : type<a_&> {};

template<typename a_, size_t b_>
struct no_array<a_(&&)[b_]> : type<a_&&> {};

template<typename a_>
struct no_array<a_[]> : type<a_> {};

template<typename a_>
struct no_array<a_(&)[]> : type<a_&> {};

template<typename a_>
struct no_array<a_(&&)[]> : type<a_&&> {};



template<typename a_, size_t b_>
struct to_array_with_size : type<a_[b_]> {};

template<typename a_, size_t b_>
struct to_array_with_size<a_&, b_> : type<a_(&)[b_]> {};

template<typename a_, size_t b_>
struct to_array_with_size<a_&&, b_> : type<a_(&&)[b_]> {};

template<typename a_, size_t b_, size_t c_>
struct to_array_with_size<a_[b_], c_> : type<a_[c_]> {};

template<typename a_, size_t b_, size_t c_>
struct to_array_with_size<a_(&)[b_], c_> : type<a_(&)[c_]> {};

template<typename a_, size_t b_, size_t c_>
struct to_array_with_size<a_(&&)[b_], c_> : type<a_(&&)[c_]> {};

template<typename a_, size_t b_>
struct to_array_with_size<a_[], b_> : type<a_[b_]> {};

template<typename a_, size_t b_>
struct to_array_with_size<a_(&)[], b_> : type<a_(&)[b_]> {};

template<typename a_, size_t b_>
struct to_array_with_size<a_(&&)[], b_> : type<a_(&&)[b_]> {};



template<typename>
size_t constexpr array_size = 0;

template<typename a_, size_t b_>
size_t constexpr array_size<a_[b_]> = b_;



}}
#endif
