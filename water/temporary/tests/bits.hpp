// Copyright 2017-2018 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_TEMPORARY_TESTS_BITS_HPP
#define WATER_TEMPORARY_TESTS_BITS_HPP
#include <water/temporary/memory.hpp>
#include <water/temporary/memory_track.hpp>
#include <water/temporary/vector.hpp>
#include <water/test.hpp>
namespace water { namespace temporary { namespace tests {

inline void dead(void *pointer, size_t size) {
	// fill with 0xdead
	if(pointer && size) {
		auto *p = static_cast<byte*>(pointer);
		auto at = reinterpret_cast<upointer_t>(pointer) & 1;
		do {
			*p++ = at ? 0xad : 0xde;
			at = !at;
			} while(--size);
		}
	}

class test_allocator {
	water::allocator_nothrow my;
	size_t myallocations = 0;
	struct cookie {
		void *allocator;
		void *pointer;
		size_t size;
		};
	void copy(void *to, void const *from, size_t size) {
		auto *t = static_cast<byte*>(to);
		auto *f = static_cast<byte const*>(from);
		while(size--) *t++ = *f++;
		}
	public:
		~test_allocator() {
			___water_test(!myallocations);
			}
		void* allocate(size_t size) {
			void *pointer = my.allocate(size + sizeof(cookie));
			if(!pointer) return 0;
			dead(pointer, size + sizeof(cookie));
			cookie c{this, pointer, size};
			copy(static_cast<char*>(pointer) + size, &c, sizeof(c));
			++myallocations;
			return pointer;
			}
		void free(void *pointer, size_t size) {
			___water_test(pointer);
			cookie c{};
			copy(&c, static_cast<char*>(pointer) + size, sizeof(c));
			___water_test(c.allocator == this);
			___water_test(c.pointer == pointer);
			___water_test(c.size == size);
			dead(pointer, size + sizeof(cookie));
			--myallocations;
			my.free(pointer, size);
			}
	};

struct test_sizer {
	size_t operator()(size_t a) const {
		return a + a / 2;
		}
	};

struct not_a_str {};

template<typename anything_> not_a_str& operator<<(not_a_str& a, anything_ const&) { return a; }

using memory_types = types::list<
	memory<>,
	memory<test_allocator>,
	memory<test_allocator, test_sizer>,
	memory_track<test_allocator>,
	memory_track<void, test_sizer, not_a_str>
	>;

template<int at_ = 0> struct
 memory_types_do {
	template<typename function_> static void function(function_& function) {
		typename types::at<memory_types, at_>::result memory;
		function(memory);
		memory_types_do<at_ + 1>::function(function);
		}
	};
template<> struct
 memory_types_do<types::size<memory_types>::result> {
 	template<typename function_> static void function(function_&) {}
 	};

template<typename function_> void memory_types_function(function_&& function) {
	memory_types_do<>::function(function);
	}

}}}
#endif
