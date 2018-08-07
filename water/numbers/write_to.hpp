// Copyright 2017-2018 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_NUMBERS_WRITE_TO_HPP
#define WATER_NUMBERS_WRITE_TO_HPP
#include <water/numbers/formatted.hpp>
#include <water/numbers/formatted_mantissa_exponent.hpp>
namespace water { namespace numbers {

/*

write a formatted number using a locale, with utf conversion

*/

namespace _ {
	
	struct write_nothing {};
	
	// bypass utf conversion when possible. does this make any performance difference ever?
	
	template<unsigned utf_> struct
	 write_one {
		template<typename to_> static unsigned do_it(to_& to, char32_t from) {
			to(static_cast<char32_t const*>(&from), 1u);
			return 1;
			}
		static unsigned do_it(write_nothing& /*to*/, char32_t /*from*/) {
			return 1;
			}
		};
	template<> struct
	 write_one<8> {
		template<typename to_> static unsigned do_it(to_& to, unsigned char from) {
			to(static_cast<char const*>(static_cast<void const*>(&from)), 1);
			return 1;
			}
		template<typename to_, typename char_> static unsigned do_it(to_& to, char_ from) {
			unsigned char c[4];
			unsigned s = unicode::utf8_encode(c, from);
			to(static_cast<char const*>(static_cast<void const*>(c)), s);
			return s;
			}
		static unsigned do_it(write_nothing& /*to*/, unsigned char /*from*/) {
			return 1;
			}
		template<typename char_> static unsigned do_it(write_nothing& /*to*/, char_ from) {
			if(from <= 0x7f)
				return 1;
			if(from <= 0x7ff)
				return 2;
			if(from <= 0xffff)
				return 3;
			return 4;
			}
		};
	template<> struct
	 write_one<16> {
		template<typename to_> static unsigned do_it(to_& to, unsigned char from) {
			return do_it(to, static_cast<char16_t>(from));
			}
		template<typename to_> static unsigned do_it(to_& to, char16_t from) {
			to(static_cast<char16_t const*>(&from), 1);
			return 1;
			}
		template<typename to_> static unsigned do_it(to_& to, char32_t from) {
			char16_t c[2];
			unsigned s = unicode::utf16_encode(c, from);
			to(static_cast<char16_t const*>(c), s);
			return s;
			}
		static unsigned do_it(write_nothing& /*to*/, unsigned char /*from*/) {
			return 1;
			}
		static unsigned do_it(write_nothing& /*to*/, char16_t /*from*/) {
			return 1;
			}
		static unsigned do_it(write_nothing& /*to*/, char32_t from) {
			return from <= 0xffff ? 1 : 2;
			}
		};
	
	template<unsigned utf_> class
	 write_element {
		size_t my = 0;
		public:
			template<typename to_, typename from_>
			 if_not_begin_end<from_, size_t> operator()(to_& to, from_ const& from) {
				size_t r = write_one<utf_>::do_it(to, unicode::cast(from));
				my += r;
				return r;
				}
			template<typename to_, typename from_>
			 if_begin_end<from_, size_t> operator()(to_& to, from_ const& from) {
				size_t r = 0;
				auto f = from.begin();
				while(f != from.end()) {
					r += write_one<utf_>::do_it(to, unicode::cast(*f));
					++f;
					}
				my += r;
				return r;
				}
			template<typename to_, typename from_>
			 size_t first_if_not_empty(to_& to, from_ const& from) {
				if(from.begin() != from.end())
					return operator()(to, *from.begin());
				return 0;
				}
			size_t written() {
				return my;
				}
		};
}

template<typename iterator_> class
 write_to_begin_size {
	iterator_ my;
	size_t
		myleft,
		mysize;
	public:
		write_to_begin_size(iterator_ begin, size_t size) :
			my{begin},
			myleft{size},
			mysize{size}
			{}
		iterator_ at() const {
			return my;
			}
		size_t left() const {
			return myleft;
			}
		template<typename char_> void operator()(char_ const* c, unsigned s) {
			if(mysize < s) {
				mysize = 0; // do not write any more, the next write might fit
				return;
				}
			myleft = mysize -= s;
			do {
				*my = *c;
				++my;
				++c;
				} while(--s);
			}
	};

template<typename iterator_, typename = void> class
 write_to_begin_end {
	iterator_ my, myend;
	public:
		write_to_begin_end(iterator_ begin, iterator_ end) :
			my{begin},
			myend{end}
			{}
		iterator_ at() const {
			return my;
			}
		template<typename char_> void operator()(char_ const* c, unsigned s) {
			// this can write incomplete utf8 utf16 sequences if begin,end is too small
			while(my != myend && s) {
				*my = *c;
				++my;
				++c;
				--s;
				}
			}
	};
template<typename iterator_> class
 write_to_begin_end<iterator_, types::to_void<decltype(iterator_{} - iterator_{})>> : public write_to_begin_size<iterator_> {
	public:
		write_to_begin_end(iterator_ begin, iterator_ end) :
			write_to_begin_size<iterator_>{begin, static_cast<size_t>(end - begin)}
			{}
	};

template<unsigned utf_, typename to_, typename iterator_, typename locale_>
 size_t write_to(to_&& to, formatted<iterator_> const& from, locale_ const& locale, bool group = false) {
	// write numer, return how many characters was written
	// returns 0 if from.digits() == 0. it must be formatted as a number even if its nan or infinity. because the locale may not have nan or infinity
	auto digits = locale.digits();
	unsigned base = 2 <= from.base() && from.base() <= 16 ? from.base() : 10;
	___water_assert(static_cast<unsigned>(digits.end() - digits.begin()) >= base);
	if(!from.digits() || static_cast<unsigned>(digits.end() - digits.begin()) < base)
		return 0;
	_::write_element<utf_> element;
	// bool true/false
	if(from.bool_true() && element.first_if_not_empty(to, locale.trues()))
		return element.written();
	if(from.bool_false() && element.first_if_not_empty(to, locale.falses()))
		return element.written();
	// +-
	if(from.minus())
		element(to, *locale.minuses().begin());
	else if(from.plus())
		element.first_if_not_empty(to, locale.pluses());
	// infinity or nan
	if(from.infinity() && element.first_if_not_empty(to, locale.infinities()))
		return element.written();
	if(from.nan() && element.first_if_not_empty(to, locale.nans()))
		return element.written();
	// base prefix
	auto bases = locale.bases();
	auto base_at = bases.begin();
	if(from.base_prefix_suffix()) {
		while(base_at != bases.end() && base_at->base() != base)
			++base_at;
		if(base_at != bases.end())
			element(to, base_at->prefix());
		}
	// digits 1 234 567.123 456
	auto group_function = locale.group_function();
	auto i = from.begin();
	unsigned
		at = 0,
		point_distance = from.point_at() ? from.point_at() : from.digits();
	bool after_point = false;
	do {
		if(point_distance) {
			if(at && group && group_function(point_distance, after_point, base))
				element.first_if_not_empty(to, locale.groups());
			after_point ? ++point_distance : --point_distance;
			}
		else {
			element(to, *locale.points().begin());
			after_point = true;
			++point_distance;
			}
		unsigned d = static_cast<unsigned>(*i);
		___water_assert(d < base);
		if(d >= base) d = 0;
		element(to, *(digits.begin() + d));
		++i;
		} while(++at != from.digits());
	// base suffix
	if(from.base_prefix_suffix() && base_at != bases.end())
		element(to, base_at->suffix());
	return element.written();
	}

template<unsigned utf_, typename to_, typename float_, typename locale_>
 size_t write_to(to_&& to, formatted_mantissa_exponent<float_> const& from, locale_ const& locale, bool group = false) {
	size_t r = write_to<utf_>(to, from.mantissa(), locale, group);
	// no exponent if nan/infinity and locale wrote that 
	if(from.mantissa().infinity()) {
		auto l = locale.infinities();
		if(l.begin() != l.end())
			return r;
		}
	if(from.mantissa().nan()) {
		auto l = locale.nans();
		if(l.begin() != l.end())
			return r;
		}
	if(r && from.exponent().digits()) {
	 	// if grouping, write a space before the exponent
	 	if(group)
	 		r += _::write_element<utf_>{}.first_if_not_empty(to, locale.spaces());
	 	for(auto const& e : locale.exponents())
	 		if(e.base() == from.mantissa().base() || (from.mantissa().base() != 10 && e.base() == 2)) {
	 			r += _::write_element<utf_>{}(to, e.prefix());
	 			break;
	 			}
	 	r += write_to<utf_>(to, from.exponent(), locale, group);
	 	}
	return r;
	}

namespace _ {
	template<unsigned utf_, typename to_> class
	 write_buffered_to {
		using char_ = typename types::ifel<utf_ == 8, char, types::ifel<utf_ == 16, char16_t, char32_t>>::result;
		static unsigned constexpr size = 256;
		char_ my[size];
		unsigned mysize = 0;
		to_ *myto;
		public:
			write_buffered_to(to_& to) :
				myto{&to}
				{}
			void operator()(char_ const* c, unsigned s) {
				if(mysize + s > size)
					flush();
				while(s) { // s is 1 to 4, never larger than size
					my[mysize++] = *c++;
					--s;
					}
				}
			void flush() {
				if(mysize)
					(*myto)(my + 0, my + mysize);
				mysize = 0;
				}
		};
	}

template<unsigned utf_, typename to_, typename iterator_, typename locale_>
 size_t write_buffered(to_&& to, formatted<iterator_> const& from, locale_ const& locale, bool group = false) {
	_::write_buffered_to<utf_, typename types::no_reference<to_>::result> t(to);
	size_t r = write_to<utf_>(t, from, locale, group);
	t.flush();
	return r;
	}

template<unsigned utf_, typename to_, typename float_, typename locale_>
 size_t write_buffered(to_&& to, formatted_mantissa_exponent<float_> const& from, locale_ const& locale, bool group = false) {
	_::write_buffered_to<utf_, typename types::no_reference<to_>::result> t(to);
	size_t r = write_to<utf_>(t, from, locale, group);
	t.flush();
	return r;
	}

template<unsigned utf_, typename iterator_, typename locale_>
 size_t write_size(formatted<iterator_> const& from, locale_ const& locale, bool group = false) {
 	return write_to<utf_>(_::write_nothing{}, from, locale, group);
 	}

template<unsigned utf_, typename float_, typename locale_>
 size_t write_size(formatted_mantissa_exponent<float_> const& from, locale_ const& locale, bool group = false) {
 	return write_to<utf_>(_::write_nothing{}, from, locale, group);
 	}

}}
#endif
