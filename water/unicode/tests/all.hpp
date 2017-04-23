// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_UNICODE_TESTS_ALL_HPP
#define WATER_UNICODE_TESTS_ALL_HPP
#include <water/unicode/unicode.hpp>
#include <water/downgrade_iterators/downgrade_iterators.hpp>
#include <water/test.hpp>
namespace water { namespace unicode { namespace tests {

/*

this should test pretty much everything utf

*/

struct all {
	size_t converted_codepoints = 0; // count how many codepoints were converted (maybe interesting for speed test)
	all() {
		char32_t hi = 0;
		do {
			size_t conversions = 0;
			char32_t
				original[0x100],
				a32[0x100],
				b32[0x100];
			char16_t
				a16[0x200],
				b16[0x200];
			char8_t
				a8[0x400],
				b8[0x400];
			char32_t lo = 0;
			do original[lo] = (hi << 8) | lo; while(++lo != 0x100);
		
			utf_length<32> length(static_cast<char32_t const*>(original), 0x100);
		
			___water_test(length_test(length, utf_length<32>(static_cast<char32_t const*>(original), static_cast<char32_t const*>(original) + 0x100), original[0]));
			++conversions;
		
			// 8 from 32
			auto a8e = utf_from_utf_verify<8, 32, true>(static_cast<char8_t*>(a8), static_cast<char32_t const*>(original), 0x100);
			auto b8e = utf_from_utf_verify<8, 32, false>(static_cast<char8_t*>(b8), static_cast<char32_t const*>(original), 0x100);
			___water_test(a8 + a8e.size() == a8e.end());
			___water_test(b8 + b8e.size() == b8e.end());
			___water_test(equal(a8 + 0, a8e.end(), b8 + 0, b8e.end()));
			auto l8 = utf_length<8>(static_cast<char8_t const*>(a8), a8e.size());
			___water_test(length_test(length, l8, original[0]));
			l8 = utf_length<8>(static_cast<char8_t const*>(b8), static_cast<char8_t const*>(b8e.end()));
			___water_test(length_test(length, l8, original[0]));
				{
				auto i = utf_iterator_with_end<8, char32_t const*, true>(original, original + 0x100);
				___water_test(equal(a8 + 0, a8e.end(), i, end(i)));
				auto l = utf_length<8>(i, end(i));
				___water_test(length_test(length, l, original[0]));
				}
				{
				auto i = utf_iterator_with_end<8, char32_t const*, false>(original, original + 0x100);
				___water_test(equal(a8 + 0, a8e.end(), i, end(i)));
				auto l = utf_length<8>(i, end(i));
				___water_test(length_test(length, l, original[0]));
				}
				{
				auto i = utf_iterator_with_size<8, char32_t const*, true>(original, 0x100);
				___water_test(equal(a8 + 0, a8e.end(), i, end(i)));
				auto l = utf_length<8>(i, length.utf8());
				___water_test(length_test(length, l, original[0]));
				}
				{
				auto i = utf_iterator_with_size<8, char32_t const*, false>(original, 0x100);
				___water_test(equal(a8 + 0, a8e.end(), i, end(i)));
				auto l = utf_length<8>(i, length.utf8());
				___water_test(length_test(length, l, original[0]));
				}
			a8e = b8e;
			conversions += 12;
		
			// 16 from 32
			auto a16e = utf_from_utf_verify<16, 32, true>(static_cast<char16_t*>(a16), static_cast<char32_t const*>(original), 0x100);
			auto b16e = utf_from_utf_verify<16, 32, false>(static_cast<char16_t*>(b16), static_cast<char32_t const*>(original), 0x100);
			___water_test(a16 + a16e.size() == a16e.end());
			___water_test(b16 + b16e.size() == b16e.end());
			___water_test(equal(a16 + 0, a16e.end(), b16 + 0, b16e.end()));
			auto l16 = utf_length<16>(static_cast<char16_t const*>(a16), a16e.size());
			___water_test(length_test(length, l16, original[0]));
			l16 = utf_length<16>(static_cast<char16_t const*>(b16), static_cast<char16_t const*>(b16e.end()));
			___water_test(length_test(length, l16, original[0]));
				{
				auto i = utf_iterator_with_end<16, char32_t const*, true>(original, original + 0x100);
				___water_test(equal(a16 + 0, a16e.end(), i, end(i)));
				auto l = utf_length<16>(i, end(i));
				___water_test(length_test(length, l, original[0]));
				}
				{
				auto i = utf_iterator_with_end<16, char32_t const*, false>(original, original + 0x100);
				___water_test(equal(a16 + 0, a16e.end(), i, end(i)));
				auto l = utf_length<16>(i, end(i));
				___water_test(length_test(length, l, original[0]));
				}
				{
				auto i = utf_iterator_with_size<16, char32_t const*, true>(original, 0x100);
				___water_test(equal(a16 + 0, a16e.end(), i, end(i)));
				auto l = utf_length<16>(i, length.utf16());
				___water_test(length_test(length, l, original[0]));
				}
				{
				auto i = utf_iterator_with_size<16, char32_t const*, false>(original, 0x100);
				___water_test(equal(a16 + 0, a16e.end(), i, end(i)));
				auto l = utf_length<16>(i, length.utf16());
				___water_test(length_test(length, l, original[0]));
				}
			a16e = b16e;
			conversions += 12;
	
			// 8 from 16
			a8e = utf_from_utf_verify<8, 16, true>(static_cast<char8_t*>(a8), static_cast<char16_t const*>(a16), length.utf16());
			b8e = utf_from_utf_verify<8, 16, false>(static_cast<char8_t*>(b8), static_cast<char16_t const*>(a16), length.utf16());
			___water_test(a8 + a8e.size() == a8e.end());
			___water_test(b8 + b8e.size() == b8e.end());
			___water_test(equal(a8 + 0, a8e.end(), b8 + 0, b8e.end()));
			l8 = utf_length<8>(static_cast<char8_t const*>(a8), a8e.size());
			___water_test(length_test(length, l8, original[0]));
			l8 = utf_length<8>(static_cast<char8_t const*>(b8), static_cast<char8_t const*>(b8e.end()));
			___water_test(length_test(length, l8, original[0]));
				{
				auto i = utf_iterator_with_end<8, char16_t const*, true>(a16, a16 + length.utf16());
				___water_test(equal(a8 + 0, a8e.end(), i, end(i)));
				auto l = utf_length<8>(i, end(i));
				___water_test(length_test(length, l, original[0]));
				}
				{
				auto i = utf_iterator_with_end<8, char16_t const*, false>(a16, a16 + length.utf16());
				___water_test(equal(a8 + 0, a8e.end(), i, end(i)));
				auto l = utf_length<8>(i, end(i));
				___water_test(length_test(length, l, original[0]));
				}
				{
				auto i = utf_iterator_with_size<8, char16_t const*, true>(a16, length.utf16());
				___water_test(equal(a8 + 0, a8e.end(), i, end(i)));
				auto l = utf_length<8>(i, length.utf8());
				___water_test(length_test(length, l, original[0]));
				}
				{
				auto i = utf_iterator_with_size<8, char16_t const*, false>(a16, length.utf16());
				___water_test(equal(a8 + 0, a8e.end(), i, end(i)));
				auto l = utf_length<8>(i, length.utf8());
				___water_test(length_test(length, l, original[0]));
				}
			a8e = b8e;
			conversions += 12;
		
			// 16 from 8
			a16e = utf_from_utf_verify<16, 8, true>(static_cast<char16_t*>(a16), static_cast<char8_t const*>(a8), length.utf8());
			b16e = utf_from_utf_verify<16, 8, false>(static_cast<char16_t*>(b16), static_cast<char8_t const*>(a8), length.utf8());
			___water_test(a16 + a16e.size() == a16e.end());
			___water_test(b16 + b16e.size() == b16e.end());
			___water_test(equal(a16 + 0, a16e.end(), b16 + 0, b16e.end()));
			l16 = utf_length<16>(static_cast<char16_t const*>(a16), a16e.size());
			___water_test(length_test(length, l16, original[0]));
			l16 = utf_length<16>(static_cast<char16_t const*>(b16), static_cast<char16_t const*>(b16e.end()));
			___water_test(length_test(length, l16, original[0]));
				{
				auto i = utf_iterator_with_end<16, char8_t const*, true>(a8, a8 + length.utf8());
				___water_test(equal(a16 + 0, a16e.end(), i, end(i)));
				auto l = utf_length<16>(i, end(i));
				___water_test(length_test(length, l, original[0]));
				}
				{
				auto i = utf_iterator_with_end<16, char8_t const*, false>(a8, a8 + length.utf8());
				___water_test(equal(a16 + 0, a16e.end(), i, end(i)));
				auto l = utf_length<16>(i, end(i));
				___water_test(length_test(length, l, original[0]));
				}
				{
				auto i = utf_iterator_with_size<16, char8_t const*, true>(a8, length.utf8());
				___water_test(equal(a16 + 0, a16e.end(), i, end(i)));
				auto l = utf_length<16>(i, length.utf16());
				___water_test(length_test(length, l, original[0]));
				}
				{
				auto i = utf_iterator_with_size<16, char8_t const*, false>(a8, length.utf8());
				___water_test(equal(a16 + 0, a16e.end(), i, end(i)));
				auto l = utf_length<16>(i, length.utf16());
				___water_test(length_test(length, l, original[0]));
				}
			a16e = b16e;
			conversions += 12;
		
			// 32 from 8
			auto a32e = utf_from_utf_verify<32, 8, true>(static_cast<char32_t*>(a32), static_cast<char8_t const*>(a8), length.utf8());
			auto b32e = utf_from_utf_verify<32, 8, false>(static_cast<char32_t*>(b32), static_cast<char8_t const*>(a8), length.utf8());
			___water_test(a32 + a32e.size() == a32e.end());
			___water_test(b32 + b32e.size() == b32e.end());
			___water_test(equal(a32 + 0, a32e.end(), b32 + 0, b32e.end()));
			auto l32 = utf_length<32>(static_cast<char32_t const*>(a32), a32e.size());
			___water_test(length_test(length, l32, original[0]));
			l32 = utf_length<32>(static_cast<char32_t const*>(b32), static_cast<char32_t const*>(b32e.end()));
			___water_test(length_test(length, l32, original[0]));
				{
				auto i = utf_iterator_with_end<32, char8_t const*, true>(a8, a8 + length.utf8());
				___water_test(equal(a32 + 0, a32e.end(), i, end(i)));
				auto l = utf_length<32>(i, end(i));
				___water_test(length_test(length, l, original[0]));
				}
				{
				auto i = utf_iterator_with_end<32, char8_t const*, false>(a8, a8 + length.utf8());
				___water_test(equal(a32 + 0, a32e.end(), i, end(i)));
				auto l = utf_length<32>(i, end(i));
				___water_test(length_test(length, l, original[0]));
				}
				{
				auto i = utf_iterator_with_size<32, char8_t const*, true>(a8, length.utf8());
				___water_test(equal(a32 + 0, a32e.end(), i, end(i)));
				auto l = utf_length<32>(i, length.utf32());
				___water_test(length_test(length, l, original[0]));
				}
				{
				auto i = utf_iterator_with_size<32, char8_t const*, false>(a8, length.utf8());
				___water_test(equal(a32 + 0, a32e.end(), i, end(i)));
				auto l = utf_length<32>(i, length.utf32());
				___water_test(length_test(length, l, original[0]));
				}
			a32e = b32e;
			conversions += 12;
		
			// 8 from 8
				{
				char8_t original8[0x400];
				memcpy(original8, a8, sizeof(original8));
				auto a8e = utf_from_utf_verify<8, 8, true>(forward(a8), forward(original8), length.utf8());
				auto b8e = utf_from_utf_verify<8, 8, false>(forward(b8), forward(original8), length.utf8());
				___water_test(a8e.size() == length.utf8());
				___water_test(b8e.size() == length.utf8());
				___water_test(equal(forward(a8), a8e.end(), forward(b8), b8e.end()));
				l8 = utf_length<8>(forward(a8), a8e.size());
				___water_test(length_test(length, l8, original[0]));
				l8 = utf_length<8>(forward(b8), b8e.end());
				___water_test(length_test(length, l8, original[0]));
					{
					auto i = utf_iterator_with_end<8, decltype(forward(original8)), true>(forward(original8), forward(original8 + length.utf8()));
					___water_test(equal(forward(a8), a8e.end(), i, end(i)));
					auto l = utf_length<8>(i, end(i));
					___water_test(length_test(length, l, original[0]));
					}
					{
					auto i = utf_iterator_with_end<8, decltype(forward(original8)), false>(forward(original8), forward(original8 + length.utf8()));
					___water_test(equal(forward(a8), a8e.end(), i, end(i)));
					auto l = utf_length<8>(i, end(i));
					___water_test(length_test(length, l, original[0]));
					}
					{
					auto i = utf_iterator_with_size<8, decltype(forward(original8)), true>(forward(original8), length.utf8());
					___water_test(equal(forward(a8), a8e.end(), i, end(i)));
					auto l = utf_length<8>(i, length.utf8());
					___water_test(length_test(length, l, original[0]));
					}
					{
					auto i = utf_iterator_with_size<8, decltype(forward(original8)), false>(forward(original8), length.utf8());
					___water_test(equal(forward(a8), a8e.end(), i, end(i)));
					auto l = utf_length<8>(i, length.utf8());
					___water_test(length_test(length, l, original[0]));
					}
				a8e = b8e;
				conversions += 12;
				}
		
			// 16 from 16
				{
				char16_t original16[0x200];
				memcpy(original16, a16, sizeof(original16));
				auto a16e = utf_from_utf_verify<16, 16, true>(forward(a16), forward(original16), length.utf16());
				auto b16e = utf_from_utf_verify<16, 16, false>(forward(b16), forward(original16), length.utf16());
				___water_test(a16e.size() == length.utf16());
				___water_test(b16e.size() == length.utf16());
				___water_test(equal(forward(a16), a16e.end(), forward(b16), b16e.end()));
				l16 = utf_length<16>(forward(a16), a16e.size());
				___water_test(length_test(length, l16, original[0]));
				l16 = utf_length<16>(forward(b16), b16e.end());
				___water_test(length_test(length, l16, original[0]));
					{
					auto i = utf_iterator_with_end<16, decltype(forward(original16)), true>(forward(original16), forward(original16 + length.utf16()));
					___water_test(equal(forward(a16), a16e.end(), i, end(i)));
					auto l = utf_length<16>(i, end(i));
					___water_test(length_test(length, l, original[0]));
					}
					{
					auto i = utf_iterator_with_end<16, decltype(forward(original16)), false>(forward(original16), forward(original16 + length.utf16()));
					___water_test(equal(forward(a16), a16e.end(), i, end(i)));
					auto l = utf_length<16>(i, end(i));
					___water_test(length_test(length, l, original[0]));
					}
					{
					auto i = utf_iterator_with_size<16, decltype(forward(original16)), true>(forward(original16), length.utf16());
					___water_test(equal(forward(a16), a16e.end(), i, end(i)));
					auto l = utf_length<16>(i, length.utf16());
					___water_test(length_test(length, l, original[0]));
					}
					{
					auto i = utf_iterator_with_size<16, decltype(forward(original16)), false>(forward(original16), length.utf16());
					___water_test(equal(forward(a16), a16e.end(), i, end(i)));
					auto l = utf_length<16>(i, length.utf16());
					___water_test(length_test(length, l, original[0]));
					}
				a16e = b16e;
				conversions += 12;
				}
		
			// 32 from 32
				{
				auto a32e = utf_from_utf_verify<32, 32, true>(forward(a32), forward(original), length.utf32());
				auto b32e = utf_from_utf_verify<32, 32, false>(forward(b32), forward(original), length.utf32());
				___water_test(a32e.size() == length.utf32());
				___water_test(b32e.size() == length.utf32());
				___water_test(equal(forward(a32), a32e.end(), forward(b32), b32e.end()));
				l32 = utf_length<32>(forward(a32), a32e.size());
				___water_test(length_test(length, l32, original[0]));
				l32 = utf_length<32>(forward(b32), b32e.end());
				___water_test(length_test(length, l32, original[0]));
					{
					auto i = utf_iterator_with_end<32, decltype(forward(original)), true>(forward(original), forward(original + length.utf32()));
					___water_test(equal(forward(a32), a32e.end(), i, end(i)));
					auto l = utf_length<32>(i, end(i));
					___water_test(length_test(length, l, original[0]));
					}
					{
					auto i = utf_iterator_with_end<32, decltype(forward(original)), false>(forward(original), forward(original + length.utf32()));
					___water_test(equal(forward(a32), a32e.end(), i, end(i)));
					auto l = utf_length<32>(i, end(i));
					___water_test(length_test(length, l, original[0]));
					}
					{
					auto i = utf_iterator_with_size<32, decltype(forward(original)), true>(forward(original), length.utf32());
					___water_test(equal(forward(a32), a32e.end(), i, end(i)));
					auto l = utf_length<32>(i, length.utf32());
					___water_test(length_test(length, l, original[0]));
					}
					{
					auto i = utf_iterator_with_size<32, decltype(forward(original)), false>(forward(original), length.utf32());
					___water_test(equal(forward(a32), a32e.end(), i, end(i)));
					auto l = utf_length<32>(i, length.utf32());
					___water_test(length_test(length, l, original[0]));
					}
				a32e = b32e;
				conversions += 12;
				}

			converted_codepoints += conversions * 0x100;
			if(++hi == 0xd8) hi = 0xe0;
			} while(hi != 0x1100);
		}
	
	template<typename iterator_> downgrade_iterators::forward<iterator_> forward(iterator_ a) {
		return downgrade_iterators::forward<iterator_>{a};
		}
	
	template<typename iterator1_, typename iterator2_> bool equal(iterator1_ b1, iterator1_ e1, iterator2_ b2, iterator2_ e2) {
		while(b1 != e1 && b2 != e2 && *b1 == *b2) ++b1, ++b2;
		return b1 == e1 && b2 == e2;
		}
	
	template<unsigned a_, unsigned b_> bool length_test(utf_length<a_> a, utf_length<b_> b, char32_t c) {
		if(!a || !b)
			return false;
		if(a.utf8() != b.utf8() || a.utf16() != b.utf16() || a.utf16() != b.utf16())
			return false;
		if(a.utf32() != 0x100)
			return false;
		if(c <= 0x7f && a.utf8() != 384)
			return false;
		if(0x80 <= c && c <= 0x7ff && a.utf8() != a.utf32() * 2)
			return false;
		if(0x800 <= c && c <= 0xffff && a.utf8() != a.utf32() * 3)
			return false;
		if(0x10000 <= c && a.utf8() != a.utf32() * 4)
			return false;
		if(c <= 0xffff && a.utf16() != a.utf32())
			return false;
		if(0x10000 <= c && a.utf16() != a.utf32() * 2)
			return false;
		return true;
		}
	};

}}}
#endif
