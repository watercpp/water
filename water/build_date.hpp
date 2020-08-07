// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_BUILD_DATE_HPP
#define WATER_BUILD_DATE_HPP(a, b, c) __DATE__[0] == a && __DATE__[1] == b && __DATE__[2] == c
namespace water { namespace build_date {

// Make compile time constants from the built in __DATE__. Remember that this can change between compilation units
//
// __DATE__
// Mon dd yyyy
// Dec 24 2016
// if day is < 10 a space is added Aug  5 2016
// months Jan Feb Mar Apr May Jun Jul Aug Sep Oct Nov Dec
// same as asctime
//
// __TIME__ is hh:mm:ss like 23:45:01
// same as asctime

constexpr unsigned month =
    WATER_BUILD_DATE_HPP('J','a','n') ? 1 :
    WATER_BUILD_DATE_HPP('F','e','b') ? 2 :
    WATER_BUILD_DATE_HPP('M','a','r') ? 3 :
    WATER_BUILD_DATE_HPP('A','p','r') ? 4 :
    WATER_BUILD_DATE_HPP('M','a','y') ? 5 :
    WATER_BUILD_DATE_HPP('J','u','n') ? 6 :
    WATER_BUILD_DATE_HPP('J','u','l') ? 7 :
    WATER_BUILD_DATE_HPP('A','u','g') ? 8 :
    WATER_BUILD_DATE_HPP('S','e','p') ? 9 :
    WATER_BUILD_DATE_HPP('O','c','t') ? 10 :
    WATER_BUILD_DATE_HPP('N','o','v') ? 11 :
    WATER_BUILD_DATE_HPP('D','e','c') ? 12 :
    0;

constexpr unsigned day = (((__DATE__[4] == ' ' ? 0 : __DATE__[4] - '0') % 10) * 10 + ((__DATE__[5] - '0') % 10)) % 32;

constexpr unsigned year =
    ((__DATE__[7] - '0') % 10) * 1000 +
    ((__DATE__[8] - '0') % 10) * 100 +
    ((__DATE__[9] - '0') % 10) * 10 +
    ((__DATE__[10] - '0') % 10);

constexpr unsigned
    hour = ((__TIME__[0] - '0') * 10 + (__TIME__[1] - '0')) % 24,
    minute = ((__TIME__[3] - '0') * 10 + (__TIME__[4] - '0')) % 60,
    second = ((__TIME__[6] - '0') * 10 + (__TIME__[7] - '0')) % 60;

inline constexpr char digit(unsigned number, unsigned divide) { return '0' + static_cast<char>((number / divide) % 10); }

// yyyy-mm-dd hh:mm:ss
constexpr char string[] {
    digit(year, 1000), digit(year, 100), digit(year, 10), digit(year, 1), '-', digit(month, 10), digit(month, 1), '-', digit(day, 10), digit(day, 1),
    ' ',
    digit(hour, 10), digit(hour, 1), ':', digit(minute, 10), digit(minute, 1), ':', digit(second, 10), digit(second, 1),
    0
};

}}
#endif
