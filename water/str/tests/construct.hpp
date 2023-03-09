// Copyright 2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_STR_TESTS_CONSTRUCT_HPP
#define WATER_STR_TESTS_CONSTRUCT_HPP
#include <water/str/tests/bits.hpp>
namespace water { namespace str { namespace tests {

/*

Test construction that was broken in visual c++ 2015. This workaround has been removed from the
code because it is 2023. The problem was construction did not happen, the data was uninitialized.

The minimal workaround is to *not* have the default constructor = default

*/

class construct_write
{
    char my[4] {'1', '2', '3', '4'};

public:

    construct_write() = default; // with this it breaks

    //constexpr construct_write() {} // with this it works
    
    template<typename ...arguments_, typename not_copy_constructor<construct_write, arguments_...>::result = 0>
    construct_write(arguments_&&... a) {
        char const x[] = {static_cast<char>(a)...};
        for(unsigned i = 0; i != sizeof(x) && i != sizeof(my); ++i)
            my[i] = x[i];
    }

    construct_write(construct_write&& a) {
        copy(my, a.my);
        copy(a.my, "move");
    }

    construct_write(construct_write const& a) {
        copy(my, a.my);
    }

    construct_write& operator=(construct_write&& a) {
        copy(my, a.my);
        copy(a.my, "move");
        return *this;
    }

    construct_write& operator=(construct_write const& a) {
        copy(my, a.my);
        return *this;
    }

    ~construct_write() {
        copy(my, "dead");
    }

    template<typename iterator_>
    void operator()(iterator_, iterator_) {
    }

    char const* data() const {
        return my;
    }

private:
    
    void copy(char* to, char const* from) {
        auto end = to + 4;
        do *to++ = *from++; while(to != end);
    }
};

template<size_t size_>
bool equal(out<construct_write> const& a, char const (&b)[size_]) {
    return equal(a.data(), 4, b, size_ - 1);
}


inline void construct_all() {
    settings s;
    out<construct_write>
        a,
        b{a},
        c{s},
        d{'a'},
        e{s, 'a'},
        f{'a', 'b'},
        g{s, 'a', 'b'},
        h{out<construct_write>{'a'}},
        i{d};
    ___water_test(equal(a, "1234"));
    ___water_test(equal(b, "1234"));
    ___water_test(equal(c, "1234"));
    ___water_test(equal(d, "a234"));
    ___water_test(equal(e, "a234"));
    ___water_test(equal(f, "ab34"));
    ___water_test(equal(g, "ab34"));
    ___water_test(equal(h, "a234"));
    ___water_test(equal(i, "a234"));
    a = g;
    b = static_cast<out<construct_write>&&>(d);
    ___water_test(equal(a, "ab34"));
    ___water_test(equal(b, "a234"));
}

}}}
#endif
