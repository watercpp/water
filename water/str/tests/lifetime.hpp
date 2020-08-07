// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_STR_TESTS_LIFETIME_HPP
#define WATER_STR_TESTS_LIFETIME_HPP
#include <water/str/tests/bits.hpp>
namespace water { namespace str { namespace tests {

/*

test that destructor of lifetime_callback created in the expression runs after the expression

*/

struct lifetime_out {
    int *number;
    ~lifetime_out() {
        number = 0;
    }
};

struct lifetime_callback {
    int *pointer = 0;
    int number = 0;
    ~lifetime_callback() {
        if(pointer) {
            ___water_test(*pointer);
            if(pointer)
                *pointer = number;
        }
    }
    void attach(int* a) {
        number = *a;
        pointer = a;
    }
};

template<typename any_>
lifetime_out& operator<<(lifetime_out&& l, any_ const& a) {
    return l << a;
}

inline lifetime_out& operator<<(lifetime_out& l, int i) {
    *l.number = i;
    return l;
}

inline lifetime_out& operator<<(lifetime_out& l, lifetime_callback&& c) {
    c.attach(l.number);
    return l;
}

inline void lifetime_all() {
    int n = 0;
    lifetime_out{&n} << 1 << lifetime_callback{} << 2 << 3;
    ___water_test(n == 1);
}

}}}
#endif
