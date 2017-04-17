// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_XML_DOCUMENT_HPP
#define WATER_XML_DOCUMENT_HPP
#include <water/xml/things/things.hpp>
#include <water/xml/things/utf.hpp>
#include <water/xml/node.hpp>
#include <water/xml/encoding.hpp>
namespace water { namespace xml {

//
// XML parser and DOM tree node allocator
//
// template
// - char_
//   - must be built in unisgned integer type or char, signed char
//   - char, unsgined char and signed char are always seen as UTF-8
//   - other types are seen as UTF-16 if they have at least 16 bits
// - allocator_
//   - must have allocate(size_t) and free(void*, size_t)
//   - can throw or return 0
//   - maybe use water::allocator_nothrow
//
// The parse functions and nodes() return the root xml::node in the DOM tree, see xml::node
//
// the callback used with parse will be called on each tag, before it's content has been parsed but
// after the attributes. it might be useful to build an index of the document. it should look like
// this:
//
//   struct callback { void operator()(const_node_type) {} };
//
// the callback can allocate() memory from this, but see allocate()
//
// this will not free memory before parsing, because allocate() can be used to allocate memory. it
// is probably a good idea to clear() before parsing if the same document is reused.
//
template<typename char_, typename allocator_> class
 document {
	public:
		using char_type = char_;
		using allocator_type = allocator_;
		using node_type = xml::node<document<char_, allocator_> >;
		using const_node_type = xml::node<document<char_, allocator_> const>;
		constexpr static bool utf8 = things::utf<char_type>::utf8;
		constexpr static bool utf16 = !utf8;
	private:
		using uchar_ = typename things::utf<char_type>::char_type;
		using node_ = things::node<uchar_>;
		struct no_callback {
			void operator()(const_node_type const&) {}
			};
	private:
		allocator_type myallocator{};
		things::block *mymemory = 0;
		size_t myallocate = sizeof(node_) * 128 + sizeof(things::block);
		node_ *my = 0; // this is kind of secret, node.in() will not return this. it is a pointer to make move/swap possible
		uchar_
			*myb = 0,
			*mye = 0,
			*myi = 0; // when parse fails this is probably pointing to where it failed
		bool myallocate_error = false;
	public:
		document() = default;
		document(allocator_type const& a) :
			myallocator{a}
			{}
		document(document&& a) :
			myallocator{static_cast<allocator_type&&>(a.myallocator)},
			mymemory{a.mymemory},
			myallocate{a.myallocate},
			my{a.my},
			myb{a.myb},
			mye{a.mye},
			myi{a.myi},
			myallocate_error{a.myallocate_error}
			{
			a.mymemory = 0;
			a.my = 0;
			}
		document(document const&) = delete;
		document& operator=(document&& a) {
			swap(a);
			return *this;
			}
		document& operator=(document const&) = delete;
		~document() {
			#ifdef ___water_debug
			___water_debug(trace() << "xml::document " << this << ". " << memory_allocations() << " allocations, " << memory_use() << " bytes, " << memory_unused() << " unused bytes\n");
			#endif
			while(auto *b = mymemory) {
				mymemory = b->list;
				myallocator.free(b->begin, b->size());
				}
			}
		void swap(document& a) {
			_::swap_from_swap(myallocator, a.myallocator);
			_::swap_from_swap(mymemory, a.mymemory);
			_::swap_from_swap(myallocate, a.myallocate);
			_::swap_from_swap(my, a.my);
			_::swap_from_swap(myb, a.myb);
			_::swap_from_swap(mye, a.mye);
			_::swap_from_swap(myi, a.myi);
			_::swap_from_swap(myallocate_error, a.myallocate_error);
			}
		template<typename callback_>
		 node_type parse_in_place(char_type *begin, char_type *end, callback_ callback) {
			// does not verify that the range is valid unicode
			my = 0;
			myb = static_cast<uchar_*>(static_cast<void*>(begin));
			mye = static_cast<uchar_*>(static_cast<void*>(end));
			if(parse(callback)) {
				auto b = mymemory;
				while(b) {
					b->no_undo();
					b = b->list;
					}
				return { this, my->nodes };
				}
			my = 0;
			return { this, 0 };
			}
		node_type parse_in_place(char_type *begin, char_type *end) {
			return parse_in_place(begin, end, no_callback());
			}
		template<typename callback_>
		 node_type parse_in_place(void *pointer, size_t bytes, callback_ callback) {
			// parse raw memory
			//
			// this should figure out if the memory is a utf8 or utf16 encoded xml document,
			// and if it is it should verfy the utf and then parse it in place if it can. if
			// it cannot parse in place, it will copy. it will convert to the utf this uses
			// if the memory is not in that utf.
			my = 0;
			encoding e(pointer, bytes);
			pointer = static_cast<char*>(pointer) + e.byte_order_mark();
			bytes -= e.byte_order_mark();
			if(e.utf8()) {
				size_t l = things::utf8_length<utf16>(pointer, bytes);
				if(l) {
					if(utf8)
						parse_in_place(static_cast<char_type*>(pointer), static_cast<char_type*>(pointer) + l, callback); // should check char_type is size 1 integer??
					else if(char_type *a = static_cast<char_type*>(allocate_with_undo(l * sizeof(char_type), alignof(char_type))))
						parse_in_place(a, things::utf8_convert(a, pointer, bytes), callback);
					}
				}
			if(e.utf16()) {
				size_t l = things::utf16_length<utf8>(pointer, bytes, e.big_endian());
				if(l) {
					if(utf16 && sizeof(char_type) == 2 && static_cast<char_type>(-1) == 0xffffu) {
						// maybe flip endian
						char_type const one = 1;
						bool big_endian = *static_cast<unsigned char const*>(static_cast<void const*>(&one)) == 0;
						if(big_endian != e.big_endian()) {
							unsigned char
								*a = static_cast<unsigned char*>(pointer),
								*ae = a + bytes;
							do {
								unsigned char a0 = a[0];
								a[0] = a[1];
								a[1] = a0;
								} while((a += 2) < ae);
							}
						parse_in_place(static_cast<char_type*>(pointer), static_cast<char_type*>(pointer) + l, callback);
						}
					else if(utf16) {
						if(char_type *a = static_cast<char_type*>(allocate_with_undo(l * sizeof(char_type), alignof(char_type)))) {
							char_type
								*ai = a,
								*ae = a + l;
							unsigned char const* b = static_cast<unsigned char const*>(pointer);
							do {
								*ai = (static_cast<char_type>(b[!e.big_endian()]) << 8) | static_cast<char_type>(b[e.big_endian()]);
								b += 2;
								} while(++ai != ae);
							parse_in_place(a, ae, callback);
							}
						}
					else if(char_type *a = static_cast<char_type*>(allocate_with_undo(l * sizeof(char_type), alignof(char_type))))
						parse_in_place(a, things::utf16_convert(a, pointer, bytes, e.big_endian()), callback);
					}
				}
			return nodes();
			}
		node_type parse_in_place(void *pointer, size_t bytes) {
			return parse_in_place(pointer, bytes, no_callback());
			}
		template<typename forward_iterator_, typename callback_>
		 node_type parse(forward_iterator_ begin, forward_iterator_ end, callback_ callback) {
			// copy range then parse
			// does not verify that the range is valid unicode
			return parse(begin, size(end - begin), callback);
			}
		template<typename forward_iterator_>
		 node_type parse(forward_iterator_ begin, forward_iterator_ end) {
			return parse(begin, end, no_callback());
			}
		template<typename forward_iterator_, typename callback_>
		 node_type parse(forward_iterator_ begin, size_t size, callback_ callback) {
			// copy range then parse
			// does not verify that the range is valid unicode
			my = 0;
			if(size) {
				if(char_type *a = static_cast<char_type*>(allocate_with_undo(size * sizeof(char_type), alignof(char_type)))) {
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
			return nodes();
			}
		template<typename forward_iterator_>
		 node_type parse(forward_iterator_ begin, size_t size) {
			return parse(begin, size, no_callback());
			}
		node_type nodes() {
			return { this, my ? my->nodes : 0 };
			}
		const_node_type nodes() const {
			return { this, my ? my->nodes : 0 };
			}
		node_type nodes(node_type a) {
			// add nodes to the root level of this document. an xml document should have only 1 node at the root level
			// returns nodes(), converts to false or throws if it failed depending on allocator
			if(!a)
				return nodes();
			if(!my && !(my = new_node(false)))
				return { this, 0 };
			node_type(this, my).nodes(a);
			return { this, my->nodes };
			}
		node_type remove_all_nodes() {
			// returns the removed nodes
			// they will exist until clear() even if a new document is parsed
			return node_type(this, my).remove_all_nodes();
			}
		node_type node() {
			// allocate a node, can throw or return false depending on allcoator
			return { this, new_node(false) };
			}
		void clear() {
			for(auto *b = mymemory; b; b = b->list) b->clear();
			myb = mye = myi = 0;
			my = 0;
			myallocate_error = false;
			}
		void* allocate(size_t bytes, size_t align = 0) {
			// allocate bytes. the memory will be freed by clear() or the destructor.
			// if the underlying allocator fails (returns 0 or throws) allocate_has_failed() will return true
			//
			// align 0 means use default, it is probably good most of the time
			//
			// if this is used from the callback the memory might be gone after parse if parse fails
			return allocate_do(bytes, align, false);
			}
		bool allocate_has_failed() const {
			// return true if allocate() has failed. clear() will reset this
			return myallocate_error;
			}
		size_t memory_block_size() const {
			return myallocate;
			}
		void memory_block_size(size_t bytes) {
			// set the size in bytes that this will allocate at a time.
			// it will allocate more if allocate() is called with more bytes
			myallocate = bytes; 
			}
		size_t memory_use() const {
			size_t r = 0;
			for(auto *b = mymemory; b; b = b->list) r += b->size();
			return r;
			}
		size_t memory_unused() const {
			size_t r = 0;
			for(auto *b = mymemory; b; b = b->list) r += b->unused();
			return r;
			}
		size_t memory_allocations() const {
			size_t r = 0;
			for(auto *b = mymemory; b; b = b->list) ++r;
			return r;
			}
	private:
		void* allocate_with_undo(size_t bytes, size_t align) {
			return allocate_do(bytes, align, true);
			}
		void* allocate_do(size_t bytes, size_t align, bool undo) {
			// allocate bytes. the memory will be freed by clear() or the destructor.
			// if the underlying allocator fails (returns 0 or throws) allocate_has_failed() will return true
			void *r = 0;
			if(mymemory && (r = mymemory->push(bytes, align, undo)))
				return r;
			// maybe another block has free space
			if(mymemory) {
				auto
					*at = mymemory->list,
					*next = mymemory;
				while(at) {
					if(r = at->push(bytes, align, undo)) {
						// make this current
						next->list = at->list;
						at->list = mymemory;
						mymemory = at;
						return r;
						}
					next = at;
					at = at->list;
					}
				}
			// allocate new block
			size_t allocate = things::block_size_align(bytes) + sizeof(things::block);
			if(allocate < myallocate)
				allocate = myallocate;
			bool error_before = myallocate_error;
			myallocate_error = true;
			if(void *a = myallocator.allocate(allocate)) { // throws maybe
				myallocate_error = error_before;
				auto *b = things::block_from(a, allocate);
				b->list = mymemory;
				mymemory = b;
				r = b->push(bytes, align, undo);
				}
			return r;
			}
		node_* new_node(bool undo) {
			node_ *n = static_cast<node_*>(allocate_do(sizeof(node_), 0, undo));
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
			if(mye - myi < s_ - 1)
				return false;
			unsigned i = 0;
			do {
				if(static_cast<uchar_>(a[i]) != myi[i])
					return false;
				} while(++i != s_ - 1);
			myi += i;
			return true;
			}
		bool is_space(uchar_ const *i) {
			// i must be < mye!!
			if(*i <= 0x20)
				return true;
			if(!utf8)
				return *i == 0x85 || *i == 0x2028;
			if(i[0] == 0xc2 && mye - i >= 2 && i[1] == 0x85)
				return true;
			if(i[0] == 0xe2 && mye - i >= 3 && i[1] == 0x80 && i[2] == 0xa8)
				return true;
			return false;
			}
		bool skip_space() {
			bool did = false;
			if(!utf8) {
				while(myi != mye && (*myi <= 0x20 || *myi == 0x85 || *myi == 0x2028)) {
					did = true;
					++myi;
					}
				}
			else {
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
				}
			return did;
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
			while(mye - myi >= s_ - 1) {
				if(is(a))
					return true;
				++myi;
				}
			return false;
			}
	 
		struct nodes_ {
			node_
				*in = 0,
				*last = 0;
			void add(node_ *a) {
				a->in = in;
				a->previous = last;
				if(last)
					last->next = a;
				else
					in->nodes = a;
				last = a;
				}
			};

		template<typename callback_>
		 bool parse(callback_ callback) {
			// bom
			// <?xml .... ?>
			// <!DOCTYPE ...>
			// <!-- -->
			// <!strange >
			// <tag>...</tag>
			// <tag/>
			// <?processing instruction?>
			// <![CDATA[ .... ]]>
			
			//out_type o;
			
			// skip bom
			myi = myb;
			if(utf16 && myi != mye && *myi == 0xfeffu)
				++myi;
			if(utf8 && mye - myi >= 3 && myi[0] == 0xef && myi[1] == 0xbb && myi[2] == 0xbf)
				myi += 3;
			
			if(is("<?xml")) {
				// <?xml version="1.1" encoding="UTF-8"?>
				if(!skip_over("?>"))
					return false;
				//o << "xml\n";
				}
			
			nodes_ nodes;
			nodes.in = my = new_node(true);
			if(!my)
				return false;
			
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
						auto *n = new_node(true);
						if(!n) return false;
						n->value_begin = e; // backwards, decoded later
						n->value_end = b;
						n->value_end_memory = e;
						if(end_memory) *end_memory = b;
						end_memory = &n->value_end_memory;
						nodes.add(n);
						//o << "text \"" << str::put(b, e) << "\"\n";
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
						auto *n = new_node(true);
						if(!n) return false;
						n->value_begin = b;
						n->value_end = e;
						n->value_end_memory = e;
						if(end_memory) *end_memory = b;
						end_memory = &n->value_end_memory;
						nodes.add(n);
						//o << "text \"" << str::put(b, e) << "\"\n";
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
					//o << "<!--\n";
					}
				else if(is("<?")) {
					// processing instruction, skip
					if(!skip_over("?>"))
						return false;
					//o << "<?\n";
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
					//o << "<!\n";
					}
				else if(is("</")) {
					// </tag >
					// </tag>
					// close of the current tag
					// this has to match in->name or something is really wrong
					if(nodes.in == my)
						return false;
					nodes.last = nodes.in;
					nodes.in = nodes.in->in;
					uchar_ const
						*n = nodes.last->name_begin,
						*ne = nodes.last->name_end;
					while(n != ne && myi != mye && *n == *myi) ++n, ++myi;
					if(n != ne)
						return false;
					skip_space();
					if(myi == mye || *myi != '>')
						return false;
					++myi;
					//o << "</\n";
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
					auto *n = new_node(true);
					if(!n)
						return false;
					n->name_begin = nb;
					n->name_end = ne;
					n->name_end_memory = ne;
					if(end_memory) *end_memory = nb;
					end_memory = &n->name_end_memory;
					//o << "<tag \"" << str::put(nb, ne) << "\"\n";
					myi = ne;
					// skip spaces, then see if attribute or close tag
					// attributes, must be separated by spaces
					// attribute="value"
					// attrubute = 'value'
					// the value cannot be cdata???
					node_ *last = 0;
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
						auto *a = new_node(true);
						if(!a)
							return false;
						a->in = n;
						a->previous = last;
						a->name_begin = ab;
						a->name_end = ae;
						a->name_end_memory = vb;
						a->value_begin = ve; // backwards, decoded later
						a->value_end = vb;
						a->value_end_memory = ve;
						if(end_memory) *end_memory = ab;
						end_memory = &a->value_end_memory;
						if(!n->attributes)
							n->attributes = a;
						else
							last->next = a;
						last = a;
						//o << "attribute \"" << str::put(ab, ae) << "\" = \"" << str::put(vb, ve) << "\"\n";
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
					callback(const_node_type{this, n});
					}
				}
			
			if(end_memory)
				*end_memory = mye;
			if(nodes.in != my)
				return false;
			return my->nodes != 0;
			}
	};

template<typename char_, typename allocator_> void
 swap(document<char_, allocator_>& a, document<char_, allocator_>& b) {
 	a.swap(b);
 	}

}}
#endif
