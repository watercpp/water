// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_TESTS_VECTORS_MORE_HPP
#define WATER_TESTS_VECTORS_MORE_HPP
#include <water/tests/vectors/bits.hpp>
namespace water { namespace tests { namespace vectors {

// test insert value from self??
// should be done with a vector of integers/floats

struct more_one {
    template<typename vector_>
    more_one(vector_&& a) {
        a.push_back({});
        do a.push_back(a.back() + 1); while(a.size() != 33);
        a.insert(a.begin(), a[1]);
        ___water_test(a[0] == a[2]);
    }
};

struct more {
    more() {
        more_one{vector<int>()};
    }
};

}}}
#endif
