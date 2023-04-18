// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_TEMPORARY_TESTS_VECTOR_HPP
#define WATER_TEMPORARY_TESTS_VECTOR_HPP
#include <water/temporary/tests/bits.hpp>
#include <water/tests/vectors/all.hpp>
namespace water { namespace temporary { namespace tests {

template<typename value_, typename allocator_>
void vector_template_deduction(value_ const& value, allocator_ const& a) {
    #ifdef __cpp_deduction_guides

    using water::tests::vectors::result_if_equal;

    value_ i[5] = {value, value, value, value, value};
    size_t is = 5;

    vector v4(i, a);
    static_assert(result_if_equal<vector<value_, allocator_>, decltype(v4)>::result, "");

    vector v8(i, i + is, a);
    vector v9(downgrade_iterators::forward_proxied_const_from(i), downgrade_iterators::forward_proxied_const_from(i + is), a);
    static_assert(result_if_equal<vector<value_, allocator_>, decltype(v8)>::result, "");
    static_assert(result_if_equal<vector<value_, allocator_>, decltype(v9)>::result, "");

    vector v12(i, is, a);
    vector v13(downgrade_iterators::forward_proxied_const_from(i), is, a);
    static_assert(result_if_equal<vector<value_, allocator_>, decltype(v12)>::result, "");
    static_assert(result_if_equal<vector<value_, allocator_>, decltype(v13)>::result, "");

    #endif
}

struct vector_test {
    template<typename memory_>
    void operator()(memory_& memory) {
        using allocator_type = decltype(allocator_for(memory));
        allocator_type a = allocator_for(memory);
    
        using namespace water::tests::vectors;
        
        construct_one<vector<int, allocator_type>>(0, a);
        construct_one<vector<size_t, allocator_type>>(0, a);
        construct_one<vector<double, allocator_type>>(0., a);
        value_complex_count c;
        construct_one<vector<value_complex, allocator_type>, false>(value_complex(c), a);
        ___water_test(c.count == 0);
        construct_one<vector<value_simple, allocator_type>>({}, a);
    
        //access
        {
            {
                vector<int, allocator_type> v(5, a);
                access_one{v};
            }
            value_complex_count c2;
            {
                vector<value_complex, allocator_type> v(5, value_complex(c2), a);
                access_one{v};
            }
            ___water_test(c2.count == 0);
            {
                vector<value_simple, allocator_type> v(5, value_simple{}, a);
                access_one{v};
            }
        }
    
        //modify
        {
            modify_one<>{vector<int, allocator_type>(5, a)};
            modify_one<>{vector<size_t, allocator_type>(5, a)};
            modify_one<>{vector<double, allocator_type>(5, a)};
            
            value_complex_count c2;
            modify_one<false>{vector<value_complex, allocator_type>(5, value_complex(c2), a)};
            ___water_test(c2.count == 0);
            modify_one<>{vector<value_simple, allocator_type>(5, value_simple{}, a)};
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

        vector_template_deduction(value_simple{}, a);
        vector_template_deduction(static_cast<int*>(0), a);
        vector_template_deduction(static_cast<size_t>(0), a);
    }
};

inline void vector_all() {
    memory_types_function(vector_test{});
}

}}}
#endif
