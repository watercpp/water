// Copyright 2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_TEMPORARY_TESTS_STD_ALLOCATOR_HPP
#define WATER_TEMPORARY_TESTS_STD_ALLOCATOR_HPP
#include <water/temporary/tests/bits.hpp>
#ifdef WATER_NO_STD
namespace water { namespace temporary { namespace tests {
    inline void std_allocator_all() {}
}}}
#else
#include <list>
namespace water { namespace temporary { namespace tests {

/*

test that std::list works with temporary

*/

struct std_allocator_test {
    template<typename memory_>
    void operator()(memory_& memory) {
        using allocator = std_allocator<int, memory_>;
        allocator a{memory};
        using container = std::list<int, allocator>;
        container
            c1{a},
            c2{{1, 2, 3}, a},
            c3{typename container::allocator_type{memory}},
            c4{c1},
            c5{container{a}},
            c6{c2.get_allocator()};
        
        swap(c3, c4);
        c1 = c2;
        c5 = container{};
        for(int i = 0; i != 123; ++i)
            c1.push_back(typename container::value_type{});
        c2.assign(c1.begin(), c1.end());
        c3.resize(12);
    }
};

inline void std_allocator_all() {
    memory_types_function(std_allocator_test{});
}

}}}
#endif
#endif
