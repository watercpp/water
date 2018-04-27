// Copyright 2017-2018 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_JSON_NODE_HPP
#define WATER_JSON_NODE_HPP
#include <water/json/bits.hpp>
#include <water/json/number.hpp>
#include <water/json/string.hpp>
#include <water/json/iterator.hpp>
namespace water { namespace json {

template<typename memory_, typename other_, typename result_> struct node_if_no_memory {};
template<typename other_, typename result_>                   struct node_if_no_memory<void, other_, result_> { using result = result_; };
template<typename result_>                                    struct node_if_no_memory<void, void, result_> {};

//
// this needs all strings to be utf-8
//
// node<> is a constant node, cannot be changed
// node<memory> is a mutable node, it can be changed using memory from that memory
//
// any node<memory> can be converted to a constant node<>
//
// it is possible to connect nodes together that use different memory_pointer(), that
// works although it's a bit dangerous.
//
// all functions can be called enven when this is 0.
// lots of functions return *this or a node, it is possible to chain many operations.
// if any operation fails it will return a "empty" node, making any operations after that also fail.
//
// operations can fail only if memory allocation fails. if memory_ does not throw
// use memory->allocate_has_failed() after operations to know if something failed.
// this makes it possible to build node trees without checking for errors until after.
//
template<typename memory_ = void> class
 node {
	using type_ = json::type;
	friend class node<>;
	struct nothing {};
	struct node_is_const;
	public:
		using memory_type = memory_;
		using node_if_mutable = typename types::ifel_type<types::if_not_void<memory_>, node<memory_>, node_is_const>::result;
		using iterator = json::iterator<node<memory_>>;
	private:
		memory_type *mym = 0; // cannot be 0 if memory_ != void and my != 0, always 0 if memory_ is void
		memory_node *my = 0;
	private:
		node(memory_type *m, memory_node *n) :
			mym{m},
			my{n}
			{}
	public:
		constexpr node() = default;
		node(typename types::if_not_void<memory_type, node_is_const>::result& m, memory_node *n) :
			mym{&m},
			my{n}
			{}
		template<typename other_> node(node<other_> a, typename node_if_no_memory<memory_, other_, nothing>::result = {}) :
			mym{0},
			my{a.my}
			{}
		explicit operator bool() const {
			return my != 0;
			}
		bool operator==(node a) const {
			return my = a.my;
			}
		bool operator!=(node a) const {
			return my != a.my;
			}
		memory_type* memory_pointer() const {
			return mym;
			}
		node in() const {
			return {mym, my ? my->in : 0};
			}
		node nodes() const {
			if(my && my->me.capacity)
				return {mym, *my->nodes};
			return {mym, 0};
			}
		node previous() const {
			return {mym, my && my->me.at ? my->in->nodes[my->me.at - 1] : 0};
			}
		node next() const {
			return {mym, my && my->in && my->me.at + 1 < my->in->me.capacity ? my->in->nodes[my->me.at + 1] : 0};
			}
		node first() const {
			return {mym, my && my->me.at ? my->in->nodes[0] : my};
			}
		node last() const {
			return {mym, my && my->in ? my->in->nodes[my->in->size - 1] : my};
			}
		iterator begin() const {
			return {*this, 0};
			}
		iterator end() const {
			return {*this, static_cast<typename iterator::difference_type>(size())};
			}
		json::type type() const {
			return my ? my->type : type_::null;
			}
		node array() const {
			return {mym, my && my->type == type_::array && my->nodes ? *my->nodes : 0};
			}
		node object() const {
			return {mym, my && my->type == type_::object && my->nodes ? *my->nodes : 0};
			}
		json::string<char const*> name() const {
			if(!my) return {};
			return {static_cast<char const*>(static_cast<void const*>(my->name)), my->name_size};
			}
		json::string<char const*> string() const {
			if(!my || my->type != type_::string) return {};
			return {static_cast<char const*>(static_cast<void const*>(my->string)), my->size};
			}
		json::number number() const {
			if(!my || my->type != type_::number) return {};
			return {my->integer, my->exponent, my->extra != 0};
			}
		bool boolean() const {
			return my && my->type == type_::boolean && my->boolean;
			}
		bool null() const {
			// returns true if it is null
			return my && my->type == type_::null;
			}
		size_t size() const {
			// array or object size
			return my && my->me.capacity ? my->size : 0;
			}
		size_t capacity() const {
			// array or object capacity, if it grows larger it must allocate more memory
			return my ? my->me.capacity : 0;
			}
		size_t at() const {
			// return position of this in in()
			return my ? my->me.at : 0;
			}
		node at(size_t a) const {
			return {mym, my && my->me.capacity > a ? my->nodes[a] : 0};
			}
		node operator[](int a) const {
			return at(static_cast<size_t>(a));
			}
		node operator[](unsigned int a) const {
			return at(static_cast<size_t>(a));
			}
		node operator[](long a) const {
			return at(static_cast<size_t>(a));
			}
		node operator[](unsigned long a) const {
			return at(static_cast<size_t>(a));
			}
		node operator[](long long a) const {
			return at(static_cast<size_t>(a));
			}
		node operator[](unsigned long long a) const {
			return at(static_cast<size_t>(a));
			}
		node operator[](char const*const& cstring) const {
			return find(cstring);
			}
		template<size_t size_>
		 node operator[](char const (&cstring)[size_]) const {
			return find(cstring);
			}
		template<typename range_>
		 typename if_range<range_, node>::result operator[](range_ const& name) const {
			return find(name);
			}
		template<typename iterator_>
		 node find(iterator_ begin, size_t size) const {
			// find first node with name begin,size
			// starts looking at this
			if(!my || my->type != type_::object || !my->size)
				return {mym, 0};
			auto
				i = my->nodes,
				e = my->nodes + my->size;
			while(i != e) {
				if((*i)->name_size == size) {
					iterator_ b = begin;
					char8_t const
						*n = (*i)->name,
						*ne = n + size;
					while(n != ne && *n == string_compare_cast(*b)) ++n, ++b;
					if(n == ne)
						return {mym, *i};
					}
				++i;
				}
			return {mym, 0};
			}
		template<typename iterator_>
		 node find(iterator_ begin, iterator_ end) const {
			if(!my) return *this;
			return find(begin, json::size(begin, end));
			}
		template<typename range_>
		 typename if_range<range_, node>::result find(range_ const& name) const {
			if(!my) return *this;
			return find(name.begin(), range_size(name));
			}
		node find(char const*const& cstring) const {
			if(!my) return *this;
			return find(cstring, clength(cstring));
			}
		template<size_t size_>
		 node find(char const (&cstring)[size_]) const {
			return find(cstring, size_ - 1);
			}
	public:
		// mutable below
		node_if_mutable& boolean(bool a) {
			// make this boolean
			if(my) {
				nodes_clear();
				my->type = type_::boolean;
				my->boolean = a;
				}
			return *this;
			}
		node_if_mutable& null(decltype(nullptr)) {
			// make this null
			if(my) {
				nodes_clear();
				my->type = type_::null;
				}
			return *this;
			}
		node_if_mutable& number(json::number a) {
			// make this a number
			if(my) {
				nodes_clear();
				my->type = type_::number;
				my->extra = a.imprecise();
				my->integer = a.integer();
				my->exponent = a.exponent();
				}
			return *this;
			}
		node_if_mutable array(size_t capacity) {
			// make this an array, and make it have capacity() >= capacity
			//
			// returns
			// - this if success
			// - 0 if allocation fails
			//
			// if this is an object, it is changed to be an array containing the same nodes.
			// if capacity is less than the current capacity, this cannot fail. array(0) cannot fail.
			//
			if(!my || !grow(capacity, true))
				return {mym, 0};
			my->type = type_::array;
			return *this;
			}
		node_if_mutable object(size_t capacity) {
			// like array(reserve) except this is an object after success
			if(!my || !grow(capacity, true))
				return {mym, 0};
			my->type = type_::object;
			return *this;
			}
		node_if_mutable capacity(size_t a) {
			// if capacity() < capacity this will allocate memory.
			// returns this if success, else 0
			return {mym, my && grow(a, true) ? my : 0};
			}
		node_if_mutable push_back(node a) {
			// if a is in something, this will remove it first
			// return a if success
			if(!a.my || !my || !grow())
				return {mym, 0};
			node(mym, a.my->in).remove(a.my->me.at);
			a.my->me.at = my->size;
			a.my->in = my;
			my->nodes[my->size++] = a.my;
			return a;
			}
		node_if_mutable push_back() {
			return my ? push_back(mym->create()) : node{mym, 0};
			}
		node_if_mutable pop_back() {
			node r {mym, 0};
			if(my && my->me.capacity && my->size) {
				r.my = my->nodes[--my->size];
				my->nodes[my->size] = 0;
				r.my->in = 0;
				r.my->me.at = 0;
				}
			return r;
			}
		node_if_mutable push_front(node a) {
			return insert(0, a);
			}
		node_if_mutable push_front() {
			return insert(0);
			}
		node_if_mutable pop_front() {
			return remove(0);
			}
		node_if_mutable insert(size_t at, node a) {
			// if a is in something, this will remove it first
			// return a if success
			if(!a.my || !my || !grow())
				return {mym, 0};
			node(mym, a.my->in).remove(a.my->me.at);
			if(at >= my->size)
				at = my->size;
			else if(my->size) {
				auto
					b = my->nodes + at,
					i = my->nodes + my->size;
				do {
					i[0] = i[-1];
					++(*i)->me.at;
					} while(--i != b);
				}
			my->nodes[at] = a.my;
			a.my->me.at = static_cast<uint32_t>(at);
			a.my->in = my;
			++my->size;
			return a;
			}
		node_if_mutable insert(size_t at) {
			return my ? insert(at, mym->create()) : node{mym, 0};
			}
		node_if_mutable remove(size_t at) {
			if(!my || !my->me.capacity || at > my->size)
				return {mym, 0};
			auto
				i = my->nodes + at,
				e = my->nodes + my->size;
			auto r = *i;
			while(++i != e) {
				--(*i)->me.at;
				i[-1] = i[0];
				}
			e[-1] = 0;
			my->nodes[--my->size] = 0;
			r->me.at = 0;
			r->in = 0;
			return {mym, r};
			}
		node_if_mutable remove() {
			// remove this from in()
			return in().remove(at());
			}
		template<typename iterator_>
		 node_if_mutable name(iterator_ begin, size_t size) {
			// set name
			//
			// returns
			// - *this
			// - if memory allocation failed, nothing is changed, returns a default constructed
			//
			// this does not have to be in an object
			if(my && !copy(my->name, my->name_size, begin, size))
				return {mym, 0};
			return *this;
			}
		template<typename iterator_>
		 node_if_mutable name(iterator_ begin, iterator_ end) {
			if(my) name(begin, json::size(begin, end));
			return *this;
			}
		template<typename range_>
		 typename if_range<range_, node_if_mutable>::result name(range_ const& a) {
			if(my) name(a.begin(), range_size(a));
			return *this;
			}
		node_if_mutable name(char const*const& cstring) {
			if(my) name(cstring, clength(cstring));
			return *this;
			}
		template<size_t size_>
		 node_if_mutable name(char const (&cstring)[size_]) {
			return name(cstring, size_ - 1);
			}
		template<typename iterator_>
		 node_if_mutable string(iterator_ begin, size_t size) {
			// makes this a string, and sets it to begin,end
			//
			// returns
			// - *this
			// - if memory allocation failed, nothing is changed, returns a default constructed
			//
			if(my) {
				char8_t *to = 0;
				typename types::no_reference<decltype(my->size)>::result to_size = 0;
				if(my->type == type_::string) {
					to = my->string;
					to_size = my->size;
					}
				if(!copy(to, to_size, begin, size))
					return {mym, 0};
				if(my->type != type_::string) {
					nodes_clear();
					my->type = type_::string;
					}
				my->string = to;
				my->size = to_size;
				}
			return *this;
			}
		template<typename iterator_>
		 node_if_mutable string(iterator_ begin, iterator_ end) {
			if(my) string(begin, json::size(begin, end));
			return *this;
			}
		template<typename range_>
		 typename if_range<range_, node_if_mutable>::result string(range_ const& a) {
			if(my) string(a.begin(), range_size(a));
			return *this;
			}
		node_if_mutable string(char const*const& cstring) {
			if(my) string(cstring, clength(cstring));
			return *this;
			}
		template<size_t size_>
		 node_if_mutable string(char const (&cstring)[size_]) {
			return string(cstring, size_ - 1);
			}
	private:
		bool grow(size_t capacity, bool exactly) {
			// when this returns true, at least capacity is available and it is an array or object
			// capacity can be 0, then it cannot fail
			if(my->me.capacity < capacity) { // ignores integer size integer overflow
				size_t s = capacity;
				if(!exactly && capacity < static_cast<uint32_t>(-1) / 2) {
					s = 16;
					while(s < capacity) s *= 2;
					}
				auto nodes = static_cast<memory_node**>(mym->allocate(sizeof(memory_node**) * s, alignof(memory_node**)));
				if(!nodes)
					return false;
				auto
					t = nodes,
					te = nodes + s;
				if(my->me.capacity) {
					auto
						f = my->nodes,
						fe = f + my->size;
					while(f != fe)
						*t++ = *f++;
					}
				else {
					if(my->type != type_::object)
						my->type = type_::array;
					my->size = 0;
					}
				while(t != te)
					*t++ = 0;
				my->me.capacity = static_cast<uint32_t>(s);
				my->nodes = nodes;
				}
			else if(!my->me.capacity) {
				if(my->type != type_::object)
					my->type = type_::array;
				my->nodes = 0;
				my->size = 0;
				}
			return true;
			}
		bool grow() {
			return grow(my->me.capacity ? my->size + 1 : 1, false);
			}
		void nodes_clear() {
			if(my->me.capacity) {
				auto i = my->nodes, e = i + my->size;
				while(i != e) {
					(*i)->in = 0;
					(*i)->me.at = 0;
					++i;
					}
				my->me.capacity = 0;
				my->nodes = 0;
				my->size = 0;
				}
			}
		size_t clength(char const* b) const {
			auto e = b;
			if(e) while(*e) ++e;
			return static_cast<size_t>(e - b);
			}
		template<typename size_, typename iterator_>
		 bool copy(char8_t*& to, size_& to_size, iterator_ from, size_t from_size) {
			if(to_size < from_size && !(to = static_cast<char8_t*>(mym->allocate(from_size, 1))))
				return false;
			char8_t *t = to, *e = to + from_size;
			while(t != e) {
				*t++ = static_cast<char8_t>(*from);
				++from;
				}
			___water_debug(bool clipped = ) string_size_clip(to_size, to, e);
			___water_assert(!clipped && "string too long");
			return true;
			}
	};

}}
#endif
