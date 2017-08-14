// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_JSON_MEMORY_HPP
#define WATER_JSON_MEMORY_HPP
#include <water/json/node.hpp>
#include <water/json/memory_block.hpp>
#include <water/allocator.hpp>
namespace water { namespace json {

size_t constexpr
	memory_block_size_default = 4096,
	memory_block_size_auto_max = 1024 * 1024;

//
// all nodes and strings are allocated and kept in a memory object
//
// memory is a kind of stack. it is not possible to deallcoate 1 allocation.
// clear() will deallocate everything.
//
template<typename allocator_ = water::allocator> class
 memory {
	public:
		using allocator_type = allocator_;
		using node_type = json::node<memory<allocator_>>;
	private:
		allocator_type myallocator {};
		size_t myblocksize = memory_block_size_default;
		memory_block *my = 0;
		bool myerror = false;
		bool myauto = true; // automatic block size up to memory_block_size_auto_max
	public:
		memory() = default;
		memory(allocator_type const& a) :
			myallocator{a}
			{}
		memory(memory&& a) :
 			myallocator{static_cast<allocator_type&&>(a.myallocator)},
 			myblocksize{a.myblocksize},
 			my{a.my},
			myerror{a.myerror},
			myauto{a.myauto}
			{
 			a.my = 0;
 			}
 		memory(memory const& a) = delete;
 		~memory() {
 			while(auto *b = my) {
 				my = b->list;
 				myallocator.free(b->begin, b->size());
 				}
 			}
 		memory& operator=(memory const&) = delete;
 		memory& operator=(memory&& a) {
 			swap(a);
 			return *this;
 			}
 		void swap(memory& a) {
 			swap_from_swap(myallocator, a.myallocator);
 			swap_from_swap(myblocksize, a.myblocksize);
 			swap_from_swap(my, a.my);
			swap_from_swap(myerror, a.myerror);
			swap_from_swap(myauto, a.myauto);
 			}
		size_t block_size() const {
			return myblocksize;
			}
		void block_size(size_t a) {
			myblocksize = a - (a % alignof(memory_node));
			if(!myblocksize)
				myblocksize = memory_block_size_default;
			}
		void block_size_automatic(bool a) {
			// if block size should grow up to memory_block_size_auto_max automatically.
			// if will grow when allocate() is called with size larger than the current block_size.
			// automatic growing probably only works well when the raw document is allocated
			// in this before it is parsed.
			myauto = a;
			}
		void* allocate(size_t bytes, size_t align = 0) {
			return allocate_do(bytes, align, false);
			}
		void* allocate_with_undo(size_t bytes, size_t align = 0) {
			// after successful allocations, call no_undo() otherwise the next allocate() or clear() will take over the allocations made with undo
			return allocate_do(bytes, align, true);
			}
		bool allocate_has_failed() const {
			return myerror;
			}
		void no_undo() {
			auto b = my;
			while(b) {
				b->no_undo();
				b = b->list;
				}
			}
		void undo() {
			// free all allocations made with allocate_with_undo
			auto b = my;
			while(b) {
				b->undo_now();
				b = b->list;
				}
			}
		void clear() {
			auto b = my;
			while(b) {
				b->clear();
				b = b->list;
				}
			myerror = false;
			}
		size_t total_size() const {
			size_t r = 0;
			auto b = my;
			while(b) {
				r += b->size();
				b = b->list;
				}
			return r;
			}
		size_t allocated_now() const {
			size_t r = 0;
			auto b = my;
			while(b) {
				r += b->used();
				b = b->list;
				}
			return r;
			}
		size_t blocks() const {
			size_t r = 0;
			auto b = my;
			while(b) {
				++r;
				b = b->list;
				}
			return r;
			}
		node_type create() {
			auto n = static_cast<memory_node*>(allocate(sizeof(memory_node), alignof(memory_node)));
			if(n) *n = {};
			return {*this, n};
			}
	private:
		void* allocate_do(size_t bytes, size_t align, bool undo) {
			// allocate bytes. the memory will be freed by clear() or the destructor.
			// if the underlying allocator fails (returns 0 or throws) allocate_has_failed() will return true
			void *r = 0;
			if(my && (r = my->push(bytes, align, undo)))
				return r;
			// maybe another block has free space
			if(my) {
				auto
					*at = my->list,
					*next = my;
				while(at) {
					if(r = at->push(bytes, align, undo)) {
						// make this current
						next->list = at->list;
						at->list = my;
						my = at;
						return r;
						}
					next = at;
					at = at->list;
					}
				}
			// allocate new block
			size_t allocate = memory_block_size_align(bytes) + sizeof(memory_block);
			if(myauto && myblocksize < allocate)
				while(myblocksize < allocate && myblocksize < memory_block_size_auto_max)
					myblocksize *= 2;
			if(allocate < myblocksize)
				allocate = myblocksize;
			bool error_before = myerror;
			myerror = true;
			if(void *a = myallocator.allocate(allocate)) { // throws maybe
				myerror = error_before;
				auto *b = memory_block_from(a, allocate);
				b->list = my;
				my = b;
				r = b->push(bytes, align, undo);
				}
			return r;
			}
	};

template<typename allocator_> void swap(memory<allocator_>& a, memory<allocator_>& b) {
	a.swap(b);
	}

template<typename allocator_> node<memory<allocator_>> create(memory<allocator_>& a) { // water::xml has this
	return a.create();
	}

}}
#endif
