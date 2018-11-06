// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_NUMBERS_WRITE_HPP
#define WATER_NUMBERS_WRITE_HPP
#include <water/numbers/formatted.hpp>
#include <water/numbers/formatted_mantissa_exponent.hpp>
#include <water/numbers/format_int.hpp>
#include <water/numbers/format_float.hpp>
#include <water/numbers/locale.hpp>
#include <water/numbers/write_to.hpp>
namespace water { namespace numbers {

/*

write a formatted number using a locale, with utf conversion
if utf_ is 0, this selects utf from the char/iterator type

use size first to measure

write<float> w(1.234);
size_t s = w.size(char{});
w(begin, end);

*/

template<typename type_, unsigned utf_ = 0> class
 write {
	static_assert(utf_ == 0 || utf_ == 8 || utf_ == 16 || utf_ == 32, "");
	static bool constexpr integer = numeric_limits<type_>::is_integer;
	public:
		using format_type = typename types::ifel<integer, format_int, format_float>::result;
		using formatted_type = decltype(types::make<format_type&>()(type_{}));
		static unsigned constexpr utf = utf_;
	private:
		formatted_type my;
		bool mygroup = false;
		size_t myused = 0;
	public:
		write(formatted_type a, bool group = false) :
			my{a},
			mygroup{group}
			{}
		write(type_ a, settings s = {}) :
			my{format_type{s}(a)},
			mygroup{s.group()}
			{}
		write& group(bool a) {
			mygroup = a;
			return *this;
		}
		
		template<typename locale_>
		 size_t size(locale_ const& locale) {
			// return how many characters is needed to write the number in utf_
			static_assert(utf, "utf cannot be 0");
			myused = 0;
			return write_size<utf>(my, locale, mygroup);
			}
		size_t size() {
			return this->size(locale{});
			}
		
		template<typename char_, typename locale_>
		 size_t size(locale_ const& locale) {
			// return how many characters of type char_ is needed to write the number
			myused = 0;
			return write_size<utf ? utf : unicode::utf_from_char<char_>::result>(my, locale, mygroup);
			}
		template<typename char_>
		 size_t size() {
			return this->template size<char_>(locale{});
			}
		
		template<typename output_iterator_, typename locale_>
		 output_iterator_ operator()(output_iterator_ begin, output_iterator_ end, locale_ const& locale) {
			// can stop in the middle of utf8 or utf16 sequences
			write_to_begin_end<output_iterator_> to{begin, end};
			myused = write_to<utf ? utf : unicode::utf_from_iterator<output_iterator_>::result>(to, my, locale, mygroup);
			return to.at();
			}
		template<typename output_iterator_>
		 output_iterator_ operator()(output_iterator_ begin, output_iterator_ end) {
			return operator()(begin, end, locale{});
			}
		
		template<typename output_iterator_, typename locale_>
		 output_iterator_ operator()(output_iterator_ begin, size_t size, locale_ const& locale) {
			// will not stop in the middle of utf8 or utf16 sequence
			write_to_begin_size<output_iterator_> to{begin, size};
			myused = write_to<utf ? utf : unicode::utf_from_iterator<output_iterator_>::result>(to, my, locale, mygroup);
			return to.at();
			}
		template<typename output_iterator_>
		 output_iterator_ operator()(output_iterator_ begin, size_t size) {
			return operator()(begin, size, locale{});
			}
		
		template<typename function_, typename locale_>
		 size_t buffered(function_&& function, locale_ const& locale) {
			// function(char const* begin, unsigned size)
			// where char can be char, char16_t, char32_t
			static_assert(utf, "utf cannot be 0");
			return myused = write_buffered<utf>(function, my, locale, mygroup);
			}
		template<typename function_>
		 size_t buffered(function_&& function) {
			return buffered(function, locale{});
			}
		
		size_t used() {
			return myused;
			}
	};

}}
#endif
