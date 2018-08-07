// Copyright 2017-2018 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_XML_WRITE_HPP
#define WATER_XML_WRITE_HPP
#include <water/xml/node.hpp>
namespace water { namespace xml {

namespace _ {
	template<typename char_, size_t size_> class
	 write_array {
	 	char_ my[size_];
	 	public:
	 		write_array(char const *a) {
	 			char_ *c = my;
	 			do *c = static_cast<char_>(*a++); while(++c != my + size_);
	 			}
	 		char_ const* begin() const {
	 			return my;
	 			}
	 		char_ const* end() const {
	 			return my + size_;
	 			}
	 	};
	
	template<typename char_, size_t size_> write_array<char_, size_ - 1> write_cstring(char const (&a)[size_]) {
		return {a};
		}

	template<typename char_, unsigned utf_> struct
	 write_xml_declaration {
		template<typename to_> static void to(to_& to) {
			to(static_cast<char_>(0xfeffu));
			auto c = utf_ == 16 ?
				_::write_cstring<char_>("<?xml version=\"1.0\" encoding=\"UTF-16\"?>") :
				_::write_cstring<char_>("<?xml version=\"1.0\" encoding=\"UTF-32\"?>");
			to(c.begin(), c.end());
			}
		};
	template<typename char_> struct
	 write_xml_declaration<char_, 8> {
		template<typename to_> static void to(to_& to) {
			auto c = _::write_cstring<char_>("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
			to(c.begin(), c.end());
			}
		};

}

template<typename to_, typename char_>
 void write_unbuffered(to_&& to, node<char_> nodes, bool xml_declaration = true, unsigned indent_spaces_or_tab = '\t' /*is 9*/) {
	// write nodes as xml to to. to must have function call operators like this:
	//   to(char_ const*, char_ const*)
	//   to(char_)
	//
	// xml_declaration: if true this will write the <?xml declaration and if UTF 16 or 32 the byte order mark feff.
	//
	// indent_spaces_or_tab: indentation.
	// - 0 means dont indent
	// - '\t' or 9 means use tab
	// - anything else means use that many spaces
	//
	// water::str::out can be used for to
	//
	// this cannot fail if to() cannot fail
	//
	if(xml_declaration) {
		_::write_xml_declaration<char_, unicode::utf_from_char<char_>::result>::to(to);
		if(indent_spaces_or_tab)
			to(static_cast<char_>('\n'));
		}
	char_ space = static_cast<char_>(indent_spaces_or_tab == '\t' ? '\t' : ' ');
	unsigned spaces = indent_spaces_or_tab == '\t' ? 1 : indent_spaces_or_tab;
	unsigned indent = 0;
	auto lt = _::write_cstring<char_>("&lt;");
	auto gt = _::write_cstring<char_>("&gt;");
	auto amp = _::write_cstring<char_>("&amp;");
	auto apos = _::write_cstring<char_>("&apos;");
	auto quot = _::write_cstring<char_>("&quot;");
	bool was_text = false;
	auto n = nodes;
	while(n) {
		if(was_text && !n.name())
			to(static_cast<char_>(' '));
		else if(indent_spaces_or_tab) {
			if(was_text)
				to(static_cast<char_>('\n'));
			unsigned s = indent * spaces;
			while(s) {
				to(space);
				--s;
				}
			}
		was_text = false;
		bool tag = false;
		bool one_value = false;
		if(n.name()) {
			tag = true;
			to(static_cast<char_>('<'));
			to(n.name().begin(), n.name().end());
			for(auto a = n.attributes(); a; a = a.next()) {
				to(static_cast<char_>(' '));
				to(a.name().begin(), a.name().end());
				to(static_cast<char_>('='));
				to(static_cast<char_>('"'));
				auto
					*vb = a.value().begin(),
					*ve = a.value().end();
				while(true) {
					auto *vi = vb;
					while(vi != ve && *vi != '&' && *vi != '\'' && *vi != '"' && *vi != '<' && *vi != '>') ++vi;
					if(vi != vb)
						to(vb, vi);
					if(vi == ve)
						break;
					switch(*vi) {
						case '<': to(lt.begin(), lt.end()); break;
						case '>': to(gt.begin(), gt.end()); break;
						case '&': to(amp.begin(), amp.end()); break;
						case '\'': to(apos.begin(), apos.end()); break;
						case '"': to(quot.begin(), quot.end()); break;
						}
					vb = vi + 1;
					}
				to(static_cast<char_>('"'));
				}
			if(!n.nodes())
				to(static_cast<char_>('/'));
			to(static_cast<char_>('>'));
			one_value = n.nodes() && !n.nodes().next() && !n.nodes().name();
			}
		if(!tag || one_value) {
			// does not really matter if value is empty
			auto value = one_value ? n.nodes().value() : n.value();
			auto
				*vb = value.begin(),
				*ve = value.end();
			was_text = !one_value && vb != ve;
			while(true) {
				auto *vi = vb;
				while(vi != ve && *vi != '&' && *vi != '<' && *vi != '>') ++vi;
				if(vi != vb)
					to(vb, vi);
				if(vi == ve)
					break;
				switch(*vi) {
					case '<': to(lt.begin(), lt.end()); break;
					case '>': to(gt.begin(), gt.end()); break;
					case '&': to(amp.begin(), amp.end()); break;
					}
				vb = vi + 1;
				}
			}
		if(n.nodes() && !one_value) {
			if(indent_spaces_or_tab) {
				to(static_cast<char_>('\n'));
				++indent;
				}
			n = n.nodes();
			}
		else {
			if(tag) {
				if(one_value) {
					to(static_cast<char_>('<'));
					to(static_cast<char_>('/'));
					to(n.name().begin(), n.name().end());
					to(static_cast<char_>('>'));
					}
				if(indent_spaces_or_tab)
					to(static_cast<char_>('\n'));
				}
			while(!n.next()) {
				if(!n.in())
					return;
				n = n.in();
				if(was_text) {
					was_text = false; // this is a loop
					if(indent_spaces_or_tab)
						to(static_cast<char_>('\n'));
					}
				if(indent) {
					unsigned s = --indent * spaces;
					while(s) {
						to(space);
						--s;
						}
					}
				to(static_cast<char_>('<'));
				to(static_cast<char_>('/'));
				to(n.name().begin(), n.name().end());
				to(static_cast<char_>('>'));
				if(indent_spaces_or_tab)
					to(static_cast<char_>('\n'));
				}
			if(!n.next())
				return;
			n = n.next();
			}
		}
	}

template<typename to_, typename char_, typename memory_>
 void write_unbuffered(to_&& to, node<char_, memory_> nodes, bool xml_declaration = true, unsigned indent_spaces_or_tab = '\t') { // reduce template bloat?
	write_unbuffered(to, node<char_>{nodes}, xml_declaration, indent_spaces_or_tab);
	}

unsigned constexpr write_buffer_size = 1024;

template<typename to_, typename char_, typename memory_>
 size_t write(to_&& to, node<char_, memory_> nodes, bool xml_declaration = true, unsigned indent_spaces_or_tab = '\t') {
 	// write nodes as xml to to with buffering. to must have:
	//   to(char const* begin, char const* end)
 	//
 	// returns the number of characters written.
 	struct buffer_ {
		typename types::no_reference<to_>::result *to;
		unsigned at;
		size_t size;
		char_ buffer[write_buffer_size];
		void operator()(char_ a) {
			if(at == write_buffer_size)
				flush();
			buffer[at++] = a;
			}
		void operator()(char_ const *b, char_ const *e) {
			size_t s = static_cast<size_t>(e - b);
			if(at + s > write_buffer_size)
				flush();
			if(s > write_buffer_size) {
				(*to)(b, e);
				size += s;
				}
			else
				while(b != e)
					buffer[at++] = *b++;
			}
		void flush() {
			if(!at)
				return;
			auto 
				b = static_cast<char_ const*>(buffer),
				e1 = b + at,
				e2 = unicode::utf_adjust_end<unicode::utf_from_char<char_>::result>(b, e1);
			if(b == e2)
				return;
			(*to)(b, e2);
			size += static_cast<size_t>(e2 - b);
			at = 0;
			while(e1 != e2)
				buffer[at++] = *e2++;
			}
		} b;
	b.to = &to;
	b.at = 0;
	b.size = 0;
	write_unbuffered(b, nodes, xml_declaration, indent_spaces_or_tab);
	b.flush();
	___water_assert(b.at == 0);
	return b.size;
	}

template<typename char_, typename memory_>
 size_t write_size(node<char_, memory_> nodes, bool xml_declaration = true, unsigned indent_spaces_or_tab = '\t') {
	// return the number of characters write or write_unbuffered will write
	struct to_ {
		size_t size = 0;
		void operator()(char_) { ++size; }
		void operator()(char_ const *b, char_ const* e) { size += static_cast<size_t>(e - b); }
		} to;
	write_unbuffered(to, nodes, xml_declaration, indent_spaces_or_tab);
	return to.size;
	}

}}
#endif
