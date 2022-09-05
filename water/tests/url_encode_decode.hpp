// Copyright 2018-2022 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_TESTS_URL_ENCODE_DECODE_HPP
#define WATER_TESTS_URL_ENCODE_DECODE_HPP
#include <water/test.hpp>
#include <water/url_encode_decode.hpp>
#include <water/numeric_limits.hpp>
#include <water/reverse_iterator.hpp>
#include <water/char8.hpp>
namespace water { namespace tests {

template<typename iterator1_, typename iterator2_>
bool url_encode_decode_equal(iterator1_ b1, iterator1_ e1, iterator2_ b2, iterator2_ e2) {
    size_t at = 0;
    while(b1 != e1 && b2 != e2) {
        auto c1 = *b1;
        auto c2 = *b2;
        bool equal = static_cast<char32_t>(c1) == static_cast<char32_t>(c2);
        ___water_test(equal);
        if(!equal)
            return false;
        ++at;
        ++b1;
        ++b2;
    }
    return b1 == e1 && b2 == e2;
}

template<typename iterator_>
void url_encode_decode(iterator_ begin, iterator_ end) {
    auto e = url_encode_range_from(begin, end);
    auto d = url_decode_range_from(e.begin(), e.end());
    ___water_test(url_encode_decode_equal(begin, end, d.begin(), d.end()));
}

template<size_t size_>
void url_encode_decode(char8_or_char const (&cstring)[size_]) {
    url_encode_decode(cstring + 0, cstring + size_ - 1);
}

template<size_t size1_, size_t size2_>
void url_encode_test(char8_or_char const (&plain)[size1_], char8_or_char const (&encoded)[size2_]) {
    auto e = url_encode_range_from(plain + 0, plain + size1_ - 1);
    ___water_test(url_encode_decode_equal(e.begin(), e.end(), encoded + 0, encoded + size2_ - 1));
}

inline void url_encode_decode_all() {
    url_encode_test(
        u8"1234567890+!\"#$%&/()=?`^~*'-:;,._<>qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM",
        u8"1234567890%2B%21%22%23%24%25%26%2F%28%29%3D%3F%60%5E%7E%2A%27-%3A%3B%2C._%3C%3EqwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM"
    );
    // visual c++ 15.7.5 thinks the 4 emojis below are 34 bytes instead of 17? why?
    // visual c++ 17.4 preview seems to not handle \xFF inside u8 string literlas :(
    unsigned char const emoji_bytes[] = {0xF0, 0x9F, 0x91, 0x81, 0xF0, 0x9F, 0xA4, 0xAA, 0xF0, 0x9F, 0xA6, 0x81, 0xF0, 0x9F, 0x90, 0x8D, 0};
    char8_or_char emoji[sizeof(emoji_bytes)] {}; // cannot initalize char with 0xF0 without cast if its signed, copy is more readable?
    auto s = sizeof(emoji_bytes);
    while(s--) emoji[s] = static_cast<char8_or_char>(emoji_bytes[s]);
    url_encode_test(
        //u8"👁🤪🦁🐍",
        //u8"\xF0\x9F\x91\x81\xF0\x9F\xA4\xAA\xF0\x9F\xA6\x81\xF0\x9F\x90\x8D",
        emoji,
        u8"%F0%9F%91%81%F0%9F%A4%AA%F0%9F%A6%81%F0%9F%90%8D"
    );

    unsigned char all_bytes[256];
    for(auto& a : all_bytes) a = static_cast<unsigned char>(&a - all_bytes);

    url_encode_decode(all_bytes + 0, all_bytes + 3);
    url_encode_decode(all_bytes + 0, all_bytes + sizeof(all_bytes));
    url_encode_decode(reverse_iterator_from(all_bytes + sizeof(all_bytes)) - 1, reverse_iterator_from(all_bytes) - 1);
    url_encode_decode(u8"hello world");
    url_encode_decode(u8"/hello/world/");
    url_encode_decode(u8"1234567890+-.,*qwertyuiopasdfghj klzxcvbnm;:_!\"\\#$%&/(){}=QWERTYUIOPASDFGHJKLZXCVBNM? ");
    url_encode_decode(u8"a");
    url_encode_decode(u8"aa");
    url_encode_decode(u8"aaa");
    url_encode_decode(u8"/");
    url_encode_decode(u8"a/");
    url_encode_decode(u8"aa/");
    url_encode_decode(u8"aaa/");
}

}}
#endif
