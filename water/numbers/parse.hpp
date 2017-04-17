// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_NUMBERS_PARSE_HPP
#define WATER_NUMBERS_PARSE_HPP
#include <water/numbers/bits.hpp>
#include <water/numbers/compare.hpp>
#include <water/numbers/settings.hpp>
#include <water/numbers/locale.hpp>
namespace water { namespace numbers {

/*

parse floatingpoint or integer number using a locale

this will not recognize octal numbers if they are prefixed with just 0, it needs a unique prefix like 0o.

no group: can use lots of decimal point characters

this will parse with 1 locale at a time. to parse numbers that can be in any of 10 locales,
try to parse with each locale and keep the one that parsed the most characters.

if the exponent is superscripted, use a separate locale with superscript digits for the exponent.

1e23 is valid hex

this will not recognize octal numbers if they are prefixed with just 0, it needs unique prefix like 0o.

the mantissa_exponent function will not parse exponent forms like this:
1.23 x 10^123 for 1.23 * pow(10, 123)
1.23 * 2^123 for 1.23 * pow(2, 123)
1.23^123 for pow(1.23, 123)
this can be useful to parse parts 1.23 10 123 separatley, but something else needs to parse the whole expression.

if the locale has fewer digits than the selected base, this will not return an error.
it will look for the digits the locale has.

*/

class parse {
	size_t myused = 0;
	unsigned mybase = 0;
	bool
		mygroup = false,
		myspace = false,
		mydigit_before_point = false,
		mydigit_after_point = false,
		myinteger = false,
		mybase_prefix_suffix = true,
		mybool_text = false;
	public:
		constexpr parse() = default;
		parse(settings a) :
			mybase{a.base()},
			mygroup{a.group()},
			myspace{a.space()},
			mydigit_before_point{a.digit_before_point()},
			mydigit_after_point{a.digit_after_point()},
			mybase_prefix_suffix{a.base_prefix_suffix()},
			mybool_text{a.bool_text()}
			{}
		parse& base(unsigned a) {
			___water_assert(a != 1 && a <= 16);
			mybase = a;
			return *this;
			}
		parse& group(bool a) {
			// if number can contain groups like 123,456.789,123e+123,456
			mygroup = a;
			return *this;
			}
		parse& space(bool a) {
			// if number can contain spaces, like - 123.456 e + 123
			myspace = a;
			return *this;
			}
		parse& digit_before_point(bool a) {
			// if number must have a digit before the point, otherwise .123 is seen as 0.123
			mydigit_before_point = a;
			return *this;
			}
		parse& digit_after_point(bool a) {
			// if number must have a digit after the point
			// true: 123.e123 and 123. is parsed as 123
			// false: 123.e123 and 123. is parsed to the end
			mydigit_after_point = a;
			return *this;
			}
		parse& integer(bool a) {
			// integer mode, will not look for decimal point, infinity, nan. ignored for mantissa_exponent
			myinteger = true;
			return *this;
			}
		parse& base_prefix_suffix(bool a) {
			// true to look for base prefix/suffix
			// when this is false, base defaults to 10
			mybase_prefix_suffix = a;
			return *this;
			}
		parse& bool_text(bool a) {
			// true to look for true/false before anything else. ignored for mantissa_exponent
			mybool_text = a;
			return *this;
			}
		size_t used() const {
			// number of characters used to parse
			return myused;
			}
		template<typename parsed_, typename forward_iterator_, typename locale_, typename transform_>
		 forward_iterator_ operator()(parsed_& to, forward_iterator_ begin, forward_iterator_ end, locale_ const& locale, transform_&& transform) {
		 	// parse as a number
		 	// return begin on error
			to.clear();
			myused = 0;
			if(begin == end || mybase == 1 || mybase > 16)
				return begin;
			size_t at = 0;
			unsigned base = mybase;
			if(!mybase_prefix_suffix && !base)
				base = 10;
			auto i = begin;
			auto spaces = locale.spaces();
			
			compare_one_of_and_move one_of;
			compare_and_move is;
			
			if(mybool_text) {
				if(one_of(i, end, locale.trues(), transform)) {
					myused = at + one_of.used();
					to.base(10);
					to.digit(1);
					return i;
					}
				if(one_of(i, end, locale.falses(), transform)) {
					myused = at + one_of.used();
					to.base(10);
					to.digit(0);
					return i;
					}
			}
			
			// sign
			int sign = 0;
			if(one_of(i, end, locale.minuses())) {
				to.minus();
				sign = -1;
				}
			else if(one_of(i, end, locale.pluses()))
				sign = 1;
			if(sign) {
				at += one_of.used();
				if(myspace)
					at += one_of(i, end, spaces).used();
				if(i == end)	
					return begin;
				}
			
			// infinity or nan
			if(!myinteger) {
				if(one_of(i, end, locale.infinities(), transform)) {
					myused = at + one_of.used();
					to.infinity();
					return i;
					}
				if(one_of(i, end, locale.nans(), transform)) {
					myused = at + one_of.used();
					to.nan();
					return i;
					}
				}
			
			// if mybase is 0 and there is no base prefix/suffix and it looks like base 16, retry as base 10. because 1e1 looks like base16
			auto retry = i;
			auto retry_at = at;
			while(true) {
				i = retry;
				at = retry_at;
				to.clear();
				if(sign < 0)
					to.minus();

				// base prefix
				auto bases = locale.bases();
				auto base_at = bases.begin();
				if(mybase_prefix_suffix)
					while(base_at != bases.end()) {
						if((!base || base == base_at->base()) && is(i, end, base_at->prefix(), transform)) {
							base = base_at->base();
							at += is.used();
							if(myspace)
								at += one_of(i, end, spaces).used();
							break;
							}
						++base_at;
						}
				bool base_set = base != 0;
				if(base_set)
					to.base(base);
				else
					base = 16;
				
				// digits
				// 123 456.789 0123
				// .123 only if !mydigit_before_point
				// 123. only if !mydigit_after_point
				// group and point must be surrounded by digits. cannot begin or end with group, cannot have group next to point
				auto digits = locale.digits();
				auto points = locale.points();
				auto groups = locale.groups();
				auto r = i;
				auto used = at;
				char last = 0;
				unsigned digit_max = 0;
				bool point = false;
				while(i != end) {
					auto id = i;
					one_of(id, end, digits, transform);
					if(one_of && one_of.which() < base) {
						if(digit_max < one_of.which())
							digit_max = static_cast<unsigned>(one_of.which());
						to.digit(static_cast<unsigned>(one_of.which()));
						last = '0';
						used = at += one_of.used();
						r = i = id;
						}
					else if(!myinteger && !point && (mydigit_before_point ? last == '0' : (last == '0' || !last)) && one_of(i, end, points)) {
						last = '.';
						point = true;
						to.point();
						at += one_of.used();
						if(!mydigit_after_point) {
							used = at;
							r = i;
							}
						}
					else if(mygroup && last == '0' && one_of(i, end, groups)) {
						last = 'g';
						at += one_of.used();
						}
					else
						break;
					}
				// if the last was not a digit, it was group or point. group can be space. rewind
				if(at != used) {
					i = r;
					at = used;
					}
				if(myspace)
					at += one_of(i, end, spaces).used();
			
				// base suffix
				if(mybase_prefix_suffix) {
					if(base_at != bases.end()) {
						// this could have prefix and suffix, like &x123;
						auto suffix = base_at->suffix();
						if(suffix.begin() != suffix.end()) {
							if(!is(i, end, suffix, transform)) {
								if(!mybase && base_at->base() != 10) {
									base = 10;
									continue;
									}
								return begin;
								}
							r = i;
							used = at += is.used();
							if(myspace)
								at += one_of(i, end, spaces).used();
							base_set = true;
							}
						}
					else if(i != end) {
						base_at = bases.begin();
						while(base_at != bases.end()) {
							if(digit_max < base_at->base() && (!base_set || base == base_at->base())) {
								auto prefix = base_at->prefix();
								if(prefix.begin() == prefix.end() && is(i, end, base_at->suffix(), transform)) {
									base_set = true;
									r = i;
									used = at += is.used();
									if(myspace)
										at += one_of(i, end, spaces).used();
									base = base_at->base();
									break;
									}
								}
							++base_at;
							}
						}
					if(!base_set) {
						base = 10;
						if(digit_max >= 10)
							continue;
						}
					to.base(base);
					}
				
				// success if any digits
				if(to.total_digits()) {
					myused = used;
					return r;
					}
				break;
				}
			return begin;
		 	}
		template<typename parsed_, typename forward_iterator_, typename locale_>
		 forward_iterator_ operator()(parsed_& to, forward_iterator_ begin, forward_iterator_ end, locale_ const& locale) {
			return operator()(to, begin, end, locale, transform_lowercase{});
			}
		template<typename parsed_, typename forward_iterator_>
		 forward_iterator_ operator()(parsed_& to, forward_iterator_ begin, forward_iterator_ end) {
			return operator()(to, begin, end, locale{}, transform_lowercase{});
			}
		template<typename mantissa_, typename exponent_, typename forward_iterator_, typename locale_, typename transform_>
		 forward_iterator_ mantissa_exponent(mantissa_& mantissa, exponent_& exponent, forward_iterator_ begin, forward_iterator_ end, locale_ const& locale, transform_&& transform) {
			// parse mantissa and exponent
			// return begin on error
			exponent.clear();
			myused = 0;
			auto copy = *this;
			copy.mybool_text = false;
			copy.myinteger = false;
			auto r = copy(mantissa, begin, end, locale, transform);
			myused = copy.myused;
			if(myused && r != end && !mantissa.is_infinity() && !mantissa.is_nan()) {
				auto i = r;
				// space between 1.23 E45, if spaces or if group. group because numbers::write will write a space here if group 
				auto spaces = locale.spaces();
				size_t used = myspace || mygroup ? compare_one_of_and_move{}(i, end, spaces).used() : 0;
				// prefix eEpP
				auto exponents = locale.exponents();
				auto e = exponents.begin();
				compare_and_move is;
				while(e != exponents.end() && (e->base() != (mantissa.base() == 10 ? 10 : 2) || !is(i, end, e->prefix(), transform)))
					++e;
				if(e != exponents.end()) {
					used += is.used();
					used += myspace ? compare_one_of_and_move{}(i, end, spaces).used() : 0;
					auto copy = *this;
					copy.myinteger = true;
					copy.mybool_text = false;
					copy.mybase = 10;
					copy.mybase_prefix_suffix = false;
					i = copy(exponent, i, end, locale, transform);
					if(copy.used()) {
						r = i;
						myused += used + copy.used();
						}
					}
				}
			return r;
			}
		template<typename mantissa_, typename exponent_, typename forward_iterator_, typename locale_>
		 forward_iterator_ mantissa_exponent(mantissa_& mantissa, exponent_& exponent, forward_iterator_ begin, forward_iterator_ end, locale_ const& locale) {
			return mantissa_exponent(mantissa, exponent, begin, end, locale, transform_lowercase{});
			}
		template<typename mantissa_, typename exponent_, typename forward_iterator_>
		 forward_iterator_ mantissa_exponent(mantissa_& mantissa, exponent_& exponent, forward_iterator_ begin, forward_iterator_ end) {
			return mantissa_exponent(mantissa, exponent, begin, end, locale{}, transform_lowercase{});
			}
	};

}}
#endif
