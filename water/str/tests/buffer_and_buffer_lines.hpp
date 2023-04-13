// Copyright 2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_STR_TESTS_BUFFER_AND_BUFFER_LINES_HPP
#define WATER_STR_TESTS_BUFFER_AND_BUFFER_LINES_HPP
#include <water/str/tests/bits.hpp>
namespace water { namespace str { namespace tests {

/*

test buffer and buffer_lines.
char type should be auto-detected from the function.
buffer_lines should add linebreaks.
they should not flush incomplete utf sequences.

*/

template<typename char_>
struct buffer_and_buffer_lines_buffer // good naming!
{
    static unsigned constexpr capacity = 256;
    char_ buffer[capacity];
    unsigned size = 0;

    void operator()(char_ const *b, char_ const *e) {
        while(b != e && size != capacity)
            buffer[size++] = *b++;
        ___water_test(b == e);
    }

    void operator()(char_ const *c) {
        if(c)
            while(*c && size != capacity)
                buffer[size++] = *c++;
        ___water_test(c && !*c);
    }
};


template<typename char_, unsigned size_>
bool equal(buffer_and_buffer_lines_buffer<char_> const& a, char const (&b)[size_]) {
    return equal(a.buffer, a.size, b, size_ - 1);
}


template<typename char_, typename char2_ = char_>
struct buffer_and_buffer_lines_begin_end {
    buffer_and_buffer_lines_buffer<char_> *to;
    void operator()(char_ const* b, char_ const* e) { (*to)(b, e); }
    void operator()(char2_ const* a) {
        ___water_test(!a); // this will always fail, it should prefer to call the begin,end variant
    }
};


template<typename char_>
struct buffer_and_buffer_lines_zero_terminated {
    buffer_and_buffer_lines_buffer<char_> *to;
    void operator()(char_ const* a) { (*to)(a); }
};


struct buffer_and_buffer_lines_template {
    template<typename iterator_> void operator()(iterator_, iterator_) {}
};


inline void buffer_and_buffer_lines_all() {
    static_assert(water::equal<buffer<void (*)(char16_t const*, char16_t const*)>::char_type, char16_t>, "");
    static_assert(water::equal<buffer<void (char32_t const*)>::char_type, char32_t>, "");
    static_assert(water::equal<buffer<buffer_and_buffer_lines_begin_end<char16_t>>::char_type, char16_t>, "");
    static_assert(water::equal<buffer<buffer_and_buffer_lines_begin_end<char32_t, char>>::char_type, char32_t>, "");
    static_assert(water::equal<buffer<buffer_and_buffer_lines_zero_terminated<char16_t>>::char_type, char16_t>, "");
    static_assert(water::equal<buffer<buffer_and_buffer_lines_template>::char_type, char>, "");
    static_assert(water::equal<buffer<buffer_and_buffer_lines_template, char16_t>::char_type, char16_t>, "");

    static_assert(water::equal<buffer_lines<void (*)(char32_t const*, char32_t const*)>::char_type, char32_t>, "");
    static_assert(water::equal<buffer_lines<void (wchar_t const*)>::char_type, wchar_t>, "");
    static_assert(water::equal<buffer_lines<buffer_and_buffer_lines_begin_end<char32_t>>::char_type, char32_t>, "");
    static_assert(water::equal<buffer_lines<buffer_and_buffer_lines_begin_end<char16_t, char>>::char_type, char16_t>, "");
    static_assert(water::equal<buffer_lines<buffer_and_buffer_lines_zero_terminated<wchar_t>>::char_type, wchar_t>, "");
    static_assert(water::equal<buffer_lines<buffer_and_buffer_lines_template>::char_type, char>, "");
    static_assert(water::equal<buffer_lines<buffer_and_buffer_lines_template, char32_t>::char_type, char32_t>, "");

    buffer_and_buffer_lines_buffer<char> b1;
    buffer_and_buffer_lines_buffer<char16_t> b2;

    b1 = {};
    using lines1 = out<buffer_lines<buffer_and_buffer_lines_begin_end<char>, void, 10>>; // 10 means a line can be max 8 chars
    lines1{&b1} << "hello\nworld";
    lines1{&b1} << "01234567890123456789";
    lines1{&b1} << "01234567890123456789\n";
    lines1{&b1} << "x\n\n";
    ___water_test(equal(b1, "hello\nworld\n01234567\n01234567\nx\n\n"));

    b1 = {};
    lines1{&b1} << "hello\nworld\n01234567890123456789\n01234567890123456789\nx\n\n";
    ___water_test(equal(b1, "hello\nworld\n01234567\n01234567\nx\n\n"));

    b2 = {};
    using lines2 = out<buffer_lines<buffer_and_buffer_lines_begin_end<char16_t>, void, 11>>;
    lines2{&b2} << "hello" << '\n' << "world" << '\n' << "01234567890123456789\n01234567890123456789\nx\n\n";
    ___water_test(equal(b2, "hello\nworld\n012345678\n012345678\nx\n\n"));

    b2 = {};
    using lines3 = out<buffer_lines<buffer_and_buffer_lines_zero_terminated<char16_t>, void, 11>>;
    lines3{&b2} << "hello" << '\n' << "world" << '\n' << "01234567890123456789\n01234567890123456789\nx\n\n";
    ___water_test(equal(b2, "hello\nworld\n012345678\n012345678\nx\n\n"));

    b2 = {};
    lines3{&b2} << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
    ___water_test(equal(b2, "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"));

    b1 = {};
    using plain1 = out<buffer<buffer_and_buffer_lines_begin_end<char>, void, 9>>; // 9 means 8 chars before flush
    plain1{&b1} << "hello\nworld";
    plain1{&b1} << "01234567890123456789";
    plain1{&b1} << "01234567890123456789\n";
    plain1{&b1} << "x\n\n";
    ___water_test(equal(b1, "hello\nworld0123456789012345678901234567890123456789\nx\n\n"));

    b1 = {};
    plain1{&b1} << "hello\nworld\n01234567890123456789\n01234567890123456789\nx\n\n";
    ___water_test(equal(b1, "hello\nworld\n01234567890123456789\n01234567890123456789\nx\n\n"));

    b2 = {};
    using plain2 = out<buffer<buffer_and_buffer_lines_zero_terminated<char16_t>, void, 9>>;
    plain2{&b2} << "hello" << '\n' << "world" << '\n' << "01234567890123456789\n01234567890123456789\nx\n\n";
    ___water_test(equal(b2, "hello\nworld\n01234567890123456789\n01234567890123456789\nx\n\n"));

    char32_t constexpr c32 = 0x1f640;
    char screaming_cats[34] {};
    auto to_screaming_cats = to_begin_end(screaming_cats);
    for(auto i = 0; i != 8; ++i) {
        if(i == 4)
            to_screaming_cats << '\n';
        to_screaming_cats << c32;
    }
    ___water_test(to_screaming_cats.size() == 33);

    b1 = {};
    out<buffer<buffer_and_buffer_lines_begin_end<char>, void, 10>>{&b1} << screaming_cats;
    ___water_test(equal(b1, screaming_cats));
    
    b1 = {};
    out<buffer<buffer_and_buffer_lines_begin_end<char>, void, 11>>{&b1} << screaming_cats;
    ___water_test(equal(b1, screaming_cats));

    b1 = {};
    out<buffer<buffer_and_buffer_lines_begin_end<char>, void, 12>>{&b1} << screaming_cats;
    ___water_test(equal(b1, screaming_cats));

    char equal8[4 + 4 + 1 + 4 + 4 + 1 + 1] {};
    to_begin_end(equal8) << c32 << c32 << '\n' << c32 << c32 << '\n';

    b1 = {};
    out<buffer_lines<buffer_and_buffer_lines_begin_end<char>, void, 10>>{&b1} << screaming_cats; // 8 chars
    ___water_test(equal(b1, equal8));

    b1 = {};
    out<buffer_lines<buffer_and_buffer_lines_begin_end<char>, void, 11>>{&b1} << screaming_cats; // 9 chars
    ___water_test(equal(b1, equal8));

    b1 = {};
    out<buffer_lines<buffer_and_buffer_lines_begin_end<char>, void, 12>>{&b1} << screaming_cats; // 10 chars
    ___water_test(equal(b1, equal8));

    b1 = {};
    out<buffer_lines<buffer_and_buffer_lines_begin_end<char>, void, 13>>{&b1} << screaming_cats; // 11 chars
    ___water_test(equal(b1, equal8));
}

}}}
#endif
