// Copyright 2017 Johan Paulsson
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
#endif
