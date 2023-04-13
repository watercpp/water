// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_IS_NO_TO_IS_NO_TO_POINTER_TO_MEMBER_HPP
#define WATER_IS_NO_TO_IS_NO_TO_POINTER_TO_MEMBER_HPP
#include <water/is_no_to/bits.hpp>
namespace water { namespace is_no_to {


// visual c++ 2015 building 32-bit x86, not 64-bit or 2017, needs the specializations for functions with ...
// probably related to error C2217: '...' requires '__cdecl'



template<typename>
bool constexpr is_pointer_to_member = false;

template<typename a_, typename b_>
bool constexpr is_pointer_to_member<a_ b_::*> = true;

template<typename a_, typename b_, typename ...c_>
bool constexpr is_pointer_to_member<a_ (b_::*)(c_..., ...)> = true;



template<typename a_>
bool constexpr is_pointer_to_member_function = false;

template<typename r_, typename c_>
bool constexpr is_pointer_to_member_function<r_ (c_::*)()> = true;

template<typename r_, typename c_, typename ...a_>
bool constexpr is_pointer_to_member_function<r_(c_::*)(a_...)> = true;

template<typename r_, typename c_, typename ...a_>
bool constexpr is_pointer_to_member_function<r_(c_::*)(a_..., ...)> = true;



template<typename a_>
struct no_pointer_to_member : type<a_> {};

template<typename a_, typename b_>
struct no_pointer_to_member<a_ b_::*> : type<a_> {};

template<typename a_, typename b_, typename ...c_>
struct no_pointer_to_member<a_ (b_::*)(c_..., ...)> : type<a_(c_..., ...)> {};



template<typename a_, typename b_>
struct to_pointer_to_member : type<a_ b_::*> {};

template<typename a_, typename b_, typename ...c_>
struct to_pointer_to_member<a_(c_..., ...), b_> : type<a_ (b_::*)(c_..., ...)> {};

template<typename a_, typename b_, typename c_>
struct to_pointer_to_member<a_ b_::*, c_> : type<a_ c_::*> {};

template<typename a_, typename b_, typename c_, typename ...d_>
struct to_pointer_to_member<a_ (b_::*)(d_..., ...), c_> : type<a_ (c_::*)(d_..., ...)> {};

template<typename a_, typename b_, typename c_>
struct to_pointer_to_member<a_ b_::*const, c_> : type<a_ c_::*> {};

template<typename a_, typename b_, typename c_, typename ...d_>
struct to_pointer_to_member<a_ (b_::*const)(d_..., ...), c_> : type<a_ (c_::*)(d_..., ...)> {};

template<typename a_, typename b_, typename c_>
struct to_pointer_to_member<a_ b_::*volatile, c_> : type<a_ c_::*> {};

template<typename a_, typename b_, typename c_, typename ...d_>
struct to_pointer_to_member<a_ (b_::*volatile)(d_..., ...), c_> : type<a_ (c_::*)(d_..., ...)> {};

template<typename a_, typename b_, typename c_>
struct to_pointer_to_member<a_ b_::*const volatile, c_> : type<a_ c_::*> {};

template<typename a_, typename b_, typename c_, typename ...d_>
struct to_pointer_to_member<a_ (b_::*const volatile)(d_..., ...), c_> : type<a_ (c_::*)(d_..., ...)> {};



template<typename a_>
struct pointer_to_member_of : type<void> {};

template<typename a_, typename b_>
struct pointer_to_member_of<a_ b_::*> : type<b_> {};

template<typename a_, typename b_, typename ...c_>
struct pointer_to_member_of<a_ (b_::*)(c_..., ...)> : type<b_> {};



}}
#endif

