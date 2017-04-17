// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_NUMERIC_LIMITS_HPP
#define WATER_NUMERIC_LIMITS_HPP
#ifdef WATER_NO_STD

#include <water/no_numeric_limits.hpp>
namespace water {
template<typename a_> class numeric_limits : public no_numeric_limits<a_> {};
using float_round_style = no_float_round_style;
using float_denorm_style = no_float_denorm_style;
}

#else

#include <limits>
namespace water {
using ::std::numeric_limits;
using ::std::float_denorm_style;
using ::std::float_denorm_style;
}

#endif
#endif
