// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_XML_WRITE_HPP
#define WATER_XML_WRITE_HPP
#include <water/xml/node.hpp>
#include <water/xml/document.hpp>
namespace water { namespace xml {

namespace _ {
	struct
	 write_length {
		size_t my = 0;
		template<typename char_> bool operator()(char_) { ++my; return true; }
		template<typename char_> bool operator()(char_ const *b, char_ const* e) { my += static_cast<size_t>(e - b); return true; }
		};

	template<typename char_, typename cast_ = char_> struct
	 write_string {
		template<typename to_, size_t size_>
		 static bool to(to_& t, char const(&f)[size_]) {
			char_ c[size_ - 1];
			unsigned i = 0;
			do c[i] = static_cast<char_>(f[i]); while(++i != size_ - 1);
			return t(static_cast<char_ const*>(c), static_cast<char_ const*>(c) + i);
			}
		template<size_t size_>
		 static bool to(write_length& t, char const(&f)[size_]) {
			t.my += size_ - 1;
			return true;
			}
		};
	template<typename cast_> struct
	 write_string<char, cast_> {
		template<typename to_, size_t size_>
		 static bool to(to_& t, char const(&f)[size_]) {
			return t(static_cast<cast_ const*>(static_cast<void const*>(f)), static_cast<cast_ const*>(static_cast<void const*>(f)) + size_ - 1);
			}
		};
	template<typename cast_> struct
	 write_string<unsigned char, cast_> :
		write_string<char, unsigned char>
			{};
	template<typename cast_> struct
	 write_string<signed char, cast_> :
		write_string<char, signed char>
			{};
}


template<typename to_, typename document_> bool
 write_indented(to_ to, node<document_> nodes, bool xml_declaration = true) {
	// write nodes as xml indented with tab to to.
	// if xml_declaration is true this will write the <?xml declaration and if utf-16 the byte order mark feff.
	//
	// to must have:
	//   bool to(char_type const*, char_type const*)
	//   bool to(char_type)
	//
	// this cannot fail if to() cannot fail
	//
	using char_type = typename node<document_>::char_type;
	using write_string = _::write_string<char_type>;
	bool ok = true;
	if(xml_declaration) {
		if(node<document_>::document_type::utf16)
			ok = to(static_cast<char_type>(0xfeffu)) && write_string::to(to, "<?xml version=\"1.0\" encoding=\"UTF-16\"?>\n");
		else
			ok = write_string::to(to, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
		}
	auto n = nodes;
	char_type const tab = static_cast<char_type>('\t');
	char_type const tabs[] = { tab, tab, tab, tab, tab, tab, tab, tab };
	unsigned const tabs_size = static_cast<unsigned>(sizeof(tabs) / sizeof(tabs[0]));
	unsigned indent = 0;
	bool was_text = false;
	while(ok && n) {
		if(was_text && !n.name()) {
			ok = to(static_cast<char_type>(' '));
			}
		else {
			if(was_text)
				ok = to(static_cast<char_type>('\n'));
			unsigned i = indent;
			while(i && ok) {
				unsigned t = i < tabs_size ? i : tabs_size;
				i -= t;
				ok = to(tabs + 0, tabs + t);
				}
			}
		was_text = false;
		bool tag = false;
		bool one_value = false;
		if(n.name()) {
			tag = true;
			ok = ok && to(static_cast<char_type>('<')) && to(n.name().begin(), n.name().end());
			for(auto a = n.attributes(); a; a = a.next()) {
				ok = ok && to(static_cast<char_type>(' ')) && to(a.name().begin(), a.name().end()) && to(static_cast<char_type>('=')) && to(static_cast<char_type>('"'));
				auto
					*vb = a.value().begin(),
					*ve = a.value().end();
				while(true) {
					auto *vi = vb;
					while(vi != ve && *vi != '&' && *vi != '\'' && *vi != '"' && *vi != '<' && *vi != '>') ++vi;
					if(vi != vb)
						ok = ok && to(vb, vi);
					if(vi == ve)
						break;
					if(ok)
						switch(*vi) {
							case '<': ok = write_string::to(to, "&lt;"); break;
							case '>': ok = write_string::to(to, "&gt;"); break;
							case '&': ok = write_string::to(to, "&amp;"); break;
							case '\'': ok = write_string::to(to, "&apos;"); break;
							case '"': ok = write_string::to(to, "&quot;"); break;
							}
					vb = vi + 1;
					}
				ok = ok && to(static_cast<char_type>('"'));
				}
			if(!n.nodes())
				ok = ok && to(static_cast<char_type>('/'));
			ok = ok && to(static_cast<char_type>('>'));
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
					ok = ok && to(vb, vi);
				if(vi == ve)
					break;
				if(ok)
					switch(*vi) {
						case '<': ok = write_string::to(to, "&lt;"); break;
						case '>': ok = write_string::to(to, "&gt;"); break;
						case '&': ok = write_string::to(to, "&amp;"); break;
						}
				vb = vi + 1;
				}
			}
		if(n.nodes() && !one_value) {
			ok = ok && to(static_cast<char_type>('\n'));
			n = n.nodes();
			++indent;
			}
		else {
			if(tag) {
				if(one_value)
					ok = ok && to(static_cast<char_type>('<')) && to(static_cast<char_type>('/')) && to(n.name().begin(), n.name().end()) && to(static_cast<char_type>('>'));
				ok = ok && to(static_cast<char_type>('\n'));
				}
			while(!n.next()) {
				if(!n.in())
					return ok;
				n = n.in();
				if(was_text) {
					ok = ok && to(static_cast<char_type>('\n'));
					was_text = false; // this is a loop
					}
				if(indent) {
					unsigned i = --indent;
					while(i && ok) {
						unsigned t = i < tabs_size ? i : tabs_size;
						i -= t;
						ok = to(tabs + 0, tabs + t);
						}
					}
				ok = ok && to(static_cast<char_type>('<')) && to(static_cast<char_type>('/')) && to(n.name().begin(), n.name().end()) && to(static_cast<char_type>('>')) && to(static_cast<char_type>('\n'));
				}
			if(!n.next())
				return ok;
			n = n.next();
			}
		}
	return ok;
	}

template<typename to_, typename char_, typename allocator_> bool
 write_indented(to_ to, document<char_, allocator_>& d, bool xml_declaration = true) {
	return write_indented<to_>(to, d.nodes(), xml_declaration); // explicit to, maybe reference
	}

template<typename document_> size_t
 write_indented_length(node<document_> nodes, bool xml_declaration = true) {
	_::write_length l;
	write_indented<_::write_length&>(l, nodes);
	return l.my;
	}

template<typename char_, typename allocator_> size_t
 write_indented_length(document<char_, allocator_>& d, bool xml_declaration = true) {
	return write_indented_length(d.nodes(), xml_declaration);
	}

namespace _ {
	template<typename file_> class
	 write_to_file {
		file_ *my;
		public:
			write_to_file(file_& a) : my(&a) {}
			template<typename char_> bool operator()(char_ a) {
				return my->write(&a, sizeof(a)) == sizeof(a);
				}
			template<typename char_> bool operator()(char_ const *b, char_ const* e) {
				size_t s = static_cast<size_t>(e - b) * sizeof(char_);
				return my->write(b, s) == s;
				}
		};
	}

template<typename file_> _::write_to_file<file_>
 write_to_file(file_& a) {
	// needs a->write(void const*, size_t) that returns size_t, the number of bytes written
	return { a };
	}

namespace _ {
	template<typename str_> class
	 write_to_str {
		str_ *my;
		public:
			write_to_str(str_& a) : my(&a) {}
			template<typename char_> bool operator()(char_ a) { (*my)(a); return true; }
			template<typename char_> bool operator()(char_ const *b, char_ const* e) { (*my)(b, e); return true; }
		};
	}

template<typename str_> _::write_to_str<str_>
 write_to_str(str_& a) {
	// use with str::out like write_indented(write_to_str(out), nodes);
	return { a };
	}

}}
#endif
