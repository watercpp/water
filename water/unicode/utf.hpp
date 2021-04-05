// Copyright 2017-2021 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_UNICODE_UTF_HPP
#define WATER_UNICODE_UTF_HPP
#include <water/unicode/bits.hpp>
namespace water { namespace unicode {

/*

Low level tools for UTF conversion.


UTF-8

.....unicode......utf8.........................
..................1.......2.......3.......4....
.......0-7f.......00-7f........................
......80-7ff......c2-df...80-bf................
.....800-fff......e0......a0-bf...80-bf........ (2 0x80-0x9f will be 0-0x7ff)
....1000-cfff.....e1-ec...80-bf...80-bf........
....d000-d7ff.....ed......80-9f...80-bf........ (2 0xa0-0xbf will be 0xd800-0xdfff)
....e000-ffff.....ee-ef...80-bf...80-bf........
...10000-3ffff....f0......90-bf...80-bf...80-bf (2 0x80-0x8f will be 0-0xffff)
...40000-fffff....f1-f3...80-bf...80-bf...80-bf
..100000-10ffff...f4......80-8f...80-bf...80-bf (2 0x90-0xbf will be 0x110000-0x13ffff)
(never c0-c1, f5-ff)

bits
..unicode...................utf8...............................
..21.17.16.....9.8......1...1........2........3........4.......
..................aaaaaaa...0aaaaaaa...........................
.............aaa.aabbbbbb...110aaaaa.10bbbbbb..................
........aaaabbbb.bbcccccc...1110aaaa.10bbbbbb.10cccccc.........
..aaabb.bbbbcccc.ccdddddd...11110aaa.10bbbbbb.10cccccc.10dddddd


UTF-16

....unicode......utf16................
.................1...........2........
......0-d7ff.....0000-d7ff............
...e000-ffff.....e000-ffff............
..10000-10ffff...d800-dbff...dc00-dfff

bits
..unicode...................utf16..............................
..21.17.16.....9.8......1...1.................2................
........aaaaaaaa.aaaaaaaa...aaaaaaaa.aaaaaaaa..................
..aaaaa.bbbbbbcc.cccccccc...110110xx.xxbbbbbb.110111cc.cccccccc
(xxxx = aaaa - 1)


*/

namespace _ {

    template<typename char_>
    struct utf_from_char_do :
        types::integer<
            unsigned,
            static_cast<char_>(-1) < 0xff ? 0 :
            static_cast<char_>(-1) < 0xffff ? 8 :
            static_cast<char_>(-1) < 0x10ffff ? 16 :
            32
        > {};
    
    template<>
    struct utf_from_char_do<char> :
        types::integer<unsigned, 8>
    {};

    template<>
    struct utf_from_char_do<signed char> :
        types::integer<unsigned, 8>
    {};

    template<>
    struct utf_from_char_do<unsigned char> :
        types::integer<unsigned, 8>
    {};

    template<>
    struct utf_from_char_do<char16_t> :
        types::integer<unsigned, 16>
    {};

    template<>
    struct utf_from_char_do<char32_t> :
        types::integer<unsigned, 32>
    {};

    template<>
    struct utf_from_char_do<wchar_t> :
        types::integer<
            unsigned,
            static_cast<wchar_t>(-1) < 0 ? 32 : // signed whcar_t as utf-32, will this create problems?
            static_cast<wchar_t>(-1) < 0xffff ? 8 :
            static_cast<wchar_t>(-1) < 0x10ffff ? 16 :
            32
        > {};
    
}

// result 8 or 16 or 32 if char_ is a char type or unsigned integer type
// if wchar_t is signed, this results 32
template<typename char_>
struct utf_from_char :
    types::ifel_type<
        types::is_int<types::no_const<char_> >,
        _::utf_from_char_do<typename types::no_const<char_>::result>,
        types::integer<unsigned, 0>
    > {};


template<unsigned utf_>
struct char_from_utf :
    types::type_plain<void>
{};

template<>
struct char_from_utf<8> :
    types::type_plain<uchar_t>
{};

template<>
struct char_from_utf<16> :
    types::type_plain<char16_t>
{};

template<>
struct char_from_utf<32> :
    types::type_plain<char32_t>
{};


template<typename iterator_>
struct utf_from_iterator :
    utf_from_char<types::no_reference<decltype(*types::make<iterator_&>())> >
{};


constexpr uchar_t utf8_first_lookup[0x100] = {
    // 1  2  3  4  5  6  7  8  9  a  b  c  d  e  f
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 0
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 1
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 2
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 3
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 4
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 5
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 6
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 7
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 8
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 9
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // a
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // b
    0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, // c
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, // d
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, // e
    4, 4, 4, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // f
};


namespace _ {

    template<typename char_, bool = (static_cast<char_>(-1) > 0 && static_cast<char_>(-1) <= 0xff)>
    struct utf8_first_lookup_do {
        static uchar_t at(char_ a) {
            return utf8_first_lookup[a];
        }
    };
    
    template<typename char_>
    struct utf8_first_lookup_do<char_, false> {
        static uchar_t at(char_ a) {
            return 0 <= a && a <= 0xff ? utf8_first_lookup[a] : 0;
        }
    };
    
}


template<typename char_>
inline uchar_t utf8_first_of(char_ a) {
    return _::utf8_first_lookup_do<char_>::at(a);
}

inline uchar_t utf8_first_of(char a) {
    return utf8_first_of(static_cast<uchar_t>(a));
}

inline uchar_t utf8_first_of(signed char a) {
    return utf8_first_of(static_cast<uchar_t>(a));
}


template<typename char_>
inline char16_t utf8_unpack(char_ c0, char_ c1) {
    // will always return a vaild codepoint
    return
        (static_cast<char16_t>(c0 & 0x1f) << 6) |
        (c1 & 0x3f);
}

inline char16_t utf8_unpack(char c0, char c1) {
    return utf8_unpack(static_cast<uchar_t>(c0), static_cast<uchar_t>(c1));
}

inline char16_t utf8_unpack(signed char c0, signed char c1) {
    return utf8_unpack(static_cast<uchar_t>(c0), static_cast<uchar_t>(c1));
}

template<typename char_>
inline char16_t utf8_unpack(char_ c0, char_ c1, char_ c2) {
    // will not always output a valid codepoint, can output 0xd800-0xdfff
    return
        (static_cast<char16_t>(c0 & 0x0f) << 12) |
        (static_cast<char16_t>(c1 & 0x3f) << 6) |
        (c2 & 0x3f);
}

inline char16_t utf8_unpack(char c0, char c1, char c2) {
    return utf8_unpack(static_cast<uchar_t>(c0), static_cast<uchar_t>(c1), static_cast<uchar_t>(c2));
}

inline char16_t utf8_unpack(signed char c0, signed char c1, signed char c2) {
    return utf8_unpack(static_cast<uchar_t>(c0), static_cast<uchar_t>(c1), static_cast<uchar_t>(c2));
}

template<typename char_>
inline char32_t utf8_unpack(char_ c0, char_ c1, char_ c2, char_ c3) {
    // will not always output a vaild codepoint, can output 0xd800-0xdfff and 0x110000-0x13ffff
    return
        (static_cast<char32_t>(c0 & 0x7) << 18) |
        (static_cast<char32_t>(c1 & 0x3f) << 12) |
        (static_cast<char32_t>(c2 & 0x3f) << 6) |
        (c3 & 0x3f);
}

inline char32_t utf8_unpack(char c0, char c1, char c2, char c3) {
    return utf8_unpack(static_cast<uchar_t>(c0), static_cast<uchar_t>(c1), static_cast<uchar_t>(c2), static_cast<uchar_t>(c3));
}

inline char32_t utf8_unpack(signed char c0, signed char c1, signed char c2, signed char c3) {
    return utf8_unpack(static_cast<uchar_t>(c0), static_cast<uchar_t>(c1), static_cast<uchar_t>(c2), static_cast<uchar_t>(c3));
}


template<typename char_>
bool utf8_verify(char_, char_ c1) {
    // use utf8_first_of before this or die
    // using < and > comparations instead of masks because they work even when char-type is more than 8 bits or signed
    return 0x80 <= c1 && c1 <= 0xbf;
}

inline bool utf8_verify(char c0, char c1) {
    return utf8_verify(static_cast<uchar_t>(c0), static_cast<uchar_t>(c1));
}

inline bool utf8_verify(signed char c0, signed char c1) {
    return utf8_verify(static_cast<uchar_t>(c0), static_cast<uchar_t>(c1));
}

template<typename char_>
bool utf8_verify(char_ c0, char_ c1, char_ c2) {
    return (
        c0 == 0xe0 ? 0xa0 <= c1 && c1 <= 0xbf :
        c0 == 0xed ? 0x80 <= c1 && c1 <= 0x9f :
        0x80 <= c1 && c1 <= 0xbf
    ) &&
        0x80 <= c2 && c2 <= 0xbf;
}

inline bool utf8_verify(char c0, char c1, char c2) {
    return utf8_verify(static_cast<uchar_t>(c0), static_cast<uchar_t>(c1), static_cast<uchar_t>(c2));
}

inline bool utf8_verify(signed char c0, signed char c1, signed char c2) {
    return utf8_verify(static_cast<uchar_t>(c0), static_cast<uchar_t>(c1), static_cast<uchar_t>(c2));
}

template<typename char_>
bool utf8_verify(char_ c0, char_ c1, char_ c2, char_ c3) {
    return (
        c0 == 0xf0 ? 0x90 <= c1 && c1 <= 0xbf :
        c0 == 0xf4 ? 0x80 <= c1 && c1 <= 0x8f :
        0x80 <= c1 && c1 <= 0xbf
    ) &&
        0x80 <= c2 && c2 <= 0xbf &&
        0x80 <= c3 && c3 <= 0xbf;
}

inline bool utf8_verify(char c0, char c1, char c2, char c3) {
    return utf8_verify(static_cast<uchar_t>(c0), static_cast<uchar_t>(c1), static_cast<uchar_t>(c2), static_cast<uchar_t>(c3));
}

inline bool utf8_verify(signed char c0, signed char c1, signed char c2, signed char c3) {
    return utf8_verify(static_cast<uchar_t>(c0), static_cast<uchar_t>(c1), static_cast<uchar_t>(c2), static_cast<uchar_t>(c3));
}


template<typename char_>
inline bool utf16_is_1_of_1(char_ a) {
    return (a & 0xf800) != 0xd800;
}


template<typename char_>
inline bool utf16_is_1_of_2(char_ a) {
    return (a & 0xfc00) == 0xd800;
}


template<typename char_>
inline bool utf16_is_2_of_2(char_ a) {
    return 0xdc00 <= a && a <= 0xdfff;
}


template<typename char_>
inline char32_t utf16_unpack(char_ c0, char_ c1) {
    // this will return a valid utf-32 character no matter what c0 and c1 is
    return 0x10000 + ((static_cast<char32_t>(c0 & 0x3ff) << 10) | (c1 & 0x3ff));
}


template<typename to_>
inline void utf8_pack(to_& to0, to_& to1, char32_t from) {
    to0 = static_cast<uchar_t>(0xc0 | (from >> 6));
    to1 = static_cast<uchar_t>(0x80 | (from & 0x3f));
}

template<typename to_>
inline void utf8_pack(to_& to0, to_& to1, to_& to2, char32_t from) {
    to0 = static_cast<uchar_t>(0xe0 | (from >> 12));
    to1 = static_cast<uchar_t>(0x80 | ((from >> 6) & 0x3f));
    to2 = static_cast<uchar_t>(0x80 | (from & 0x3f));
}

template<typename to_>
inline void utf8_pack(to_& to0, to_& to1, to_& to2, to_& to3, char32_t from) {
    to0 = static_cast<uchar_t>(0xf0 | (from >> 18));
    to1 = static_cast<uchar_t>(0x80 | ((from >> 12) & 0x3f));
    to2 = static_cast<uchar_t>(0x80 | ((from >> 6) & 0x3f));
    to3 = static_cast<uchar_t>(0x80 | (from & 0x3f));
}


template<typename to_>
inline void utf16_pack(to_& to0, to_& to1, char32_t from) {
    from -= 0x10000;
    to0 = static_cast<char16_t>(0xd800 | ((from >> 10) & 0x3ff));
    to1 = static_cast<char16_t>(0xdc00 | (from & 0x3ff));
}


template<typename char_>
bool utf32_verify(char_ c) {
    return c <= 0x10ffff && (c >> 11) != 0x1b; // d800-dfff
}

// utf_decode_and_move
/////////////////////////////////////////////////////////////////////
// read 1 codepoint from begin,end or begin,size range
// these do not verify that they read a valid codepoint, but it is
// easy to check the decoded codepoint afterwards

template<typename to_, typename iterator_, typename int_>
typename types::ifel_type<types::is_int<int_>, unsigned>::result utf8_decode_and_move(
    to_& to,
    iterator_& from,
    int_& size
) {
    // returns 0 to 4, how many characters was read from from.
    if(!size)
        return 0;
    auto f = *from; // read once
    unsigned n = utf8_first_of(f);
    if(n > size)
        return static_cast<unsigned>(size = 0);
    switch(n) {
        case 2:
            to = utf8_unpack(f, *++from);
            break;
        case 3: {
            auto f1 = *++from, f2 = *++from;
            to = utf8_unpack(f, f1, f2);
            break;
        }
        case 4: {
            auto f1 = *++from, f2 = *++from, f3 = *++from;
            to = utf8_unpack(f, f1, f2, f3);
            break;
        }
        default:
            n = 1;
            to = cast(f);
    }
    size -= n;
    ++from;
    return n;
}

template<typename to_, typename iterator_>
typename if_random_access<iterator_, unsigned>::result utf8_decode_and_move(
    to_& to,
    iterator_& from,
    typename types::type<iterator_>::result const& end
) {
    // return 0 to 4, 0 if from reached end
    if(from == end)
        return 0;
    auto f = *from;
    unsigned n = utf8_first_of(f);
    if(static_cast<size_t>(end - from) < n)
        return 0;
    switch(n) {
        case 2:
            to = utf8_unpack(f, from[1]);
            break;
        case 3:
            to = utf8_unpack(f, from[1], from[2]);
            break;
        case 4:
            to = utf8_unpack(f, from[1], from[2], from[3]);
            break;
        default:
            n = 1;
            to = cast(f);
    }
    from += n;
    return n;
}

template<typename to_, typename iterator_>
typename if_not_random_access<iterator_, unsigned>::result utf8_decode_and_move(
    to_& to,
    iterator_& from,
    typename types::type<iterator_>::result const& end
) {
    // returns 0 to 4, how many characters was read from from.
    // pre: from != end
    // post: if return 0, from == end
    if(from == end)
        return 0;
    auto f0 = *from; // read once
    ++from;
    unsigned n = utf8_first_of(f0);
    if(n < 2) {
        to = cast(f0);
        return 1;
    }
    else if(from != end) {
        auto f1 = *from;
        ++from;
        if(n == 2) {
            to = utf8_unpack(f0, f1);
            return 2;
        }
        else if(from != end) {
            auto f2 = *from;
            ++from;
            if(n == 3) {
                to = utf8_unpack(f0, f1, f2);
                return 3;
            }
            else if(from != end) {
                to = utf8_unpack(f0, f1, f2, *from);
                ++from;
                return 4;
            }
        }
    }
    return 0;
}

template<typename to_, typename iterator_, typename int_>
typename types::ifel_type<types::is_int<int_>, unsigned>::result utf16_decode_and_move(
    to_& to,
    iterator_& from,
    int_& size
) {
    // return 0 1 2, how many from from was used
    if(!size)
        return 0;
    auto f = *from; // read once
    ++from;
    --size;
    if(utf16_is_1_of_1(f)) {
        to = f;
        return 1;
    }
    else if(!size)
        return 0;
    to = utf16_unpack(f, *from);
    ++from;
    --size;
    return 2;
}

template<typename to_, typename iterator_>
unsigned utf16_decode_and_move(
    to_& to,
    iterator_& from,
    typename types::type<iterator_>::result const& end
) {
    // return 0 1 2, how many from from was used
    // pre: from != end
    // post: if return 0, from == end
    if(from == end)
        return 0;
    auto f = *from; // read once
    ++from;
    if(utf16_is_1_of_1(f)) {
        to = f;
        return 1;
    }
    else if(from == end)
        return 0;
    to = utf16_unpack(f, *from);
    ++from;
    return 2;
}

template<typename to_, typename iterator_, typename int_>
typename types::ifel_type<types::is_int<int_>, unsigned>::result utf32_decode_and_move(
    to_& to,
    iterator_& from,
    int_& size
) {
    if(size) {
        to = *from;
        ++from;
        --size;
        return 1;
    }
    return 0;
}

template<typename to_, typename iterator_>
unsigned utf32_decode_and_move(
    to_& to,
    iterator_& from,
    typename types::type<iterator_>::result const& end
) {
    if(from != end) {
        to = *from;
        ++from;
        return 1;
    }
    return 0;
}

template<unsigned utf_, typename to_, typename iterator_, typename int_>
typename types::ifel<utf_ == 8 && types::is_int<int_>::result, unsigned>::result utf_decode_and_move(
    to_& to,
    iterator_& from,
    int_& size
) {
    return utf8_decode_and_move(to, from, size);
}

template<unsigned utf_, typename to_, typename iterator_, typename int_>
typename types::ifel<utf_ == 16 && types::is_int<int_>::result, unsigned>::result utf_decode_and_move(
    to_& to,
    iterator_& from,
    int_& size
) {
    return utf16_decode_and_move(to, from, size);
}

template<unsigned utf_, typename to_, typename iterator_, typename int_>
typename types::ifel<utf_ == 32 && types::is_int<int_>::result, unsigned>::result utf_decode_and_move(
    to_& to,
    iterator_& from,
    int_& size
) {
    return utf32_decode_and_move(to, from, size);
}

template<unsigned utf_, typename to_, typename iterator_>
typename types::ifel<utf_ == 8, unsigned>::result utf_decode_and_move(
    to_& to,
    iterator_& from,
    typename types::type<iterator_>::result const& end
) {
    return utf8_decode_and_move(to, from, end);
}

template<unsigned utf_, typename to_, typename iterator_>
typename types::ifel<utf_ == 16, unsigned>::result utf_decode_and_move(
    to_& to,
    iterator_& from,
    typename types::type<iterator_>::result const& end
) {
    return utf16_decode_and_move(to, from, end);
}

template<unsigned utf_, typename to_, typename iterator_>
typename types::ifel<utf_ == 32, unsigned>::result utf_decode_and_move(
    to_& to,
    iterator_& from,
    typename types::type<iterator_>::result const& end
) {
    return utf32_decode_and_move(to, from, end);
}

// utf_decode_verify_and_move
/////////////////////////////////////////////////////////////////////
// like utf_decode_and_move except it will verify from
// the only way to know if from was bad or if all of from was decoded
// is to know if size != 0 or begin != end before calling this
//

template<typename to_, typename iterator_, typename int_>
typename types::ifel_type<types::is_int<int_>, unsigned>::result utf8_decode_verify_and_move(
    to_& to,
    iterator_& from,
    int_& size
) {
    // returns 0 to 4
    if(!size)
        return 0;
    auto f = *from; // read once
    unsigned n = utf8_first_of(f);
    if(n > size)
        return static_cast<unsigned>(size = 0);
    switch(n) {
        case 0:
            return 0;
        case 2: {
            auto f1 = *++from;
            if(!utf8_verify(f, f1))
                return 0;
            to = utf8_unpack(f, f1);
            break;
        }
        case 3: {
            auto f1 = *++from, f2 = *++from;
            if(!utf8_verify(f, f1, f2))
                return 0;
            to = utf8_unpack(f, f1, f2);
            break;
        }
        case 4: {
            auto f1 = *++from, f2 = *++from, f3 = *++from;
            if(!utf8_verify(f, f1, f2, f3))
                return 0;
            to = utf8_unpack(f, f1, f2, f3);
            break;
        }
        default:
            to = cast(f);
    }
    size -= n;
    ++from;
    return n;
}

template<typename to_, typename iterator_>
typename if_random_access<iterator_, unsigned>::result utf8_decode_verify_and_move(
    to_& to,
    iterator_& from,
    typename types::type<iterator_>::result const& end
) {
    // return 0 to 4, 0 if from reached end
    if(from == end)
        return 0;
    auto f = *from;
    unsigned n = utf8_first_of(f);
    if(!n || static_cast<size_t>(end - from) < n)
        return 0;
    switch(n) {
        case 2: {
            auto f1 = from[1];
            if(!utf8_verify(f, f1))
                return 0;
            to = utf8_unpack(f, f1);
            break;
        }
        case 3: {
            auto f1 = from[1], f2 = from[2];
            if(!utf8_verify(f, f1, f2))
                return 0;
            to = utf8_unpack(f, f1, f2);
            break;
        }
        case 4: {
            auto f1 = from[1], f2 = from[2], f3 = from[3];
            if(!utf8_verify(f, f1, f2, f3))
                return 0;
            to = utf8_unpack(f, f1, f2, f3);
            break;
        }
        default:
            to = cast(f);
    }
    from += n;
    return n;
}

template<typename to_, typename iterator_>
typename if_not_random_access<iterator_, unsigned>::result utf8_decode_verify_and_move(
    to_& to,
    iterator_& from,
    typename types::type<iterator_>::result const& end
) {
    // returns 0 to 4, how many characters was read from from.
    // pre: from != end
    // post: if return 0, from == end
    if(from == end)
        return 0;
    auto f0 = *from; // read once
    ++from;
    unsigned n = utf8_first_of(f0);
    if(n == 1) {
        to = cast(f0);
        return 1;
    }
    else if(n && from != end) {
        auto f1 = *from;
        ++from;
        if(n == 2) {
            if(!utf8_verify(f0, f1))
                return 0;
            to = utf8_unpack(f0, f1);
            return 2;
        }
        else if(from != end) {
            auto f2 = *from;
            ++from;
            if(n == 3) {
                if(!utf8_verify(f0, f1, f2))
                    return 0;
                to = utf8_unpack(f0, f1, f2);
                return 3;
            }
            else if(from != end) {
                auto f3 = *from;
                ++from;
                if(!utf8_verify(f0, f1, f2, f3))
                    return 0;
                to = utf8_unpack(f0, f1, f2, f3);
                return 4;
            }
        }
    }
    return 0;
}

template<typename to_, typename iterator_, typename int_>
typename types::ifel_type<types::is_int<int_>, unsigned>::result utf16_decode_verify_and_move(
    to_& to,
    iterator_& from,
    int_& size
) {
    // return 0 1 2
    if(!size)
        return 0;
    auto f = *from; // read once
    ++from;
    --size;
    if(f > 0xffff)
        return 0;
    if((f & 0xf800) != 0xd800) { // 1 of 1
        to = f;
        return 1;
    }
    else if(!size || (f & 0xfc00) != 0xd800) // not 1 of 2
        return 0;
    auto f1 = *from;
    if(f1 < 0xdc00 || 0xdfff < f1) // not 2 of 2
        return 0;
    to = utf16_unpack(f, f1);
    ++from;
    --size;
    return 2;
}

template<typename to_, typename iterator_>
unsigned utf16_decode_verify_and_move(
    to_& to,
    iterator_& from,
    typename types::type<iterator_>::result const& end
) {
    // return 0 1 2
    if(from == end)
        return 0;
    auto f = *from; // read once
    ++from;
    if(f > 0xffff)
        return 0;
    if((f & 0xf800) != 0xd800) { // 1 of 1
        to = f;
        return 1;
    }
    else if(from == end || (f & 0xfc00) != 0xd800) // not 1 of 2
        return 0;
    auto f1 = *from;
    if(f1 < 0xdc00 || 0xdfff < f1) // not 2 of 2
        return 0;
    to = utf16_unpack(f, f1);
    ++from;
    return 2;
}

template<typename to_, typename iterator_, typename int_>
typename types::ifel_type<types::is_int<int_>, unsigned>::result utf32_decode_verify_and_move(
    to_& to,
    iterator_& from,
    int_& size
) {
    if(size && utf32_verify(to = *from)) {
        ++from;
        --size;
        return 1;
    }
    return 0;
}

template<typename to_, typename iterator_>
unsigned utf32_decode_verify_and_move(
    to_& to,
    iterator_& from,
    typename types::type<iterator_>::result const& end
) {
    if(from != end && utf32_verify(to = *from)) {
        ++from;
        return 1;
    }
    return 0;
}

template<unsigned utf_, typename to_, typename iterator_, typename int_>
typename types::ifel<utf_ == 8 && types::is_int<int_>::result, unsigned>::result utf_decode_verify_and_move(
    to_& to,
    iterator_& from,
    int_& size
) {
    return utf8_decode_verify_and_move(to, from, size);
}

template<unsigned utf_, typename to_, typename iterator_, typename int_>
typename types::ifel<utf_ == 16 && types::is_int<int_>::result, unsigned>::result utf_decode_verify_and_move(
    to_& to,
    iterator_& from,
    int_& size
) {
    return utf16_decode_verify_and_move(to, from, size);
}

template<unsigned utf_, typename to_, typename iterator_, typename int_>
typename types::ifel<utf_ == 32 && types::is_int<int_>::result, unsigned>::result utf_decode_verify_and_move(
    to_& to,
    iterator_& from,
    int_& size
) {
    return utf32_decode_verify_and_move(to, from, size);
}

template<unsigned utf_, typename to_, typename iterator_>
typename types::ifel<utf_ == 8, unsigned>::result utf_decode_verify_and_move(
    to_& to,
    iterator_& from,
    typename types::type<iterator_>::result const& end
) {
    return utf8_decode_verify_and_move(to, from, end);
}

template<unsigned utf_, typename to_, typename iterator_>
typename types::ifel<utf_ == 16, unsigned>::result utf_decode_verify_and_move(
    to_& to,
    iterator_& from,
    typename types::type<iterator_>::result const& end
) {
    return utf16_decode_verify_and_move(to, from, end);
}

template<unsigned utf_, typename to_, typename iterator_>
typename types::ifel<utf_ == 32, unsigned>::result utf_decode_verify_and_move(
    to_& to,
    iterator_& from,
    typename types::type<iterator_>::result const& end
) {
    return utf32_decode_verify_and_move(to, from, end);
}

// utf_decode_verify_and_move with verify_ true/false
/////////////////////////////////////////////////////////////////////

template<unsigned utf_, bool verify_, typename to_, typename iterator_, typename int_>
typename types::ifel<utf_ == 8 && verify_ && types::is_int<int_>::result, unsigned>::result utf_decode_verify_and_move(
    to_& to,
    iterator_& from,
    int_& size
) {
    return utf8_decode_verify_and_move(to, from, size);
}

template<unsigned utf_, bool verify_, typename to_, typename iterator_, typename int_>
typename types::ifel<utf_ == 16 && verify_ && types::is_int<int_>::result, unsigned>::result utf_decode_verify_and_move(
    to_& to,
    iterator_& from,
    int_& size
) {
    return utf16_decode_verify_and_move(to, from, size);
}

template<unsigned utf_, bool verify_, typename to_, typename iterator_, typename int_>
typename types::ifel<utf_ == 32 && verify_ && types::is_int<int_>::result, unsigned>::result utf_decode_verify_and_move(
    to_& to,
    iterator_& from,
    int_& size
) {
    return utf32_decode_verify_and_move(to, from, size);
}

template<unsigned utf_, bool verify_, typename to_, typename iterator_>
typename types::ifel<utf_ == 8 && verify_, unsigned>::result utf_decode_verify_and_move(
    to_& to,
    iterator_& from,
    typename types::type<iterator_>::result const& end
) {
    return utf8_decode_verify_and_move(to, from, end);
}

template<unsigned utf_, bool verify_, typename to_, typename iterator_>
typename types::ifel<utf_ == 16 && verify_, unsigned>::result utf_decode_verify_and_move(
    to_& to,
    iterator_& from,
    typename types::type<iterator_>::result const& end
) {
    return utf16_decode_verify_and_move(to, from, end);
}

template<unsigned utf_, bool verify_, typename to_, typename iterator_>
typename types::ifel<utf_ == 32 && verify_, unsigned>::result utf_decode_verify_and_move(
    to_& to,
    iterator_& from,
    typename types::type<iterator_>::result const& end
) {
    return utf32_decode_verify_and_move(to, from, end);
}

template<unsigned utf_, bool verify_, typename to_, typename iterator_, typename int_>
typename types::ifel<utf_ == 8 && !verify_ && types::is_int<int_>::result, unsigned>::result utf_decode_verify_and_move(
    to_& to,
    iterator_& from,
    int_& size
) {
    return utf8_decode_and_move(to, from, size);
}

template<unsigned utf_, bool verify_, typename to_, typename iterator_, typename int_>
typename types::ifel<utf_ == 16 && !verify_ && types::is_int<int_>::result, unsigned>::result utf_decode_verify_and_move(
    to_& to,
    iterator_& from,
    int_& size
) {
    return utf16_decode_and_move(to, from, size);
}

template<unsigned utf_, bool verify_, typename to_, typename iterator_, typename int_>
typename types::ifel<utf_ == 32 && !verify_ && types::is_int<int_>::result, unsigned>::result utf_decode_verify_and_move(
    to_& to,
    iterator_& from,
    int_& size
) {
    return utf32_decode_and_move(to, from, size);
}

template<unsigned utf_, bool verify_, typename to_, typename iterator_>
typename types::ifel<utf_ == 8 && !verify_, unsigned>::result utf_decode_verify_and_move(
    to_& to,
    iterator_& from,
    typename types::type<iterator_>::result const& end
) {
    return utf8_decode_and_move(to, from, end);
}

template<unsigned utf_, bool verify_, typename to_, typename iterator_>
typename types::ifel<utf_ == 16 && !verify_, unsigned>::result utf_decode_verify_and_move(
    to_& to,
    iterator_& from,
    typename types::type<iterator_>::result const& end
) {
    return utf16_decode_and_move(to, from, end);
}

template<unsigned utf_, bool verify_, typename to_, typename iterator_>
typename types::ifel<utf_ == 32 && !verify_, unsigned>::result utf_decode_verify_and_move(
    to_& to,
    iterator_& from,
    typename types::type<iterator_>::result const& end
) {
    return utf32_decode_and_move(to, from, end);
}

// utf_encode
/////////////////////////////////////////////////////////////////////
// write 1 codepoint to iterator. there must be enough free space

template<typename iterator_>
unsigned utf8_encode(iterator_ to, char32_t from) {
    // return 1 to 4, the number of characters written
    if(from <= 0x7f) {
        *to = static_cast<uchar_t>(from);
        return 1;
    }
    if(from <= 0x7ff) {
        *to = static_cast<uchar_t>(0xc0 | (from >> 6));
        *++to = static_cast<uchar_t>(0x80 | (from & 0x3f));
        return 2;
    }
    if(from <= 0xffffu) {
        *to = static_cast<uchar_t>(0xe0 | (from >> 12));
        *++to = static_cast<uchar_t>(0x80 | ((from >> 6) & 0x3f));
        *++to = static_cast<uchar_t>(0x80 | (from & 0x3f));
        return 3;
    }
    *to = static_cast<uchar_t>(0xf0 | (from >> 18));
    *++to = static_cast<uchar_t>(0x80 | ((from >> 12) & 0x3f));
    *++to = static_cast<uchar_t>(0x80 | ((from >> 6) & 0x3f));
    *++to = static_cast<uchar_t>(0x80 | (from & 0x3f));
    return 4;
}

template<typename iterator_>
unsigned utf16_encode(iterator_ to, char32_t from) {
    if(from <= 0xffff) {
        *to = static_cast<char16_t>(from);
        return 1;
    }
    from -= 0x10000;
    *to = static_cast<char16_t>(0xd800 | ((from >> 10) & 0x3ff));
    *++to = static_cast<char16_t>(0xdc00 | (from & 0x3ff));
    return 2;
}

template<typename iterator_>
unsigned utf32_encode(iterator_ to, char32_t from) {
    *to = from;
    return 1;
}

template<unsigned utf_, typename iterator_>
typename types::ifel<utf_ == 8, unsigned>::result utf_encode(iterator_ to, char32_t from) {
    return utf8_encode(to, from);
}

template<unsigned utf_, typename iterator_>
typename types::ifel<utf_ == 16, unsigned>::result utf_encode(iterator_ to, char32_t from) {
    return utf16_encode(to, from);
}

template<unsigned utf_, typename iterator_>
typename types::ifel<utf_ == 32, unsigned>::result utf_encode(iterator_ to, char32_t from) {
    return utf32_encode(to, from);
}

// utf_encode_and_move
/////////////////////////////////////////////////////////////////////
// write 1 codepoint to iterator, and move iterator. there must be enough free space

template<typename iterator_>
unsigned utf8_encode_and_move(iterator_& to, char32_t from) {
    // return 1 to 4, the number of characters written
    if(from <= 0x7f) {
        *to = static_cast<uchar_t>(from);
        ++to;
        return 1;
    }
    if(from <= 0x7ff) {
        *to = static_cast<uchar_t>(0xc0 | (from >> 6));
        *++to = static_cast<uchar_t>(0x80 | (from & 0x3f));
        ++to;
        return 2;
    }
    if(from <= 0xffffu) {
        *to = static_cast<uchar_t>(0xe0 | (from >> 12));
        *++to = static_cast<uchar_t>(0x80 | ((from >> 6) & 0x3f));
        *++to = static_cast<uchar_t>(0x80 | (from & 0x3f));
        ++to;
        return 3;
    }
    *to = static_cast<uchar_t>(0xf0 | (from >> 18));
    *++to = static_cast<uchar_t>(0x80 | ((from >> 12) & 0x3f));
    *++to = static_cast<uchar_t>(0x80 | ((from >> 6) & 0x3f));
    *++to = static_cast<uchar_t>(0x80 | (from & 0x3f));
    ++to;
    return 4;
}

template<typename iterator_>
unsigned utf16_encode_and_move(iterator_& to, char32_t from) {
    if(from <= 0xffff) {
        *to = static_cast<char16_t>(from);
        ++to;
        return 1;
    }
    from -= 0x10000;
    *to = static_cast<char16_t>(0xd800 | ((from >> 10) & 0x3ff));
    *++to = static_cast<char16_t>(0xdc00 | (from & 0x3ff));
    ++to;
    return 2;
}

template<typename iterator_>
unsigned utf32_encode_and_move(iterator_& to, char32_t from) {
    *to = from;
    ++to;
    return 1;
}

template<unsigned utf_, typename iterator_>
typename types::ifel<utf_ == 8, unsigned>::result utf_encode_and_move(iterator_& to, char32_t from) {
    return utf8_encode_and_move(to, from);
}

template<unsigned utf_, typename iterator_>
typename types::ifel<utf_ == 16, unsigned>::result utf_encode_and_move(iterator_& to, char32_t from) {
    return utf16_encode_and_move(to, from);
}

template<unsigned utf_, typename iterator_>
typename types::ifel<utf_ == 32, unsigned>::result utf_encode_and_move(iterator_& to, char32_t from) {
    return utf32_encode_and_move(to, from);
}

// utf_from_utf
/////////////////////////////////////////////////////////////////////
// convert from one utf to antother, or simply copy if they are the
// same. the destination must have enough space, or this will overrun

template<typename iterator_>
class utf_from_utf_return {
    iterator_ myend;
    size_t mysize;
public:
    utf_from_utf_return() :
        myend{},
        mysize{}
    {}
    utf_from_utf_return(iterator_ end, size_t size) :
        myend{end},
        mysize{size}
    {}
    iterator_ end() const {
        return myend;
    }
    size_t size() const {
        return mysize;
    }
};

constexpr inline unsigned utf_from_utf_maximum_length(unsigned to, unsigned from) {
    return to >= from ? 1 :
        to == 8 && from == 16 ? 3 :
        to == 8 && from == 32 ? 4 :
        2; // must be to 16 from 32
}

template<unsigned to_, unsigned from_, typename to_iterator_, typename from_iterator_>
utf_from_utf_return<to_iterator_> utf_from_utf(
    to_iterator_ to,
    from_iterator_ from,
    typename types::ifel<to_ != from_, size_t>::result from_size
) {
    static_assert(to_ == 8 || to_ == 16 || to_ == 32, "to_ must be 8, 16, 32");
    static_assert(from_ == 8 || from_ == 16 || from_ == 32, "from_ must be 8, 16, 32");
    size_t to_size = 0;
    char32_t c;
    while(utf_decode_and_move<from_>(c, from, from_size))
        to_size += utf_encode_and_move<to_>(to, c);
    return { to, to_size };
}

template<unsigned to_, unsigned from_, typename to_iterator_, typename from_iterator_>
utf_from_utf_return<to_iterator_> utf_from_utf(
    to_iterator_ to,
    from_iterator_ from,
    typename types::ifel<to_ != from_, from_iterator_>::result from_end
) {
    static_assert(to_ == 8 || to_ == 16 || to_ == 32, "to_ must be 8, 16, 32");
    static_assert(from_ == 8 || from_ == 16 || from_ == 32, "from_ must be 8, 16, 32");
    size_t to_size = 0;
    char32_t c;
    while(utf_decode_and_move<from_>(c, from, from_end))
        to_size += utf_encode_and_move<to_>(to, c);
    return { to, to_size };
}

template<unsigned to_, unsigned from_, typename to_iterator_, typename from_iterator_>
utf_from_utf_return<to_iterator_> utf_from_utf(
    to_iterator_ to,
    from_iterator_ from,
    typename types::ifel<to_ == from_, size_t>::result from_size
) {
    size_t to_size = 0;
    while(to_size != from_size) {
        *to = *from;
        ++from;
        ++to;
        ++to_size;
    }
    return { to, to_size };
}

template<unsigned to_, unsigned from_, typename to_iterator_, typename from_iterator_>
utf_from_utf_return<to_iterator_> utf_from_utf(
    to_iterator_ to,
    from_iterator_ from,
    typename types::ifel<to_ == from_, from_iterator_>::result from_end
) {
    size_t to_size = 0;
    while(from != from_end) {
        *to = *from;
        ++from;
        ++to;
        ++to_size;
    }
    return { to, to_size };
}

template<unsigned to_, unsigned from_, typename to_iterator_, typename from_iterator_>
utf_from_utf_return<to_iterator_> utf_from_utf_verify(
    to_iterator_ to,
    from_iterator_ from,
    size_t from_size
) {
    static_assert(to_ == 8 || to_ == 16 || to_ == 32, "to_ must be 8, 16, 32");
    static_assert(from_ == 8 || from_ == 16 || from_ == 32, "from_ must be 8, 16, 32");
    size_t to_size = 0;
    while(from_size) {
        char32_t c;
        if(!utf_decode_verify_and_move<from_>(c, from, from_size))
            return { to, 0 };
        to_size += utf_encode_and_move<to_>(to, c);
    }
    return { to, to_size };
}

template<unsigned to_, unsigned from_, typename to_iterator_, typename from_iterator_>
utf_from_utf_return<to_iterator_> utf_from_utf_verify(
    to_iterator_ to,
    from_iterator_ from,
    typename types::type<from_iterator_>::result from_end
) {
    static_assert(to_ == 8 || to_ == 16 || to_ == 32, "to_ must be 8, 16, 32");
    static_assert(from_ == 8 || from_ == 16 || from_ == 32, "from_ must be 8, 16, 32");
    size_t to_size = 0;
    while(from != from_end) {
        char32_t c;
        if(!utf_decode_verify_and_move<from_>(c, from, from_end))
            return { to, 0 };
        to_size += utf_encode_and_move<to_>(to, c);
    }
    return { to, to_size };
}

template<unsigned to_, unsigned from_, bool verify_, typename to_iterator_, typename from_iterator_>
utf_from_utf_return<to_iterator_> utf_from_utf_verify(
    to_iterator_ to,
    from_iterator_ from,
    typename types::ifel<verify_, size_t>::result from_size
) {
    return utf_from_utf_verify<to_, from_>(to, from, from_size);
}

template<unsigned to_, unsigned from_, bool verify_, typename to_iterator_, typename from_iterator_>
utf_from_utf_return<to_iterator_> utf_from_utf_verify(
    to_iterator_ to,
    from_iterator_ from,
    typename types::ifel<verify_, from_iterator_>::result from_end
) {
    return utf_from_utf_verify<to_, from_>(to, from, from_end);
}

template<unsigned to_, unsigned from_, bool verify_, typename to_iterator_, typename from_iterator_>
utf_from_utf_return<to_iterator_> utf_from_utf_verify(
    to_iterator_ to,
    from_iterator_ from,
    typename types::ifel<!verify_, size_t>::result from_size
) {
    return utf_from_utf<to_, from_>(to, from, from_size);
}

template<unsigned to_, unsigned from_, bool verify_, typename to_iterator_, typename from_iterator_>
utf_from_utf_return<to_iterator_> utf_from_utf_verify(
    to_iterator_ to,
    from_iterator_ from,
    typename types::ifel<!verify_, from_iterator_>::result from_end
) {
    return utf_from_utf<to_, from_>(to, from, from_end);
}

// with automatic utf

template<typename to_iterator_, typename from_iterator_>
utf_from_utf_return<to_iterator_> utf_from_utf(
    to_iterator_ to,
    from_iterator_ from,
    size_t from_size
) {
    return utf_from_utf<
        utf_from_iterator<to_iterator_>::result,
        utf_from_iterator<from_iterator_>::result
    >(to, from, from_size);
}

template<typename to_iterator_, typename from_iterator_>
utf_from_utf_return<to_iterator_> utf_from_utf(
    to_iterator_ to,
    from_iterator_ from,
    from_iterator_ from_end
) {
    return utf_from_utf<
        utf_from_iterator<to_iterator_>::result,
        utf_from_iterator<from_iterator_>::result
    >(to, from, from_end);
}

template<typename to_iterator_, typename from_iterator_>
utf_from_utf_return<to_iterator_> utf_from_utf_verify(
    to_iterator_ to,
    from_iterator_ from,
    size_t from_size
) {
    return utf_from_utf_verify<
        utf_from_iterator<to_iterator_>::result,
        utf_from_iterator<from_iterator_>::result
    >(to, from, from_size);
}

template<typename to_iterator_, typename from_iterator_>
utf_from_utf_return<to_iterator_> utf_from_utf_verify(
    to_iterator_ to,
    from_iterator_ from,
    from_iterator_ from_end
) {
    return utf_from_utf_verify<
        utf_from_iterator<to_iterator_>::result,
        utf_from_iterator<from_iterator_>::result
    >(to, from, from_end);
}

// utf_adjust_end
/////////////////////////////////////////////////////////////////////
// move the end back so it is not in the middle of a sequence
// returns the end
// useful when cutting strings in the middle
// begin must point to the beginning of a sequence, and the begin,end range must be valid UTF

template<typename iterator_>
iterator_ utf8_adjust_end(iterator_ begin, iterator_ end) {
    auto e = end;
    unsigned s = 0;
    while(e != begin) {
        --e;
        ++s;
        auto n = utf8_first_of(*e);
        if(n)
            return n > s ? e : end;
    }
    return end;
}

template<typename iterator_>
iterator_ utf16_adjust_end(iterator_ begin, iterator_ end) {
    auto e = end;
    if(begin != e && utf16_is_1_of_2(*--e))
        return e;
    return end;
}

template<typename iterator_>
iterator_ utf32_adjust_end(iterator_ /*begin*/, iterator_ end) {
    return end;
}

template<unsigned utf_, typename iterator_>
typename types::ifel<utf_ == 8, iterator_>::result utf_adjust_end(iterator_ begin, iterator_ end) {
    return utf8_adjust_end(begin, end);
}

template<unsigned utf_, typename iterator_>
typename types::ifel<utf_ == 16, iterator_>::result utf_adjust_end(iterator_ begin, iterator_ end) {
    return utf16_adjust_end(begin, end);
}

template<unsigned utf_, typename iterator_>
typename types::ifel<utf_ == 32, iterator_>::result utf_adjust_end(iterator_ begin, iterator_ end) {
    return utf32_adjust_end(begin, end);
}

}}
#endif
