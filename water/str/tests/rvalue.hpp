// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_STR_TESTS_RVALUE_HPP
#define WATER_STR_TESTS_RVALUE_HPP
#include <water/str/tests/bits.hpp>
namespace water { namespace str { namespace tests {

/*

test that rvalue reference << operator works

*/

template<typename base_>
struct rvalue_out {
    unsigned what = 0;
};

template<typename base_>
rvalue_out<base_>& operator<<(rvalue_out<base_>& o, int) {
    o.what = 1;
    return o;
}

template<typename base_, typename any_>
rvalue_out<base_>& operator<<(rvalue_out<base_>&& o, any_ const& a) {
    return o << a;
}

template<typename base_>
rvalue_out<base_>& operator<<(rvalue_out<base_>& o, unsigned) {
    o.what = 2;
    return o;
}

inline void rvalue_all() {
    rvalue_out<void> r1;
    r1 << 1;
    ___water_test(r1.what == 1);
    auto what = (rvalue_out<void>{} << 1u).what;
    ___water_test(what == 2);
}

}}}
#endif
