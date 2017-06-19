// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_TESTS_NO_NUMERIC_LIMITS_VS_STD_HPP
#define WATER_TESTS_NO_NUMERIC_LIMITS_VS_STD_HPP
#include <water/test.hpp>
#include <water/numeric_limits.hpp>
#include <water/no_numeric_limits.hpp>
#include <water/cmath.hpp> // isnan
namespace water { namespace tests {

/*

Compare no_numeric_limits to water::numeric_limits

water::numeric_limits is usually std::numeric_limits but it can also be no_numeric_limits and this test is pointless

is_modulo and traps is skipped because the llvm c++ library seems to have this wrong?

*/

template<typename type_> typename types::ifel_type<types::is_float<type_>, bool>::result no_numeric_limits_vs_std_nan(type_ mine, type_ std) {
	return isnan_strict(mine) == isnan_strict(std);
	}
template<typename type_> typename types::ifel_type_not<types::is_float<type_>, bool>::result no_numeric_limits_vs_std_nan(type_ mine, type_ std) {
	return mine == std;
	}

template<typename type_> void no_numeric_limits_vs_std_type() {
	using mine = no_numeric_limits<type_>;
	using std = numeric_limits<type_>;
	static_assert(mine::is_specialized == std::is_specialized, "is_specialized");
	static_assert(mine::is_bounded == std::is_bounded, "is_bounded");
	static_assert(mine::is_iec559 == std::is_iec559, "is_iec559");
	static_assert(mine::is_integer == std::is_integer, "is_integer");
	static_assert(mine::is_signed == std::is_signed, "is_signed");
	//static_assert(mine::is_modulo == std::is_modulo, "is_modulo");
	static_assert(mine::is_exact == std::is_exact, "is_exact");
	//static_assert(mine::has_denorm_loss == std::has_denorm_loss, "has_denorm_loss");
	static_assert(mine::has_infinity == std::has_infinity, "has_infinity");
	static_assert(mine::has_quiet_NaN == std::has_quiet_NaN, "has_quiet_NaN");
	static_assert(mine::has_signaling_NaN == std::has_signaling_NaN, "has_signaling_NaN");
	//static_assert(mine::tinyness_before == std::tinyness_before, "tinyness_before");
	//static_assert(mine::traps == std::traps, "traps");
	static_assert(mine::digits == std::digits, "digits");
	static_assert(mine::digits10 == std::digits10, "digits10");
	static_assert(mine::max_digits10 == std::max_digits10, "max_digits10");
	static_assert(mine::max_exponent == std::max_exponent, "max_exponent");
	static_assert(mine::min_exponent == std::min_exponent, "min_exponent");
	static_assert(mine::max_exponent10 == std::max_exponent10, "max_exponent10");
	static_assert(mine::min_exponent10 == std::min_exponent10, "min_exponent10");
	static_assert(mine::radix == std::radix, "radix");
	static_assert(static_cast<int>(mine::has_denorm) == static_cast<int>(std::has_denorm), "has_denorm");
	static_assert(static_cast<int>(mine::round_style) == static_cast<int>(std::round_style), "round_style");
	type_
		mine_denorm_min = mine::denorm_min(),
		std_denorm_min = std::denorm_min(),
		mine_epsilon = mine::epsilon(),
		std_epsilon = std::epsilon(),
		mine_infinity = mine::infinity(),
		std_infinity = std::infinity(),
		mine_max = mine::max(),
		std_max = std::max(),
		mine_min = mine::min(),
		std_min = std::min(),
		mine_lowest = mine::lowest(),
		std_lowest = std::lowest(),
		mine_round_error = mine::round_error(),
		std_round_error = std::round_error(),
		mine_quiet_NaN = mine::quiet_NaN(),
		std_quiet_NaN = std::quiet_NaN(),
		mine_signaling_NaN = mine::signaling_NaN(),
		std_signaling_NaN = std::signaling_NaN();
	___water_test(mine_denorm_min == std_denorm_min);
	___water_test(mine_epsilon == std_epsilon);
	___water_test(mine_infinity == std_infinity);
	___water_test(mine_max == std_max);
	___water_test(mine_min == std_min);
	___water_test(mine_lowest == std_lowest);
	___water_test(mine_round_error == std_round_error);
	___water_test(no_numeric_limits_vs_std_nan<type_>(mine_quiet_NaN, std_quiet_NaN));
	___water_test(no_numeric_limits_vs_std_nan<type_>(mine_signaling_NaN, std_signaling_NaN));
	}

struct no_numeric_limits_vs_std_t {
	bool operator==(no_numeric_limits_vs_std_t) const { return true; }
	};

inline void no_numeric_limits_vs_std() {
	no_numeric_limits_vs_std_type<void*>();
	no_numeric_limits_vs_std_type<bool>();
	no_numeric_limits_vs_std_type<char>();
	no_numeric_limits_vs_std_type<signed char>();
	no_numeric_limits_vs_std_type<unsigned char>();
	no_numeric_limits_vs_std_type<short>();
	no_numeric_limits_vs_std_type<unsigned short>();
	no_numeric_limits_vs_std_type<int>();
	no_numeric_limits_vs_std_type<unsigned int>();
	no_numeric_limits_vs_std_type<long>();
	no_numeric_limits_vs_std_type<unsigned long>();
	no_numeric_limits_vs_std_type<long long>();
	no_numeric_limits_vs_std_type<unsigned long long>();
	no_numeric_limits_vs_std_type<float>();
	no_numeric_limits_vs_std_type<double>();
	no_numeric_limits_vs_std_type<long double>();
	no_numeric_limits_vs_std_type<char16_t>();
	no_numeric_limits_vs_std_type<char32_t>();
	no_numeric_limits_vs_std_type<wchar_t>();
	no_numeric_limits_vs_std_type<no_numeric_limits_vs_std_t>();
	}

}}
#endif
