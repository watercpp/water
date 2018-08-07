// Copyright 2017-2018 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_XML_READ_HPP
#define WATER_XML_READ_HPP
#include <water/xml/memory.hpp>
#include <water/xml/encoding.hpp>
#include <water/unicode/utf_length.hpp>
#include <water/unicode/byterator.hpp>
#include <water/later.hpp>
namespace water { namespace xml {

//
// XML parser
//
// template
// - char_ must be built in unisgned integer type or char, signed char. water::unicode::utf_from_char is used to select UTF encoding
// - memory_ must be a xml::memory
//
// The callback used with parse will be called on each tag, before it's content has been parsed but
// after the attributes. It might be useful to build an index of the document. The nodes attributes can
// be modified by the callback, but do not change anything else. It should look like this:
//
//   struct callback { void operator()(node_type) {} };
//
// The callback can allocate() memory from the xml::memory this uses
//
// This will not free memory before parsing, because allocate() can be used to allocate memory. It
// is probably a good idea to clear() before parsing if the same memory is reused.
//
// This will parse text content and attribute values lazily, when they are first accessed by node::value.
// This means node::value is not really a const operation, it can modify data. To avoid this, use lazy(flalse).
// This matters if the nodes are read by mutlple threads at the same time, in that case always use lazy(false).
//
// All allocations made by this use xml::memory::alloacte_with_undo, and xml::memory::no_undo only if parsing is successful.
//
template<typename char_ = char, typename memory_ = memory<>> class
 read {
 	static_assert(unicode::utf_from_char<char_>::result, "use a built in type that works with water::unicode");
 	static_assert(unicode::utf_from_char<char_>::result != 8 || sizeof(char_) == 1, "");
	public:
		using char_type = char_;
		using memory_type = memory_;
		using node_type = node<char_type, memory_type>;
		static unsigned constexpr utf = unicode::utf_from_char<char_type>::result;
	private:
		using memory_node = xml::memory_node<char_type>;
		using uchar_ = typename memory_node::char_type;
		struct no_callback {
			void operator()(node_type const&) {}
			};
		template<unsigned> struct utf_select {};
	private:
		memory_type *mymemory;
		memory_node *my = 0;
		uchar_
			*myi = 0, // when parse fails this is probably pointing to where it failed
			*mye = 0;
		bool mydecode = false;
	public:
		read(memory_type& a) :
			mymemory{&a}
			{}
		explicit operator bool() const {
			return my != 0;
			}
		read& lazy(bool a) {
			// decode all values at once.
			mydecode = !a;
			return *this;
			}
		text<char_type const*> parse_error() const {	
			// returns the xml text remaining when parsing failed, it is probably where the error is
			if(myi == mye)
				return {};
			return {static_cast<char_type const*>(static_cast<void const*>(myi)), static_cast<char_type const*>(static_cast<void const*>(mye))};
			}
		node_type nodes() {
			return { *mymemory, my };
			}
		template<typename callback_>
		 read& parse_in_place(char_type *begin, char_type *end, callback_&& callback) {
		 	// does not verify that the range is valid unicode
			my = 0;
			myi = static_cast<uchar_*>(static_cast<void*>(begin));
			mye = static_cast<uchar_*>(static_cast<void*>(end));
			if(parse(callback))
				mymemory->no_undo();
			else
				my = 0;
			return *this;
		 	}
		read& parse_in_place(char_type *begin, char_type *end) {
			return parse_in_place(begin, end, no_callback{});
			}
		template<typename callback_>
		 read& parse_in_place(void *pointer, size_t bytes, callback_&& callback) {
			// parse raw memory
			//
			// This expects each byte of the input to contain 8 bits.
			//
			// This should figure out if the memory is a UTF-8, UTF-16 or UTF-32 encoded xml document.
			// If it is it will verfy the UTF and then parse it in place if it can. If it cannot parse in place, it will copy.
			// It will convert to the UTF this uses if the memory is not in the same UTF.
			my = 0;
			encoding e(pointer, bytes);
			pointer = static_cast<char*>(pointer) + e.byte_order_mark();
			bytes -= e.byte_order_mark();
			char_type
				*cb = 0,
				*ce = 0;
			// this function has been split into the parse_in_place_from_utfX functions to avoid
			// conditional expression is constant warning
			if(e.utf8()) {
				unicode::utf_length<8> l{static_cast<unsigned char*>(pointer), bytes, [](char32_t a) { return a != 0; }};
				if(l)
					parse_in_place_from_utf8(cb, ce, pointer, bytes, l, utf_select<utf == 8 ? 8 : 0>{});
				}
			else if(e.utf16() && !(bytes % 2)) {
				auto b = unicode::byterator<16>(pointer, e.big_endian());
				unicode::utf_length<16> l{b, bytes / 2, [](char32_t a) { return a != 0; }};
				if(l)
					parse_in_place_from_utf16(cb, ce, e, pointer, b, l, utf_select<utf == 16 && sizeof(char_type) == 2 && static_cast<char_type>(-1) == 0xffffu ? 16 : 0>{});
				}
			else if(e.utf32() && !(bytes % 4)) {
				auto b = unicode::byterator<32>(pointer, e.big_endian());
				unicode::utf_length<32> l{b, bytes / 4, [](char32_t a) { return a != 0; }};
				if(l)
					parse_in_place_from_utf32(
						cb,
						ce,
						e,
						pointer,
						b,
						l,
						utf_select<
							utf == 32 && sizeof(char_type) == 4 && static_cast<char_type>(-1) == 0xfffffffful ? 1 :
							sizeof(char_type) <= utf / 8 ? 2 : // can convert in place unless char_type is odd
							0
							>{}
						);
				}
			if(cb)
				parse_in_place(cb, ce, callback);
			return *this;
			}
		read& parse_in_place(void *pointer, size_t bytes) {
			return parse_in_place(pointer, bytes, no_callback());
			}
		template<typename forward_iterator_, typename callback_>
		 read& operator()(forward_iterator_ begin, forward_iterator_ end, callback_&& callback) {
			// copy range then parse
			// does not verify that the range is valid unicode
			return (*this)(begin, xml::size(begin, end), callback);
			}
		template<typename forward_iterator_>
		 read& operator()(forward_iterator_ begin, forward_iterator_ end) {
			return (*this)(begin, end, no_callback());
			}
		template<typename forward_iterator_, typename callback_>
		 read& operator()(forward_iterator_ begin, size_t size, callback_&& callback) {
			// copy range then parse
			// does not verify that the range is valid unicode
			my = 0;
			if(size) {
				if(char_type *a = static_cast<char_type*>(mymemory->allocate_with_undo(size * sizeof(char_type), alignof(char_type)))) {
					char_type
						*ai = a,
						*ae = a + size;
					do {
						*ai = static_cast<char_type>(*begin);
						++begin;
						} while(++ai != ae);
					parse_in_place(a, ae, callback);
					}
				}
			return *this;
			}
		template<typename forward_iterator_>
		 read& operator()(forward_iterator_ begin, size_t size) {
			return (*this)(begin, size, no_callback());
			}
	private:
		void parse_in_place_from_utf8(char_type*& cb, char_type*& ce, void *pointer, size_t, unicode::utf_length<8> l, utf_select<8>) {
			cb = static_cast<char_type*>(pointer); // char_type has size 1
			ce = cb + l.utf8();
			}
		void parse_in_place_from_utf8(char_type*& cb, char_type*& ce, void *pointer, size_t bytes, unicode::utf_length<8> l, utf_select<0>) {
			cb = static_cast<char_type*>(mymemory->allocate_with_undo((utf == 16 ? l.utf16() : l.utf32()) * sizeof(char_type), alignof(char_type)));
			if(cb)
				ce = unicode::utf_from_utf<utf, 8>(cb, static_cast<unsigned char*>(pointer), bytes).end();
			}
		void parse_in_place_from_utf16(char_type*& cb, char_type*& ce, encoding const& e, void *pointer, unicode::byterator<16> b, unicode::utf_length<16> l, utf_select<16>) {
			cb = static_cast<char_type*>(pointer);
			ce = cb + l.utf16();
			endian_change_maybe(cb, ce, b, e.big_endian());
			}
		void parse_in_place_from_utf16(char_type*& cb, char_type*& ce, encoding const&, void*, unicode::byterator<16> b, unicode::utf_length<16> l, utf_select<0>) {
			size_t cs = utf == 8 ? l.utf8() : utf == 16 ? l.utf16() : l.utf32();
			cb = static_cast<char_type*>(mymemory->allocate_with_undo(cs * sizeof(char_type), alignof(char_type)));
			if(cb)
				ce = unicode::utf_from_utf<utf, 16>(cb, b, l.utf16()).end();
			}
		void parse_in_place_from_utf32(char_type*& cb, char_type*& ce, encoding const& e, void *pointer, unicode::byterator<32> b, unicode::utf_length<32> l, utf_select<1>) {
			cb = static_cast<char_type*>(pointer);
			ce = cb + l.utf32();
			endian_change_maybe(cb, ce, b, e.big_endian());
			}
		void parse_in_place_from_utf32(char_type*& cb, char_type*& ce, encoding const&, void *pointer, unicode::byterator<32> b, unicode::utf_length<32> l, utf_select<2>) {
			// can convert in place unless char_type is odd
			cb = static_cast<char_type*>(pointer);
			ce = unicode::utf_from_utf<utf, 32>(cb, b, l.utf32()).end();
			}
		void parse_in_place_from_utf32(char_type*& cb, char_type*& ce, encoding const&, void*, unicode::byterator<32> b, unicode::utf_length<32> l, utf_select<0>) {
			size_t cs = utf == 8 ? l.utf8() : utf == 16 ? l.utf16() : l.utf32();
			cb = static_cast<char_type*>(mymemory->allocate_with_undo(cs * sizeof(char_type), alignof(char_type)));
			if(cb)
				ce = unicode::utf_from_utf<utf, 32>(cb, b, l.utf32()).end();
			}
		template<typename iterator_> void endian_change_maybe(char_type *c, char_type *ce, iterator_ f, bool from_big_endian) {
			char_type const one = 1;
			bool big_endian = *static_cast<unsigned char const*>(static_cast<void const*>(&one)) == 0;
			if(big_endian == from_big_endian)
				return;
			while(c != ce) {
				*c = static_cast<char_type>(*f);
				++c;
				++f;
				};
			}
		memory_node* create() {
			memory_node *n = static_cast<memory_node*>(mymemory->allocate_with_undo(sizeof(memory_node)));
			if(n) *n = {};
			return n;
			}
		bool is(char a) {
			if(*myi != a)
				return false;
			++myi;
			return true;
			}
		template<size_t s_>
		 bool is(char const (&a)[s_]) {
			if(mye - myi < static_cast<ptrdiff_t>(s_ - 1))
				return false;
			unsigned i = 0;
			do {
				if(static_cast<uchar_>(a[i]) != myi[i])
					return false;
				} while(++i != s_ - 1);
			myi += i;
			return true;
			}
		bool is_space(uchar_ const *i, utf_select<8>) {
			if(*i <= 0x20)
				return true;
			if(i[0] == 0xc2 && mye - i >= 2 && i[1] == 0x85)
				return true;
			if(i[0] == 0xe2 && mye - i >= 3 && i[1] == 0x80 && i[2] == 0xa8)
				return true;
			return false;
			}
		bool is_space(uchar_ const *i, utf_select<0>) {
			return *i <= 0x20 || *i == 0x85 || *i == 0x2028;
			}
		bool is_space(uchar_ const *i) {
			// i must be < mye!!
			return is_space(i, utf_select<utf == 8 ? 8 : 0>{});
			}
		bool skip_space(utf_select<8>) {
			bool did = false;
			while(myi != mye) {
				if(*myi <= 0x20)
					++myi;
				else if(myi[0] == 0xc2 && mye - myi >= 2 && myi[1] == 0x85)
					myi += 2;
				else if(myi[0] == 0xe2 && mye - myi >= 3 && myi[1] == 0x80 && myi[2] == 0xa8)
					myi += 3;
				else
					break;
				did = true;
				}
			return did;
			}
		bool skip_space(utf_select<0>) {
			bool did = false;
			while(myi != mye && (*myi <= 0x20 || *myi == 0x85 || *myi == 0x2028)) {
				did = true;
				++myi;
				}
			return did;
			}
		bool skip_space() {
			return skip_space(utf_select<utf == 8 ? 8 : 0>{});
			}
		bool skip_over(char a) {
			while(myi != mye) {
				if(is(a))
					return true;
				++myi;
				}
			return false;
			}
		template<size_t s_>
		 bool skip_over(char const (&a)[s_]) {
			while(mye - myi >= static_cast<ptrdiff_t>(s_ - 1)) {
				if(is(a))
					return true;
				++myi;
				}
			return false;
			}
		void skip_bom(utf_select<0>) {
			if(myi != mye && *myi == 0xfeffu)
				++myi;
			}
		void skip_bom(utf_select<8>) {
			if(mye - myi >= 3 && myi[0] == 0xef && myi[1] == 0xbb && myi[2] == 0xbf)
				myi += 3;
			}
	 
		struct nodes_ {
			memory_node
				*first = 0,
				*in = 0,
				*last = 0;
			void add(memory_node *a) {
				if(!first)
					first = a;
				a->in = in;
				a->previous = last;
				if(last)
					last->next = a;
				else if(in)
					in->nodes = a;
				last = a;
				}
			};

		template<typename callback_>
		 bool parse(callback_& callback) {
			// bom
			// <?xml .... ?>
			// <!DOCTYPE ...>
			// <!-- -->
			// <!strange >
			// <tag>...</tag>
			// <tag/>
			// <?processing instruction?>
			// <![CDATA[ .... ]]>
			
			// undo_mode means the callback wont allcoate() and overwrite something in the undo allocations
			mymemory->undo_mode(true);
			auto undo = later([this] { mymemory->undo_mode(false); });
			
			skip_bom(utf_select<utf == 8 ? 8 : 0>{});
			
			if(is("<?xml")) {
				// <?xml version="1.1" encoding="UTF-8"?>
				if(!skip_over("?>"))
					return false;
				}
			
			nodes_ nodes;
			
			uchar_ **end_memory = 0;
			
			while(myi != mye) {
				if(skip_space()) {
					// continue
					}
				else if(*myi != '<') {
					// text content
					uchar_
						*b = myi,
						*e = ++myi;
					while(myi != mye && *myi != '<')
						if(!skip_space()) // skip all trailing whitespace
							e = ++myi;
					if(b != e) {
						// add node!
						auto *n = create();
						if(!n) return false;
						if(mydecode) {
							n->value_begin = b;
							n->value_end = decode(b, e);
							}
						else {
							n->value_begin = e; // backwards, decoded later
							n->value_end = b;
							}
						n->value_end_memory = e;
						if(end_memory) *end_memory = b;
						end_memory = &n->value_end_memory;
						nodes.add(n);
						}
					}
				else if(is("<![CDATA[")) {
					// text content as is
					uchar_
						*b = myi,
						*e = myi;
					if(!skip_over("]]>"))
						return false;
					e = myi - 3;
					if(b != e) {
						// add node!
						auto *n = create();
						if(!n) return false;
						n->value_begin = b;
						n->value_end = e;
						n->value_end_memory = e;
						if(end_memory) *end_memory = b;
						end_memory = &n->value_end_memory;
						nodes.add(n);
						}
					}
				else if(is("<![")) {
					// conditional sections, should only exist in DTD?
					// <![INCLUDE[ ... ]]>
					// <![IGNORE[ ... ]]>
					// include / ignore can be entities
					// <!ENTITY % draft 'INCLUDE' >
					// <!ENTITY % final 'IGNORE' >
					// <![%draft;[ <!ELEMENT book (comments*, title, body, supplements?)> ]]>
					// <![%final;[ <!ELEMENT book (title, body, supplements?)> ]]>
					//
					// they can nest, any <![ must be matched by a ]]>
					//
					unsigned z = 1;
					while(myi != mye) {
						while(*myi != '<' && *myi != ']' && ++myi != mye);
						if(myi == mye)
							return false;
						if(is("<!["))
							++z;
						else if(is("]]>")) {
							if(!--z)
								break;
							}
						else
							++myi;
						}
					if(z)
						return false;
					}
				else if(is("<!--")) {
					// comment, skip. are not supposed to contain -- ever
					if(!skip_over("-->"))
						return false;
					}
				else if(is("<?")) {
					// processing instruction, skip
					if(!skip_over("?>"))
						return false;
					}
				else if(is("<!")) {
					// doctype or something, skip
					// <!DOCTYPE ...> can have nested < and >, but they are inside []
					unsigned z = 0;
					bool error = true;
					while(myi != mye) {
						if(!z && *myi == '>') {
							++myi;
							error = false;
							break;
							}
						if(*myi == '[')
							++z;
						else if(*myi == ']' && z)
							--z;
						++myi;
						}
					if(error)
						return false;
					}
				else if(is("</")) {
					// </tag >
					// </tag>
					// close of the current tag
					// this has to match in->name or something is really wrong
					if(!nodes.in)
						return false;
					nodes.last = nodes.in;
					nodes.in = nodes.in->in;
					uchar_ const
						*n = nodes.last->name_begin,
						*ne = nodes.last->name_end;
					while(n != ne && myi != mye && *n == *myi) {
						++n;
						++myi;
						}
					if(n != ne)
						return false;
					skip_space();
					if(myi == mye || *myi != '>')
						return false;
					++myi;
					}
				else {
					// something beginning with <
					// has to be a regular tag
					// <tag ></tag>
					// <tag />
					// <tag attribute... >
					// tag names cannot contain /
					uchar_
						*nb = ++myi,
						*ne = nb;
					while(ne != mye && *ne != '>' && *ne != '/' && !is_space(ne))
						++ne;
					if(nb == ne)
						return false; // never empty name
					auto *n = create();
					if(!n)
						return false;
					n->name_begin = nb;
					n->name_end = ne;
					n->name_end_memory = ne;
					if(end_memory) *end_memory = nb;
					end_memory = &n->name_end_memory;
					myi = ne;
					// skip spaces, then see if attribute or close tag
					// attributes, must be separated by spaces
					// attribute="value"
					// attrubute = 'value'
					// the value cannot be cdata???
					memory_node *last = 0;
					while(true) {
						bool spaces = skip_space();
						if(myi == mye)
							return false;
						if(!spaces || *myi == '/' || *myi == '>')
							break;
						// must be a attribute here
						uchar_
							*ab = myi,
							*ae = ab;
						while(ae != mye && *ae != '/' && *ae != '>' && *ae != '=' && !is_space(ae)) ++ae;
						myi = ae;
						skip_space();
						if(myi == mye || *myi != '=' || ab == ae)
							return false;
						++myi;
						skip_space();
						if(myi == mye || (*myi != '"' && *myi != '\''))
							return false;
						uchar_
							q = *myi,
							*vb = ++myi,
							*ve = vb;
						while(ve != mye && *ve != q) ++ve;
						if(ve == mye)
							return false;
						myi = ve + 1;
						// a good attribute, save it
						auto *a = create();
						if(!a)
							return false;
						a->in = n;
						a->previous = last;
						a->name_begin = ab;
						a->name_end = ae;
						a->name_end_memory = vb;
						if(mydecode) {
							a->value_begin = vb;
							a->value_end = decode_attribute(vb, ve);
							}
						else {
							a->value_begin = ve; // backwards, decoded later
							a->value_end = vb;
							}
						a->value_end_memory = ve;
						if(end_memory) *end_memory = ab;
						end_memory = &a->value_end_memory;
						if(!n->attributes)
							n->attributes = a;
						else
							last->next = a;
						last = a;
						}
					// must be > or />
					bool in = false;
					if(mye - myi < 2)
						return false;
					if(*myi == '>') {
						in = true;
						++myi;
						}
					else if(myi[0] == '/' && myi[1] == '>')
						myi += 2;
					else
						return false;
					nodes.add(n);
					if(in) {
						nodes.in = n;
						nodes.last = 0;
						}
					callback(node_type{*mymemory, n});
					}
				}
			
			if(end_memory)
				*end_memory = mye;
			if(nodes.in)
				return false;
			my = nodes.first;
			return my != 0;
			}
	};

template<typename char_, typename allocator_> read<char_, memory<allocator_>> read_to_memory(memory<allocator_>& a) {
	return {a};
	}

template<typename allocator_> read<char, memory<allocator_>> read_to_memory(memory<allocator_>& a) {
	return {a};
	}

}}
#endif
