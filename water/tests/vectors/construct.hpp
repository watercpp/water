// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_TESTS_VECTORS_CONSTRUCT_HPP
#define WATER_TESTS_VECTORS_CONSTRUCT_HPP
#include <water/tests/vectors/bits.hpp>
namespace water { namespace tests { namespace vectors {

// construct assign swap

template<typename vector_>
struct construct_one {
    using value_type = typename vector_::value_type;
    construct_one(value_type const& value = {}) {
        typename vector_::allocator_type a;
        typename vector_::sizer_type s;
        char raw[sizeof(value_type) * 5];
        value_type *i = static_cast<value_type*>(static_cast<void*>(raw));
        unsigned is = 0;
        do new(here(i + is)) value_type(value); while(++is != 5);
        {
            vector_
                v0,
                v1(v0),
                v2(static_cast<vector_&&>(v1)),
                v3(a),
                v4(s),
                v5(a, s),
                v6(s, a);
            v0 = v1;
            v1 = static_cast<vector_&&>(v0);
            swap(v4, v5);
        }
        {
            vector_
                v1(1, i[1]),
                v2(2, i[0]),
                v3(3, i[0], a),
                v4(i + 0, is),
                v5(v4.begin(), v4.end()),
                v6(downgrade_iterators::input_from(i + 0), downgrade_iterators::input_from(i + is)),
                v7(downgrade_iterators::input_from(i + 0), is),
                v8(downgrade_iterators::input_proxied_from(i + 0), downgrade_iterators::input_proxied_from(i + is)),
                v9(downgrade_iterators::input_proxied_from(i + 0), is),
                v10(downgrade_iterators::forward_proxied_const_from(i + 0), downgrade_iterators::forward_proxied_const_from(i + is)),
                v11(downgrade_iterators::forward_proxied_const_from(i + 0), is),
                v12(downgrade_iterators::forward_const_from(i + 0), downgrade_iterators::forward_const_from(i + is)),
                v13(downgrade_iterators::forward_const_from(i + 0), is),
                v14(downgrade_iterators::bidirectional_proxied_const_from(i + 0), downgrade_iterators::bidirectional_proxied_const_from(i + is)),
                v15(downgrade_iterators::bidirectional_proxied_const_from(i + 0), is),
                v16(downgrade_iterators::bidirectional_const_from(i + 0), downgrade_iterators::bidirectional_const_from(i + is)),
                v17(downgrade_iterators::bidirectional_const_from(i + 0), is),
                v18(downgrade_iterators::random_access_proxied_const_from(i + 0), downgrade_iterators::random_access_proxied_const_from(i + is)),
                v19(downgrade_iterators::random_access_proxied_const_from(i + 0), is),
                v20(downgrade_iterators::random_access_const_from(i + 0), downgrade_iterators::random_access_const_from(i + is)),
                v21(downgrade_iterators::random_access_const_from(i + 0), is),
                // empty:
                v22(downgrade_iterators::input_proxied_from(i + 0), downgrade_iterators::input_proxied_from(i + 0)),
                v23(downgrade_iterators::input_proxied_from(i + 0), 0),
                v24(static_cast<value_type*>(0), static_cast<value_type*>(0)),
                v25(static_cast<value_type*>(0), 0);
            ___water_test(v1.size() == 1);
            ___water_test(v2.size() == 2);
            ___water_test(v3.size() == 3);
            ___water_test(v4.size() == is);
            ___water_test(v5.size() == is);
            ___water_test(v6.size() == is);
            ___water_test(v7.size() == is);
            ___water_test(v8.size() == is);
            ___water_test(v9.size() == is);
            ___water_test(v10.size() == is);
            ___water_test(v11.size() == is);
            ___water_test(v12.size() == is);
            ___water_test(v13.size() == is);
            ___water_test(v14.size() == is);
            ___water_test(v15.size() == is);
            ___water_test(v16.size() == is);
            ___water_test(v17.size() == is);
            ___water_test(v18.size() == is);
            ___water_test(v19.size() == is);
            ___water_test(v20.size() == is);
            ___water_test(v21.size() == is);
            ___water_test(v22.size() == 0);
            ___water_test(v23.size() == 0);
            ___water_test(v24.size() == 0);
            ___water_test(v25.size() == 0);

            v1 = v2;
            v2 = static_cast<vector_&&>(v1);
            swap(v6, v7);
        }
        do i[--is].~value_type(); while(is);
    }
};

struct construct {
    construct() {
        construct_one<vector<int>>();
        value_complex_count c;
        construct_one<vector<value_complex>>(value_complex(c));
        ___water_test(c.count == 0);
        construct_one<vector<value_simple>>();
    }
};

}}}
#endif
