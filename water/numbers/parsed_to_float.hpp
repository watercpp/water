// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_NUMBERS_PARSED_TO_FLOAT_HPP
#define WATER_NUMBERS_PARSED_TO_FLOAT_HPP
#include <water/numbers/parsed_to_int.hpp>
#include <water/numbers/max_digits.hpp>
namespace water { namespace numbers {

/*

convert parsed mantissa + exponent to float

numeric_limits lowest() is better than -max() ?

*/

template<typename float_> class
 parsed_to_float {
	float_ my = 0;
	bool
		myerror = false,
		myinexact = false,
		myoverflow = false,
		myinfinity = false,
		mynan = false;
	public:
		template<typename mantissa_, typename exponent_> parsed_to_float(mantissa_ const& mantissa, exponent_ const& exponent, unsigned exponent_power_of = 0) {
			using limits = numeric_limits<float_>;
			if(!mantissa.anything() || mantissa.error() || exponent.error())
				myerror = true;
			else if(myinfinity = mantissa.is_infinity())
				my = limits::has_infinity ? limits::infinity() : limits::max();
			else if(mynan = mantissa.is_nan())
				my = limits::quiet_NaN();
			else if(mantissa.base() < 2)
				myerror = true;
			else {
				// mantissa
				unsigned long long
					x = 0,
					m = 1;
				auto digits = mantissa.digits();
				auto
					d = digits.begin(),
					de = digits.end();
				___water_assert((d == de || *d) && "first digit must not be 0");
				while(d != de && !de[-1]) --de; // cut zeros at the end
				unsigned digits_used = static_cast<unsigned>(de - d); // crazy if overflows
				while(d != de) {
					___water_assert(*d < mantissa.base());
					m *= mantissa.base();
					x *= mantissa.base();
					x += *d;
					if(m > static_cast<unsigned long long>(-1) / mantissa.base()) {
						my *= m;
						my += x;
						x = 0;
						m = 1;
						}
					++d;
					}
				if(m != 1) {
					my *= m;
					my += x;
					}
				if(my) {
					// mantissa exponent, from decimal point
					unsigned const me_max = static_cast<unsigned>(numeric_limits<int>::max());
					if(me_max < mantissa.leading_zeros() || me_max < mantissa.point_at() || me_max - mantissa.leading_zeros() < digits_used)
						myerror = true;
					else {
						int me = static_cast<int>(mantissa.point_at()) - static_cast<int>(mantissa.leading_zeros() + digits_used);
						// exponent
						if(!exponent_power_of)
							exponent_power_of = mantissa.base() == 10 ? 10 : 2;
						int e = 0;
						if(exponent.anything()) { // is it an error if no exponent? probably not				
							parsed_to_int<int> from(exponent);
							if(from.overflow()) {
								if(static_cast<int>(from) < 0) {
									my = 0;
									myinexact = true;
									}
								else {
									my = limits::has_infinity ? limits::infinity() : limits::max();
									myoverflow = true;
									}
								}
							else if(from.error())
								myerror = true;
							else {
								e = from;
								myinexact = myinexact || from.inexact();
								}
							}
						if(e || me) {
							// merge exponents if possible
							if(mantissa.base() == exponent_power_of) {
								if(me < 0 && e < numeric_limits<int>::min() - me) {
									e = 0;
									my = 0;
									myinexact = true;
									}
								if(me > 0 && e > numeric_limits<int>::max() - me) {
									e = 0;
									my = limits::has_infinity ? limits::infinity() : limits::max();
									myoverflow = true;
									}
								else
									e += me;
								me = 0;
								}
							if(me)
								pow_do(static_cast<float_>(mantissa.base()), me);
							if(e)
								pow_do(static_cast<float_>(exponent_power_of), e);
							myoverflow = isinf(my);
							}
						}
					}
				if(!myinexact && !myerror && !myoverflow)
					myinexact =
						(my == 0 && digits_used) || // it became 0 somehow
						mantissa.overflow() ||
						digits_used > max_digits<float_>(mantissa.base());
				}
			if(myerror)
				my = 0;
			else if(mantissa.is_minus())
				my = -my;
			}
		operator float_() const {
			return my;
			}
		bool error() const {
			// if the parsed number was not valid
			return myerror;
			}
		bool inexact() const {
			// could happen because
			// - more significant digits than float_ can reperesent was read
			// - exponent was really small so the number became 0
			// - exponent had fraction digits
			return myinexact;
			}
		bool overflow() const {
			// if the calcualted float is infinity, but the parsed number was not
			return myoverflow;
			}
		bool infinity() const {
			// if the parsed number was infinity
			return myinfinity;
			}
		bool nan() const {
			return mynan;
			}
	private:
		void pow_do(float_ b, int e) {
			// if e is very small or large, pow can become 0, subnormal or infinity
			float_ p = pow(b, static_cast<float_>(e));
			if(isfinite(p) && (p >= numeric_limits<float_>::min() || p <= -numeric_limits<float_>::min()))
				my *= p;
			else {
				my *= pow(b, e / 2);
				my *= pow(b, e - (e / 2));
				}
			}
	};

}}
#endif
