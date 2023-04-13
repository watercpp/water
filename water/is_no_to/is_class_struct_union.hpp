// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_IS_NO_TO_IS_CLASS_STRUCT_UNION_HPP
#define WATER_IS_NO_TO_IS_CLASS_STRUCT_UNION_HPP
#include <water/is_no_to/bits.hpp>
namespace water { namespace is_no_to {

// this works for const/volatile types, but is_no_to.hpp will

template<typename a_>
true_size is_class_struct_union_test(int a_::*);

template<typename a_>
false_size is_class_struct_union_test(...);

template<typename a_>
bool constexpr is_class_struct_union = sizeof(water::is_no_to::is_class_struct_union_test<a_>(0)) == sizeof(true);

}}
#endif
