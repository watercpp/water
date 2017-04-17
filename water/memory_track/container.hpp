// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_MEMORY_TRACK_CONTAINER_HPP
#define WATER_MEMORY_TRACK_CONTAINER_HPP
#include <water/memory_track/bits.hpp>
#include <water/memory_track/underlying_allocator.hpp>
namespace water { namespace memory_track {

// because water::vector header can depend on this being included before
// this is also safe to use after its been destroyed

template<typename value_> class
 container {
	public:
		using value_type = value_;
	private:
		value_type
			*mybegin = 0,
			*myend = 0,
			*mycapacity = 0;
	public:
		constexpr container() = default;
		container(container const&) = delete;
		container& operator=(container const&) = delete;
		~container() {
			if(mybegin) {
				auto *i = mycapacity;
				do (--i)->~value_type(); while(i != mybegin);
				underlying_allocator_nothrow{}.free(mybegin, static_cast<size_t>(mycapacity - mybegin) * sizeof(value_type));
				mybegin = myend = mycapacity = 0; // this matters, it can be used after its destroyed
				}
			}
		void swap(container& a) noexcept {
			swap(mybegin, a.mybegin);
			swap(myend, a.myend);
			swap(mycapacity, a.mycapacity);
			}
		value_type* begin() noexcept {
			return mybegin;
			}
		value_type const* begin() const noexcept {
			return mybegin;
			}
		value_type* end() noexcept {
			return myend;
			}
		value_type const* end() const noexcept {
			return myend;
			}
		size_t capacity() const noexcept {
			return mycapacity - mybegin;
			}
		size_t size() const noexcept {
			return myend - mybegin;
			}
		bool empty() const noexcept {
			return mybegin == myend;
			}
		value_type* insert(size_t at, value_type v) noexcept {
			if(myend == mycapacity && !grow(capacity() * 2))
				return 0;
			auto
				i = mybegin + at,
				e = myend;
			while(i != e) { // move
				e[0] = e[-1];
				--e;
				}
			++myend;
			*i = v;
			return i;
			}
		value_type* resize(size_t a) {
			if(capacity() < a && !grow(a))
				return 0;
			myend = mybegin + a; // works even if a is 0
			return mybegin;
			}
	private:
		static void swap(value_type*& a, value_type*& b) noexcept {
			value_type* s = a;
			a = b;
			b = s;
			}
		bool grow(size_t capacity) noexcept {
			if(!capacity)
				capacity = 32;
			container a;
			a.mybegin = static_cast<value_type*>(underlying_allocator_nothrow{}.allocate(sizeof(value_type) * capacity));
			___water_assert(a.mybegin && "memory allocation failed");
			if(!a.mybegin)
				return false;
			a.myend = a.mybegin + size();
			a.mycapacity = a.mybegin + capacity;
			auto
				f = mybegin,
				t = a.mybegin;
			while(t != a.myend)
				new(here(t++)) value_type(*f++);
			while(t != a.mycapacity)
				new(here(t++)) value_type();
			swap(a);
			return true;
			}
	};

}}
#endif
