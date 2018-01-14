// Copyright 2018 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_FIXED_TESTS_MEMORY_FUNCTIONS_HPP
#define WATER_FIXED_TESTS_MEMORY_FUNCTIONS_HPP
#include <water/fixed/tests/bits.hpp>
#include <water/vector.hpp>
#include <water/later.hpp>
namespace water { namespace fixed { namespace tests {

/*

test that all memory_atomic functions work, with one thread

*/

class memory_functions_allocator {
	water::allocator_nothrow my;
	memory_functions_allocator *mypointer;
	public:
		memory_functions_allocator() :	
			my{}, 
			mypointer{this}
			{}
		void* allocate(size_t bytes) {
			___water_test(mypointer == this);
			if(mypointer != this)
				return 0;
			void *r = my.allocate(align_bytes(bytes) + sizeof(size_t));
			if(!r)
				return 0;
			*static_cast<size_t*>(static_cast<void*>(static_cast<char*>(r) + align_bytes(bytes))) = bytes;
			return r;
			}
		void free(void *pointer, size_t bytes) {
			___water_test(mypointer == this);
			___water_test(pointer && bytes);
			size_t *saved = static_cast<size_t*>(static_cast<void*>(static_cast<char*>(pointer) + align_bytes(bytes)));
			___water_test(*saved == bytes);
			my.free(pointer, align_bytes(bytes) + sizeof(size_t));
			}
	private:
		static constexpr size_t align_bytes(size_t a) {
 			return a + (a % alignof(size_t) ? alignof(size_t) - (a % alignof(size_t)) : 0);
 			}
	};

template<bool statistics_> void memory_functions_statistics(size_t bytes) {
	memory_atomic<memory_functions_allocator, statistics_> memory{bytes};
	___water_test(memory.bytes() >= bytes); // it can round up to align bytes
	
	vector<void*> allocations;
	auto free_allocations = later([&allocations, &memory] {
		for(auto a : allocations)
			if(a)
				memory.free(a);
		allocations.clear();
		});
	
	auto allocator_nothrow = allocator_nothrow_for(memory);
	auto allocator_nothrow_lock_free = allocator_nothrow_lock_free_for(memory);
	
	// tests the allocate / free functions
	
	void *p = allocator_nothrow_lock_free.allocate(1);
	___water_test(!p);
	p = allocator_nothrow_lock_free.template allocate<char>();
	___water_test(!p);
	
	size_t memory_use = memory.memory_use();
	___water_test(!memory_use);
	
	memory.block_size(9);
	___water_test(memory.block_size() == 9);
	allocations.push_back(memory.allocate());
	___water_test(allocations.back());
	allocations.push_back(memory.allocate_lock_free());
	___water_test(allocations.back());
	allocations.push_back(allocator_nothrow.allocate(1));
	___water_test(allocations.back());
	allocations.push_back(allocator_nothrow_lock_free.allocate(bytes));
	___water_test(allocations.back());
	allocations.push_back(allocator_nothrow.template allocate<char>());
	___water_test(allocations.back());
	allocations.push_back(allocator_nothrow_lock_free.template allocate<char>(memory.bytes()));
	___water_test(allocations.back());
	
	allocator_nothrow_lock_free.template free<char>(allocations.back(), memory.bytes());
	allocations.pop_back();
	allocator_nothrow.template free<char>(allocations.back());
	allocations.pop_back();
	allocator_nothrow_lock_free.free(allocations.back(), bytes);
	allocations.pop_back();
	allocator_nothrow.free(allocations.back(), 1);
	allocations.pop_back();
	memory.free(allocations.back());
	allocations.pop_back();
	memory.free(allocations.back());
	allocations.pop_back();
	
	___water_test(allocations.empty());
	
	// allocate more
	
	size_t s = 21;
	while(s--) {
		void *a = memory.allocate();
		___water_test(a);
		allocations.push_back(a);
		}
	
	memory.block_size(memory_atomic_block_size);
	___water_test(memory.block_size() == memory_atomic_block_size);
	
	s = 1234;
	
	while(s--) {
		void *a = allocator_nothrow.allocate((s % memory.bytes()) + 1);
		___water_test(a);
		size_t l = memory.lookup(a);
		void *b = memory.lookup(l);
		___water_test(l);
		___water_test(a == b);
		allocator_nothrow.free(a, (s % memory.bytes()) + 1);
		a = memory.allocate_lock_free();
		___water_test(a);
		allocations.push_back(a);
		if(allocations.size() == 100)
			memory.block_size(66);
		}
	
	// free some and then allocate more again
	
	size_t
		full = allocations.size(),
		rounds = 10;
	
	while(rounds--) {
		// free every other or every 4th
		auto a = allocations.begin();
		bool flip = false;
		while(a < allocations.end()) {
			if(flip)
				memory.free(allocations.back());
			else {
				memory.free(*a);
				*a = allocations.back();
				}
			flip = !flip;
			allocations.pop_back();
			a += 1 + (rounds & 1);
			}
		// fill up again
		size_t fill = full - allocations.size();
		if(!(rounds & 1))
			fill /= 2;
		while(fill--) {
			void *a = memory.allocate_lock_free();
			___water_test(a);
			size_t l = memory.lookup(a);
			void *b = memory.lookup(l);
			___water_test(l);
			___water_test(a == b);
			allocations.push_back(a);
			}
		}
	
	memory_use = memory.memory_use();
	auto statistics = memory.statistics();
	___water_test(memory_use == statistics.memory_use);
	___water_test(!statistics_ || statistics.allocations_now == allocations.size());
	}

inline void memory_functions() {
	memory_functions_statistics<true>(1);
	memory_functions_statistics<false>(1);
	memory_functions_statistics<true>(99);
	memory_functions_statistics<false>(99);
	memory_functions_statistics<true>(sizeof(size_t));
	memory_functions_statistics<true>(sizeof(size_t));
	}

}}}
#endif
