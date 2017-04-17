// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_NUMBERS_TESTS_WRITE_READ_HPP
#define WATER_NUMBERS_TESTS_WRITE_READ_HPP
#include <water/numbers/tests/bits.hpp>
namespace water { namespace numbers { namespace tests {

/*

write number
read number
compare read number with original

*/

template<typename float_> bool write_read_almost_equal(float_ a, float_ b) {
	// on os x the accuracy of long double seems to be less than what it could be, and pow() seems to be the problem?
	// using double epsilon instead
	using limit_type = typename types::ifel_type<types::equal<long double, float_>, double, float_>::result;
	float_ const e = static_cast<float_>(1) + numeric_limits<limit_type>::epsilon() * static_cast<float_>(1.5);
	bool r = a < 0 ?
		(a / e >= b && b >= a * e) || (b / e >= a && a >= b * e) :
		(a / e <= b && b <= a * e) || (b / e <= a && a <= b * e);
	//___water_assert(r);
	return r;
	}

inline void write_read_0(char *x, char *xe) {
	while(xe != x) *--xe = 0;
	}

template<typename float_> void write_read_float(settings s) {
	//trace() << "\nfloat " << sizeof(float_);
	char x[10 * 1024] = {};
	float_ const floats[] = {
		0,
		1,
		-1,
		10,
		-10,
		static_cast<float_>(-1.23456789012345678901234567890e-50l),
		static_cast<float_>(+1.23456789012345678901234567890e-40l),
		static_cast<float_>(-1.23456789012345678901234567890e-30l),
		static_cast<float_>(+1.23456789012345678901234567890e-20l),
		static_cast<float_>(-1.23456789012345678901234567890e-10l),
		static_cast<float_>(+1.23456789012345678901234567890e-9l),
		static_cast<float_>(-1.23456789012345678901234567890e-8l),
		static_cast<float_>(+1.23456789012345678901234567890e-7l),
		static_cast<float_>(-1.23456789012345678901234567890e-6l),
		static_cast<float_>(+1.23456789012345678901234567890e-5l),
		static_cast<float_>(-1.23456789012345678901234567890e-4l),
		static_cast<float_>(+1.23456789012345678901234567890e-3l),
		static_cast<float_>(-1.23456789012345678901234567890e-2l),
		static_cast<float_>(+1.23456789012345678901234567890e-1l),
		static_cast<float_>(-1.23456789012345678901234567890e0l),
		static_cast<float_>(+1.23456789012345678901234567890e1l),
		static_cast<float_>(-1.23456789012345678901234567890e2l),
		static_cast<float_>(+1.23456789012345678901234567890e3l),
		static_cast<float_>(-1.23456789012345678901234567890e4l),
		static_cast<float_>(+1.23456789012345678901234567890e5l),
		static_cast<float_>(-1.23456789012345678901234567890e6l),
		static_cast<float_>(+1.23456789012345678901234567890e7l),
		static_cast<float_>(-1.23456789012345678901234567890e8l),
		static_cast<float_>(+1.23456789012345678901234567890e9l),
		static_cast<float_>(-1.23456789012345678901234567890e10l), 
		static_cast<float_>(+1.23456789012345678901234567890e20l),
		static_cast<float_>(-1.23456789012345678901234567890e30l),
		static_cast<float_>(+1.23456789012345678901234567890e40l),
		static_cast<float_>(-1.23456789012345678901234567890e50l),
		static_cast<float_>(-1.010101010101010101010101010101010101010101010101010101010101010l),
		static_cast<float_>(-1.010101010101010101010101010101010101010101010101010101010101010e-20l),
		static_cast<float_>(9.09090909090909090909090909090909090909090909090909090909l),
		static_cast<float_>(9.09090909090909090909090909090909090909090909090909090909e20l),
		static_cast<float_>(3.141592653589793238462643383279502884197169399375l),
		numeric_limits<float_>::min(),
		numeric_limits<float_>::max(),
		numeric_limits<float_>::infinity(),
		numeric_limits<float_>::quiet_NaN(),
		numeric_limits<float_>::epsilon(),
		numeric_limits<float_>::denorm_min(),
		-numeric_limits<float_>::min(),
		-numeric_limits<float_>::max(),
		-numeric_limits<float_>::infinity(),
		-numeric_limits<float_>::quiet_NaN(),
		-numeric_limits<float_>::epsilon(),
		-numeric_limits<float_>::denorm_min()
		};
	for(float_ f : floats) {
		s.precision(max_digits<float_>(s.base() ? s.base() : 10) + 1);
		write<float_> w(f, s);
		size_t ws = w.template size<char>();
		char *xe = w(x, sizeof(x) - 1);
		___water_test(!*xe);
		//trace() << x;
		size_t xs = static_cast<size_t>(xe - x);
		read<float_> r(s);
		char const* re = r(static_cast<char const*>(x), static_cast<char const*>(xe));
		size_t ru = r.used();
		size_t rs = static_cast<size_t>(re - x);
		float_ rf = r;
		___water_test(ws == xs && "write size same as acutally written");
		___water_test(ws && "wrote something");
		___water_test(ru == rs && "read used size same as acutally read");
		___water_test(rs == ws && "read all written");
		if(isinf(f))
			___water_test(r.infinity() && "written and parsed close to original");
		else if(isnan(f))
			___water_test(r.nan() && "written and parsed close to original");
		else
			___water_test(write_read_almost_equal(f, rf) && "written and parsed close to original");
		write_read_0(x, xe);
		}
	}

template<typename int_> void write_read_signed(settings s) {
	//trace() << "\nsigned " << sizeof(int_);
	char x[10 * 1024] = {};
	int_ const ints[] = {
		0,
		1,
		-1,
		10,
		-10,
		static_cast<int_>(0xcafe),
		static_cast<int_>(-0xdead),
		numeric_limits<int_>::min(),
		numeric_limits<int_>::max(),
		numeric_limits<int_>::min() + 1,
		numeric_limits<int_>::max() - 1,
		numeric_limits<int_>::min() / 2,
		numeric_limits<int_>::max() / 2,
		numeric_limits<int_>::min() / 3,
		numeric_limits<int_>::max() / 3
		};
	for(int_ i : ints) {
		write<int_> w(i, s);
		size_t ws = w.template size<char>();
		char *xe = w(x, sizeof(x) - 1);
		___water_test(!*xe);
		*xe = 0;
		//trace() << x;
		size_t xs = static_cast<size_t>(xe - x);
		read<int_> r(s);
		char const* re = r(static_cast<char const*>(x), static_cast<char const*>(xe));
		size_t ru = r.used();
		size_t rs = static_cast<size_t>(re - x);
		int_ ri = r;
		___water_test(ws == xs && "write size same as acutally written");
		___water_test(ws && "wrote something");
		___water_test(ru == rs && "read used size same as acutally read");
		___water_test(rs == ws && "read all written");
		___water_test(i == ri && "written and parsed close to original");
		write_read_0(x, xe);
		}
	}

template<typename int_> void write_read_unsigned(settings s) {
	//trace() << "\nunsigned " << sizeof(int_);
	char x[10 * 1024] = {};
	int_ const ints[] = {
		0,
		1,
		10,
		static_cast<int_>(0xcafe),
		static_cast<int_>(0xdead),
		numeric_limits<int_>::min(),
		numeric_limits<int_>::max(),
		static_cast<int_>(numeric_limits<int_>::max() - 1),
		static_cast<int_>(numeric_limits<int_>::max() / 2),
		static_cast<int_>(numeric_limits<int_>::max() / 3)
		};
	for(int_ i : ints) {
		write<int_> w(i, s);
		size_t ws = w.template size<char>();
		char *xe = w(x, sizeof(x) -1);
		___water_test(!*xe);
		*xe = 0;
		//trace() << x;
		size_t xs = static_cast<size_t>(xe - x);
		read<int_> r(s);
		char const* re = r(static_cast<char const*>(x), static_cast<char const*>(xe));
		size_t ru = r.used();
		size_t rs = static_cast<size_t>(re - x);
		int_ ri = r;
		___water_test(ws == xs && "write size same as acutally written");
		___water_test(ws && "wrote something");
		___water_test(ru == rs && "read used size same as acutally read");
		___water_test(rs == ws && "read all written");
		___water_test(i == ri && "written and parsed close to original");
		write_read_0(x, xe);
		}
	}

inline void write_read_bool(settings s) {
	//trace() << "\nbool " << sizeof(bool);
	char x[10 * 1024] = {};
	bool const bools[] = {
		false,
		true
		};
	for(bool i : bools) {
		write<bool> w(i, s);
		size_t ws = w.template size<char>();
		char *xe = w(x, sizeof(x) - 1);
		___water_test(!*xe);
		*xe = 0;
		//trace() << x;
		size_t xs = static_cast<size_t>(xe - x);
		read<bool> r(s);
		char const* re = r(static_cast<char const*>(x), static_cast<char const*>(xe));
		size_t ru = r.used();
		size_t rs = static_cast<size_t>(re - x);
		bool ri = r;
		___water_test(ws == xs && "write size same as acutally written");
		___water_test(ws && "wrote something");
		___water_test(ru == rs && "read used size same as acutally read");
		___water_test(rs == ws && "read all written");
		___water_test(i == ri && "written and parsed close to original");
		write_read_0(x, xe);
		}
	}

inline void write_read_all() {
	settings settings_float[] {
		settings{},
		settings{}.base(2),
		settings{}.base(8).plus(true),
		settings{}.base(10),
		settings{}.base(16).plus(true),
		settings{}.group(true).space(true),
		settings{}.base(2).group(true).space(true).plus(true),
		settings{}.base(8).group(true).space(true),
		settings{}.base(10).group(true).space(true).plus(true),
		settings{}.base(16).group(true).space(true),
		settings{}.base(10).no_exponent().trailing_zeros(true).digits(255),
		settings{}.base(10).scientific(true).exponent(),
		settings{}.base(10).scientific(false).exponent(),
		settings{}.base(10).scientific(true).exponent().group(true).space(true).plus(true),
		settings{}.base(10).scientific(false).exponent().group(true).space(true).plus(true),
		settings{}.base(2).base_prefix_suffix(false),
		settings{}.base(8).base_prefix_suffix(false),
		settings{}.base(16).base_prefix_suffix(false)
		};
	for(auto s : settings_float) {
		write_read_float<float>(s);
		write_read_float<double>(s);
		if(test_long_double)
			write_read_float<long double>(s);
		}
	
	settings settings_int[] {
		settings{},
		settings{}.bool_text(true),
		settings{}.digits(255).plus(true),
		settings{}.digits(255).plus(true).group(true),
		settings{}.base(2).base_prefix_suffix(false),
		settings{}.base(8).base_prefix_suffix(false),
		settings{}.base(16).base_prefix_suffix(false),
		settings{}.base(2).base_prefix_suffix(false).group(true).space(true),
		settings{}.base(8).base_prefix_suffix(false).group(true).space(true),
		settings{}.base(16).base_prefix_suffix(false).group(true).space(true),
		settings{}.base(2).digits(255).plus(true),
		settings{}.base(2).digits(255).plus(true).group(true),
		settings{}.base(8).digits(255).plus(true),
		settings{}.base(8).digits(255).plus(true).group(true),
		settings{}.base(16).digits(255).plus(true),
		settings{}.base(16).digits(255).plus(true).group(true)
		};
	for(auto s : settings_int) {
		write_read_signed<signed char>(s);
		write_read_signed<signed short>(s);
		write_read_signed<signed int>(s);
		write_read_signed<signed long>(s);
		write_read_signed<signed long long>(s);
	
		write_read_unsigned<unsigned char>(s);
		write_read_unsigned<unsigned short>(s);
		write_read_unsigned<unsigned int>(s);
		write_read_unsigned<unsigned long>(s);
		write_read_unsigned<unsigned long long>(s);
	
		write_read_bool(s);
		}
	}

}}}
#endif
