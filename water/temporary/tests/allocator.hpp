// Copyright 2018 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_TEMPORARY_TESTS_ALLOCATOR_HPP
#define WATER_TEMPORARY_TESTS_ALLOCATOR_HPP
#include <water/temporary/tests/bits.hpp>
namespace water { namespace temporary { namespace tests {

struct allocator_test {
	struct type {	
		upointer_t a, b;
		};
	template<typename memory_> void operator()(memory_& memory) {
		test(allocator_for(memory));
		test(allocator_nothrow_for(memory));
		}
	template<typename allocator_> void test(allocator_ allocator) {
		auto allocator2{allocator};
		allocator = allocator2;
			
		// allocate + free
			{
			size_t ps = 123;
			void *p = allocator.allocate(ps);
			___water_test(ps);
			dead(p, ps);
	
			size_t is = 123;
			int *i = allocator.template allocate<int>(is);
			___water_test(i);
			dead(i, is * sizeof(int));
	
			allocator.template free<int>(i, is);
			allocator.free(p, ps);
			}
		
		// push pop retain release
			{
			size_t as = 1;
			void *a = allocator.push(as);
			dead(a, as);
			___water_test(allocator.find(a));
			___water_test(!allocator.find(this));
			___water_test(allocator.retain(a));
			___water_test(allocator.release(a));
			___water_test(!allocator.retain(this));
			___water_test(!allocator.release(this));
			
			size_t bs = 123;
			void *b = allocator.resize(a, as, bs);
			___water_test(a == b);
			dead(b, bs);
			as = 54;
			a = allocator.resize(a, bs, as);
			dead(a, as);
			___water_test(a == b);
			allocator.pop(a, as);
		
			a = 0;
			as = 0;
			b = 0;
			bs = 0;
			a = allocator.push_at_least(as, 67);
			___water_test(a);
			___water_test(as >= 123);
			___water_test(allocator.find(static_cast<byte*>(a) + 45));
			dead(a, as);
			allocator.pop(a, static_cast<void*>(static_cast<byte*>(a) + as));
			
			size_t r = allocator.retain(a); // returns new retain count
			___water_test(r);
			dead(a, as);
			
			bs = 1;
			b = allocator.push(1);
			___water_test(b);
			___water_test(a != b);
			
			allocator.pop(b, bs);
			r = allocator.release(a); // returns old reatin count
			___water_test(r == 1);
			}
		
		// push pop retain release with type
			{
			size_t as = 123;
			type *a = allocator.template push<type>(as);
			dead(a, as * sizeof(type));
			___water_test(allocator.find(a + 122));
			___water_test(!allocator.find(this));
			___water_test(allocator.retain(a));
			___water_test(allocator.release(a));
			___water_test(!allocator.retain(this));
			___water_test(!allocator.release(this));
			
			size_t bs = 54;
			type *b = allocator.resize(a, as, bs);
			___water_test(a == b);
			dead(b, bs * sizeof(type));
			as = 123;
			a = allocator.resize(a, bs, as);
			dead(a, as);
			___water_test(a == b);
			allocator.pop(a, as);
		
			a = 0;
			as = 0;
			b = 0;
			bs = 0;
			a = allocator.template push_at_least<type>(as, 67);
			___water_test(a);
			___water_test(as >= 123);
			___water_test(allocator.find(a + 45));
			dead(a, as * sizeof(type));
			allocator.pop(a, a + as);
			
			size_t r = allocator.retain(a); // returns new retain count
			___water_test(r);
			dead(a, as * sizeof(type));
			
			bs = 1;
			b = allocator.template push<type>(1);
			___water_test(b);
			___water_test(a != b);
			
			allocator.pop(b, bs);
			r = allocator.release(a); // returns old reatin count
			___water_test(r == 1);
			}
		
		}
	};

inline void allocator_all() {
	memory_types_function(allocator_test{});
	}

}}}
#endif
