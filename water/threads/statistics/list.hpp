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
		atomic::uint_t mysize = 0;
		atomic::uint_t mylock = 0;
	private:
		static list me;
		constexpr list() noexcept = default;
	public:
		static list& get() noexcept {
			return me;
			}
		data* create(reference& to, void const* address, char const* class_name) {
		 	while(atomic::get_set<atomic::acquire>(mylock, 1))
		 		yield();
		 	data* r = atomic::get<atomic::none>(to);
		 	if(!r) {
		 		auto size = atomic::get<atomic::none>(mysize);
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
		 			atomic::set<atomic::none>(to, r);
		 			atomic::add1<atomic::none>(mysize);
		 			}
		 		}
		 	atomic::set<atomic::release>(mylock, 0);
		 	return r;
		 	}
		size_t size() noexcept {
			// it is safe to read up to this size
			return atomic::get<atomic::acquire>(mysize);
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
