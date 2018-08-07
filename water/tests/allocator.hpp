// Copyright 2018 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_TESTS_ALLOCATOR_HPP
#define WATER_TESTS_ALLOCATOR_HPP
#include <water/water.hpp>
#include <water/test.hpp>
#include <water/allocator.hpp>
#include <water/allocator_malloc.hpp>
#include <water/allocator_nothrow.hpp>
#include <water/allocator_pointer.hpp>
namespace water { namespace tests {

/*

basic tests for water::allocator and similar classes

*/

template<size_t size_> struct
 allocator_test_type {
	char array[size_];
	};

inline void allocator_write_0(void *to, size_t size) {
	auto *t = static_cast<char*>(to);
	while(size) t[--size] = 0;
	}

template<typename allocator_> void allocator_size(allocator_&& allocator, size_t size) {
	void *pointer = allocator.allocate(size);
	___water_test(pointer);
	allocator_write_0(pointer, size);
	allocator.free(pointer, size);
	}

template<typename type_, typename allocator_> void allocator_type(allocator_&& allocator) {
	type_ *pointer = allocator.template allocate<type_>();
	___water_test(pointer);
	allocator_write_0(pointer, sizeof(type_));
	allocator.template free<type_>(pointer);
	}

template<typename type_, typename allocator_> void allocator_type(allocator_&& allocator, size_t count) {
	type_ *pointer = allocator.template allocate<type_>(count);
	___water_test(pointer);
	allocator_write_0(pointer, sizeof(type_) * count);
	allocator.template free<type_>(pointer, count);
	}

template<typename allocator_> void allocator_test(allocator_&& allocator) {
	size_t sizes[] {1, 4, 16, 128, 1024, 1024 * 1024};
	for(auto size : sizes)
		allocator_size(allocator, size);
	allocator_type<int>(allocator);
	allocator_type<int>(allocator, 123);
	allocator_type<allocator_test_type<128>>(allocator);
	allocator_type<allocator_test_type<128>>(allocator, 12);
	allocator_type<allocator_test_type<1024>>(allocator);
	allocator_type<allocator_test_type<1024>>(allocator, 12);
	}

inline void allocator_all() {
	allocator_test(allocator{});
	allocator_test(allocator_malloc{});
	allocator_test(allocator_malloc_nothrow{});
	allocator_test(allocator_nothrow{});
	allocator a;
	allocator_test(allocator_pointer_from(a));
	}

}}
#endif
