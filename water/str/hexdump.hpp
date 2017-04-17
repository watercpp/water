// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_STR_HEXDUMP_HPP
#define WATER_STR_HEXDUMP_HPP
#include <water/str/out.hpp>
namespace water { namespace str {

/*

000: 01234567 89abcdef 01234567 89abcdef hello world????
016: 01234567 89abcdef 01234567 89abcdef hello world????
032: 01234567 89...... ........ ........ hello

*/

class hexdump : public callback<hexdump> {
	void const *my;
	size_t mybytes;
	public:
		hexdump(hexdump const&) = default;
		hexdump(hexdump&&) = default;
		template<typename type_> explicit hexdump(type_ const& a) :
			my{&a},
			mybytes{sizeof(a)}
			{}
		hexdump(void const *pointer, size_t bytes) :
			my{pointer},
			mybytes{bytes}
			{}
		hexdump(void const *begin, void const *end) :
			my{begin},
			mybytes{static_cast<size_t>(static_cast<char const*>(end) - static_cast<char const*>(begin))}
			{}
		template<typename o_> void operator()(out<o_>& o) const {
			auto r = restore_settings(o);
			size_t bytes = mybytes;
			unsigned line_digits = 1;
			while(bytes /= 10) ++line_digits;
			unsigned byte_digits = numbers::max_digits<unsigned char>(16);
			auto settings_byte = settings{}.base(16).base_prefix_suffix(false).digits(byte_digits);
			auto
				b = static_cast<unsigned char const*>(my),
				i = b,
				e = b + mybytes;
			while(i != e) {
				o << settings{}.digits(line_digits) << (i - b) << u8": " << settings_byte;
				char c[18] = {static_cast<char>(u' ')}; // 18 to add newline
				unsigned	
					cs = 0,
					n = 0;
				auto ie = (e - i) < 16 ? e : i + 16;
				while(i != ie) {
					if(n == 4) {
						o << static_cast<char>(u' ');
						n = 0;
						}
					o << *i;
					c[++cs] = 0x20 <= *i && *i <= 0x7e ? static_cast<char>(*i) : static_cast<char>(u'?');
					++i;
					++n;
					}
				if(cs != 16) {
					unsigned x = cs;
					do {
						if(n == 4) {
							o << static_cast<char>(u' ');
							n = 0;
							}
						++n;
						o << repeat(static_cast<char>(u'.'), byte_digits);
						} while(++x != 16);
					}
				c[++cs] = '\n';
				o(c + 0, c + cs + 1);
				}
			}
	};

}}
#endif
