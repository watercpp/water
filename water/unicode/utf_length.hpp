// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_UNICODE_UTF_LENGTH_HPP
#define WATER_UNICODE_UTF_LENGTH_HPP
#include <water/unicode/utf.hpp>
namespace water { namespace unicode {

/*

Verify that input is valid utf, and returns the length of the input in all utf encodings.

The optional verify argument to the constructors can be used to verify each char32_t codepoint more.
For example this checks that its not 0:

utf_lenght<8> length{begin, end, [](char32_t a) { return a != 0; }};

*/

template<unsigned utf_> class
 utf_length {
	static_assert(utf_ == 8 || utf_ == 16 || utf_ == 32, "utf_ must be 8, 16, 32");
	using select_ = typename
		types::ifel<utf_ == 8, char8_t,
		types::ifel<utf_ == 16, char16_t,
		types::ifel<utf_ == 32, char32_t
		> > >::result;
	size_t
		my8 = 0,
		my16 = 0,
		my32 = 0;
	bool myok = true;
	public:
		template<typename forward_iterator_>
		 utf_length(forward_iterator_ begin, size_t size) {
			length(static_cast<select_*>(0), begin, size, no_verify{});
			}
		template<typename forward_iterator_>
		 utf_length(forward_iterator_ begin, forward_iterator_ end) {
			length(static_cast<select_*>(0), begin, end, no_verify{});
			}
		template<typename forward_iterator_, typename verify_>
		 utf_length(forward_iterator_ begin, size_t size, verify_&& verify) {
			length(static_cast<select_*>(0), begin, size, verify);
			}
		template<typename forward_iterator_, typename verify_>
		 utf_length(forward_iterator_ begin, forward_iterator_ end, verify_&& verify) {
			length(static_cast<select_*>(0), begin, end, verify);
			}
		explicit operator bool() const {
			return myok;
			}
		size_t utf8() const {
			return myok ? my8 : 0;
			}
		size_t utf16() const {
			return myok ? my16 : 0;
			}
		size_t utf32() const {
			return myok ? my32 : 0;
			}
		size_t utf8_until_error() const {
			return my8;
			}
		size_t utf16_until_error() const {
			return my16;
			}
		size_t utf32_until_error() const {
			return my32;
			}
	private:
		struct no_verify {
			bool operator()(char32_t) { return true; }
			};
		template<typename iterator_, typename verify_> void
		 length(char8_t*, iterator_ b, size_t s, verify_&& verify) {
			while(s) {
				char32_t c;
				unsigned n = utf8_decode_verify_and_move(c, b, s);
				if(!n || !verify(c)) {
					myok = false;
					return;
					}
				my8 += n;
				my16 += n == 4 ? 2 : 1;
				++my32;
				}
			}
		template<typename iterator_, typename verify_> void
		 length(char8_t*, iterator_ b, iterator_ e, verify_&& verify) {
			while(b != e) {
				char32_t c;
				unsigned n = utf8_decode_verify_and_move(c, b, e);
				if(!n || !verify(c)) {
					myok = false;
					return;
					}
				my8 += n;
				my16 += n == 4 ? 2 : 1;
				++my32;
				}
			}
		template<typename iterator_, typename verify_> void
		 length(char16_t*, iterator_ b, size_t s, verify_&& verify) {
			while(s) {
				char32_t c;
				unsigned n = utf16_decode_verify_and_move(c, b, s);
				if(!n || !verify(c)) {
					myok = false;
					return;
					}
				my8 += n == 2 ? 4 : 1 + (c > 0x7f) + (c > 0x7ff);
				my16 += n;
				++my32;
				}
			}
		template<typename iterator_, typename verify_> void
		 length(char16_t*, iterator_ b, iterator_ e, verify_&& verify) {
			while(b != e) {
				char32_t c;
				unsigned n = utf16_decode_verify_and_move(c, b, e);
				if(!n || !verify(c)) {
					myok = false;
					return;
					}
				my8 += n == 2 ? 4 : 1 + (c > 0x7f) + (c > 0x7ff);
				my16 += n;
				++my32;
				}
			}
		template<typename iterator_, typename verify_> void
		 length(char32_t*, iterator_ b, size_t s, verify_&& verify) {
			while(s && lenght32(*b, verify)) {
				++b;
				--s;
				}
			}
		template<typename iterator_, typename verify_> void
		 length(char32_t*, iterator_ b, iterator_ e, verify_&& verify) {
			while(b != e && lenght32(*b, verify))
				++b;
			}
		template<typename char_, typename verify_> bool
		 lenght32(char_ c, verify_& verify) {
			if(!utf32_verify(c) || !verify(static_cast<char32_t>(c)))
				return myok = false;
			if(c > 0xffff) {
				my8 += 4;
				my16 += 2;
				}
			else {
				my8 += 1 + (c > 0x7f) + (c > 0x7ff);
				++my16;
				}
			++my32;
			return true;
			}
	};

using utf8_length = utf_length<8>;
using utf16_length = utf_length<16>;
using utf32_length = utf_length<32>;

template<typename forward_iterator_> utf_length<utf_from_iterator<forward_iterator_>::result>
 utf_length_from(forward_iterator_ begin, size_t size) {
	return {begin, size};
	}

template<typename forward_iterator_> utf_length<utf_from_iterator<forward_iterator_>::result>
 utf_length_from(forward_iterator_ begin, forward_iterator_ end) {
	return {begin, end};
	}

}}
#endif
