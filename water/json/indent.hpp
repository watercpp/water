// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_JSON_INDENT_HPP
#define WATER_JSON_INDENT_HPP
#include <water/json/bits.hpp>
namespace water { namespace json {

template<typename to_, typename iterator_> void
 indent(to_ to, iterator_ begin, iterator_ end, size_t spaces_or_tab = 2) {
	// to(*begin)
	//
	// spaces_or_tab == 0 means minify
	// spaces_or_tab == '\t' means use 1 tab (is 9)
	// spaces_or_tab == something else means use that many spaces
	using char_ = typename types::no_reference<types::no_const<decltype(*begin)>>::result;
	char_ last = 0;
	char_ const space = spaces_or_tab == '\t' ? '\t' : ' ';
	size_t const spaces = spaces_or_tab == '\t' ? 1 : spaces_or_tab;
	size_t indent = 0;
	bool quotes = false;
	while(begin != end) {
		if(!quotes) {
			while(static_cast<char32_t>(*begin) <= 0x20 && ++begin != end);
			if(begin == end)
				break;
			}
		char_ c = *begin;
		bool was_quotes = quotes;
		if(quotes)
			quotes = c != '"' || last == '\\';
		else if(spaces) {
			bool linebreak = false;
			if((last == '[' && c != ']') || (last == '{' && c != '}')) {
				// indent, unless the current is ] or }
				linebreak = true;
				++indent;
				}
			else if((c == ']' && last != '[') || (c == '}' && last != '{')) {
				linebreak = true;
				if(indent) --indent;
				}
			else if(last == ',')
				linebreak = true;
			if(linebreak) {
				to('\n');
				size_t s = spaces * indent;
				while(s) {
					to(static_cast<char_>(space));
					--s;
					}
				}
			}
		if(!was_quotes)
			quotes = c == '"';
		to(c);
		if(spaces && !quotes && c == ':') to(static_cast<char_>(' ')); // space after a :
		last = last == '\\' ? 0 : c;
		++begin;
		}
	}

}}
#endif
