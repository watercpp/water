// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_NO_NUMERIC_LIMITS_HPP
#define WATER_NO_NUMERIC_LIMITS_HPP
#include <water/water.hpp>
#include <water/types.hpp>
#include <water/is_no_to.hpp>
#include <water/signed_representation.hpp>
#ifdef WATER_NO_CHEADERS
    #include <float.h>
    #include <limits.h>
    #include <math.h> // INFINITY + NAN
#else
    #include <cfloat>
    #include <climits>
    #include <cmath>
#endif
namespace water {

/*

no_numeric_limits is used by water when there is no std::numeric_limits. It should work exactly like
std::numeric_limits for the built in integer, floatingpoint and bool types except that signaling_NaN
returns quiet_NaN.

If the floatingpoint types are not IEC 60559 (IEEE 754) this wont work.

*/

namespace _ { namespace no_numeric_limits {
    
    template<
        typename int_,
        int_ value_,
        unsigned result_ = 0,
        unsigned shift_ =
            (value_ / (static_cast<int_>(1) << (sizeof(int_) * 4))) ? sizeof(int_) * 4 + 1 :
            (value_ / (static_cast<int_>(1) << (sizeof(int_) * 2))) ? sizeof(int_) * 2 + 1 :
            (value_ / (static_cast<int_>(1) << (sizeof(int_) * 1))) ? sizeof(int_) * 1 + 1 :
            value_ ? 1 :
            0
    >
    struct bitsof_int :
        bitsof_int<int_, (value_ / (static_cast<int_>(1) << shift_)), result_ + shift_>
    {};
    
    template<typename int_, int_ value_, unsigned result_>
    struct bitsof_int<int_, value_, result_, 0> {
        static unsigned constexpr result = result_;
    };
    
    template<typename a_, int = !is_int<a_> ? 0 : is_unsigned<a_> ? 1 : -1>
    struct max_min_int {
        static constexpr unsigned (max)() { return 0; }
        static constexpr unsigned (min)() { return 0; }
    };
    
    template<typename a_>
    struct max_min_int<a_, 1> {
        static constexpr a_ (max)() { return static_cast<a_>(-1); }
        static constexpr a_ (min)() { return 0; }
    };
    
    template<>
    struct max_min_int<char, -1> {
        static constexpr char (max)() { return CHAR_MAX; }
        static constexpr char (min)() { return CHAR_MIN; }
    };
    
    template<>
    struct max_min_int<signed char, -1> {
        static constexpr signed char (max)() { return SCHAR_MAX; }
        static constexpr signed char (min)() { return SCHAR_MIN; }
    };
    
    template<>
    struct max_min_int<short, -1> {
        static constexpr short (max)() { return SHRT_MAX; }
        static constexpr short (min)() { return SHRT_MIN; }
    };
    
    template<>
    struct max_min_int<int, -1> {
        static constexpr int (max)() { return INT_MAX; }
        static constexpr int (min)() { return INT_MIN; }
    };
    
    template<>
    struct max_min_int<long, -1> {
        static constexpr long (max)() { return LONG_MAX; }
        static constexpr long (min)() { return LONG_MIN; }
    };
    
    template<>
    struct max_min_int<long long, -1> {
        static constexpr long long (max)() { return LLONG_MAX; }
        static constexpr long long (min)() { return LLONG_MIN; }
    };

    // there is no unsigned wchar_t. use another type if signed, there is another integer type with the same number of bits.
    // do not use WCHAR_MAX from wchar.h because it does not have to be maximum value of the integer type, only maximum value of character in some encoding?
    // also on mac os x before 10.3 WCHAR_MAX did not exist
    // int_min and int_max in int.hpp depends on this
    template<>
    struct max_min_int<wchar_t, -1> {
        using same =
            ifel<sizeof(wchar_t) == sizeof(int), int,
            ifel<sizeof(wchar_t) == sizeof(signed char), signed char,
            ifel<sizeof(wchar_t) == sizeof(short), short,
            ifel<sizeof(wchar_t) == sizeof(long), long,
            ifel<sizeof(wchar_t) == sizeof(long long), long long,
            void
            >>>>>;
        static constexpr wchar_t (max)() { return static_cast<wchar_t>((max_min_int<same>::max)()); }
        static constexpr wchar_t (min)() { return static_cast<wchar_t>((max_min_int<same>::min)()); }
    };

    template<typename a_, bool = is_int<a_>>
    struct digits {
        static int constexpr result =
            equal<float, a_> ? static_cast<int>(FLT_MANT_DIG) :
            equal<double, a_> ? static_cast<int>(DBL_MANT_DIG) :
            equal<long double, a_> ? static_cast<int>(LDBL_MANT_DIG) :
            0;
    };
    
    template<>
    struct digits<bool, is_int<bool>> {
        static int constexpr result = 1;
    };
    
    template<typename a_>
    struct digits<a_, true> : bitsof_int<a_, (max_min_int<a_>::max)()>
    {};
    
    template<typename a_>
    struct digits10 {
        static int constexpr result =
            equal<float, a_> ? static_cast<int>(FLT_DIG) :
            equal<double, a_> ? static_cast<int>(DBL_DIG) :
            equal<long double, a_> ? static_cast<int>(LDBL_DIG) :
            static_cast<int>((digits<a_>::result * 301l) / 1000l); // log(2) / log(10) = 0.301029995
    };

    template<typename a_>
    struct max_exponent {
        static int constexpr result =
            equal<float, a_> ? static_cast<int>(FLT_MAX_EXP) :
            equal<double, a_> ? static_cast<int>(DBL_MAX_EXP) :
            equal<long double, a_> ? static_cast<int>(LDBL_MAX_EXP) :
            0;
    };

    template<typename a_>
    struct min_exponent {
        static int constexpr result =
            equal<float, a_> ? static_cast<int>(FLT_MIN_EXP) :
            equal<double, a_> ? static_cast<int>(DBL_MIN_EXP) :
            equal<long double, a_> ? static_cast<int>(LDBL_MIN_EXP) :
            0;
    };

    template<typename a_>
    struct max_exponent10 {
        static int constexpr result =
            equal<float, a_> ? static_cast<int>(FLT_MAX_10_EXP) :
            equal<double, a_> ? static_cast<int>(DBL_MAX_10_EXP) :
            equal<long double, a_> ? static_cast<int>(LDBL_MAX_10_EXP) :
            0;
    };

    template<typename a_>
    struct min_exponent10 {
        static int constexpr result =
            equal<float, a_> ? static_cast<int>(FLT_MIN_10_EXP) :
            equal<double, a_> ? static_cast<int>(DBL_MIN_10_EXP) :
            equal<long double, a_> ? static_cast<int>(LDBL_MIN_10_EXP) :
            0;
    };
        
    template<typename a_>
    struct radix {
        static int constexpr result =
            is_float<a_> ? static_cast<int>(FLT_RADIX) :
            (is_int<a_> || equal<bool, a_>) ? 2 :
            0;
    };
    
    template<typename a_, bool is_int_ = is_int<a_>>
    struct max_min {
        static constexpr a_ (max)() { return a_{}; }
        static constexpr a_ (min)() { return a_{}; }
        static constexpr a_ lowest() { return a_{}; }
    };
    
    template<typename a_>
    struct max_min<a_, true> : max_min_int<a_> {
        static constexpr a_ lowest() noexcept { return (max_min_int<a_>::min)(); }
    };
    
    template<>
    struct max_min<bool, is_int<bool>> {
        static constexpr bool (max)() noexcept { return true; }
        static constexpr bool (min)() noexcept { return false; }
        static constexpr bool lowest() noexcept { return false; }
    };
    
    template<>
    struct max_min<float, false> {
        static constexpr float (max)() noexcept { return FLT_MAX; }
        static constexpr float (min)() noexcept { return FLT_MIN; }
        static constexpr float lowest() noexcept { return -FLT_MAX; }
    };
    
    template<>
    struct max_min<double, false> {
        static constexpr double (max)() noexcept { return DBL_MAX; }
        static constexpr double (min)() noexcept { return DBL_MIN; }
        static constexpr double lowest() noexcept { return -DBL_MAX; }
    };
    
    template<>
    struct max_min<long double, false> {
        static constexpr long double (max)() noexcept { return LDBL_MAX; }
        static constexpr long double (min)() noexcept { return LDBL_MIN; }
        static constexpr long double lowest() noexcept { return -LDBL_MAX; }
    };
    
    template<>
    struct max_min<void, false> {
        static void (max)() noexcept {}
        static void (min)() noexcept {}
        static void lowest() noexcept {}
    };

    // denorm_min = min / pow(radix, digits - 1)  or pow(radix, min_exponent - digits)
    
    template<typename float_, unsigned digits_ = digits<float_>::result, unsigned bits_ = bitsof_int<unsigned long long, static_cast<unsigned long long>(-1)>::result, bool = (digits_ <= bits_)>
    struct denorm_min_divide {
        static_assert(radix<float_>::result == 2, "float raidx is not 2?");
        static constexpr unsigned long long do_it() noexcept { return 1ull << (digits_ - 1); }
    };
    
    template<typename float_, unsigned digits_, unsigned bits_>
    struct denorm_min_divide<float_, digits_, bits_, false> {
        static constexpr float_ do_it() noexcept { return static_cast<float_>(1ull << (bits_ - 1)) * denorm_min_divide<float_, digits_ - (bits_ - 1)>::do_it(); }
    };

    template<typename a_> constexpr a_          denorm_min()              noexcept { return a_{}; }
    template<>            constexpr float       denorm_min<float>()       noexcept { return FLT_MIN / denorm_min_divide<float, FLT_MANT_DIG>::do_it(); }
    template<>            constexpr double      denorm_min<double>()      noexcept { return DBL_MIN / denorm_min_divide<double, DBL_MANT_DIG>::do_it(); }
    template<>            constexpr long double denorm_min<long double>() noexcept { return LDBL_MIN / denorm_min_divide<long double, LDBL_MANT_DIG>::do_it(); }
    
    template<typename a_> constexpr a_          epsilon()              noexcept { return a_{}; }
    template<>            constexpr float       epsilon<float>()       noexcept { return FLT_EPSILON; }
    template<>            constexpr double      epsilon<double>()      noexcept { return DBL_EPSILON; }
    template<>            constexpr long double epsilon<long double>() noexcept { return LDBL_EPSILON; }

    template<typename a_>
    constexpr ifel<is_float<a_>, a_>  infinity() noexcept { return static_cast<a_>(INFINITY); }
    template<typename a_>
    constexpr ifel<!is_float<a_>, a_> infinity() noexcept { return a_{}; }
    
    template<typename a_>
    constexpr ifel<is_float<a_>, a_>  nan() noexcept { return static_cast<a_>(NAN); }
    template<typename a_>
    constexpr ifel<!is_float<a_>, a_> nan() noexcept { return a_{}; }

    // FLT_ROUNDS is not a compile time constant
    template<typename a_>
    constexpr ifel<is_float<a_>, a_>  round_error() noexcept { return static_cast<a_>(0.5); }
    template<typename a_>
    constexpr ifel<!is_float<a_>, a_> round_error() noexcept { return a_{}; }

}}

enum no_float_round_style {
    round_indeterminate = -1,
    round_toward_zero = 0,
    round_to_nearest = 1,
    round_toward_infinity = 2,
    round_toward_neg_infinity = 3
};

enum no_float_denorm_style {
    denorm_indeterminate = -1,
    denorm_absent = 0,
    denorm_present = 1
};

template<typename a_>
struct no_numeric_limits
{
    static bool constexpr
        is_specialized = is_int<a_> || is_float<a_> || equal<bool, a_>,
        is_bounded = is_specialized,
        is_iec559 = is_float<a_>,
        is_integer = is_specialized && !is_iec559,
        is_signed = is_float<a_> || (is_int<a_> && !is_unsigned<a_>),
        is_modulo = is_unsigned<a_> || is_twos_complement<a_>(),
        is_exact = is_integer,
        has_denorm_loss = false,
        has_infinity = is_iec559,
        has_quiet_NaN = is_iec559,
        has_signaling_NaN = is_iec559,
        tinyness_before = false,
        traps = is_specialized && !equal<bool, a_>; // division by 0 is integer trap, iec559 signaling nan is trap
    static int constexpr
        digits = _::no_numeric_limits::digits<a_>::result,
        digits10 = _::no_numeric_limits::digits10<a_>::result,
        max_digits10 = is_float<a_> ? 2 + static_cast<int>((digits * 301l) / 1000l) : 0,
        max_exponent = _::no_numeric_limits::max_exponent<a_>::result,
        min_exponent = _::no_numeric_limits::min_exponent<a_>::result,
        max_exponent10 = _::no_numeric_limits::max_exponent10<a_>::result,
        min_exponent10 = _::no_numeric_limits::min_exponent10<a_>::result,
        radix = digits ? 2 : 0;
    static no_float_denorm_style constexpr has_denorm = is_iec559 ? denorm_present : denorm_absent;
    static no_float_round_style constexpr round_style = is_iec559 ? round_to_nearest : round_toward_zero;
    static constexpr a_ denorm_min()    noexcept { return _::no_numeric_limits::denorm_min<a_>(); }
    static constexpr a_ epsilon()       noexcept { return _::no_numeric_limits::epsilon<a_>(); }
    static constexpr a_ infinity()      noexcept { return _::no_numeric_limits::infinity<a_>(); }
    static constexpr a_ (max)()         noexcept { return (_::no_numeric_limits::max_min<a_>::max)(); }
    static constexpr a_ (min)()         noexcept { return (_::no_numeric_limits::max_min<a_>::min)(); }
    static constexpr a_ lowest()        noexcept { return _::no_numeric_limits::max_min<a_>::lowest(); }
    static constexpr a_ quiet_NaN()     noexcept { return _::no_numeric_limits::nan<a_>(); }
    static constexpr a_ round_error()   noexcept { return _::no_numeric_limits::round_error<a_>(); }
    static constexpr a_ signaling_NaN() noexcept { return _::no_numeric_limits::nan<a_>(); }
};
template<typename a_> bool constexpr no_numeric_limits<a_>::is_bounded;
template<typename a_> bool constexpr no_numeric_limits<a_>::is_exact;
template<typename a_> bool constexpr no_numeric_limits<a_>::is_iec559;
template<typename a_> bool constexpr no_numeric_limits<a_>::is_integer;
template<typename a_> bool constexpr no_numeric_limits<a_>::is_modulo;
template<typename a_> bool constexpr no_numeric_limits<a_>::is_signed;
template<typename a_> bool constexpr no_numeric_limits<a_>::is_specialized;
template<typename a_> bool constexpr no_numeric_limits<a_>::has_denorm_loss;
template<typename a_> bool constexpr no_numeric_limits<a_>::has_infinity;
template<typename a_> bool constexpr no_numeric_limits<a_>::has_quiet_NaN;
template<typename a_> bool constexpr no_numeric_limits<a_>::has_signaling_NaN;
template<typename a_> bool constexpr no_numeric_limits<a_>::tinyness_before;
template<typename a_> bool constexpr no_numeric_limits<a_>::traps;
template<typename a_> int constexpr no_numeric_limits<a_>::digits;
template<typename a_> int constexpr no_numeric_limits<a_>::digits10;
template<typename a_> int constexpr no_numeric_limits<a_>::max_digits10;
template<typename a_> int constexpr no_numeric_limits<a_>::max_exponent;
template<typename a_> int constexpr no_numeric_limits<a_>::min_exponent;
template<typename a_> int constexpr no_numeric_limits<a_>::max_exponent10;
template<typename a_> int constexpr no_numeric_limits<a_>::min_exponent10;
template<typename a_> int constexpr no_numeric_limits<a_>::radix;
template<typename a_> no_float_denorm_style constexpr no_numeric_limits<a_>::has_denorm;
template<typename a_> no_float_round_style constexpr no_numeric_limits<a_>::round_style;


template<typename a_> struct no_numeric_limits<a_ const> : no_numeric_limits<a_> {};
template<typename a_> struct no_numeric_limits<a_ volatile> : no_numeric_limits<a_> {};
template<typename a_> struct no_numeric_limits<a_ const volatile> : no_numeric_limits<a_> {};

}
#endif
