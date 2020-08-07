// Copyright 2017-2018 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_CMATH_HPP
#define WATER_CMATH_HPP
#include <water/water.hpp>
#ifndef WATER_NO_CHEADERS

#include <cmath>
namespace water {
    
using ::std::fmod;
using ::std::frexp;
using ::std::log;
using ::std::log10;
using ::std::pow;

using ::std::isfinite;
using ::std::isinf;
using ::std::isnan;
using ::std::isnormal;
using ::std::signbit;
using ::std::fpclassify;

// isnormal = not 0, subnormal, infinity or nan. it may not work for subnormals when building without strict IEEE compliance, like -ffast-math
// isfinite = not nan or infinity

}

#else

#include <math.h>
namespace water {

// this is not needed if the math.h header is made for c++

inline float       fmod(float a, float b)             { return ::fmodf(a, b); }
inline double      fmod(double a, double b)           { return ::fmod(a, b); }
inline long double fmod(long double a, long double b) { return ::fmodl(a, b); }

inline float       frexp(float a, int* b)       { return ::frexpf(a, b); }
inline double      frexp(double a, int* b)      { return ::frexp(a, b); }
inline long double frexp(long double a, int* b) { return ::frexpl(a, b); }

inline float       log(float a)       { return ::logf(a); }
inline double      log(double a)      { return ::log(a); }
inline long double log(long double a) { return ::logl(a); }

inline float       log10(float a)       { return ::log10f(a); }
inline double      log10(double a)      { return ::log10(a); }
inline long double log10(long double a) { return ::log10l(a); }

inline float       pow(float a, float b)             { return ::powf(a, b); }
inline double      pow(double a, double b)           { return ::pow(a, b); }
inline long double pow(long double a, long double b) { return ::powl(a, b); }
    
}

#endif

#if !defined(WATER_CMATH_NO_SLOW_MATH) && (((defined(WATER_COMPILER_GCC) || defined(WATER_COMPILER_CLANG)) && defined(__FAST_MATH__)) || defined(WATER_CMATH_SLOW_MATH))

// On GCC and Clang with -ffast-math (enabled with -Ofast) isinf + isnan is optimized away to always return false.
// Sometimes fpclassify seems to work, not sure if it depends on the C library or GCC/Clang version.
// slow_math below should detect NaN and infinity for all types as long as one of them is a IEEE 754 / IEC 60559 32 bit or 64 bit type.
// The motivation for this is to be able to build with -ffast-math but still be able to sanitize floating point values

#include <water/int.hpp>
#include <water/numeric_limits.hpp>
#include <water/types/types.hpp>
namespace water {

namespace _ {

    template<
        typename float_,
        unsigned bits_ = sizeof(float_) * numeric_limits<unsigned char>::digits,
        typename unsigned_ = uint_bits<bits_>,
        bool = (bits_ == 32 || bits_ == 64) && !types::is_void<unsigned_>::result && numeric_limits<float_>::is_iec559
    >
    struct slow_math
    {
        static bool constexpr can = true;
        bool
            finite = true,
            normal = true,
            infinity = false,
            nan = false;
        
        slow_math(float_ f) {
            // 32 bit
            // exponent 8 bits, 128 = 0, range -126 - 127
            // fraction 23 bits
            //
            // 64 bit
            // exponent 11 bits, 1023 = 0, range -1022 - 1023
            // fraction 52 bits
            unsigned constexpr
                exponent_bits = bits_ == 32 ? 8 : 11,
                fraction_bits = bits_ == 32 ? 23 : 52;
            unsigned_ constexpr exponent_max = (1 << exponent_bits) - 1;
            unsigned_ u = 0;
            auto ub = static_cast<unsigned char*>(static_cast<void*>(&u));
            auto fb = static_cast<unsigned char const*>(static_cast<void const*>(&f)), fe = fb + sizeof(f);
            do *ub++ = *fb++; while(fb != fe);
            unsigned_ exponent = (u >> fraction_bits) & exponent_max;
            if(!exponent) // subnormal or 0
                normal = false;
            else if(exponent == exponent_max) { // infinity or nan
                if(u & ((static_cast<unsigned_>(1) << fraction_bits) - 1)) // fraction not 0 == nan
                    nan = true;
                else
                    infinity = true;
                finite = normal = false;
            }
        }
    };
    
    template<typename float_, unsigned bits_, typename unsigned_>
    struct slow_math<float_, bits_, unsigned_, false>
    {
        static bool constexpr can = false;
        bool
            finite = true,
            normal = true,
            infinity = false,
            nan = false;
        
        slow_math(float_ f) {
            auto c = fpclassify(f);
            if(c == FP_NAN)
                nan = true;
            else if(c == FP_INFINITE)
                infinity = true;
            else if(other(static_cast<double>(f), f) || other(static_cast<float>(f), f) || other(static_cast<long double>(f), f))
                ;
            else if(f < -numeric_limits<float_>::max() || numeric_limits<float_>::max() < f) // will this be optimized away?
                infinity = true;
            else if(f != f) // will this be optimized away?
                nan = true;
            if(nan || infinity)
                normal = finite = false;
            else if(c == FP_ZERO || c == FP_SUBNORMAL || (-numeric_limits<float_>::min() < f && f < numeric_limits<float_>::min()))
                normal = false;
        }

    private:
        bool other(float_, float_) {
            return false;
        }

        template<typename other_>
        bool other(other_ o, float_ f) {
            if(!slow_math<other_>::can)
                return false;
            auto s = slow_math<other_>{o};
            if(s.nan)
                nan = true;
            if(s.infinity && static_cast<float_>(o) == f)
                infinity = true;
            return true;
        }
    };

}

template<typename float_>
bool isfinite_strict(float_ a) {
    return _::slow_math<float_>{a}.finite;
}

template<typename float_>
bool isinf_strict(float_ a) {
    return _::slow_math<float_>{a}.infinity;
}

template<typename float_>
bool isnan_strict(float_ a) {
    return _::slow_math<float_>{a}.nan;
}

template<typename float_>
bool isnormal_strict(float_ a) {
    return _::slow_math<float_>{a}.normal;
}

}

#else

namespace water {

// fpclassify seems less likley to be optimized away

template<typename float_>
bool isfinite_strict(float_ a) {
    auto c = fpclassify(a);
    return c != FP_INFINITE && c != FP_NAN;
}

template<typename float_>
bool isinf_strict(float_ a) {
    return fpclassify(a) == FP_INFINITE;
}

template<typename float_>
bool isnan_strict(float_ a) {
    return fpclassify(a) == FP_NAN;
}

template<typename float_>
bool isnormal_strict(float_ a) {
    auto c = fpclassify(a);
    return c != FP_INFINITE && c != FP_NAN && c != FP_ZERO && c != FP_SUBNORMAL;
}

}
#endif

#endif
