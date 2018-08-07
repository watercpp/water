// Copyright 2017-2018 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_TYPES_TESTS_POINTER_TO_MEMBER_HPP
#define WATER_TYPES_TESTS_POINTER_TO_MEMBER_HPP
#include <water/types/is_no_to_pointer_to_member.hpp>
#include <water/types/is_pointer_to_member_function.hpp>
namespace water { namespace types { namespace tests {
namespace pointer_to_memeber_hpp {

class test;
class test2;

int constexpr assert1 =
	type_assert<nots<is_pointer_to_member<void>>>::assert +
	type_assert<is_pointer_to_member<int test::*>>::assert +
	type_assert<is_pointer_to_member<int (test::*)()>>::assert +
	type_assert<is_pointer_to_member<int (test::*)(int)>>::assert +
	type_assert<is_pointer_to_member<int (test::*)(int, ...)>>::assert +
	type_assert<equal<no_pointer_to_member<int test::*>, int>>::assert +
	type_assert<equal<no_pointer_to_member<int (test::*)(int)>, int(int)>>::assert +
	type_assert<equal<no_pointer_to_member<int (test::*)(int, ...)>, int(int, ...)>>::assert +
	type_assert<equal<no_pointer_to_member<int (test::*)(...)>, int(...)>>::assert +
	type_assert<equal<pointer_to_member_of<int test::*>, test>>::assert +
	type_assert<equal<to_pointer_to_member<int, test>, int test::*>>::assert +
	type_assert<equal<to_pointer_to_member<int(int), test>, int (test::*)(int)>>::assert +
	type_assert<equal<to_pointer_to_member<int(int, ...), test>, int (test::*)(int, ...)>>::assert +
	type_assert<equal<to_pointer_to_member<int(test2::*)(int), test>, int (test::*)(int)>>::assert +
	type_assert<equal<to_pointer_to_member<int(test2::*)(int, ...), test>, int (test::*)(int, ...)>>::assert;

int constexpr assert2 =
	type_assert<nots<is_pointer_to_member_function<void>>>::assert +
	type_assert<nots<is_pointer_to_member_function<void(*)()>>>::assert +
	type_assert<is_pointer_to_member_function<int (test::*)()>>::assert +
	type_assert<is_pointer_to_member_function<int (test::*)(int)>>::assert +
	type_assert<is_pointer_to_member_function<int (test::*)(int, int, int, int, int, int, int, int, int, int, int, int, int, int, int)>>::assert +
	type_assert<is_pointer_to_member_function<int (test::*)(...)>>::assert +
	type_assert<is_pointer_to_member_function<int (test::*)(int, int, int, int, int, int, int, int, int, int, int, int, int, int, ...)>>::assert;

}
}}}
#endif
