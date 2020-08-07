// Copyright 2018 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_TESTS_ASCII_HPP
#define WATER_TESTS_ASCII_HPP
#include <water/water.hpp>
#include <water/test.hpp>
#include <water/ascii_lower_upper.hpp>
#include <water/ascii_lowercase_iterator.hpp>
namespace water { namespace tests {

char constexpr
    ascii_lowercase_string[] = u8"§1234567890+´°!\"#$%&\\()=?`qwertyuiop^asdfghjkl'<zxcvbnm,.-^*>_",
    ascii_uppercase_string[] = u8"§1234567890+´°!\"#$%&\\()=?`QWERTYUIOP^ASDFGHJKL'<ZXCVBNM,.-^*>_";

class ascii_test_increment_iterator {
    int my;
public:
    explicit ascii_test_increment_iterator(int a = 0) :
        my{a}
    {}
    int operator*() const {
        return my;
    }
    ascii_test_increment_iterator& operator++() {
        ++my;
        return *this;
    }
};

template<typename iterator1_, typename iterator2_, typename function_>
void ascii_test_equal(iterator1_ i1, iterator2_ i2, size_t size, function_&& equal) {
    while(size) {
        auto c1 = *i1;
        auto c2 = *i2;
        ___water_test(equal(c1, c2));
        ++i1;
        ++i2;
        --size;
    }
}

inline void ascii_all() {
    auto
        *lower = ascii_lowercase_string,
        *upper = ascii_uppercase_string;
    size_t size = sizeof(ascii_lowercase_string);
    ascii_test_equal(
        lower,
        ascii_lowercase_iterator_from(upper),
        size,
        [](char a, char b) { return a == b; }
    );
    ascii_test_equal(
        lower,
        upper,
        size,
        [](char a, char b) { return ascii_to_upper(a) == ascii_to_upper(b); }
    );
    ascii_test_equal(
        lower,
        upper,
        size,
        [](char a, char b) { return ascii_to_lower(a) == ascii_to_lower(b); }
    );
    ascii_test_equal(
        lower,
        upper,
        size,
        [](char a, char b) { return a == (ascii_is_lower(a) ? ascii_to_lower(b) : b); }
    );
    ascii_test_equal(
        lower,
        upper,
        size,
        [](char a, char b) { return (ascii_is_upper(b) ? ascii_to_upper(a) : a) == b; }
    );
    
    ascii_test_equal(
        ascii_test_increment_iterator{-123},
        ascii_lowercase_iterator_from(ascii_test_increment_iterator{-123}),
        512,
        [](int a, int b) { return ascii_to_lower(a) == b; }
    );
    ascii_test_equal(
        ascii_test_increment_iterator{-123},
        ascii_lowercase_iterator_from(ascii_test_increment_iterator{-123}),
        512,
        [](int a, int b) { return ascii_to_upper(a) == ascii_to_upper(b); }
    );
}

}}
#endif
