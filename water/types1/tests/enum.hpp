// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_TYPES1_TESTS_ENUM_HPP
#define WATER_TYPES1_TESTS_ENUM_HPP
#include <water/types1/types.hpp>
namespace water { namespace types1 { namespace tests {
namespace enum_hpp {

enum type1 { value1 };
enum type2 : unsigned long long { value2 };
enum class type3 : unsigned long long { value3 };

int constexpr i = static_cast<int>(type3::value3);

struct not_enum {};

int constexpr assert1 =
    type_assert<is_enum<type1>>::assert +
    type_assert<is_enum<type2>>::assert +
    type_assert<is_enum<type3>>::assert +
    type_assert<nots<is_enum<type1&>>>::assert +
    type_assert<nots<is_enum<type2 const>>>::assert +
    type_assert<nots<is_enum<type3 volatile>>>::assert +
    type_assert<nots<is_enum<type3&&>>>::assert +
    type_assert<nots<is_enum<type1 const volatile&>>>::assert +
    type_assert<nots<is_enum<int>>>::assert +
    type_assert<nots<is_enum<void>>>::assert +
    type_assert<nots<is_enum<not_enum>>>::assert +
    type_assert<nots<is_enum<not_enum*>>>::assert;

}
}}}
#endif
