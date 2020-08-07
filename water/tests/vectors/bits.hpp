// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_TESTS_VECTORS_BITS_HPP
#define WATER_TESTS_VECTORS_BITS_HPP
#include <water/test.hpp>
#include <water/vector.hpp>
#include <water/downgrade_iterators/downgrade_iterators.hpp>
#include <water/tests/vectors/value_complex.hpp>
#include <water/tests/vectors/value_move_only.hpp>
#include <water/tests/vectors/value_simple.hpp>
namespace water { namespace tests { namespace vectors {

template<typename vector_, typename ...arguments_>
void fill_emplace(vector_& v, size_t count, arguments_&& ...arguments) {
    if(!count)
        return;
    size_t reserve = v.size() + count;
    v.reserve(reserve);
    ___water_test(reserve);
    do v.emplace_back(static_cast<arguments_&&>(arguments)...); while(--count);
    ___water_test(v.size() == reserve);
}

}}}
#endif
