// Copyright 2017-2021 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_JSON_READ_STRING_HPP
#define WATER_JSON_READ_STRING_HPP
#include <water/json/bits.hpp>
namespace water { namespace json {

inline long read_hex(uchar_t *begin, uchar_t* end) {
    // return -1 or 0...0xffff
    if(end - begin < 4)
        return -1;
    end = begin + 4;
    long r = 0;
    while(begin != end) {
        long d;
        if('0' <= *begin && *begin <= '9')
            d = static_cast<long>(*begin - '0');
        else if('a' <= *begin && *begin <= 'f')
            d = static_cast<long>(*begin - 'a') + 10;
        else if('A' <= *begin && *begin <= 'F')
            d = static_cast<long>(*begin - 'A') + 10;
        else
            return -1;
        r = (r << 4) | d;
        ++begin;
    }
    return r;
}


inline uchar_t* read_string(uchar_t*& from, uchar_t* end) {
    // return the new end, 0 if error
    // overwrites begin...end
    // if error from should point to where the error was, anything before that can be destroyed
    uchar_t *to = from;
    while(from != end && 0x20 <= *from && *from != '"') {
        if(*from == '\\') {
            if(++from == end)
                return 0;
            switch(*from) {
                case '"':
                case '\\':
                case '/': *to++ = *from++; break;
                case 'b': *to++ = '\b'; ++from; break; //backspace
                case 'f': *to++ = '\f'; ++from; break; //form feed
                case 'n': *to++ = '\n'; ++from; break;
                case 'r': *to++ = '\r'; ++from; break;
                case 't': *to++ = '\t'; ++from; break;
                case 'u':
                    // \u#### 4 hex chars, the UTF-16 char
                    // codepoints > 0xffff must be encoded as a UTF-16 pair
                    if(++from != end) {
                        auto u = read_hex(from, end);
                        if(u >= 0) {
                            from += 4;
                            if(unicode::utf16_is_1_of_2(u) && end - from >= 6 && from[0] == '\\' && from[1] == 'u') { // 1 of 2
                                from += 2;
                                auto u1 = read_hex(from, end);
                                if(unicode::utf16_is_2_of_2(u1)) {
                                    u = unicode::utf16_unpack(u, u1);
                                    from += 4;
                                }
                                else
                                    u = -1;
                            }
                            else if(!unicode::utf16_is_1_of_1(u))
                                u = -1;
                        }
                        if(u >= 0) {
                            unicode::utf8_encode_and_move(to, static_cast<char32_t>(u));
                            break;
                        }
                    }
                default: return 0;
            }
        }
        else {
            ptrdiff_t n = static_cast<ptrdiff_t>(unicode::utf8_first_of(*from));
            if(!n || n > end - from)
                return 0;
            switch(n) {
                case 1:
                    *to++ = *from++;
                    break;
                case 2:
                    if(!unicode::utf8_verify(from[0], from[1])) return 0;
                    *to++ = *from++;
                    *to++ = *from++;
                    break;
                case 3:
                    if(!unicode::utf8_verify(from[0], from[1], from[2])) return 0;
                    *to++ = *from++;
                    *to++ = *from++;
                    *to++ = *from++;
                    break;
                case 4:
                    if(!unicode::utf8_verify(from[0], from[1], from[2], from[3])) return 0;
                    *to++ = *from++;
                    *to++ = *from++;
                    *to++ = *from++;
                    *to++ = *from++;
                    break;
            }
        }
    }
    if(from != end && *from == '"') {
        ++from;
        return to;
    }
    return 0;
}

}}
#endif
