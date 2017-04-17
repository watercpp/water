// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_XML_TESTS_PARSE_HPP
#define WATER_XML_TESTS_PARSE_HPP
#include <water/xml/xml.hpp>
#include <water/str/str.hpp>
#include <water/allocator_nothrow.hpp>
#ifdef WATER_NO_CHEADERS
	#include <stdio.h>
#else
	#include <cstdio>
#endif
namespace water { namespace xml { namespace tests {

#ifndef WATER_NO_CHEADERS
using std::fopen;
using std::fclose;
using std::fseek;
using std::ftell;
using std::fread;
using std::fputs;
using std::fflush;
#endif

template<typename out_> struct
 parse_callback {
	out_ *my;
	template<typename node_>
	 void operator()(node_ n) { // not const function, just to test
		if(my) {
			unsigned indent = 0;
			node_ i = n;
			while(i = i.in()) ++indent;
			*my << str::repeat(' ', indent * 2) << '<' << n.name();
			i = n.attributes();
			while(i) {
				*my << ' ' << i.name() << "=\"" << i.value() << "\"";
				i = i.next();
				}
			*my << '>' << str::el;
			}
		}
	};

template<typename char_> class
 parse {
	unsigned
		success = 0,
		failed = 0,
		errors = 0;
	struct close {
		FILE *file;
		~close() { if(file) fclose(file); }
		};
	public:
		template<typename o_>
		 void file(str::out<o_>& o, char const *file_path, bool quiet = false, bool tags = false) {
			o << file_path << str::el;
			close file { fopen(file_path, "rb") };
			long file_size;
			if(file.file && !fseek(file.file, 0, SEEK_END) && (file_size = ftell(file.file)) > 0 && !fseek(file.file, 0, SEEK_SET)) {
				size_t bytes = static_cast<size_t>(file_size);
				xml::document<char_, water::allocator_nothrow> document;
				void *memory = document.allocate(bytes);
				if(!memory)
					o << "allocation of " << bytes << " bytes failed!\n";
				else if(fread(memory, 1, bytes, file.file) != bytes)
					o << "file read failed!\n";
				else {
					xml::encoding e(memory, bytes);
					if(!quiet) {
						if(e.bits() <= 8 || 32 < e.bits())
							o << str::string(static_cast<char*>(memory), static_cast<char*>(memory) + bytes);
						else if(e.bits() <= 16)
							o << str::string(static_cast<char16_t const*>(memory), static_cast<char16_t const*>(memory) + bytes / sizeof(char16_t));
						else if(e.bits() <= 32)
							o << str::string(static_cast<char32_t const*>(memory), static_cast<char32_t const*>(memory) + bytes / sizeof(char32_t));
						o << '\n' << str::el;
						}
					o << "encoding ";
					if(e.name()) o << e.name() << " ";
					o << "bits=" << e.bits() << " ";
					if(e.utf8()) o << "utf-8 ";
					if(e.utf16()) o << "utf-16 ";
					if(e.utf32()) o << "utf-32 ";
					if(e.big_endian()) o << "big_endian ";
					if(e.litte_endian()) o << "litte_endian ";
					if(e.byte_order_mark()) o << "byte_order_mark=" << e.byte_order_mark();
					o << str::el;
					if(document.parse_in_place(memory, bytes, parse_callback<str::out<o_> >{ tags ? &o : 0 })) {
						++success;
						o << "parse success! "
							<< write_indented_length(document) << " characters, "
							<< document.memory_allocations() << " allocations, " << document.memory_use() << " bytes, " << document.memory_unused() << " unused bytes"
							<< str::el;
						if(!quiet) {
							write_indented(write_to_str(o), document);
							o	<< str::el;
							}
						}
					else if(e.utf8() || e.utf16()) {
						++errors;
						o << "parse error!" << str::el;
						}
					else {
						++failed;
						o << "parse failed! probably encoding?" << str::el;
						}
					}
				}
			else
				o << "could not read file, maybe a directory or 0 byte file?" << str::el;
			if(quiet)
				o << str::el;
			else
				o << "\n\n................................................................\n" << str::el;
			}
		template<typename o_>
		 void summary(str::out<o_>& o) {
			o << "success " << success << '\n'
				<< "failed  " << failed << '\n'
				<< "errors  " << errors << '\n' << str::el;
			}
	};

struct out_write {
	void operator()(char const* a) const {
		fputs(a, stdout);
		fflush(stdout);
		}
	template<typename a_> void operator()(char const* a, a_) const {
		(*this)(a);
		}
	};
	
using out_type = str::out<str::buffer_lines<out_write>>;

inline bool cequal(char const *a, char const *b) {
	if(a == b)
		return true;
	if(!a || !b)
		return false;
	while(*a && *b && *a == *b)
		++a, ++b;
	return !*a && !*b;
	}

inline int parse_main(int c, char **v) {
	out_type o;
	if(c <= 1)
		o << "Parse xml files. Use -utf16 to parse into UTF-16 instead of UTF-8. Use -quiet to output less. Use -tags to output tag names as they are parsed\n";
	else {
		o << '\n';
		bool
			utf16 = cequal(v[1], "-utf16") || c > 2 && cequal(v[2], "-utf16") || c > 3 && cequal(v[3], "-utf16"),
			quiet = cequal(v[1], "-quiet") || c > 2 && cequal(v[2], "-quiet") || c > 3 && cequal(v[3], "-quiet"),
			tags = cequal(v[1], "-tags") || c > 2 && cequal(v[2], "-tags") || c > 3 && cequal(v[3], "-tags");
		int i = 1 + utf16 + quiet + tags;
		if(utf16) {
			parse<char16_t> p;
			while(i < c)
				p.file(o, v[i++], quiet, tags);
			p.summary(o);
			}
		else {
			parse<char> p;
			while(i < c)
				p.file(o, v[i++], quiet, tags);
			p.summary(o);
			}
		}
	return 0;
	}

}}}
#endif
