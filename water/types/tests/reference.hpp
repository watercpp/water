// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_TYPES_TESTS_REFERENCE_HPP
#define WATER_TYPES_TESTS_REFERENCE_HPP
#include <water/types/types.hpp>
namespace water { namespace types { namespace tests {

int constexpr reference_hpp =
	
	type_assert<is_reference<int&>>::assert +
	type_assert<is_reference<int&&>>::assert +
	type_assert<nots<is_reference<int>>>::assert +
	type_assert<nots<is_rvalue_reference<int&>>>::assert +
	type_assert<is_rvalue_reference<int&&>>::assert +
	type_assert<nots<is_rvalue_reference<int>>>::assert +
	
	type_assert<equal<no_reference<int&>, int>>::assert +
	type_assert<equal<no_reference<int&&>, int>>::assert +
	type_assert<equal<no_reference<int>, int>>::assert +
	
	type_assert<equal<to_reference<int&>, int&>>::assert +
	type_assert<equal<to_reference<int&&>, int&>>::assert +
	type_assert<equal<to_reference<int>, int&>>::assert +
	
	type_assert<equal<no_rvalue_reference<int&>, int&>>::assert +
	type_assert<equal<no_rvalue_reference<int&&>, int>>::assert +
	type_assert<equal<no_rvalue_reference<int>, int>>::assert +
	
	type_assert<equal<to_rvalue_reference<int&>, int&&>>::assert +
	type_assert<equal<to_rvalue_reference<int&&>, int&&>>::assert +
	type_assert<equal<to_rvalue_reference<int>, int&&>>::assert;
	

}}}
#endif
