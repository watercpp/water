// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_TYPES_TESTS_FUNCTION_HPP
#define WATER_TYPES_TESTS_FUNCTION_HPP
#include <water/types/list.hpp>
#include <water/types/is_function.hpp>
namespace water { namespace types { namespace tests {
namespace function_hpp {

/*

Test detecting function argument types
Test is_function

*/

template<typename function_>
struct list_from_function
{};

template<typename return_, typename ...arguments_>
struct list_from_function<return_(arguments_...)> : list<arguments_...>
{};

template<typename function_>
struct list_from_function<function_*> : list_from_function<function_>
{};

int constexpr assert1 =
    type_assert<equal<
        enum_result<3>,
        size<list_from_function<void(int, int, int)>>
    >>::assert +
    type_assert<equal<
        enum_result<0>,
        size<list_from_function<void()>>
    >>::assert +
    type_assert<equal<
        enum_result<2>,
        size<list_from_function<int(*)(int, int)>>
    >>::assert
    ;

int constexpr assert2 =
    type_assert<nots<is_function<void>>>::assert +
    type_assert<is_function<void()>>::assert +
    type_assert<nots<is_function<int(*)()>>>::assert +
    type_assert<is_function<no_pointer<int(*)(int)>>>::assert +
    type_assert<is_function<int(int, int, int, int, int, int, int, int, int, int, int, int, int, int, int)>>::assert +
    type_assert<is_function<int(...)>>::assert +
    type_assert<is_function<int(int, ...)>>::assert +
    type_assert<is_function<int(int, int, int, int, int, int, int, int, int, int, int, int, int, int, ...)>>::assert;

}
}}}
#endif
