// Copyright 2024 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_STR_TESTS_LIST_HPP
#define WATER_STR_TESTS_LIST_HPP
#include <water/str/tests/bits.hpp>
namespace water { namespace str { namespace tests {

/*

Test str::list

*/

template<typename to_, size_t size_>
bool list_equal(to_&& to, char const (&b)[size_]) {
    return equal(to.begin(), to.end(), b, b + size_ - 1);
}

inline void list_all() {
    char buffer[1024];
    auto const to0 = to_begin_end(buffer);

    int int_array[] {1, 2, 3, 4, 5};
    auto int_range = begin_end_from(int_array);
    char const char_array[] {1, 2, 3, 4, 5}; // chars will be written as integers
    water::begin_end<char const*> string_array[] {
        {"one", 3},
        {"two", 3},
        {"three", 5}
    };
    
    auto to = to0;
    to << list(int_array);
    ___water_test(list_equal(to, "1, 2, 3, 4, 5"));

    (to = to0) << list(int_range);
    ___water_test(list_equal(to, "1, 2, 3, 4, 5"));

    (to = to0) << list(begin_end_from(int_array)); // rvalue
    ___water_test(list_equal(to, "1, 2, 3, 4, 5"));

    (to = to0) << list(char_array);
    ___water_test(list_equal(to, "1, 2, 3, 4, 5"));

    (to = to0) << list(int_range.begin(), int_range.end());
    ___water_test(list_equal(to, "1, 2, 3, 4, 5"));

    (to = to0) << list(int_range.begin(), int_range.size());
    ___water_test(list_equal(to, "1, 2, 3, 4, 5"));

    (to = to0) << list(int_array, int_array + array_size(int_array));
    ___water_test(list_equal(to, "1, 2, 3, 4, 5"));

    (to = to0) << list(int_array, 2);
    ___water_test(list_equal(to, "1, 2"));

    (to = to0) << list(int_array, 1);
    ___water_test(list_equal(to, "1"));

    (to = to0) << list(int_array, 0);
    ___water_test(list_equal(to, ""));

    (to = to0) << list(string_array);
    ___water_test(list_equal(to, "one, two, three"));

    // settings
    
    (to = to0) << list(int_array).settings(settings{}.base(16)) << " after " << 6;
    ___water_test(list_equal(to, "0x1, 0x2, 0x3, 0x4, 0x5 after 6"));

    // settings + separator

    char16_t constexpr s[] = u"] [";

    (to = to0) << list(int_array).settings(settings{}.base(16)).separator(s) << " after " << 6;
    ___water_test(list_equal(to, "0x1] [0x2] [0x3] [0x4] [0x5 after 6"));

    (to = to0) << list(int_array).separator(s).settings(settings{}.base(16)) << " after " << 6; // separator before settings
    ___water_test(list_equal(to, "0x1] [0x2] [0x3] [0x4] [0x5 after 6"));

    // separator

    (to = to0) << list(int_array).separator(static_cast<char16_t const*>(s));
    ___water_test(list_equal(to, "1] [2] [3] [4] [5"));

    (to = to0) << list(int_array).separator(begin_end_from(s, 3)); // rvalue
    ___water_test(list_equal(to, "1] [2] [3] [4] [5"));

    (to = to0) << list(int_array).separator('s');
    ___water_test(list_equal(to, "1s2s3s4s5"));

    (to = to0) << list(int_array).separator("");
    ___water_test(list_equal(to, "12345"));

    (to = to0) << list(int_array).separator(99);
    ___water_test(list_equal(to, "1992993994995"));

}

}}}
#endif
