// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_TYPES_ASSERT_HPP
#define WATER_TYPES_ASSERT_HPP
#include <water/types/to_bool.hpp>
namespace water { namespace types {

/*

type_assert<type_>::assert generates a compile time error when types::to_bool<type_>::result is false.
This error will usually describe the type of type_, static_assert sometimes does not.
(This also works with C++ 98)  

The techique is very simple:

	template<bool if_> struct if_assert        { enum { assert = 1 }; };
	template<>         struct if_assert<false> {};
	bool const b = ...;
	int constexpr e = if_assert<b>::assert;
	static_assert(if_assert<b>::assert, "hello");
	static_cast<void>(if_assert<b>::assert);

When b above is false, the compiler error can look like
- assert is not a member of if_assert<0>
- if_assert<0> has no member assert
- undefined identifier if_assert<0>::assert

This is not very useful. but when the template argument is a type instead of bool, the compiler error
usually describes the type.

Example: a function requires it's template argument to be a built in integer type:

	template<typename type_> type_ function(type_ a) {
		static_cast<void>(types::type_assert<types::is_int<type_>>::assert);
		return a;
		}

If function(1.23) is called (with a double) the compiler error could be:
"assert is not a member of types::type_assert<types::is_int<double>>"

The assert member must be used in the code, static_assert(type_assert<...>::assert, "") is a simple way to use it.

*/

template<int if_> struct
 if_assert {
	enum type { assert = 1 }; // 1 to make it work in static_assert
	};
template<> struct
 if_assert<0>
	{};

template<typename if_> struct
 type_assert :
	if_assert<to_bool<if_>::result>
		{};

}}
#endif
