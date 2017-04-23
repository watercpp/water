// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_JSON_WRITE_STRING_HPP
#define WATER_JSON_WRITE_STRING_HPP
#include <water/json/node.hpp>
namespace water { namespace json {

template<typename to_> void write_hex(to_& to, char16_t u) {
	// write \u1234
	char c[6] = { '\\', 'u' };
	unsigned i = 6;
	while(--i != 1) {
		char32_t d = u & 0xf;
		if(d <= 9) d += '0';
		else d = 'a' + (d - 0xa);
		c[i] = static_cast<char>(d);
		u >>= 4;
		}
	to(c + 0, c + 6);
	}

template<typename to_> void write_string(to_ to, char8_t const* begin, char8_t const* end, bool escape_all) {
	// does not write the surrounding quotes
	auto part = begin;
	char32_t last = 0;
	while(begin != end) {
		char32_t u = 0;
		unsigned n = unicode::utf8_decode_and_move(u, begin, end);
		___water_assert(n && "bad utf8");
		if(!n) return;
		bool escape =
			u < 0x20 ||
			u == '"' ||
			u == '\\' ||
			u == 0x2028 || u == 0x2029 || // unicode whitespace, javascript needs these escaped because they are seen as linebreaks
			(last == '<' && u == '/'); // avoid </ if this is inside html <script>
		if(!escape && escape_all)
			escape = u >= 0x80 || u == '/';
		if(escape) {
			last = 0;
			auto part_end = begin - n;
			if(part != part_end)
				to(static_cast<char const*>(static_cast<void const*>(part)), static_cast<char const*>(static_cast<void const*>(part_end)));
			part = begin;
			char one = 0;
			switch(u) {
				case '"':
				case '\\':
				case '/': one = static_cast<char>(u); break;
				case '\b': one = 'b'; break; //backspace
				case '\f': one = 'f'; break; //form feed
				case '\n': one = 'n'; break;
				case '\r': one = 'r'; break;
				case '\t': one = 't'; break;
				}
			if(one) {
				char const c[] = {'\\', one};
				to(c + 0, c + 2);
				}
			else {
				char16_t u0 = 0, u1 = 0;
				if(u <= 0xffff)
					u0 = static_cast<char16_t>(u);
				else
					unicode::utf16_pack(u0, u1, u);
				write_hex(to, u0);
				if(u1)
					write_hex(to, u1);
				}
			}
		else
			last = u;
		}
	if(part != end)
		to(static_cast<char const*>(static_cast<void const*>(part)), static_cast<char const*>(static_cast<void const*>(end)));
	}

}}
#endif
