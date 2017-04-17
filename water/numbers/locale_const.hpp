// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_NUMBERS_LOCALE_CONST_HPP
#define WATER_NUMBERS_LOCALE_CONST_HPP
#include <water/numbers/bits.hpp>
namespace water { namespace numbers {

/*

locale_const can be used for a constexpr locale like this:

water::numbers::locale_const<char> constexpr my_locale = {
	{"0123456789abcdef", 16},
	{"+", 1},
	{"-", 1},
	{" ", 1},
	{".,", 2},
	{" ", 1},
	{{"infinity", 9}, {"inf", 3}},
	{{"nan", 3}},
	{{"true", 4}, {"yes", 3}},
	{{"false", 5}, {"no", 2}},
	{
		{16, "0x", 2},
		{8, "0o", 2},
		{2, "0b", 2},
		{16, 0, 0, "h", 1}
	},
	{
		{10, "e", 1},
		{2, "p", 1}
	}
	};

*/

template<typename char_> struct
 locale_const_string {
	char_ const* mybegin;
	size_t mysize;
	char_ const* begin() const {
		return mybegin;
		}
	char_ const* end() const {
		return mybegin + mysize;
		}
	explicit operator bool() const {
		return mysize != 0;
		}
	};

template<typename char_> struct
 locale_const_base {
	unsigned mybase;
	locale_const_string<char_>
		myprefix,
		mysuffix;
	unsigned base() const {
		return mybase;
		}
	locale_const_string<char_> prefix() const {
		return myprefix;
		}
	locale_const_string<char_> suffix() const {
		return mysuffix;
		}
	explicit operator bool() const {
		return mybase != 0;
		}
	};

template<typename char_> struct
 locale_const_exponent {
	unsigned mybase;
	locale_const_string<char_> myprefix;
	unsigned base() const {
		return mybase;
		}
	locale_const_string<char_> prefix() const {
		return myprefix;
		}
	explicit operator bool() const {
		return mybase != 0;
		}
	};

template<typename char_, typename group_ = group> struct
 locale_const {
 	locale_const_string<char_>
 		mydigits,
 		mypluses,
 		myminuses,
 		mygroups,
 		mypoints,
 		myspaces,
 		myinfinities[8],
 		mynans[8],
 		mytrues[8],
 		myfalses[8];
 	locale_const_base<char_> mybases[16];
 	locale_const_exponent<char_> myexponents[8];
 	group_ mygroup;
 	public:
		locale_const_string<char_> digits() const {
			return mydigits;
			}
		locale_const_string<char_> pluses() const {
			return mypluses;
			}
		locale_const_string<char_> minuses() const {
			return myminuses;
			}
		locale_const_string<char_> groups() const {
			return mygroups;
			}
		locale_const_string<char_> points() const {
			return mypoints;
			}
		locale_const_string<char_> spaces() const {
			return myspaces;
			}
		begin_end<locale_const_string<char_> const*> infinities() const {
			return {myinfinities, size(myinfinities)};
			}
		begin_end<locale_const_string<char_> const*> nans() const {
			return {mynans, size(mynans)};
			}
		begin_end<locale_const_string<char_> const*> trues() const {
			return {mytrues, size(mytrues)};
			}
		begin_end<locale_const_string<char_> const*> falses() const {
			return {myfalses, size(myfalses)};
			}
		begin_end<locale_const_base<char_> const*> bases() const {
			return {mybases, size(mybases)};
			}
		begin_end<locale_const_exponent<char_> const*> exponents() const {
			return {myexponents, size(myexponents)};
			}
		group_ group_function() const {
			return {};
			}
	private:
		template<typename type_, size_t size_> size_t size(type_ const (&a)[size_]) const {
			size_t r = 0;
			while(a[r] && ++r != size_);
			return r;
			}
	};

}}
#endif
