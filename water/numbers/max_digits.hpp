// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_NUMBERS_MAX_DIGITS_HPP
#define WATER_NUMBERS_MAX_DIGITS_HPP
#include <water/numbers/bits.hpp>
namespace water { namespace numbers {

/*

calcualte maximum number of digits a type can represent

*/

namespace _ {

    template<
        unsigned long a_,
        unsigned long r_ = 0,
        unsigned long x_ =
            (a_ >= (1ul << 16)) ? 16 :
            (a_ >= (1ul << 8)) ? 8 :
            (a_ >= (1ul << 4)) ? 4 :
            (a_ >= (1ul << 2)) ? 2 :
            1
    >
    struct hibit :
        hibit<(a_ >> x_), r_ + x_>
    {};
    
    template<unsigned long r_, unsigned long x_>
    struct hibit<0, r_, x_> {
        static unsigned long constexpr result = r_;
    };
    
    template<unsigned long r_, unsigned long x_>
    struct hibit<1, r_, x_> {
        static unsigned long constexpr result = r_;
    };
    
    // log(1. + a_ / 32.) / log(2.) * (1 << 16) + .5
    template<unsigned long a_>
    struct log2_lookup {
        static unsigned long constexpr result =
            a_ == 1 ? 2909ul :
            a_ == 2 ? 5732ul :
            a_ == 3 ? 8473ul :
            a_ == 4 ? 11136ul :
            a_ == 5 ? 13727ul :
            a_ == 6 ? 16248ul :
            a_ == 7 ? 18704ul :
            a_ == 8 ? 21098ul :
            a_ == 9 ? 23433ul :
            a_ == 10 ? 25711ul :
            a_ == 11 ? 27936ul :
            a_ == 12 ? 30109ul :
            a_ == 13 ? 32234ul :
            a_ == 14 ? 34312ul :
            a_ == 15 ? 36346ul :
            a_ == 16 ? 38336ul :
            a_ == 17 ? 40286ul :
            a_ == 18 ? 42196ul :
            a_ == 19 ? 44068ul :
            a_ == 20 ? 45904ul :
            a_ == 21 ? 47705ul :
            a_ == 22 ? 49472ul :
            a_ == 23 ? 51207ul :
            a_ == 24 ? 52911ul :
            a_ == 25 ? 54584ul :
            a_ == 26 ? 56229ul :
            a_ == 27 ? 57845ul :
            a_ == 28 ? 59434ul :
            a_ == 29 ? 60997ul :
            a_ == 30 ? 62534ul :
            a_ == 31 ? 64047ul :
            a_ == 32 ? 65536ul :
            0ul;
    };
    
    template<
        unsigned long a_,
        unsigned long hi_ = hibit<a_>::result,
        unsigned long f_ = a_ - (1ul << hi_),
        unsigned long l_ = ((f_ << 5) >> hi_), // 5 because lookup-size is 1<<5, 32
        unsigned long m_ = ((f_ << (16 + 5)) >> hi_) & 0xfffful
    >
    struct log2 {
        static unsigned long constexpr result =
            (hi_ << 16) +
            ((log2_lookup<l_>::result * (0x10000ul - m_) + log2_lookup<l_ + 1>::result * m_ + 0x8000ul) >> 16);
    };
    
    constexpr unsigned zero_if_less_than(unsigned a, unsigned b) { return a < b ? 0 : a; }
    
    template<
        typename a_,
        unsigned radix_ = zero_if_less_than(numeric_limits<a_>::radix, 2), // using x >= 2 ? x : 0 triggered a gcc warning :(
        unsigned digits_ = zero_if_less_than(numeric_limits<a_>::digits, 1),
        char select_ =
            (is_int<a_> || is_bool<a_>) ? 'i' :
            (!radix_ || !digits_) ? 0 :
            !(radix_ & (radix_ - 1)) ? '2' :
            (radix_ <= 63) ? 'l' :
            0
    >
    struct max_binary
    {};
    
    template<unsigned r_, unsigned d_>
    struct max_binary<bool, r_, d_, 'i'> {
        static unsigned constexpr result = 1;
    };
    
    template<typename a_, unsigned r_, unsigned d_>
    struct max_binary<a_, r_, d_, 'i'> {
        static unsigned constexpr result =
            numeric_limits<a_>::digits +
            (numeric_limits<a_>::is_signed && is_twos_complement<a_>() ? 1 : 0);
    };
    
    template<typename a_, unsigned r_, unsigned d_>
    struct max_binary<a_, r_, d_, '2'> {
        static unsigned constexpr result = 
            static_cast<unsigned>(d_ * hibit<r_>::result) +
            (numeric_limits<a_>::is_integer && numeric_limits<a_>::is_signed ? 1 : 0);
    };
    
    template<typename a_, unsigned r_, unsigned d_>
    struct max_binary<a_, r_, d_, 'l'> {
        static unsigned constexpr result = static_cast<unsigned>(
            ((static_cast<uint_largest_t>(d_) * (log2<r_>::result + 1) + 0xffffu) >> 16) +
            (numeric_limits<a_>::is_integer && numeric_limits<a_>::is_signed ? 1 : 0)
        );
    };
    
    template<typename a_, bool i_ = is_int<a_>>
    struct max_decimal {
        static unsigned constexpr result = 
            equal<a_, bool> ? 1 :
            (!numeric_limits<a_>::digits10 && !numeric_limits<a_>::digits) ? 0 :
            numeric_limits<a_>::digits10 +
            (numeric_limits<a_>::radix != 10 || (numeric_limits<a_>::is_integer && numeric_limits<a_>::is_signed) ? 1 : 0);
    };
    
    template<typename a_>
    struct max_decimal<a_, true> { // log10(2) * (1 << 16) = 19728
        static unsigned constexpr result = 
            static_cast<unsigned>((max_binary<a_>::result * 19728ul) >> 16) +
            ((max_binary<a_>::result * 19728ul) & 0xfffful ? 1 : 0);
    };
    
}

template<typename type_>
constexpr unsigned max_binary_digits() {
    // return the maximum numer of digits a value of type_ can represent
    //
    // this has a result only if
    // - numeric_limits<type_>::radix is a power of 2, or...
    // - 3 <= numeric_limits<type_>::radix <= 63
    //
    // this is made to never return fewer digits than nessesary, but it
    // could return slightly more. it can be used for buffer sizes.
    return _::max_binary<type_>::result;
}

template<typename type_>
constexpr unsigned max_decimal_digits() {
    // return the maximum numer of digits a value of type_ can represent in base 10, usually numeric_limits<type_>::digits10 + 1
    return _::max_decimal<type_>::result;
}

namespace _ {

    template<unsigned r_, typename t_, bool pow2_ = (r_ & (r_ - 1)) == 0>
    struct max_digits_do {
        static unsigned do_it(unsigned base) {
            // radix is power of 2
            if(base == 10)
                return max_decimal_digits<t_>();
            if(base == 2)
                return max_binary_digits<t_>();
            if(!(base & (base - 1))) {
                unsigned d = 2;
                while((base >> d) != 1) ++d;
                return max_binary_digits<t_>() / d + (max_binary_digits<t_>() % d ? 1 : 0);
            }
            return static_cast<unsigned>(max_binary_digits<t_>() * (.69314718056 / log(static_cast<double>(base))) + 1.);
        }
    };

    template<unsigned r_, typename t_>
    struct max_digits_do<r_, t_, false> {
        static unsigned do_it(unsigned base) {
            // radix not power of 2
            typedef numeric_limits<t_> l_;
            if(base == 10)
                return max_decimal_digits<t_>();
            if(base == r_)
                return static_cast<unsigned>(l_::digits) + (l_::is_integer && l_::is_signed ? 1 : 0);
            return static_cast<unsigned>(l_::digits * (log(static_cast<double>(r_)) / log(static_cast<double>(base))) + 1.);
        }
    };

}

template<typename type_>
unsigned max_digits(unsigned base = 10) {
    // return the maximum number of digits a value of type_ can represent in base
    //
    // returns 0 if any of these things are true
    // - base < 2
    // - numeric_limits<type_>::radix < 2
    // - numeric_limits<type_>::digits < 1
    if(numeric_limits<type_>::radix >= 2 && numeric_limits<type_>::digits > 0 && base >= 2)
        return _::max_digits_do<numeric_limits<type_>::radix, type_>::do_it(base);
    return 0;
}

template<typename type_>
unsigned max_fraction_digits(unsigned base = 10) {
    // return
    // - the maximum number of fraction digits a value of type_ can represent in base, with 1 integer digit
    // - 0 if type_ is not a floating point type
    //
    // if base == 10, this returns the maxumum fraction digits when the integer digit is 1-9, for example 1.234 or 9.876
    // if base != 10, this returns the maxumum fraction digits when the integer digit is 1, for example 0x1.2345 or 0x1.cafe
    //
    if(!numeric_limits<type_>::max_exponent || base <= 1)
        return 0;
    if(base == 10)
        return numeric_limits<type_>::digits10 - (numeric_limits<type_>::digits10 && numeric_limits<type_>::radix == 10 ? 1 : 0);
    if(!max_binary_digits<type_>())
        return 0;
    unsigned const max2 = max_binary_digits<type_>() - 1;
    if(base == 2)
        return max2;
    if(!(base & (base - 1))) {
        unsigned d = 2;
        while((base >> d) != 1) ++d;
        return max2 / d + (max2 % d ? 1 : 0);
    }
    return static_cast<unsigned>(max2 * (.69314718056 / log(static_cast<double>(base))) + 1.);
}

}}
#endif
