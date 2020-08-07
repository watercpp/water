// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_JSON_WRITE_NUMBER_HPP
#define WATER_JSON_WRITE_NUMBER_HPP
#include <water/json/node.hpp>
namespace water { namespace json {

template<typename to_>
void write_number(to_ to, number n) {
    int64_t
        integer = n.integer(),
        divide = integer < 0 ? -1 : 1;
    int32_t
        exponent = n.exponent(),
        digits = 1;
    // cannot divide int min with -1 when 2s complement...
    if(integer <= numeric_limits<int64_t>::min() / 10) {
        divide *= 10;
        ++digits;
    }
    while(integer / divide >= 10) {
        divide *= 10;
        ++digits;
    }
    int32_t point_at = digits;
    if(exponent) {
        // make it 1.23e45 not 123e43
        int32_t fraction_digits = digits - 1;
        if(fraction_digits > 0 && exponent < numeric_limits<int32_t>::max() - fraction_digits) {
            point_at = 1;
            exponent += fraction_digits;
            if(0 < exponent && exponent < fraction_digits) {
                // 1.23e1 can be 12.3
                point_at += exponent;
                exponent = 0;
            }
        }
    }
    // write
    if(integer < 0)
        to('-');
    int32_t at = 0;
    do {
        if(at == point_at) to('.');
        int64_t d;
        if(divide == -1)
            d = -(integer - (integer / 10) * 10);
        else
            d = (integer / divide) % 10;
        to(static_cast<char>('0' + d));
        divide /= 10;
    } while(++at != digits);
    if(exponent) {
        divide = exponent < 0 ? -1 : 1;
        digits = 1;
        if(integer <= numeric_limits<int32_t>::min() / 10) {
            divide *= 10;
            ++digits;
        }
        while(exponent / divide >= 10) {
            divide *= 10;
            ++digits;
        }
        to('e');
        if(exponent < 0) to('-');
        do {
            int32_t d;
            if(divide == -1)
                d = -(exponent - (exponent / 10) * 10);
            else
                d = (exponent / divide) % 10;
            to(static_cast<char>('0' + d));
            divide /= 10;
        } while(--digits);
    }
}

}}
#endif
