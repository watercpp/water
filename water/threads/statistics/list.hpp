// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_STATISTICS_LIST_HPP
#define WATER_THREADS_STATISTICS_LIST_HPP
#include <water/threads/statistics/reference.hpp>
#ifndef WATER_THREADS_STATISTICS_ALLOCATOR
	#include <water/allocator_nothrow.hpp>
#endif
#include <water/new_here.hpp>
#include <water/threads/yield.hpp>
namespace water { namespace threads { namespace statistics {

/*

global list of data
the list will leak memory, it is never freed
adding to the list uses atomic + yield lock

*/

template<typename = void> class
 list {
	struct block {
		data* begin;
		block *next;
		static constexpr unsigned size = 128;
		};
	using allocator =
		#ifdef WATER_THREADS_STATISTICS_ALLOCATOR
		WATER_THREADS_STATISTICS_ALLOCATOR;
		#else
		allocator_nothrow;
		#endif
	private:
		block* my = 0;
		atomic_uint mysize{0};
		atomic_uint mylock{0};
	private:
		static list me;
		constexpr list() noexcept = default;
	public:
		static list& get() noexcept {
			return me;
			}
		data* create(reference& to, void const* address, char const* class_name) {
		 	while(mylock.exchange(1, memory_order_acquire))
		 		yield();
		 	data* r = to.get();
		 	if(!r) {
		 		auto size = mysize.load(memory_order_relaxed);
		 		if(!(size % block::size)) {
		 			void *a = allocator{}.allocate(sizeof(data) * block::size + sizeof(block));
		 			___water_assert(a);
		 			if(a) {					
						block *b = static_cast<block*>(static_cast<void*>(static_cast<unsigned char*>(a) + sizeof(data) * block::size));
						r = b->begin = static_cast<data*>(a);
						b->next = 0;
						if(!my) my = b;
						else {
							block *p = my;
							while(p->next) p = p->next;
							p->next = b;
							}
						}
		 			}
		 		else {
		 			block *b = my;
		 			while(b->next) b = b->next;
		 			r = b->begin + size % block::size;
		 			}
		 		if(r) {
		 			new(here(r)) data(address, class_name);
		 			to.set(r);
		 			mysize.fetch_add(1, memory_order_relaxed);
		 			}
		 		}
		 	mylock.store(0, memory_order_release);
		 	return r;
		 	}
		size_t size() noexcept {
			// it is safe to read up to this size
			return mysize.load(memory_order_acquire);
			}
		block* blocks() noexcept {
			return my;
			}
		data* at(size_t a) noexcept {
			if(a >= size())
				return 0;
			size_t
				bi = 0, 
				ba = a / block::size;
			block *b = my;
			while(bi++ != ba) b = b->next;
			return b->begin + a % block::size;
			}
		data* find(void const* address) noexcept {
			size_t s = size(), i = 0;
			block *b = my;
			while(i != s) {
				data *d = b->begin + i % block::size;
				if(d->address() == address)
					return d;
				++i;
				if(!(i % block::size)) b = b->next;
				}
			return 0;
			}
		data* find(name const& a) noexcept {
			size_t s = size(), i = 0;
			block *b = my;
			while(i != s) {
				data *d = b->begin + i % block::size;
				if(d->name() == a)
					return d;
				++i;
				if(!(i % block::size)) b = b->next;
				}
			return 0;
			}
	};
	template<typename void_> list<void_> list<void_>::me;

}}}
#endif
