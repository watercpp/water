// Copyright 2017-2021 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_NUMBERS_LOCALE_HPP
#define WATER_NUMBERS_LOCALE_HPP
#include <water/numbers/locale_const.hpp>
namespace water { namespace numbers {

/*

locale, empty class with all constexpr data
this is not localized at all.

it mixes lower and uppercase, digits are uppercase but bases are lowercase. 0xCAFE

locale_lowercase is all lowercase
locale_uppercase has uppercase bases

*/

namespace _ { namespace locales {

    locale_const_string<char16_t> constexpr infinities[] {
        {u"infinity", 8},
        {u"inf", 3},
        {u"\u221E", 1} // oo
    };

    locale_const_string<char8_or_char> constexpr nans_lowercase[] {
        {u8"nan", 3}
    };

    locale_const_string<char8_or_char> constexpr nans[] {
        {u8"NaN", 3}
    };

    locale_const_base<char8_or_char> constexpr bases_lowercase[] {
        {16, {u8"0x", 2}},
        {8, {u8"0o", 2}},
        {2, {u8"0b", 2}},
        {16, {u8"#", 1}},
        {16, {0, 0}, {u8"h", 1}},
        {8, {0, 0}, {u8"o", 1}},
        {2, {0, 0}, {u8"b", 1}} // works if parser is set to read base 2
    };

    locale_const_base<char8_or_char> constexpr bases_uppercase[] {
        {16, {u8"0X", 2}},
        {8, {u8"0O", 2}},
        {2, {u8"0B", 2}},
        {16, {u8"#", 1}},
        {16, {0, 0}, {u8"H", 1}},
        {8, {0, 0}, {u8"O", 1}},
        {2, {0, 0}, {u8"B", 1}} // works if parser is set to read base 2
    };

    locale_const_exponent<char8_or_char> constexpr exponents_lowercase[] {
        {10, {u8"e", 1}},
        {2, {u8"p", 1}}
    };

    locale_const_exponent<char8_or_char> constexpr exponents_uppercase[] {
        {10, {u8"E", 1}},
        {2, {u8"P", 1}}
    };

    locale_const_string<char8_or_char> constexpr trues[] {
        {u8"true", 4},
        {u8"yes", 3}
    };

    locale_const_string<char8_or_char> constexpr falses[] {
        {u8"false", 5},
        {u8"no", 2}
    };

}}

struct locale
{
    begin_end<char8_or_char const*> digits() const {
        return {u8"0123456789ABCDEF", 16};
    }

    begin_end<char8_or_char const*> pluses() const {
        return {u8"+", 1};
    }

    begin_end<char16_t const*> minuses() const {
        return {u"-\u2212", 2};
    }

    begin_end<char16_t const*> groups() const {
        return spaces();
    }

    begin_end<char8_or_char const*> points() const {
        return {u8".,", 2};
    }

    begin_end<char16_t const*> spaces() const {
        // https://en.wikipedia.org/wiki/Whitespace_character
        return {u" \u00a0\u2000\u2001\u2002\u2003\u2004\u2005\u2006\u2007\u2008\u2009\u200a\u202f\u205f\u3000", 16};
    }

    begin_end<locale_const_string<char16_t> const*> infinities() const {
        return {_::locales::infinities, _::locales::infinities + sizeof(_::locales::infinities) / sizeof(_::locales::infinities[0])};
    }

    begin_end<locale_const_string<char8_or_char> const*> nans() const {
        return {_::locales::nans, _::locales::nans +  sizeof(_::locales::nans) / sizeof(_::locales::nans[0])};
    }

    begin_end<locale_const_base<char8_or_char> const*> bases() const {
        return {_::locales::bases_lowercase, _::locales::bases_lowercase + sizeof(_::locales::bases_lowercase) / sizeof(_::locales::bases_lowercase[0])};
    }

    begin_end<locale_const_exponent<char8_or_char> const*> exponents() const {
        return {_::locales::exponents_uppercase, _::locales::exponents_uppercase + sizeof(_::locales::exponents_uppercase) / sizeof(_::locales::exponents_uppercase[0])};
    }

    begin_end<locale_const_string<char8_or_char> const*> trues() const {
        return {_::locales::trues, _::locales::trues +  sizeof(_::locales::trues) / sizeof(_::locales::trues[0])};
    }

    begin_end<locale_const_string<char8_or_char> const*> falses() const {
        return {_::locales::falses, _::locales::falses +  sizeof(_::locales::falses) / sizeof(_::locales::falses[0])};
    }

    group group_function() const {
        return {};
    }
};

struct locale_lowercase : locale
{
    begin_end<char8_or_char const*> digits() const {
        return {u8"0123456789abcdef", 16};
    }

    begin_end<locale_const_string<char8_or_char> const*> nans() const {
        return {_::locales::nans_lowercase, _::locales::nans_lowercase +  sizeof(_::locales::nans_lowercase) / sizeof(_::locales::nans_lowercase[0])};
    }

    begin_end<locale_const_base<char8_or_char> const*> bases() const {
        return {_::locales::bases_uppercase, _::locales::bases_uppercase + sizeof(_::locales::bases_uppercase) / sizeof(_::locales::bases_uppercase[0])};
    }

    begin_end<locale_const_exponent<char8_or_char> const*> exponents() const {
        return {_::locales::exponents_lowercase, _::locales::exponents_lowercase + sizeof(_::locales::exponents_lowercase) / sizeof(_::locales::exponents_lowercase[0])};
    }
};

struct locale_uppercase : locale
{
    begin_end<locale_const_base<char8_or_char> const*> bases() const {
        return {_::locales::bases_uppercase, _::locales::bases_uppercase + sizeof(_::locales::bases_uppercase) / sizeof(_::locales::bases_uppercase[0])};
    }
};

}}
#endif
