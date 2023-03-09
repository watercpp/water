// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_XTR_FLOAT_HPP
#define WATER_XTR_FLOAT_HPP
#include <water/xtr/base.hpp>
#include <water/numeric_limits.hpp>
#include <water/cmath.hpp>
namespace water { namespace xtr {

/*

this needs
- numeric_limits
- numeric_limits<float_type>::radix is 2 or 10
- math.h functions
  - fmod
  - frexp
  - log10
  - pow
  - isinf
  - isnan
  - signbit
  
number_format::digits > 0 will round the mantissa to that many significant digits.
3 would round 1.234 to 1.23, or 9.876 to 9.88, or 9.999 to 10.
It does not affect the exponent. And it does not write exactly that many digits.

Always assume exponent can use all digits of int and not that it is limited to max_exponent10 or
min_exponent from numeric_limts. When there are no real long-double math functions strange things
happen.

*/

template<typename type_, typename number_format_ = number_format<>>
class write_float
{
    static_assert(numeric_limits<type_>::radix == 2 || numeric_limits<type_>::radix == 10, "");
    
    using write_exponent = write_int<
        int,
        number_format_digits<0, // exponent is never a fixed number of digits
        number_format_base<10, // and always base 10
        number_format_>>
    >;
    
    static constexpr unsigned largest(unsigned a, unsigned b) { return a >= b ? a : b; }
    
    static int constexpr
        digits2 =
            numeric_limits<type_>::radix == 2 ? numeric_limits<type_>::digits :
            static_cast<int>((numeric_limits<type_>::digits * 217706ul + 0xfffful) >> 16), // log(10) / log(2) * (1 << 16) => 3.32192809488 * 0x10000 => 217705.9
        
        mantissa_digits = // without sign or dot or base-prefix
            number_format_::digits > 0 ? number_format_::digits :
            number_format_::base == 2 ? digits2 :
            number_format_::base == 8 ? 1 + (digits2 - 1) / 3 + ((digits2 - 1) % 3 ? 1 : 0) : // 1.777 without dot
            number_format_::base == 16 ? 1 + (digits2 - 1) / 4 + ((digits2 - 1) % 4 ? 1 : 0) : // 1.fff without dot
            numeric_limits<type_>::digits10 + 1,
            
        // if base 10 the no exponent form could be larger than the maximum exponent form
        // no_exponent_max is the number of digits
        // -no_exponent_min is the number of zeros added before mantissa_digits
        no_exponent_digits = // without sign or dot or base-prefix
            number_format_::base != 10 ? 0 :
            number_format_::no_exponent_max > mantissa_digits - number_format_::no_exponent_min ?
                number_format_::no_exponent_max :
                mantissa_digits - number_format_::no_exponent_min;

public:
    static constexpr unsigned
        base = number_format_::base,
        size = largest(
            sizeof(u8"infinity"), // 0 at end = sign included
            1 + (base == 10 || number_format_::hide_base ? 0 : 2) + // -0x
            largest(
                mantissa_digits + 1 + 1 + write_exponent::size, // 1.23e-123
                no_exponent_digits + 1 // 000.123
            )
        );

private:
    type_ my;

public:
    write_float(type_ a) :
        my(a)
    {}

    template<typename write_>
    void operator()(write_&& write) const {
        bool minus = signbit(my);
        if(minus)
            write(static_cast<char>(u'-'));
        else if(number_format_::show_plus)
            write(static_cast<char>(u'+'));
        if(isnan_strict(my))
            write(number_format_::uppercase ? u8"NAN" : number_format_::mixedcase ? u8"NaN" : u8"nan", 3);
        else if(isinf_strict(my))
            write(number_format_::uppercase ? u8"INFINITY" : u8"infinity", sizeof(u8"infinity") - 1);
        else {
            type_ m = minus ? -my : my;
            // mantissa + exponent
            bool
                exponent = true,
                round_last_digit = false;
            int
                e = 0,
                point_at = 1,
                leading_zeros = 0,
                trailing_zeros = 0,
                digits;
            if(!m) {
                digits = 1;
                exponent = base != 10 || number_format_::no_exponent_max == 0;
            }
            else if(constant(base == 10)) {
                e = static_cast<int>(log10(m));
                int p = e;
                if((numeric_limits<type_>::is_iec559 || numeric_limits<type_>::has_denorm > 0) && m < numeric_limits<type_>::min())
                    do ++p; while((m *= static_cast<type_>(10)) < numeric_limits<type_>::min());
                m /= pow(static_cast<type_>(10), static_cast<type_>(p));
                digits = round(m, e, round_last_digit);
                if(
                    (constant(number_format_::no_exponent_max) < 0 && 0 <= e && e < mantissa_digits) ||
                    (constant(number_format_::no_exponent_max) > 0 && e >= 0 && number_format_::no_exponent_max >= e)
                ) {
                    // 123000 or 123.456
                    point_at = e + 1;
                    if(point_at > digits)
                        trailing_zeros = point_at - digits;
                    exponent = false;
                }
                else if(constant(number_format_::no_exponent_min < 0) && e < 0 && number_format_::no_exponent_min <= e) {
                    // 0.000123
                    point_at = -1;
                    leading_zeros = -e;
                    exponent = false;
                }
            }
            else {
                m = frexp(m, &e) * static_cast<type_>(2); //returns 0.5 <= mantissa < 1
                --e;
                digits = round(m, e, round_last_digit);
            }
            // write mantissa
            if(!number_format_::hide_base)
                _::write_base_prefix(write, base, number_format_::uppercase);
            if(constant(base == 10) && leading_zeros) {
                // 0.00123
                write(_::digits[!number_format_::lowercase][0]);
                write(static_cast<char>(u'.'));
                while(--leading_zeros)
                    write(_::digits[!number_format_::lowercase][0]);
            }
            int at = 0;
            do {
                if(at == point_at)
                    write(static_cast<char>(u'.'));
                unsigned d = static_cast<unsigned>(m);
                if(++at == digits && round_last_digit)
                    ++d;
                write(_::digits[!number_format_::lowercase][d & 0xf]);
                m = fmod(m, static_cast<type_>(1.)) * base;
            } while(at != digits);
            while(trailing_zeros--)
                write(_::digits[!number_format_::lowercase][0]);
            if(exponent) {
                // write exponent even if 0 because printf(%a) does
                write(base == 10 ? static_cast<char>(number_format_::lowercase ? u'e' : u'E') : static_cast<char>(number_format_::uppercase ? u'P' : u'p'));
                write_exponent{e}(write);
            }
        }
    }

private:
    template<typename a_>
    constexpr a_ constant(a_ a) const {
        // used to avoid "warning C4127: conditional expression is constant" :(
        return a;
    }

    static int round(type_& f, int& e, bool& round_last_digit) {
        // things here could in theory oveflow int
        type_ const power = base == 10 ? 10 : 2;
        round_last_digit = false;
        if(f < 1) {
            f *= power;
            --e;
        }
        else if(f >= power) {
            f /= power;
            ++e;
        }
        type_ ff = f;
        unsigned
            at = 0,
            nines = base != 10 || static_cast<unsigned>(ff) == 9 ? 1 : 0, // base != 10 always starts with 1
            zeros = 0;
        ff = fmod(ff, static_cast<type_>(1.)) * base;
        while(++at != mantissa_digits) {
            unsigned d = static_cast<unsigned>(ff);
            ff = fmod(ff, static_cast<type_>(1.)) * base;
            if(d == base - 1) {
                ++nines;
                zeros = 0;
            }
            else {
                nines = 0;
                if(d)
                    zeros = 0;
                else
                    ++zeros;
            }
        }
        // just after last digit
        unsigned d = static_cast<unsigned>(ff);
        if(d < base / 2 + (base & 1))
            return static_cast<int>(mantissa_digits - zeros);
        if(nines != mantissa_digits) {
            round_last_digit = true;
            return static_cast<int>(mantissa_digits - nines);
        }
        // 9.9995
        f = 1;
        ++e;
        return 1;
    }
};

template<typename p_, typename w_>
auto operator<<(expression<p_, w_>&& x, float a) -> expression<expression<p_, w_>, write_float<float, typename expression<p_, w_>::number_format>> {
    return {x, a};
}

template<typename p_, typename w_>
auto operator<<(expression<p_, w_>&& x, double a) -> expression<expression<p_, w_>, write_float<double, typename expression<p_, w_>::number_format>> {
    return {x, a};
}

template<typename p_, typename w_>
auto operator<<(expression<p_, w_>&& x, long double a) -> expression<expression<p_, w_>, write_float<long double, typename expression<p_, w_>::number_format>> {
    return {x, a};
}

}}
#endif
