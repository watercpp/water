// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_FIXED_MEMORY_ATOMIC_HPP
#define WATER_FIXED_MEMORY_ATOMIC_HPP
#include <water/threads/mutex.hpp>
#include <water/allocator_nothrow.hpp>
#include <water/new_here.hpp>
#include <water/fixed/allocator.hpp>
#include <water/fixed/block_atomic.hpp>
namespace water { namespace fixed {

unsigned constexpr memory_atomic_block_size = 512;

struct memory_atomic_statistics {
	size_t memory_use;
	size_t blocks;
	atomic::uint_t
		allocate_quick, // allocated using the "hint", this should be larger than all others
		allocate_lock_free, // allocate_lock_free or allocate did not have to lock
		allocate_lock_free_failed, // allocate_lock_free returned 0
		allocate_locked, // means a new block was allocated
		allocate_locked_failed, // block allocation failed
		free,
		allocations_now;
	};
	
struct memory_atomic_no_statistics {
	size_t memory_use;
	size_t blocks;
	};

// allocator_ must not throw

template<typename allocator_ = void, bool statistics_ = false> class
 memory_atomic {
 	public:
 		using allocator_type = typename types::if_not_void<allocator_, water::allocator_nothrow>::result;
 		static unsigned constexpr align = sizeof(size_t);
 		using statistics_type = typename types::ifel<statistics_, memory_atomic_statistics, memory_atomic_no_statistics>::result;
 	private:
	 	atomic::alias<block_atomic*>
	 		mylist {},
 			myhint {}; // last seen block with free space
 		threads::mutex<threads::need_constexpr_constructor> mymutex;
 		allocator_type *myallocator = 0;
 		char myallocator_memory[sizeof(allocator_type)] {}; // need {} for constexpr constructor
 		size_t
 			mybytes,
 			myblocksize;
 		statistics_type mystatistics{};
 	public:
 		constexpr memory_atomic(size_t bytes, size_t block_size = 0) :
 			mybytes(align_bytes(bytes)),
 			myblocksize(block_size ? block_size : memory_atomic_block_size)
 			{}
 		memory_atomic(memory_atomic const&) = delete;
 		memory_atomic& operator=(memory_atomic const&) = delete;
 		~memory_atomic() {
 			block_atomic *list = atomic::get<atomic::acquire>(mylist);
 			if(list) {
 				auto lock = lock_move(mymutex);
 				do {
 					block_atomic *f = list;
 					list = list->list();
 					myallocator->free(f, sizeof(*f) + f->size() * mybytes);
 					} while(list);
 				myallocator->~allocator_type();
 				}
 			}
 		size_t bytes() const {
 			return mybytes;
 			}
 		memory_atomic& block_size(size_t a) {
 			auto lock = lock_move(mymutex);
 			if(a) myblocksize = a;
 			return *this;
 			}
 		size_t block_size() {
 			auto lock = lock_move(mymutex);
 			return myblocksize;
 			}
 		void* allocate() {
 			return allocate(false);
 			}
 		void* allocate_lock_free() {
 			return allocate(true);
 			}
 		void free(void* pointer) {
 			block_atomic *list = atomic::get<atomic::acquire>(mylist);
 			while(list && !list->inside(pointer, mybytes))
 				list = list->list();
 			if(list) {
 				list->free(pointer, mybytes);
 				atomic::set<atomic::none>(myhint, list);
 				statistics_free(mystatistics);
 				}
 			___water_assert(list);
 			}
 		bool allocate_block(size_t block_size = 0) {
 			// could throw
 			auto lock = lock_move(mymutex);
 			return allocate_block_locked(atomic::get(mylist), block_size) != 0;
 			}
 		size_t memory_use() {
 			block_atomic *list = atomic::get<atomic::acquire>(mylist);
 			size_t r = 0;
 			while(list) {
 				r += list->size();
 				list = list->list();
 				}
 			return r * mybytes;
 			}
 		statistics_type statistics() {
 			block_atomic *list = atomic::get<atomic::acquire>(mylist);
 			statistics_type r = mystatistics;
 			while(list) {
 				++r.blocks;
 				r.memory_use += list->size();
 				list = list->list();
 				}
 			r.memory_use *= mybytes;
 			// allocations now is sum of all allocations - free
 			r.allocations_now = (r.allocate_quick + r.allocate_lock_free + r.allocate_locked) - r.free;
 			return r;
 			}
 	private:
 		static constexpr size_t align_bytes(size_t a) {
 			return a + (a % align ? align - (a % align) : 0);
 			}
 		void* allocate(bool lock_free) {
			bool more;
			auto block = atomic::get<atomic::none>(myhint);
 			if(block)
 				if(void *r = block->allocate(mybytes, more)) {
 					statistics_allocate_quick(mystatistics);
 					return r;
 					}
 			auto list = atomic::get<atomic::acquire>(mylist);
			block = list;
			bool did_lock = false;
			while(true) {
	 			while(block) {
 					void *r = block->allocate(mybytes, more);
 					if(r) {
 						if(more)
 							atomic::set<atomic::none>(myhint, block);
 						statistics_allocate(mystatistics, did_lock);
 						return r;
 						}
 					block = block->list();
 					}
 				if(lock_free)
 					break;
 				auto lock = lock_move(mymutex);
 				did_lock = true;
 				block_atomic *now = atomic::get<atomic::none>(mylist);
 				if(list == now) {
 					list = block = allocate_block_locked(list); // throws
 					if(!block)
 						break;
 					}
 				}
 			statistics_allocate_failed(mystatistics, lock_free);
 			return 0;
 			}
 		block_atomic* allocate_block_locked(block_atomic *list, size_t block_size = 0) {
 			___water_threads_statistics(
 				if(!atomic::get<atomic::none>(mylist))
 					name_if(statistics_pointer(mymutex)) << "water::fixed::memory_atomic bytes=" << mybytes;
 				)
 			if(!block_size)
 				block_size = myblocksize;
 			if(!myblocksize)
 				return 0;
 			if(!myallocator)
 				myallocator = new(here(myallocator_memory)) allocator_type; // throws???
 			void *a = myallocator->allocate(mybytes * block_size + sizeof(block_atomic));
 			if(!a)
 				return 0;
 			block_atomic *b = new(here(a)) block_atomic(mybytes, block_size, list);
 			atomic::set<atomic::release>(mylist, b);
 			return b;
 			}
 		static void statistics_allocate_quick(memory_atomic_statistics& s) {
 			atomic::add1<atomic::none>(s.allocate_quick);
 			}
 		static void statistics_allocate(memory_atomic_statistics& s, bool did_lock) {
 			atomic::add1<atomic::none>(did_lock ? s.allocate_locked : s.allocate_lock_free);
 			}
 		static void statistics_allocate_failed(memory_atomic_statistics& s, bool lock_free) {
 			atomic::add1<atomic::none>(lock_free ? s.allocate_lock_free_failed : s.allocate_locked_failed);
 			}
 		static void statistics_free(memory_atomic_statistics& s) {
 			atomic::add1<atomic::none>(s.free);
 			}
 		static void statistics_allocate_quick(memory_atomic_no_statistics const&) {}
 		static void statistics_allocate(memory_atomic_no_statistics const&, bool) {}
 		static void statistics_allocate_failed(memory_atomic_no_statistics const&) {}
 		static void statistics_free(memory_atomic_no_statistics const&, bool) {}
 	};

template<typename allocator_, bool statistics_> allocator_throw<memory_atomic<allocator_, statistics_>>
 allocator_for(memory_atomic<allocator_, statistics_>& a) noexcept {
 	return a;
 	}

template<typename allocator_, bool statistics_> allocator_nothrow<memory_atomic<allocator_, statistics_>>
 allocator_nothrow_for(memory_atomic<allocator_, statistics_>& a) noexcept {
 	return a;
 	}

template<typename allocator_, bool statistics_> allocator_throw_lock_free<memory_atomic<allocator_, statistics_>>
 allocator_lock_free_for(memory_atomic<allocator_, statistics_>& a) noexcept {
 	return a;
 	}

template<typename allocator_, bool statistics_> allocator_nothrow_lock_free<memory_atomic<allocator_, statistics_>>
 allocator_nothrow_lock_free_for(memory_atomic<allocator_, statistics_>& a) noexcept {
 	return a;
 	}

}}
#endif
