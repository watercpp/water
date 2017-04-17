// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_XML_THINGS_THINGS_HPP
#define WATER_XML_THINGS_THINGS_HPP
#include <water/xml/bits.hpp>
namespace water { namespace xml { namespace things {

// node is the data structure
// - tags have name but no value
// - text content has value but no name, nodes, attributes
// - attributes have name and value, but no nodes, attributes
//
// value is backwards, value_begin > value_end, if value should be decode()-ed
//
// the name_end_memory and value_end_memory is useful if the document is edited
// it should avoid many allocatins since the useable memory ends where the next
// tag name, value, attribute name or attribute value begins. for example it is
// possible to use the whole </tag> for the value here <tag>value</tag>

template<typename char_> struct
 node {
	node
		*previous,
		*next,
		*in,
		*nodes,
		*attributes;
	char_
		*name_begin,
		*name_end,
		*name_end_memory, // name_begin to name_end_memory can be used, name_begin to name_end is used now
		*value_begin,
		*value_end,
		*value_end_memory;
	};

constexpr size_t alignment = alignof(node<char>) > alignof(void*) ? alignof(node<char>) : alignof(void*);

// block is last in memory [ .... memory ....  [block]], begin points to start, free to the first free byte. this pointer is end of allocateable memory
// push with set_undo = true to set undo point if its not set
// push with set_undo = false will undo any allocations done with set_undo = true
// no_undo() to keep allocations made with undo
struct
 block {
	block *list;
	char *begin;
	char *free;
	char *undo;
	size_t size() const {
		return static_cast<size_t>(static_cast<char const*>(static_cast<void const*>(this + 1)) - begin);
		}
	size_t unused() const {
		return static_cast<char const*>(static_cast<void const*>(this)) - (undo ? undo : free);
		}
	void* push(size_t bytes, size_t align, bool set_undo) {
		char *end = static_cast<char*>(static_cast<void*>(this));
		if(!set_undo && undo)
			free = undo;
		if(!align)
			align = alignment;
		char *free_aligned = free;
		if(size_t a = static_cast<size_t>(free - begin) % align)
			free_aligned += align - a;
		if(static_cast<size_t>(end - free_aligned) < bytes)
			return 0;
		if(set_undo && !undo)
			undo = free;
		free = free_aligned + bytes;
		return free_aligned;
		}
	void no_undo() {
		undo = 0;
		}
	void clear() {
		free = begin;
		undo = 0;
		}
	};

inline size_t block_size_align(size_t bytes) {
	size_t constexpr align = alignof(block) > alignment ? alignof(block) : alignment;
	size_t a = bytes % align;
	if(a)
		bytes += align - a;
	return bytes;
	}


inline block*
 block_from(void *pointer, size_t bytes) {
	//___assert(pointer && !(bytes % alignment) && bytes >= sizeof(block));
	char *p = static_cast<char*>(pointer);
	block *b = static_cast<block*>(static_cast<void*>(p + bytes - sizeof(block)));
	b->list = 0;
	b->begin = b->free = p;
	b->undo = 0;
	return b;
	}

template<typename iterator1_, typename iterator2_> bool
 equal(iterator1_ b1, iterator1_ e1, iterator2_ b2, iterator2_ e2) {
	if(e1 - b1 != e2 - b2)
		return false;
	while(b1 != e1 && *b1 == *b2) ++b1, ++b2;
	return b1 == e1;
	}
template<typename iterator_, typename char_, size_t size_> bool
 equal(char_ const (&a)[size_], iterator_ b, iterator_ e) {
	return equal(a + 0, a + size_ - 1, b, e);
	}

}}}
#endif
