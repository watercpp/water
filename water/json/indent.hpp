// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_JSON_INDENT_HPP
#define WATER_JSON_INDENT_HPP
#include <water/json/bits.hpp>
namespace water { namespace json {

// Indent or minify json text.
// It can be used directly with json text, or as a "to" for json::write or json::write_unbuffered
//
// template to_ can be a pointer or a reference or a plain class type. This writes 1 character at a
// time to to_, it must have function call operator like this:
//   to(char_type a)
//
// constructor argument spaces_or_tag:
// spaces_or_tab == 0 means minify
// spaces_or_tab == '\t' means use 1 tab (is 9)
// spaces_or_tab == something else means use that many spaces

template<typename to_> class
 indent {
	to_ my;
	size_t myspaces;
	size_t myindent = 0;
	char32_t mylast = 0;
	char const myspace;
	bool myquotes = false;
	public:
		indent(to_ const& to, size_t spaces_or_tab = 2) :
			my{to},
			myspaces{spaces_or_tab == '\t' ? 1 : spaces_or_tab},
			myspace{spaces_or_tab == '\t' ? '\t' : ' '}
			{}
		indent& spaces(size_t a) {
			myspaces = a;
			myspace = ' ';
			return *this;
			}
		indent& tab() {
			myspaces = 1;
			myspace = '\t';
			return *this;
			}
		indent& minify() {
			myspace = 0;
			return *this;
			}
		template<typename iterator_>
		 void operator()(iterator_ begin, iterator_ end) {
			while(begin != end) {
				(*this)(*begin);
				++begin;
				}
			}
		template<typename char_>
		 void operator()(char_ c) {
			if(!myquotes && static_cast<char32_t>(c) <= 0x20)
				return;
			bool was_quotes = myquotes;
			if(myquotes)
				myquotes = c != '"' || mylast == '\\';
			else if(myspaces) {
				bool linebreak = false;
				if((mylast == '[' && c != ']') || (mylast == '{' && c != '}')) {
					// indent, unless the current is ] or }
					linebreak = true;
					++myindent;
					}
				else if((c == ']' && mylast != '[') || (c == '}' && mylast != '{')) {
					linebreak = true;
					if(myindent) --myindent;
					}
				else if(mylast == ',')
					linebreak = true;
				if(linebreak) {
					to(static_cast<char_>('\n'));
					size_t s = myspaces * myindent;
					while(s) {
						to(static_cast<char_>(myspace));
						--s;
						}
					}
				}
			if(!was_quotes)
				myquotes = c == '"';
			to(c);
			if(myspaces && !myquotes && c == ':')
				to(static_cast<char_>(' ')); // space after a :
			mylast = mylast == '\\' ? 0 : static_cast<char32_t>(c);
			}
	private:
		template<typename char_> void to(char_ c) {
			to(my, c);
			}
		template<typename to2_, typename char_> void to(to2_& to, char_ c) {
			to(c);
			}
		template<typename to2_, typename char_> void to(to2_*& to, char_ c) {
			(*to)(c);
			}
	};
template<typename to_> class
 indent<to_&> : public indent<to_*> {
 	public:
 		indent(to_& to, size_t spaces_or_tab = 2) :
 			indent<to_*>{&to, spaces_or_tab}
			{}
 	};

template<typename to_> indent<to_> indent_to(to_&& to, size_t spaces_or_tab = 2) {
	return {to, spaces_or_tab};
	}

}}
#endif
