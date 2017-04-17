// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_XML_TESTS_UNICODE_HPP
#define WATER_XML_TESTS_UNICODE_HPP
#include <water/xml/xml.hpp>
#include <water/str/str.hpp>
#include <water/allocator_nothrow.hpp>
namespace water { namespace xml { namespace tests {
/*
<unicode>
	<plane number="0x0">
		<ascii first="0x20" last="0x73">....</ascii>
		<codepoints first="0x20" last="0x73">....</byte1>
	</plane>
	<plane number="0x1">....</plane>
	....
</unicode>
*/

class
 unicode {
	using char8_t = unsigned char;
	document<char8_t, allocator_nothrow> my8;
	document<char16_t, allocator_nothrow> my16;
	char mynumber[16];
	struct memory {
		void *pointer;
		size_t size;
		char8_t* allocate(size_t bytes) {
			return static_cast<char8_t*>(pointer = allocator_nothrow{}.allocate(size = bytes));
			}
		void copy(memory& a) {
			memcpy(allocate(a.size), a.pointer, a.size);
			}
		void flip_endian() {
			char8_t *c = static_cast<char8_t*>(pointer);
			size_t s = size / 2;
			while(s) {
				char8_t cs = c[0];
				c[0] = c[1];
				c[1] = cs;
				c += 2;
				--s;
				}
			}
		bool operator==(memory const& a) const {
			if(size != a.size)
				return false;
			char8_t const
				*x = static_cast<char8_t const*>(pointer),
				*y = static_cast<char8_t const*>(a.pointer),
				*xe = x + size;
			while(x != xe && *x == *y) ++x, ++y;
			return x == xe;
			}
		} mymemory[16] = {};
	struct memory_write {
		char8_t *my;
		bool operator()(char8_t a) {
			*my++ = a;
			return true;
			}
		bool operator()(char8_t const *b, char8_t const *e) {
			while(b != e) *my++ = *b++;
			return true;
			}
		bool operator()(char16_t a) {
			*my++ = static_cast<char8_t>(a >> 8);
			*my++ = static_cast<char8_t>(a & 0xff);
			return true;
			}
		bool operator()(char16_t const *b, char16_t const *e) {
			while(b != e) {
				*my++ = static_cast<char8_t>(*b >> 8);
				*my++ = static_cast<char8_t>(*b & 0xff);
				++b;
				}
			return true;
			}
		};
	public:
		~unicode() {
			auto *m = mymemory;
			while(m->pointer) {
				allocator_nothrow{}.free(m->pointer, m->size);
				++m;
				}
			}
		template<typename o_>
		 void operator()(str::out<o_>& o) {
			o << "water::xml::tests::unicode start" << str::el;
			
			// the generated contains cr nel and 0x2028, this will be gone from the others
			generate();
			o << "generated UTF-16 document. memory_use=" << my16.memory_use() << " memory_unused=" << my16.memory_unused() << " memory_allocations=" << my16.memory_allocations() << str::el;
			auto memory16 = write_to_memory(o, my16);
			
			// parse 8 from 16
			auto node8 = my8.parse_in_place(memory16->pointer, memory16->size);
			___water_assert(node8);
			o << "parsed UTF-8 document from UTF-16 text. memory_use=" << my8.memory_use() << " memory_unused=" << my8.memory_unused() << " memory_allocations=" << my8.memory_allocations() << str::el;
			auto memory8 = write_to_memory(o, my8);

			// parse 16 from 8
			my16.clear();
			auto node16 = my16.parse_in_place(memory8->pointer, memory8->size);
			___water_assert(node16);
			o << "parsed UTF-16 document from UTF-8 text. memory_use=" << my16.memory_use() << " memory_unused=" << my16.memory_unused() << " memory_allocations=" << my16.memory_allocations() << str::el;
			auto memory16b = write_to_memory(o, my16);
			
			// parse 16 from 16
			my16.clear();
			memory16b[1].copy(memory16b[0]);
			node16 = my16.parse_in_place(memory16b[1].pointer, memory16b[1].size);
			___water_assert(node16);
			o << "parsed UTF-16 document from UTF-16 text. memory_use=" << my16.memory_use() << " memory_unused=" << my16.memory_unused() << " memory_allocations=" << my16.memory_allocations() << str::el;
			auto memory16c = write_to_memory(o, my16);
			if(*memory16b == *memory16c)
				o << "  success, memory is the same" << str::el;
			else
				o << "  error :( memory is not same" << str::el;
			
			// parse 8 from 8
			my8.clear();
			memory16c[1].copy(memory8[0]);
			node8 = my8.parse_in_place(memory16c[1].pointer, memory16c[1].size);
			___water_assert(node8);
			o << "parsed UTF-8 document from UTF-8 text. memory_use=" << my8.memory_use() << " memory_unused=" << my8.memory_unused() << " memory_allocations=" << my8.memory_allocations() << str::el;
			auto memory8b = write_to_memory(o, my8);
			if(*memory8b == *memory8)
				o << "  success, memory is the same" << str::el;
			else
				o << "  error :( memory is not same" << str::el;
			
			my16.clear();
			memory8b[1].copy(memory16c[0]);
			memory8b[1].flip_endian();
			node16 = my16.parse_in_place(memory8b[1].pointer, memory8b[1].size);
			___water_assert(node16);
			o << "parsed UTF-16 document from UTF-16 text, flipped endian. memory_use=" << my16.memory_use() << " memory_unused=" << my16.memory_unused() << " memory_allocations=" << my16.memory_allocations() << str::el;
			auto memory16d = write_to_memory(o, my16);
			if(*memory16b == *memory16d)
				o << "  success, memory is the same" << str::el;
			else
				o << "  error :( memory is not same" << str::el;
			
			
			// parse 16 from 8 and 16 buffer, compare to original 16
			// parse 8 from 8 buffer, compare to original 8
			
			o << "water::xml::tests::unicode done" << str::el;
			}
		void const* utf8_memory_pointer() const {
			return mymemory[1].pointer;
			}
		size_t utf8_memory_size() const {
			return mymemory[1].size;
			}
		void const* utf16_memory_pointer() const {
			return mymemory[0].pointer;
			}
		size_t utf16_memory_size() const {
			return mymemory[0].size;
			}
	private:
		template<typename o_, typename document_>
		 memory* write_to_memory(str::out<o_>& o, document_& d) {
			auto *m = mymemory;
			while(m->pointer)
				++m;
			size_t bytes = write_indented_length(d);
			if(document_::utf16)
				bytes *= 2;
			memory_write w { m->allocate(bytes) };
			write_indented<memory_write&>(w, d);
			o << "write_indented " << (document_::utf16 ? "UTF-16" : "UTF-8") << " document to memory, wrote " << (w.my - static_cast<char8_t*>(m->pointer)) << " bytes" << str::el;
			___water_assert(w.my == static_cast<char8_t*>(m->pointer) + m->size);
			if(w.my != static_cast<char8_t*>(m->pointer) + m->size)
				o << " error :( should have written " << m->size << " bytes!" << str::el;
			return m;
			}
		void generate() {
			my8.clear();
			my16.clear();
			my8.memory_block_size(0x10000);
			my16.memory_block_size(0x10000);
			
			auto root = my16.node().name("unicode");
			my16.nodes(root);
			
			auto plane = my16.node().name("plane");
			root.nodes(plane);
			plane.attributes(my16.node().name("number").value(number(0)));
			
			unsigned char const ascii[] {
				'h', 'e', 'l', 'l', 'o', // will keep whitespace below
				0x9, 0xa, 0xd, // tab lf cr
				0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
				0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
				0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f,
				0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f,
				0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
				0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e
				};
			auto n = my16.node().name("ascii_codepoints").first_value(ascii);
			plane.nodes(n);
			n.attributes(my16.node().name("first").value(number(0x9)));
			n.attributes(my16.node().name("last").value(number(0x7e)));
			
			unsigned char byte1[0x60]; // 0xa0 - 0xff
			unsigned char b = 0;
			do byte1[b] = 0xa0 + b; while(++b != 0x60);
			n = my16.node().name("codepoints").first_value(byte1);
			plane.nodes(n);
			n.attributes(my16.node().name("first").value(number(0xa0)));
			n.attributes(my16.node().name("last").value(number(0xff)));
			
			char16_t
				chars[0x200],
				*c = chars;
			char32_t u = 0x100;
			do {
				if(u == 0xd800)
					u = 0xe000;
				*c++ = u;
				if((u & 0xff) == 0xff || u == 0xfffd || u == 0xfdcf) { // fdd0-fdef are non-characters
					n = my16.node().name("codepoints").first_value(chars, c);
					plane.nodes(n);
					n.attributes(my16.node().name("first").value(number(chars[0])));
					n.attributes(my16.node().name("last").value(number(u)));
					c = chars;
					if(u == 0xfdcf)
						u = 0xfdef;
					}
				} while(++u != 0xfffe);
			
			char32_t hi = 0x10000;
			do {
				c = chars;
				u = 0;
				auto plane = my16.node().name("plane");
				root.nodes(plane);
				plane.attributes(my16.node().name("number").value(number(hi >> 16)));
				do {
					char32_t x = (hi | u) - 0x10000;
					*c++ = static_cast<char16_t>(0xd800 | ((x >> 10) & 0x3ff));
					*c++ = static_cast<char16_t>(0xdc00 | (x & 0x3ff));
					if((u & 0xff) == 0xff || u == 0xfffd) {
						n = my16.node().name("codepoints").first_value(chars, c);
						plane.nodes(n);
						n.attributes(my16.node().name("first").value(number((hi | u) & 0xffff00)));
						n.attributes(my16.node().name("last").value(number(hi | u)));
						c = chars;
						}
					} while(++u != 0xfffe);
				hi += 0x10000;
				} while(hi != 0x110000);
			}
		char const* number(unsigned a) {
			str::out<str::begin_end<char*> > x(mynumber + 0, mynumber + sizeof(mynumber));
			x << str::number(a, str::settings().base(16)) << char(0);
			return mynumber;
			}
	};


}}}
#endif
