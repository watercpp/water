// Copyright 2017-2018 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_XML_JSON_MEMORY_BLOCK_HPP
#define WATER_XML_JSON_MEMORY_BLOCK_HPP
#include <water/align_max.hpp>
namespace water { namespace xml_json {

// block is last in memory [ .... memory ....  [block]], begin points to start, free to the first free byte. this pointer is end of allocateable memory
// push with set_undo = true to set undo point if its not set
// push with set_undo = false will undo any allocations done with set_undo = true
// no_undo() to keep allocations made with undo
struct memory_block {
	memory_block *list;
	char *begin;
	char *free;
	char *undo;
	size_t size() const {
		return static_cast<size_t>(static_cast<char const*>(static_cast<void const*>(this + 1)) - begin);
		}
	size_t unused() const {
		return static_cast<char const*>(static_cast<void const*>(this)) - (undo ? undo : free);
		}
	size_t used() const {
		return static_cast<size_t>((undo ? undo : free) - begin);
		}
	void* push(size_t bytes, size_t align, bool set_undo) {
		char *end = static_cast<char*>(static_cast<void*>(this));
		if(!set_undo)
			undo_now();
		if(!align)
			align = align_max;
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
	void undo_now() {
		if(undo) {
			free = undo;
			undo = 0;
			}
		}
	void clear() {
		free = begin;
		undo = 0;
		}
	};

inline size_t memory_block_size_align(size_t bytes) {
	size_t a = bytes % align_max;
	if(a) bytes += align_max - a;
	return bytes;
	}


inline memory_block* memory_block_from(void *pointer, size_t bytes) {
	___water_assert(pointer && !(bytes % align_max) && bytes >= sizeof(memory_block));
	char *p = static_cast<char*>(pointer);
	auto b = static_cast<memory_block*>(static_cast<void*>(p + bytes - sizeof(memory_block)));
	b->list = 0;
	b->begin = b->free = p;
	b->undo = 0;
	return b;
	}

static_assert(align_max % alignof(memory_block) == 0, "");

}}
#endif
