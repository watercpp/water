// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_TESTS_VECTORS_ACCESS_HPP
#define WATER_TESTS_VECTORS_ACCESS_HPP
#include <water/tests/vectors/bits.hpp>
namespace water { namespace tests { namespace vectors {

// content access things for const and non const vector

struct access_one {
    template<typename vector_>
    access_one(vector_& a = {}) {
        {
            vector_& v = a;
            typename vector_::value_type *p;
            typename vector_::iterator ib, ie;
            typename vector_::reverse_iterator rb, re;
            p = &v[2 % v.size()];
            p = &v.at(3 % v.size());
            p = &v.front();
            p = &v.back();
            for(auto &r : v)
                p = &r;
            ib = v.begin();
            ie = v.end();
            while(ib != ie) {
                p = &*ib;
                ++ib;
            }
            rb = v.rbegin();
            re = v.rend();
            while(rb != re) {
                p = &*rb;
                ++rb;
            }
            ___water_test(!v.empty());
            p = v.data();
            ___water_test(p == &v.front() && p == &v[0] && p == &v.begin()[0]);
        }
        {
            vector_ const& v = a;
            typename vector_::value_type const *p;
            typename vector_::const_iterator ib, ie;
            typename vector_::const_reverse_iterator rb, re;
            p = &v[2 % v.size()];
            p = &v.at(3 % v.size());
            p = &v.front();
            p = &v.back();
            for(auto &r : v)
                p = &r;
            ib = v.begin();
            ie = v.end();
            while(ib != ie) {
                p = &*ib;
                ++ib;
            }
            rb = v.rbegin();
            re = v.rend();
            while(rb != re) {
                p = &*rb;
                ++rb;
            }
            ___water_test(!v.empty());
            p = v.data();
            ___water_test(p == &v.front() && p == &v[0] && p == &v.begin()[0]);
        }
    }
};

struct access {
    access() {
        {
            vector<int> v(5);
            access_one{v};
        }
        value_complex_count c;
        {
            vector<value_complex> v(5, value_complex(c));
            access_one{v};
        }
        ___water_test(c.count == 0);
        {
            vector<value_simple> v(5);
            access_one{v};
        }
    }
};

}}}
#endif
