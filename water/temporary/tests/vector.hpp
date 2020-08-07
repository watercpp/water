// Copyright 2017-2018 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_TEMPORARY_TESTS_VECTOR_HPP
#define WATER_TEMPORARY_TESTS_VECTOR_HPP
#include <water/temporary/tests/bits.hpp>
#include <water/tests/vectors/all.hpp>
namespace water { namespace temporary { namespace tests {

struct vector_test {
    template<typename memory_>
    void operator()(memory_& memory) {
        using allocator_type = decltype(allocator_for(memory));
        allocator_type a = allocator_for(memory);
    
        using namespace water::tests::vectors;
    
        //access
        {
            {
                vector<int, allocator_type> v(5, 0, a);
                access_one{v};
            }
            value_complex_count c;
            {
                vector<value_complex, allocator_type> v(5, value_complex(c), a);
                access_one{v};
            }
            ___water_test(c.count == 0);
            {
                vector<value_simple, allocator_type> v(5, value_simple{}, a);
                access_one{v};
            }
        }
    
        //modify
        {
            modify_one{vector<int, allocator_type>(5, 0, a)};
            value_complex_count c;
            modify_one{vector<value_complex, allocator_type>(5, value_complex(c), a)};
            ___water_test(c.count == 0);
            modify_one{vector<value_simple, allocator_type>(5, value_simple{}, a)};
        }
    
        //more
        {
            more_one(vector<int, allocator_type>(a));
        }
    
        //move
        {
            vector<value_move_only, allocator_type> v{a};
            move_one{v};
        }
    }
};

inline void vector_all() {
    memory_types_function(vector_test{});
}

}}}
#endif
