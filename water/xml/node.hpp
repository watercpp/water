// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_XML_NODE_HPP
#define WATER_XML_NODE_HPP
#include <water/xml/things/decode.hpp>
namespace water { namespace xml {
namespace _ {
	
	template<typename a_> struct
	 node_no_const {
		using result = a_;
		};
	template<typename a_> struct
	 node_no_const<a_ const> {
		using result = a_;
		};
	
	template<typename a_, typename mutable_, typename const_> struct
	 node_mutable_const {
		using result = mutable_;
		};
	template<typename a_, typename mutable_, typename const_> struct
	 node_mutable_const<a_ const, mutable_, const_> {
		using result = const_;
		};
	
};

//
// DOM tree node
//
// This is just a pointer into an underlying structure owned by xml::document. It is very small and cheap
// to copy. All nodes are owned by an xml::document and freed when that document is destroyed.
//
// This is a doubly linked list, and the next() previous() last() first() functions are used to move around.
//
// in() is the node that this is inside of, or false if its not inside another node
//
// nodes() is the nodes this element contains
// attributes() is the attributes this element has
//
// name() is the tag-name or attribute-type, text content has no name
// value() is the text content or attribute-value, tags have no value
// first_value() is the first text content found in nodes(), short way to read content from the tag
//
// Use create() or the xml::document node() function to create new empty nodes.
//
// All functions return something that converts to true or false. And all functions are safe to call even
// if this does not point to a node. This makes it possible to chain many function calls that reutrn node
// without checking the return value of each one
//
//   auto node = document.nodes().find("root").nodes("hello").attributes("world");
//
// It is also possible to create a DOM tree without much error checking and then use
// xml::document allocate_has_failed() to see if it worked.
//
// If any function call in a chain fails, this will return something that converts to false. And when a
// a function fails no other function in that chain will have any effect.
//
// nodes_or_add(name) and attributes_or_add(name) can be useful to add or overwrite something
//
template<typename document_> class
 node {
	struct node_is_const;
	public:
		using document_type = typename _::node_no_const<document_>::result;
		using char_type = typename document_type::char_type;
		using text_type = text<char_type const*>;
		using node_if_mutable = typename _::node_mutable_const<document_, node<document_>, node_is_const>::result;
		using mutable_node = node<document_type>;
	private:
		friend typename _::node_mutable_const<document_, node<document_type const>, node<document_type> >::result;
		using node_ = things::node<typename things::utf<char_type>::char_type>;
		node_ *my {}; // this must never ever be documents root node, if my->in is 0 this is removed
		document_ *myd {};
	public:
		node() = default;
		node(document_ *d, node_ *n) :
			my{n},
			myd{d}
			{}
		node(mutable_node const& a) :
			my{a.my},
			myd{a.myd}
			{}
		node& operator=(mutable_node const& a) {
			my = a.my;
			myd = a.myd;
			return *this;
			}
		bool operator==(node const& a) const {
			return my == a.my;
			}
		bool operator!=(node const& a) const {
			return my != a.my;
			}
		explicit operator bool() const {
			return my != 0;
			}
		document_type* document() const {
			return myd;
			}
		node first() const {
			return { myd, my ? first(my) : 0 };
			}
		node last() const {
			return { myd, my ? last(my) : 0 };
			}
		node next() const {
			return { myd, my ? my->next : 0 };
			}
		node previous() const {
			return { myd, my ? my->previous : 0 };
			}
		node in() const {
			return { myd, my && (!myd->nodes().my || my->in != myd->nodes().my->in) ? my->in : 0 }; // do not go into documents root node
			}
		node attributes() const {
			return { myd, my ? my->attributes : 0 };
			}
		template<typename iterator_>
		 node attributes(iterator_ begin, iterator_ end) const {
			return attributes().find(begin, end);
			}
		template<typename iterator_>
		 node attributes(iterator_ begin, size_t size) const {
			return attributes().find(begin, size);
			}
		template<typename char_>
		 node attributes(char_ const *const& cstring) const {
			return attributes().find(cstring);
			}
		template<typename char_, size_t size_>
		 node attributes(char_ const (&cstring)[size_]) const {
			return attributes().find(cstring);
			}
		template<typename range_>
		 typename if_range<range_, node>::result attributes(range_ const& range) const {
			return attributes().find(range.begin(), range_size(range));
			}
		node nodes() const {
			return { myd, my ? my->nodes : 0 };
			}
		template<typename iterator_>
		 node nodes(iterator_ begin, iterator_ end) const {
			return nodes().find(begin, end);
			}
		template<typename iterator_>
		 node nodes(iterator_ begin, size_t size) const {
			return nodes().find(begin, size);
			}
		template<typename char_>
		 node nodes(char_ const *const& cstring) const {
			return nodes().find(cstring);
			}
		template<typename char_, size_t size_>
		 node nodes(char_ const (&cstring)[size_]) const {
			return nodes().find(cstring);
			}
		template<typename range_>
		 typename if_range<range_, node>::result nodes(range_ const& range) const {
			return nodes().find(range.begin(), range_size(range));
			}
		text_type name() const {
			if(!my)
				return {};
			return {
				static_cast<char_type const*>(static_cast<void const*>(my->name_begin)),
				static_cast<char_type const*>(static_cast<void const*>(my->name_end))
				};
			}
		text_type value() const {
			if(!my)
				return {};
			if(my->value_begin > my->value_end) { // decode this
				auto
					*b = my->value_end,
					*e = my->value_begin;
				if(my->in && my->in->attributes && my->in->attributes == first(my))
					e = things::decode_attribute(b, e);
				else
					e = things::decode(b, e);
				my->value_begin = b;
				my->value_end = e;
				}
			return {
				static_cast<char_type const*>(static_cast<void const*>(my->value_begin)),
				static_cast<char_type const*>(static_cast<void const*>(my->value_end))
				};
			}
		text_type first_value() const {
			// value of first content node in nodes()
			if(my)
				if(node_ *n = my->nodes)
					do {
						if(n->name_begin == n->name_end)
							return node{ myd, n }.value();
						}	while(n = n->next);
			return {};
			}
		template<typename iterator_>
		 node find(iterator_ begin, size_t size) const {
			// find the first with name == begin,size inclduing this one
			// use the next() to find the next not including this one
			if(!size)
				return {};
			node_ *n = my;
			while(n && !name_equal(n, begin, size))
				n = n->next;
			return { myd, n };
			}
		template<typename iterator_>
		 node find(iterator_ begin, iterator_ end) const {
			return find(begin, size(begin, end));
			}
		template<typename char_>
		 node find(char_ const *const& cstring) const {
			return find(cstring, cend(cstring));
			}
		template<typename char_, size_t size_>
		 node find(char_ const (&cstring)[size_]) const {
			return find(cstring, size_ - 1);
			}
		template<typename range_>
		 typename if_range<range_, node>::result find(range_ const& range) const {
			return find(range.begin(), range_size(range));
			}
		template<typename iterator_>
		 node next(iterator_ begin, iterator_ end) const {
			return next().find(begin, end);
			}
		template<typename iterator_>
		 node next(iterator_ begin, size_t size) const {
			return next().find(begin, size);
			}
		template<typename char_>
		 node next(char_ const *const& cstring) const {
			return next().find(cstring);
			}
		template<typename char_, size_t size_>
		 node next(char_ const (&cstring)[size_]) const {
			return next().find(cstring);
			}
		template<typename range_>
		 typename if_range<range_, node>::result next(range_ const& range) const {
			return next().find(range.begin(), range_size(range));
			}
		template<typename iterator_>
		 node previous(iterator_ begin, size_t size) const {
			// find the closest previous node with name == begin,size
			if(!size)
				return {};
			node_ *n = my;
			if(n) while((n = n->previous) && !name_equal(n, begin, size));
			return { myd, n };
			}
		template<typename iterator_>
		 node previous(iterator_ begin, iterator_ end) const {
			return previous(begin, size(begin, end));
			}
		template<typename char_>
		 node previous(char_ const *const& cstring) const {
			return previous(cstring, cend(cstring));
			}
		template<typename char_, size_t size_>
		 node previous(char_ const (&cstring)[size_]) const {
			return previous(cstring, size_ - 1);;
			}
		template<typename range_>
		 typename if_range<range_, node>::result previous(range_ const& range) const {
			return previous(range.begin(), range_size(range));
			}
	public:
		node_if_mutable create() {
			if(!myd)
				return {};
			return myd->node();
			}
		node_if_mutable remove_all_nodes() {
			if(!my)
				return {};
			node_ *n = my->nodes;
			while(n) {
				n->in = 0;
				n = n->next;
				}
			n = my->nodes;
			my->nodes = 0;
			return { myd, n };
			}
		node_if_mutable remove_all_attributes() {
			if(!my)
				return {};
			node_ *n = my->attributes;
			while(n) {
				n->in = 0;
				n = n->next;
				}
			n = my->attributes;
			my->attributes = 0;
			return { myd, n };
			}
		node_if_mutable nodes(node_if_mutable a) {
			// insert the node-list a at the end of nodes()
			if(!my || !a.my)
				return {};
			node_ *n = first(a.my);
			if(!my->nodes)
				my->nodes = n;
			else {
				node_ *l = last(my->nodes);
				(l->next = n)->previous = l;
				}
			while(n) {
				n->in = my;
				n = n->next;
				}
			return *this;
			}
		template<typename iterator_>
		 node_if_mutable nodes_or_add(iterator_ begin, size_t size) {
			// find the first with name == begin,size in nodes()
			// if it does not exist, this->nodes(create().name(begin, size))
			if(!size || !my)
				return {};
			node r = nodes(begin, size);
			if(!r) {
				r = create().name(begin, size);
				nodes(r);
				}
			return r;
			}
		template<typename iterator_>
		 node_if_mutable nodes_or_add(iterator_ begin, iterator_ end) {
			return nodes_or_add(begin, size(begin, end));
			}
		template<typename char_>
		 node_if_mutable nodes_or_add(char_ const *const& cstring) {
			return nodes_or_add(cstring, cend(cstring));
			}
		template<typename char_, size_t size_>
		 node_if_mutable nodes_or_add(char_ const (&cstring)[size_]) {
			return nodes_or_add(cstring, size_ - 1);
			}
		template<typename range_>
		 typename if_range<range_, node_if_mutable>::result nodes_or_add(range_ const& range) {
			return nodes_or_add(range.begin(), range_size(range));
			}
		node_if_mutable attributes(node_if_mutable a) {
			// insert the node-list a at the end of attributes()
			if(!my || !a.my)
				return {};
			node_ *n = first(a.my);
			if(!my->attributes)
				my->attributes = n;
			else {
				node_ *l = last(my->attributes);
				(l->next = n)->previous = l;
				}
			while(n) {
				n->in = my;
				n = n->next;
				}
			return *this;
			}
		template<typename iterator_>
		 node_if_mutable attributes_or_add(iterator_ begin, size_t size) {
			// find the first with name == begin,size in attributes()
			// if it does not exist, this->attributes(create().name(begin, size))
			if(!size || !my)
				return {};
			node r = attributes(begin, size);
			if(!r) {
				r = create().name(begin, size);
				attributes(r);
				}
			return r;
			}
		template<typename iterator_>
		 node_if_mutable attributes_or_add(iterator_ begin, iterator_ end) {
			return attributes_or_add(begin, size(begin, end));
			}
		template<typename char_>
		 node_if_mutable attributes_or_add(char_ const *const& cstring) {
			return attributes_or_add(cstring, cend(cstring));
			}
		template<typename char_, size_t size_>
		 node_if_mutable attributes_or_add(char_ const (&cstring)[size_]) {
			return attributes_or_add(cstring, size_ - 1);
			}
		template<typename range_>
		 typename if_range<range_, node_if_mutable>::result attributes_or_add(range_ const& range) {
			return attributes_or_add(range.begin(), range_size(range));
			}
		node_if_mutable& remove() {
			// remove this, return this
			if(my) {
				if(my->previous)
					my->previous->next = my->next;
				else if(my->in) {
					if(my->in->nodes = my)
						my->in->nodes = my->next;
					else
						my->in->attributes = my->next;
					}
				if(my->next)
					my->next->previous = my->previous;
				my->next = my->previous = my->in = 0;
				}
			return *this;
			}
		node_if_mutable insert_before(node_if_mutable before) {
			// insert this before before, return this
			// will this->remove() first
			if(!my || !before.my)
				return {};
			remove();
			my->in = before.my->in;
			if(before.my->previous)
				(my->previous = before.my->previous)->next = my;
			else if(before.my->in) {
				if(before.my->in->nodes == before.my)
					before.my->in->nodes = my;
				else
					before.my->in->attributes = my;
				}
			(my->next = before.my)->previous = my;
			return *this;
			}
		node_if_mutable insert_after(node_if_mutable after) {
			// insert this after after, returns this
			// will this->reomve() first
			if(!my || !after.my)
				return {};
			remove();
			my->in = after.my->in;
			if(after.my->next)
				(my->next = after.my->next)->previous = my;
			(after.my->next = my)->previous = after.my;
			return *this;
			}
		template<typename iterator_>
		 node_if_mutable name(iterator_ begin, size_t size) {
			if(!my)
				return *this;
			using char_ = typename things::utf<char_type>::char_type;
			size_t s = static_cast<size_t>(my->name_end_memory - my->name_begin);
			if(s < size) {
				// if this has name and value, maybe it will fit after the value?
				// do not move the value if decoded because someone might have a pointer to it.
				if(my->value_end_memory) {
					bool decoded = my->value_begin <= my->value_end;
					value(); // decode
					if(!decoded && my->name_end_memory == my->value_begin && s + static_cast<size_t>(my->value_end_memory - my->value_end) >= size) {
						// can make space by moving value if its after name, was not decoded and enough space after
						auto
							*f = my->value_end,
							*t = f + size - s;
						while(f != my->value_begin)
							*--f = *--t;
						my->name_end_memory = my->value_begin += size - s;
						my->value_end += size - s;
						s = size;
						}
					else if(my->value_end_memory == my->name_begin && s + static_cast<size_t>(my->value_end_memory - my->value_end) >= size) {
						// if name after value, can make space by taking from value
						my->name_begin = my->name_end = my->value_end_memory -= size - s;
						s = size;
						}
					else if(static_cast<size_t>(my->value_end_memory - my->value_end) >= size) {
						// else maybe it will fit after value anyway
						my->name_end_memory = my->value_end_memory;
						my->name_begin = my->name_end = my->value_end_memory -= size;
						s = size;
						}
					}
				if(s < size)  {
					// allocate :(
					char_ *a = static_cast<char_*>(myd->allocate(size * sizeof(char_), alignof(char_)));
					if(!a)
						return {};
					// see if value can get memory. it can if it has no memory or if it has the memory before this (never move value)
					// could move value if decoded = false, maybe a good idea because this is an attribute with probably not much text?
					if(my->name_end_memory) {
						if(!my->value_end_memory) {
							my->value_begin = my->value_end = my->name_begin;
							my->value_end_memory = my->name_end_memory;
							}
						else if(my->value_end_memory == my->name_begin)
							my->value_end_memory = my->name_end_memory;
						}
					my->name_begin = my->name_end = a;
					my->name_end_memory = a + size;
					}
				}
			// copy
			char_ *b = my->name_begin;
			my->name_end = b;
			while(size) {
				*b++ = static_cast<char_>(*begin);
				++begin;
				--size;
				}
			my->name_end = b; // set this last, if iterator_ throws this will be empty and never contain half utf-8
			return *this;
			}
		template<typename iterator_>
		 node_if_mutable name(iterator_ begin, iterator_ end) {
			return name(begin, size(begin, end));
			}
		template<typename char_>
		 node_if_mutable name(char_ const *const& cstring) {
			return name(cstring, cend(cstring));
			}
		template<typename char_, size_t size_>
		 node_if_mutable name(char_ const (&cstring)[size_]) {
			return name(cstring, size_ - 1);
			}
		template<typename range_>
		 typename if_range<range_, node_if_mutable>::result name(range_ const& range) {
			return name(range.begin(), range_size(range));
			}
		template<typename iterator_>
		 node_if_mutable value(iterator_ begin, size_t size) {
			if(!my)
				return *this;
			using char_ = typename things::utf<char_type>::char_type;
			auto *b = my->value_begin < my->value_end ? my->value_begin : my->value_end;
			size_t s = static_cast<size_t>(my->value_end_memory - b);
			if(s < size) {
				// if this has name and value, maybe it will fit if some space is stolen from the name?
				if(my->name_end_memory) {
					if(b == my->name_end_memory && s + static_cast<size_t>(my->name_end_memory - my->name_end) >= size) {
						// name before value, can take from name
						b = my->value_begin = my->value_end = my->name_end_memory -= size - s;
						s = size;
						}
					else if(static_cast<size_t>(my->name_end_memory - my->name_end) >= size) {
						// value fits in end of name
						my->value_end_memory = my->name_end_memory;
						b = my->value_begin = my->value_end = my->name_end_memory -= size;
						s = size;
						}
					}
				if(s < size)  {
					// allocate :(
					char_ *a = static_cast<char_*>(myd->allocate(size * sizeof(char_), alignof(char_)));
					if(!a)
						return {};
					// see if name can get memory. it can if it has no memory or if it has the memory before this (never move name)
					if(my->value_end_memory) {
						if(!my->name_end_memory) {
							my->name_begin = my->name_end = b;
							my->name_end_memory = my->value_end_memory;
							}
						else if(my->name_end_memory == b)
							my->name_end_memory = my->value_end_memory;
						}
					b = my->value_begin = my->value_end = a;
					my->value_end_memory = a + size;
					}
				}
			// copy
			my->value_begin = b; // because maybe value_end < value_begin
			my->value_end = b;
			while(size) {
				*b++ = static_cast<char_>(*begin);
				++begin;
				--size;
				}
			my->value_end = b; // set this last, if iterator_ throws this will be empty and never contain half utf-8
			return *this;
			}
		template<typename iterator_>
		 node_if_mutable value(iterator_ begin, iterator_ end) {
			return value(begin, size(begin, end));
			}
		template<typename char_>
		 node_if_mutable value(char_ const *const& cstring) {
			return value(cstring, cend(cstring));
			}
		template<typename char_, size_t size_>
		 node_if_mutable value(char_ const (&cstring)[size_]) {
			return value(cstring, size_ - 1);
			}
		template<typename range_>
		 typename if_range<range_, node_if_mutable>::result value(range_ const& range) {
			return value(range.begin(), range_size(range));
			}
		template<typename iterator_>
		 node_if_mutable first_value(iterator_ begin, size_t size) {
			// set the value of the first content node in nodes(), or create a node if there is none and size != 0. the created node will be first in nodes()
			if(!my)
				return *this;
			node_ *n = my->nodes;
			while(n && n->name_begin != n->name_end)
				n = n->next;
			if(!n) {
				if(!size)
					return *this; // all is good
				auto a = myd->node(); // leak this if set value fails
				if(!a)
					return {};
				n = a.my;
				}
			if(!node{myd, n}.value(begin, size))
				return {};
			if(!n->in) {
				n->in = my;
				if(n->next = my->nodes)
					n->next->previous = n;
				my->nodes = n;
				}
			return *this;
			}
		template<typename iterator_>
		 node_if_mutable first_value(iterator_ begin, iterator_ end) {
			return first_value(begin, size(begin, end));
			}
		template<typename char_>
		 node_if_mutable first_value(char_ const *const& cstring) {
			return first_value(cstring, cend(cstring));
			}
		template<typename char_, size_t size_>
		 node_if_mutable first_value(char_ const (&cstring)[size_]) {
			return first_value(cstring, size_ - 1);
			}
		template<typename range_>
		 typename if_range<range_, node_if_mutable>::result first_value(range_ const& range) {
			return first_value(range.begin(), range_size(range));
			}
	private:
		template<typename iterator_>
		 static bool name_equal(node_ *n, iterator_ begin, size_t size) {
			if(static_cast<size_t>(n->name_end - n->name_begin) == size) {
				auto ni = n->name_begin;
				iterator_ i = begin;
				while(*i == *ni && ++ni != n->name_end) ++i;
				if(ni == n->name_end)
					return true;
				}
			return false;
			}
		template<typename char_>
		 static char_ const* cend(char_ const *a) {
			if(a) while(*a) ++a;
			return a;
			}
		static node_* first(node_ *a) {
			while(a->previous) a = a->previous;
			return a;
			}
		static node_* last(node_ *a) {
			while(a->next) a = a->next;
			return a;
			}
	};

}}
#endif
