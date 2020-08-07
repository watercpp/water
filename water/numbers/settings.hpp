// Copyright 2017-2018 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_NUMBERS_SETTINGS_HPP
#define WATER_NUMBERS_SETTINGS_HPP
#include <water/numbers/bits.hpp>
namespace water { namespace numbers {

/*

settings for writing and reading

this class should be 8 bytes

*/

namespace setting {
    unsigned constexpr
        no_base_prefix_suffix = 1 << 5, // cafe not 0xcafe
        plus = 1 << 6, // show + and -0
        precision_is_fraction_digits = 1 << 7,
        scientific = 1 << 8, // make exponent a multiple of 3
        trailing_zeros = 1 << 9, // 1.2300
        bool_text = 1 << 10, // ture/false insetad of 1/0
        group = 1 << 11, // 1 000 000
        digit_before_point = 1 << 12, // parser
        digit_after_point = 1 << 13, // parser
        space = 1 << 14; // parser
}

class settings
{
    using uint8 = unsigned char;
    using int16 = water::int_bits_at_least<16>;
    using uint16 = water::uint_bits_at_least<16>;
    static constexpr uint16
        base_mask = 0x1f,
        bits_mask = 0xffe0;

private:
    uint16 mybase_bits = 0; // low 5 bits is base, 0 - 31, the others 11 are setting-bits
    int16 my_no_exponent_min = 0;
    int16 my_no_exponent_max = -1;
    uint8 myprecision = 0;
    uint8 mydigits = 0; // integer and exponent

public:
    constexpr settings() = default;

    constexpr explicit settings(
        unsigned base,
        unsigned setting_bits = 0,
        unsigned precision = 0,
        unsigned digits = 0,
        int no_exponent_min = 0,
        int no_exponent_max = -1
    ) :
        mybase_bits{static_cast<uint16>(
            (2 <= base && base <= 16 ? base : 0) | (setting_bits & bits_mask)
        )},
        my_no_exponent_min{static_cast<int16>(
            no_exponent_min > 0 ? 0 :
            no_exponent_min < numeric_limits<int16>::min() ? numeric_limits<int16>::min() :
            no_exponent_min
        )},
        my_no_exponent_max{static_cast<int16>(
            no_exponent_max < -1 ? -1 :
            no_exponent_max > numeric_limits<int16>::max() ? numeric_limits<int16>::max() :
            no_exponent_max
        )},
        myprecision{
            precision <= static_cast<uint8>(-1) ? static_cast<uint8>(precision) : static_cast<uint8>(-1)
        },
        mydigits{
            digits <= static_cast<uint8>(-1) ? static_cast<uint8>(digits) : static_cast<uint8>(-1)
        }
    {}

    settings& base(unsigned a) {
        // must be between 2 and 16, or 0.
        // write: 0 means use base 10
        // read: 0 means reading detects base from input, or 10 if base_prefix_suffix(false)
        ___water_assert(a != 1 && a <= 16);
        mybase_bits = (mybase_bits & bits_mask) | static_cast<uint16>(a != 1 && a <= 16 ? a : 0);
        return *this;
    }

    settings& base_prefix_suffix(bool a) {
        // write:
        // - true: 0xcafe
        // - false: cafe
        // read
        // - true: can detect base from input, and will parse 0x
        // - false: will not parse 0x, base defaults to 10
        return bit(setting::no_base_prefix_suffix, !a);
    }

    settings& bool_text(bool a) {
        // write true/false instead of 1/0
        // read will parse true/false as 1/0 if true, if false it will not parse true/false
        return bit(setting::bool_text, a);
    }

    settings& digits(unsigned a) {
        // write integer and exponents as this many digits.
        // zeros will be prepended if the number has fewer digits.
        // if the number has more digits, the most significant digits are not written
        // 3 writes 1 as 001, 1234 as 234
        mydigits = a <= static_cast<uint8>(-1) ? static_cast<uint8>(a) : static_cast<uint8>(-1);
        return *this;
    }

    settings& group(bool a) {
        // write:
        // - true: group digits by thousands or what the locale decides. also writes a space before the exponent
        //   1.234567e1234 is 1.234 567 e1 234
        // read:
        // - true: parses group characters. 1 2 3 4567 is 1234567
        // - false: will not parse any group characters 1 2 3 4567 is 1
        return bit(setting::group, a);
    }

    settings& trailing_zeros(bool a) {
        // write floatingpoint trailing zeros to fill precision() or fraction_digits() digits
        // - true: 1.2300
        // - false: 1.23
        return bit(setting::trailing_zeros, a);
    }

    settings& exponent() {
        // write base 10 floatingpoint with exponent form always, see no_exponent_min_max
        my_no_exponent_min = my_no_exponent_max = 0;
        return *this;
    }

    settings& no_exponent() {
        // write base 10 floatingpoint without exponent always. this is dumb, becuase it can write thousands of digits. see no_exponent_min_max
        my_no_exponent_min = numeric_limits<int16>::min();
        my_no_exponent_max = numeric_limits<int16>::max();
        return *this;
    }

    settings& no_exponent_min(int a) {
        // see no_exponent_min_max
        my_no_exponent_min = static_cast<int16>(a > 0 ? 0 : a < numeric_limits<int16>::min() ? numeric_limits<int16>::min() : a);
        return *this;
    }

    settings& no_exponent_max(int a) {
        // see no_exponent_min_max
        my_no_exponent_max = static_cast<int16>(a < -1 ? -1 : a > numeric_limits<int16>::max() ? numeric_limits<int16>::max() : a);
        return *this;
    }

    settings& no_exponent_min_max(int min, int max) {
        // write base 10 floatingpoint in exponent form or not depeding on this.
        // min  0: always use exponent when its < 0
        // min -n: no exponent when its < 0 && >= -n. 0.00123 instead of 1.23e-3
        // max -1: no exponent when its >= 0 && < precision. 123.45 instead of 1.2345e2
        // max  0: always use exponent when its >= 0
        // max  n: exponent only if its >= n
        //
        // its not possible to hide exponent only if its 0
        no_exponent_min(min);
        no_exponent_max(max);
        return *this;
    }

    settings& plus(bool a) {
        // write +123 instead of 123. also writes -0 for floatingpoint negative 0
        return bit(setting::plus, a);
    }

    settings& precision(unsigned a) {
        // write rounds floatingpoint to this number of significant digits. overwrites fraction_digits()
        // - 0 means select the floatingpoint types precision
        // - 4 means 12345 is written as 12350
        myprecision = a <= static_cast<uint8>(-1) ? static_cast<uint8>(a) : static_cast<uint8>(-1);
        return bit(setting::precision_is_fraction_digits, false);
    }

    settings& fraction_digits(unsigned a) {
        // write rounds floatingpoint to this number of fraction digits. overwrites precision()
        // - 0 means 123.4567 is written as 123
        // - 3 means 123.4567 is written as 123.457
        myprecision = a <= static_cast<uint8>(-1) ? static_cast<uint8>(a) : static_cast<uint8>(-1);
        return bit(setting::precision_is_fraction_digits, true);
    }

    settings& scientific(bool a) {
        // write base 10 floatinpoint numers in "scientific" form, the exponent will be a multiple of 3
        // this is used only if the number is written in exponent form, see no_exponent_min_max
        return bit(setting::scientific, a);
    }

    settings& digit_after_point(bool a) {
        // read: require a digit after the decimal point. "123.e1" is read as "123.0e1" if false, "123" if true
        // write: no effect
        return bit(setting::digit_after_point, a);
    }

    settings& digit_before_point(bool a) {
        // read: require a digit before the decimal point. if false ".123" is read as 0.123
        // write: no effect
        return bit(setting::digit_before_point, a);
    }

    settings& space(bool a) {
        // read: true means the parser will skip over a space between parts of the number, "+ 0x 1.23 p - 123" is read as +0x1.23p-123
        // write: no effect
        return bit(setting::space, a);
    }

public:
    unsigned base() const {
        return mybase_bits & base_mask;
    }

    bool base_prefix_suffix() const {
        return !bit(setting::no_base_prefix_suffix);
    }

    bool bool_text() const {
        return bit(setting::bool_text);
    }

    bool digit_after_point() const {
        return bit(setting::digit_after_point);
    }

    bool digit_before_point() const {
        return bit(setting::digit_before_point);
    }

    unsigned digits() const {
        return mydigits;
    }

    unsigned fraction_digits() const {
        return bit(setting::precision_is_fraction_digits) ? myprecision : 0;
    }

    bool fraction_digits_set() const {
        // for fraction_digits(0)
        return bit(setting::precision_is_fraction_digits);
    }

    bool group() const {
        return bit(setting::group);
    }

    bool trailing_zeros() const {
        return bit(setting::trailing_zeros);
    }

    int no_exponent_min() const {
        return my_no_exponent_min;
    }

    int no_exponent_max() const {
        return my_no_exponent_max;
    }

    bool plus() const {
        return bit(setting::plus);
    }

    unsigned precision() const {
        return bit(setting::precision_is_fraction_digits) ? 0 : myprecision;
    }

    bool scientific() const {
        return bit(setting::scientific);
    }

    bool space() const {
        return bit(setting::space);
    }

private:
    bool bit(unsigned f) const {
        return (mybase_bits & f) != 0;
    }

    settings& bit(unsigned f, bool a) {
        if(a)
            mybase_bits |= static_cast<uint16>(f);
        else
            mybase_bits &= static_cast<uint16>(-1) ^ static_cast<uint16>(f);
        return *this;
    }
};

}}
#endif
