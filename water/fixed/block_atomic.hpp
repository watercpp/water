// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_FIXED_BLOCK_ATOMIC_HPP
#define WATER_FIXED_BLOCK_ATOMIC_HPP
#include <water/water.hpp>
#include <water/atomic/atomic.hpp>
namespace water { namespace fixed {

// this is used by memory_atomic, dont use it alone

class block_atomic {
	using unsigned_ = atomic::uint_t;
	unsigned_
		my = 0,
		mysize, // number of allocations, not size in bytes
		mymask = 0; // number of bits needed for mysize, the rest of my is a change counter
	block_atomic *mylist;
	public:
		block_atomic(size_t bytes, size_t size, block_atomic *list) :
			mysize(static_cast<unsigned_>(size)),
			mylist(list)
			{
			while((mymask = (mymask << 1) | 1) < mysize);
			char *m = static_cast<char*>(memory());
			size_t i = 0;
			do {
				*static_cast<size_t*>(static_cast<void*>(m + bytes * i)) = i + 1;
				} while(++i != mysize);
			}
		void* allocate(size_t bytes, bool& more) {
			unsigned_ a = atomic::get<atomic::acquire>(my);
			void *r = 0;
			unsigned_ at, count;
			do {
				at = a & mymask;
				count = a & ~mymask; // change count is only changed by free
				if(at >= mysize)
					r = 0;
				else {
					r = static_cast<char*>(memory()) + at * bytes;
					at = static_cast<unsigned_>(*static_cast<size_t*>(r));
					}
				more = at < mysize;
				} while(!atomic::compare_set_else_non_atomic_get(my, a, at | count, a));
			return r;
			}
		void free(void *pointer, size_t bytes) {
			unsigned_
				p = static_cast<unsigned_>(static_cast<char*>(pointer) - static_cast<char*>(memory())) / bytes,
				a = my,
				next,
				count;
			do {
				next = a & mymask;
				count = (a & ~mymask) + (mymask + 1); // change change count, will wrap around
				*static_cast<size_t*>(pointer) = next;
				} while(!atomic::compare_set_else_non_atomic_get<atomic::release>(my, a, p | count, a));
			}
		bool inside(void *pointer, size_t bytes) {
			return memory() <= pointer && pointer <= static_cast<char*>(memory()) + mysize * bytes;
			}
		block_atomic* list() const {
			return mylist;
			}
		size_t size() const {
			return mysize;
			}
	private:
		void *memory() {
			return this + 1;
			}
	};

}}
#endif
