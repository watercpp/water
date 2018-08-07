// Copyright 2018 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_TEMPORARY_TESTS_BLOCK_HPP
#define WATER_TEMPORARY_TESTS_BLOCK_HPP
#include <water/temporary/tests/bits.hpp>
namespace water { namespace temporary { namespace tests {

/*

test functions and block allocation from block.hpp

*/

template<typename allocator_, typename sizer_> void block_test(allocator_ allocator, sizer_ sizer) {
	size_t
		sizes[] = {1234, 12345, 123456, 1234567},
		large = 123456789;
	block *list = 0;
	bool once = true;
	while(true) {
		for(auto size : sizes) {
			list = allocate_block(&allocator, list, size, sizer);
			___water_test(list);
			___water_test(list->capacity() >= size);
			
			void *p = push(list, size, 1, 0, &allocator, sizer);
			___water_test(p);
			___water_test(find(list, p));
			dead(p, size);
			
			void *zero = push(list, large, 0, 0, &allocator, sizer);
			___water_test(!zero);
			size_t zero_size = 123;
			zero = push_at_least(list, zero_size, large, 0, 0, 0, &allocator, sizer);
			___water_test(!zero);
			___water_test(!zero_size);
			
			size_t r = retain(list, p);
			___water_test(r);
			r = retain(list, &allocator);
			___water_test(!r);
			r = retain(list, zero);
			___water_test(!r);
			r = release(list, p);
			___water_test(r);
			r = release(list, zero);
			___water_test(!r);
			r = release(list, &allocator);
			___water_test(!r);
			
			pop(list, p, size);
			r = release(list, p);
			___water_test(!r);
			
			size_t s = 0;
			p = push_at_least(list, s, size / 2, 0, 0, 0, &allocator, sizer);
			dead(p, s);
			___water_test(p);
			___water_test(s >= size / 2);
			p = resize(list, p, s, s - 1);
			___water_test(p);
			p = resize(list, p, s - 1, s);
			___water_test(p);
			
			size_t s2 = 0;
			void * p2 = push_at_least(list, s2, size, 2, alignof(block), 99, &allocator, sizer);
			s2 *= 2;
			___water_test(p2);
			___water_test(s2 >= size * 2);
			___water_test(reinterpret_cast<upointer_t>(p2) % alignof(block) == 0);
			dead(p2, s2);
			
			block *b = block_from_memory(0, p2, s2);
			___water_test(b);
			
			pop(list, p, static_cast<byte*>(p) + s);
			pop(list, p2, s2);
			}
		
		if(once) {
			once = false;
			void *p = push(list, 123, 0, 0, &allocator, sizer);
			___water_test(p);
			list = free_unused_blocks(&allocator, list);
			___water_test(list);
			___water_test(find(list, static_cast<byte*>(p) + 56));
			pop(list, p, 123);
			}
		else
			break;
		}
	auto l = list;
	do {
		___water_test(!l->retain);
		l = l->next;
		} while(l != list);
	list = free_block(&allocator, list);
	free_all_blocks(&allocator, list);
	list = 0;
	}

inline void block_all() {
	block_test(water::allocator_nothrow{}, sizer{});
	block_test(test_allocator{}, test_sizer{});
	}

}}}
#endif
