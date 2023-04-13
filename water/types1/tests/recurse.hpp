// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_TYPES1_TESTS_RECURSE_HPP
#define WATER_TYPES1_TESTS_RECURSE_HPP
namespace water { namespace types1 { namespace tests {

// test compiler template instantation depth

unsigned constexpr recurse_depth = 1026;

template<unsigned at_>
struct recurse2;

template<unsigned at_ = 0>
struct recurse : recurse2<at_ + 1>
{};

template<>
struct recurse<recurse_depth> {
    enum type { result };
};

template<unsigned at_>
struct recurse2 : recurse<at_ + 1>
{};

template<>
struct recurse2<recurse_depth> {
    enum type { result };
};

int constexpr recurse_result = recurse<>::result;

}}}
#endif
