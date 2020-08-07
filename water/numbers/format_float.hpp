// Copyright 2017-2018 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_NUMBERS_FORMAT_FLOAT_HPP
#define WATER_NUMBERS_FORMAT_FLOAT_HPP
#include <water/numbers/max_digits.hpp>
#include <water/numbers/format_int.hpp>
#include <water/numbers/formatted_mantissa_exponent.hpp>
#include <water/numbers/split_mantissa_exponent.hpp>
#include <water/numbers/round_mantissa_exponent.hpp>
#include <water/numbers/settings.hpp>
namespace water { namespace numbers {

/*

+-nan (also +-0)
+-infinity (also +-max)
+-1.23
+-12.3
+-123
+-0.123
+-1.23e+-4
+-0x1.23
+-0x1.23e+-4

scientific will move point to make exponent a multiple of 3

*/

class format_float
{
public:
    // these limits prevent integer overflows and ridiculously long output
    static unsigned constexpr
        precision_max = 10000,
        fraction_digits_max = precision_max,
        exponent_digits_max = format_int::digits_max;
    static int constexpr
        no_exponent_min_min = -10000,
        no_exponent_max_max = 10000;

private:
    unsigned mybase = 10;
    unsigned myprecision = 0; // 0 auto, unless myfraction_digits then this is the number of fraction digits
    unsigned myexponent_digits = 0; // 0 auto
    int my_no_exponent_min = 0;
    int my_no_exponent_max = -1;
    bool myfraction_digits = false;
    bool myplus = false;
    bool mytrailing0 = false;
    bool mybase_prefix_suffix = true;
    bool myscientific = false;

private:
    static constexpr int no_exponent_min_limit(int a) {
        return
            a < no_exponent_min_min ? no_exponent_min_min :
            a < 0 ? a :
            0;
    }

    static constexpr int no_exponent_max_limit(int a) {
        return
            a > no_exponent_max_max ? no_exponent_max_max :
            a > -1 ? a :
            -1;
    }

public:
    constexpr format_float() = default;

    format_float(settings a) :
        mybase{a.base() ? a.base() : 10},
        myprecision{
            a.fraction_digits_set() ?
                (a.fraction_digits() < fraction_digits_max ? a.fraction_digits() : fraction_digits_max) :
                (a.precision() < precision_max ? a.precision() : precision_max)
        },
        myexponent_digits{a.digits() < exponent_digits_max ? a.digits() : exponent_digits_max},
        my_no_exponent_min{no_exponent_min_limit(a.no_exponent_min())},
        my_no_exponent_max{no_exponent_max_limit(a.no_exponent_max())},
        myfraction_digits{a.fraction_digits_set()},
        myplus{a.plus()},
        mytrailing0{a.trailing_zeros()},
        mybase_prefix_suffix{a.base_prefix_suffix()},
        myscientific{a.scientific()}
    {}

    format_float& base(unsigned a) {
        // base != 10 has same format as hexfloat 0x1.ffffp123
        ___water_assert(a != 1 && a <= 16);
        mybase = 2 <= a && a <= 16 ? a : 10;
        return *this;
    }

    format_float& base_prefix_suffix(bool a) {
        mybase_prefix_suffix = a;
        return *this;
    }

    format_float& precision(unsigned a) {
        // round to this many significant digits
        myprecision = a < precision_max ? a : precision_max;
        myfraction_digits = false;
        return *this;
    }

    format_float& fraction_digits(unsigned a) {
        // round to this many fraction digits. 0 means round to integer
        myprecision = a < fraction_digits_max ? a : fraction_digits_max;
        myfraction_digits = true;
        return *this;
    }

    format_float& exponent_digits(unsigned a) {
        // exponent is always this many digits. if the exponent does not fit, the most significant digits are cut
        // 0 means auto
        myexponent_digits = a;
        return *this;
    }

    format_float& no_exponent_min_max(int min, int max) {
        // base 10 only.
        // min  0: always use exponent when its < 0
        // min -n: no exponent when its < 0 && >= -n. 0.00123 instead of 1.23e-3
        // max -1: no exponent when its >= 0 && < precision. 123.45 instead of 1.2345e2
        // max  0: always use exponent when its >= 0
        // max  n: exponent only if its >= n
        //
        // its not possible to hide exponent only if its 0
        //
        // base != 10 always has exponent
        my_no_exponent_min = no_exponent_min_limit(min);
        my_no_exponent_max = no_exponent_max_limit(max);
        return *this;
    }

    format_float& plus(bool a) {
        // show + and -0
        myplus = a;
        return *this;
    }

    format_float& scientific(bool a) {
        // exponent is a multiple of 3
        myscientific = a;
        return *this;
    }

    format_float& trailing_zeros(bool a) {
        // 1.23000 instead of 1.23
        mytrailing0 = a;
        return *this;
    }

    template<typename float_>
    formatted_mantissa_exponent<float_> operator()(float_ f) const {
        formatted<float_iterator<float_>> m;
        if(isinf_strict(f)) {
            m.infinity(true);
            f = signbit(f) ? -numeric_limits<float_>::max() : numeric_limits<float_>::max();
        }
        else if(isnan_strict(f)) {
            m.nan(true);
            f = signbit(f) ? -static_cast<float_>(0) : static_cast<float_>(0);
        }
        split_mantissa_exponent<float_> x(f, mybase);
        m.base(mybase);
        m.base_prefix_suffix(mybase != 10 && mybase_prefix_suffix);
        m.sign(x.minus ? -1 : myplus ? 1 : 0);
        m.point_at(1);
        unsigned leading0 = 0;
        bool zero = x.mantissa == 0;
        bool exponent_form = true;
        numbers::round_mantissa_exponent<float_> round;
        if(!zero) {
            // after this if
            // - m.digits() is set or zero = true
            // - rounded
            if(mybase == 10 && myfraction_digits) {
                // myprecision is the fraction digits, rounding depends on exponent.
                // can become zero if no_exponent_min is lower than -(myprecision + 1)
                if(x.exponent >= 0) {
                    int no_exponent_max = my_no_exponent_max >= 0 ? my_no_exponent_max : 1; // 1 because then rounds to 1 + fraction digits
                    if(no_exponent_max && x.exponent <= no_exponent_max) {
                        unsigned digits = static_cast<unsigned>(x.exponent) + myprecision + 1;
                        round(x.mantissa, x.exponent, mybase, digits);
                        // if round x.exponent + 1 and it should now be exponent form, the rounding will still be correct because rounded to >= 1 + myprecision digits
                        if(x.exponent <= no_exponent_max) {
                            exponent_form = false;
                            unsigned point_at = static_cast<unsigned>(x.exponent) + 1;
                            m.digits(
                                mytrailing0 ? digits :
                                round.digits() > point_at ? round.digits() :
                                point_at
                            );
                            m.point_at(m.digits() > point_at ? point_at : 0);
                        }
                    }
                    else
                        round(x.mantissa, x.exponent, mybase, myprecision + 1);
                    if(exponent_form)
                        m.digits(mytrailing0 ? myprecision + 1 : round.digits());
                }
                else if(x.exponent >= my_no_exponent_min) {
                    // exponent < 0. the number can be zero if my_no_exponent_min is < -myprecision
                    // 5e-3 is 0.005, if myprecision == 2 that should round to 0.01
                    leading0 = static_cast<unsigned>(-x.exponent);
                    if(leading0 > myprecision + 1)
                        zero = true;
                    else {
                        round(x.mantissa, x.exponent, mybase, (myprecision + 1) - leading0); // can round to 0 digits
                        leading0 = static_cast<unsigned>(-x.exponent);
                        if(leading0 < myprecision + 1) {
                            exponent_form = false;
                            m.digits(mytrailing0 ? myprecision + 1 : leading0 + round.digits());
                        }
                        else
                            zero = true;
                    }
                }
            }
            else {
                // always round to myprecision
                unsigned digits = myprecision ? myprecision : max_fraction_digits<float_>(mybase) + 1; // never 0
                round(x.mantissa, x.exponent, mybase, digits);
                m.digits(mytrailing0 ? digits : round.digits());
                if(mybase == 10) {
                    if(x.exponent >= 0) {
                        int no_exponent_max = my_no_exponent_max >= 0 ? my_no_exponent_max : static_cast<int>(digits - 1);
                        if(no_exponent_max && x.exponent <= no_exponent_max) {
                            // 12.3
                            // 123
                            // 123000
                            exponent_form = false;
                            unsigned point_at = static_cast<unsigned>(x.exponent) + 1;
                            if(point_at > m.digits()) {
                                m.digits(point_at);
                                point_at = 0;
                            }
                            m.point_at(point_at);
                        }
                    }
                    else if(x.exponent >= my_no_exponent_min) {
                        // 0.0123
                        exponent_form = false;
                        leading0 = static_cast<unsigned>(-x.exponent);
                        m.digits(leading0 + (mytrailing0 ? digits : round.digits()));
                    }
                }
            }
        }
        if(!zero) {
            // +-1.2345e+-123
            m.begin(float_iterator<float_>(x.mantissa, mybase, leading0, round.digits(), round.up()));
            if(exponent_form && myscientific && mybase == 10 && (x.exponent % 3)) {
                // scientific: reduce exponent and move point to the right.
                int em = (x.exponent % 3);
                if(em < 0)
                    em += 3;
                if(x.exponent < numeric_limits<int>::min() + em)
                    zero = true;
                else {
                    x.exponent -= em;
                    unsigned point_at = 1 + static_cast<unsigned>(em);
                    if(m.digits() <= point_at) {
                        m.digits(point_at);
                        m.point_at(0);
                    }
                    else
                        m.point_at(point_at);
                }
            }
            else if(m.digits() == 1)
                m.point_at(0);
        }
        if(zero) {
            // +-0
            // +-0.0000
            // +-0.00e+-0
            x.exponent = 0;
            m.digits(
                !mytrailing0 ? 1 :
                myfraction_digits ? myprecision + 1 :
                myprecision ? myprecision :
                max_fraction_digits<float_>(mybase) + 1
            );
            if(m.digits() == 1)
                m.point_at(0);
            m.sign(!myplus ? 0 : x.minus ? -1 : 1);
            exponent_form = mybase != 10 || my_no_exponent_max == 0;
        }
        // exponent
        formatted<int_iterator<int>> e;
        if(exponent_form && (x.exponent || !my_no_exponent_max))
            e = format_int{}.digits(myexponent_digits).plus(myplus)(x.exponent);
        return {m, e};
    }
};

}}
#endif
