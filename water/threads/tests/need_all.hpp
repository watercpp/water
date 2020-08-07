// Copyright 2017-2018 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_TESTS_NEED_ALL_HPP
#define WATER_THREADS_TESTS_NEED_ALL_HPP
#include <water/threads/tests/bits.hpp>
#include <water/threads/bits.hpp>
namespace water { namespace threads { namespace tests {

inline void need_all() {
    unsigned constexpr
        all = needs<
            need_system,
            need_water,
            need_timeout,
            need_recursive,
            need_constexpr_constructor,
            need_trivial_destructor,
            need_spin
        >::need,
        none = needs<>::need,
        some = needs<
            need_water,
            need_recursive,
            need_trivial_destructor
        >::need;
    static_assert(all == (1 << 7) - 1, "test");
    static_assert(none == 0, "test");
    static_assert(some != 0, "test");
}

}}}
#endif
