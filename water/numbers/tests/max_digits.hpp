// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_NUMBERS_TESTS_MAX_DIGITS_HPP
#define WATER_NUMBERS_TESTS_MAX_DIGITS_HPP
#include <water/numbers/tests/bits.hpp>
namespace water { namespace numbers { namespace tests {


/*

test max_digits compiletime implementation vs runtime implementation here

*/

template<typename out_>
void max_digits_make_log2_lookup(out_ o, unsigned bits = 16, unsigned size = 32) {
    o << "log2 lookup bits=" << bits << ", size=" << size << '\n';
    unsigned long one = 1ul << bits;
    unsigned i = 1;
    do {
        o << "a_ == " << i << " ? " << static_cast<unsigned long>((log(1. + i / static_cast<double>(size)) / log(2.)) * one + .5) << "ul :" << '\n';
    } while(++i <= size);
    o << 0 << '\n';
}

template<int radix_, int digits_, bool integer_ = false, bool signed_ = false> struct max_digits_t;

}}}

#ifdef WATER_NO_STD
namespace water {
#else
namespace std {
#endif

    // only what max_binary_digits wants
    template<int radix_, int digits_, bool integer_, bool signed_>
    class numeric_limits<water::numbers::tests::max_digits_t<radix_, digits_, integer_, signed_> > {
    public:
        static bool constexpr
            is_specialized = true,
            is_integer = integer_,
            is_signed = signed_;
        static int constexpr
            digits = digits_,
            radix = radix_;
    };
    
}

namespace water { namespace numbers { namespace tests {

namespace max_digits_tests {

    inline unsigned hibit(unsigned long a) {
        unsigned r = 0;
        while(a > 1) {
            unsigned x =
                (a >= (1ul << 16)) ? 16 :
                (a >= (1ul << 8)) ? 8 :
                (a >= (1ul << 4)) ? 4 :
                (a >= (1ul << 2)) ? 2 :
                1;
            a >>= x;
            r += x;
        }
        return r;
    }
    
    inline unsigned long log2_lookup(unsigned long a_) {
        return
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
    }
    
    inline unsigned long log2(unsigned long a) {
        unsigned long
            hi = hibit(a),
            f = a - (1ul << hi),
            l = ((f << 5) >> hi),
            m = ((f << (16 + 5)) >> hi) & 0xfffful;
        return
            (hi << 16) +
            ((log2_lookup(l) * (0x10000ul - m) + log2_lookup(l + 1) * m + 0x8000ul) >> 16);
    }
    
    inline unsigned max_binary(unsigned radix, unsigned digits) {
        if(!(radix & (radix - 1)))
            return static_cast<unsigned>(digits * hibit(radix));
        typedef uint_largest_t u_;
        u_ x = static_cast<u_>(digits) * (log2(radix) + 1) + 0xfffful;
        return static_cast<unsigned>(x >> 16);
    }
    
    inline unsigned max_binary_float(unsigned radix, unsigned digits) {
        double d = digits * (log(static_cast<double>(radix)) / log(2.));
        unsigned u = static_cast<unsigned>(d);
        if(d - static_cast<double>(u)) ++u;
        return u;
    }
    
}

inline void max_digits_binary() {
    unsigned radix = 1;
    do {
        ++radix;
        unsigned digits = 0;
        do {
            ++digits;
            unsigned
                mine = max_digits_tests::max_binary(radix, digits),
                correct = max_digits_tests::max_binary_float(radix, digits);
            ___water_test(mine >= correct);
            if(mine > correct) {
                auto error = mine / static_cast<double>(correct) - 1.;
                ___water_test(error < 0.005);
            }
        } while(digits != 0xffffu);
    } while(radix != 31);
}

inline void max_digits_test_log2() {
    unsigned radix = 1;
    while(++radix != 65) {
        long
            mine = static_cast<long>(max_digits_tests::log2(radix)),
            correct = static_cast<long>((log(static_cast<double>(radix)) / log(2.)) * 0x10000ul + .5);
        ___water_test(mine == correct);
    }
}

template<unsigned radix_, unsigned digits_max_, unsigned digits_ = 1, bool end_ = !(digits_ <= digits_max_)>
struct max_digits_test_radix {
    static void do_it() {
        auto here = max_digits_tests::max_binary(radix_, digits_);
        auto real = max_binary_digits<max_digits_t<radix_, digits_> >();
        ___water_test(here == real);
        max_digits_test_radix<radix_, digits_max_, digits_ + digits_ / 2 + (digits_ / 2 ? 0 : 1)>::do_it();
    }
};

template<unsigned radix_, unsigned digits_max_, unsigned digits_>
struct max_digits_test_radix<radix_, digits_max_, digits_, true> {
    static void do_it() {}
};

template<unsigned radix_max_, unsigned digits_max_, unsigned radix_ = 2, bool end_ = !(radix_ <= radix_max_)>
struct max_digits_test_do {
    static void do_it() {
        max_digits_test_radix<radix_, digits_max_>::do_it();
        max_digits_test_do<radix_max_, digits_max_, radix_ + 1>::do_it();
    }
};

template<unsigned radix_max_, unsigned digits_max_, unsigned radix_>
struct max_digits_test_do<radix_max_, digits_max_, radix_, true> {
    static void do_it() {}
};

template<unsigned radix_max_, unsigned digits_max_>
void max_digits_test() {
    max_digits_test_do<radix_max_, digits_max_>::do_it();
}

inline void max_digits_all() {
    //max_digits_make_log2_lookup(trace);
    max_digits_test_log2();
    max_digits_binary();
    max_digits_test<64, 4096>();
}

}}}
#endif
