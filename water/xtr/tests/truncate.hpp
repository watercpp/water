// Copyright 2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_XTR_TESTS_TRUNCATE_HPP
#define WATER_XTR_TESTS_TRUNCATE_HPP
#include <water/xtr/tests/bits.hpp>
namespace water { namespace xtr { namespace tests {


// test that the string is trucated right when it does not fit


template<typename p_, typename w_>
inline void truncate(expression<p_, w_>&& x) {
    // this does not work if the expression contains utf-8 sequences
    auto all = string(x);
    ___water_test(all.size() < all.capacity());
    char array[expression<p_, w_>::size + 1];
    static_assert(expression<p_, w_>::size == all.capacity(), "");
    unsigned s = all.size();
    while(s) {
        for(auto& a : array)
            a = 0;
        auto end = write(x, array, array + s);
        ___water_test(end == array + s);
        ___water_test(equal(array, array + s, all.begin(), all.begin() + s));
        --s;
    }
}


inline void truncate_utf() {
    
    char32_t constexpr c32 = 0x1f640; // this is 4 utf-8 characters
    
    char c[123];
    
    // to_begin_end knows if its truncated, but it cuts in the middle of utf-8 sequences
    
    auto x = to_begin_end(c, 4);
    x << c32;
    ___water_test(x.end() == c + 4 && !x.truncated());
    
    x = to_begin_end(c, 3);
    x << c32;
    ___water_test(x.end() == c + 3 && x.truncated());
    
    x = to_begin_end(c, 2);
    x << c32;
    ___water_test(x.end() == c + 2 && x.truncated());
    
    x = to_begin_end(c, 1);
    x << c32;
    ___water_test(x.end() == c + 1 && x.truncated());
    
    x = to_begin_end(c, 0);
    x << c32;
    ___water_test(x.end() == c + 0 && x.truncated());
    
    // char_array will not cut in the middle of utf-8 sequences
    
    char_array<4> a4{no << c32};
    ___water_test(a4.size() == 4);
    
    char_array<3> a3{no << c32};
    ___water_test(a3.size() == 0);
    
    char_array<2> a2{no << c32};
    ___water_test(a2.size() == 0);
    
    char_array<1> a1{no << c32};
    ___water_test(a1.size() == 0);
    
    // test with something that always ends with a
    
    using ends_with_a = expression<
        void,
        configuration<
            number_format_default,
            settings_end<settings_default, 'a'>
        >
    >;
    
    char_array<5> a5{ends_with_a{} << c32};
    ___water_test(a5.size() == 5 && a5[4] == 'a');
    
    a4 = {ends_with_a{} << c32};
    ___water_test(a4.size() == 1 && a4[0] == 'a');
    
    a3 = {ends_with_a{} << c32};
    ___water_test(a3.size() == 1);
    
    a2 = {ends_with_a{} << c32};
    ___water_test(a2.size() == 1);
    
    a1 = {ends_with_a{} << c32};
    ___water_test(a1.size() == 1);
    
    // a string, make sure it cuts by multiple of 4
    
    char32_t constexpr s32[] = {c32, c32, c32};
    
    char_array<12> a12{no << s32};
    ___water_test(a12.size() == 12);
    
    char_array<11> a11{no << s32};
    ___water_test(a11.size() == 8);
    
    char_array<10> a10{no << s32};
    ___water_test(a10.size() == 8);
    
    char_array<9> a9{no << s32};
    ___water_test(a9.size() == 8);
    
    char_array<8> a8{no << s32};
    ___water_test(a8.size() == 8);
}


inline void truncate() {
    truncate(no << "hello world" << static_cast<char const*>("hello world") << cstring("hello world"));
    truncate(no << 'a' << 'b');
    truncate(no << true << false);
    int i = 0;
    truncate(no
        << 123456789
        << -123456789
        << &i
        << numeric_limits<int>::min()
        << numeric_limits<unsigned>::max()
        << digits<2>
        << 1234
        << -1234
        << digits<8>
        << base<16>
        << 1234
        << base<2>
        << 1234
    );
    truncate(no
        << 1.23456789
        << -123.456789
        << 9.876543e21
        << numeric_limits<float>::min()
        << numeric_limits<float>::infinity()
        << base<16>
        << 1.23456789
        << -123.456789
        << 9.876543e21
        << numeric_limits<float>::min()
        << numeric_limits<float>::infinity()
    );
    truncate_utf();
}

}}}
#endif
