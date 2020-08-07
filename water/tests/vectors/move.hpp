// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_TESTS_VECTORS_MOVE_HPP
#define WATER_TESTS_VECTORS_MOVE_HPP
#include <water/tests/vectors/bits.hpp>
namespace water { namespace tests { namespace vectors {

// test things that should work with value_move_only type

struct move_one {
    template<typename vector_>
    move_one(vector_& a) {
        value_move_only_construct c[5];
        a.reserve(5);
        size_t s = a.capacity() + 1;
        do a.emplace_back(c[0]); while(a.size() != s);
        ___water_test(a.size() == s);
        value_move_only v(static_cast<value_move_only&&>(a[0]));
        a[0] = static_cast<value_move_only&&>(v);
        a.emplace(a.begin() + a.size() / 2, c[0]);
        a.insert(a.begin() + a.size() / 2, value_move_only(c[0]));
        a.push_back(value_move_only(c[0]));
        a.insert(a.begin(), c + 0, c + 5);
        a.insert(a.end(), c + 0, 5);
        
        s = a.size();
        vector_ b{static_cast<vector_&&>(a)};
        a = static_cast<vector_&&>(b);
        swap(a, b);
        ___water_test(b.size() == s);
        
        b.erase(b.begin() + 1, b.begin() + 3);
        b.pop_back();
        
        b.reserve(b.capacity() + 1);
        b.shrink_to_fit();
        
        bool good = true;
        for(auto &r : b)
            good = r.current() && r.valid() && good;
        ___water_test(good);
    }
};


struct move {
    move() {
        vector<value_move_only> v;
        move_one{v};
    }
};

}}}
#endif
