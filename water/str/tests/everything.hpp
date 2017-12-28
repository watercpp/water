// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_STR_TESTS_EVERYTHING_HPP
#define WATER_STR_TESTS_EVERYTHING_HPP
#include <water/str/tests/bits.hpp>
namespace water { namespace str { namespace tests {

/*

test that all operations compile and run.

does not care if they produce the correct output

*/

template<typename char_> class
 everything_container {
	public:
		using value_type = char_;
		everything_container* end() { return this; }
		template<typename iterator_> void insert(everything_container*, iterator_ b, iterator_ e) {}
	};

class everything_function {
	public:
		template<typename iterator_> void operator()(iterator_ b, iterator_ e) {}
	};

template<typename char_> class 
 everything_iterator {
	char_ my {};
	public:
		char_& operator*() { return my; }
		bool operator!=(everything_iterator) const { return true; }
		void operator++() {}
	};

char constexpr everything_char[] = u8"hello";
char16_t constexpr everything_char16[] = u"hello";
char32_t constexpr everything_char32[] = U"hello";
wchar_t constexpr everything_wchar[] = L"hello";

struct everything_begin_end {
	char const* begin() const {
		return everything_char;
		}
	char const* end() const {
		return everything_char + sizeof(everything_char) - 1;
		}
	};

struct everything_thing {
	everything_thing(char const*) {}
	};

template<typename o_> out<o_>& operator<<(out<o_>& o, specific<everything_thing> s) {
	return o;
	}

template<typename o_> void everything(out<o_>& o) {
	unsigned array[1] = { out<o_>::utf };
	typename out<o_>::char_type c[2] = {0x20, 0};
	o(c[0]);
	o(c); // cstring
	o(c, c + 1);
	o.number(1);
	o
		<< everything_char
		<< everything_char16
		<< everything_char32
		<< everything_wchar
		<< '\n'
		<< (everything_char + 0)
		<< (everything_char16 + 0)
		<< (everything_char32 + 0)
		<< (everything_wchar + 0)
		<< everything_begin_end{}
		<< '\n'
		<< u'\n'
		<< U'\n'
		<< L'\n'
		<< everything_thing{0}
		<< array; // should be pointer
	o
		<< true << false
		<< 1 << 0
		<< static_cast<signed char>(1)
		<< static_cast<unsigned char>(1)
		<< static_cast<short>(1)
		<< static_cast<unsigned short>(1)
		<< 1u
		<< 1l
		<< 1ul
		<< 1ll
		<< 1ull
		<< 1.f
		<< 1.
		<< 1.l
		<< static_cast<void const*>(everything_char)
		<< '\n';
	auto restore = o.restore_settings();
	o
		<< restore_settings
		<< &o
		<< settings{}
		<< flush
		<< el
		<< repeat('a', 4)
		<< number(1)
		<< number(1, settings())
		<< string(everything_char32 + 1, everything_char32 + 3)
		<< base(2)
		<< base_prefix_suffix(true)
		<< bool_text(true)
		<< digits(1)
		<< exponent
		<< fraction_digits(1)
		<< group(true)
		<< no_exponent
		<< no_exponent_min_max(-10, 10)
		<< plus(true)
		<< precision(1)
		<< scientific(true)
		<< trailing_zeros(true)
		<< 1
		<< '\n';
	o.locale();
	o.flush();
	o.settings() = settings{};
	o
		<< bytes(everything_char)
		<< bytes(everything_char, 3)
		<< type_name(everything_char)
		<< type_name<out<o_>>()
		<< hexdump(everything_char)
		<< hexdump(everything_char, 3);
	}

template<typename o_> void everything(out<o_>&& o) {
	everything(o);
	}

template<typename char_> void everything_type() {
	everything(out<begin_end<everything_iterator<char_>>>{});
	everything(out<buffer_lines<everything_function, char_, 128>>{});
	everything(out<buffer<everything_function, char_, 128>>{});
	everything(out<container<everything_container<char_>>>{});
	everything(out_function([](char_ const*, char_ const*){}));
	}

inline void everything_all() {
	everything_type<char>();
	everything_type<char16_t>();
	everything_type<char32_t>();
	everything_type<wchar_t>();
	}

}}}
#endif
