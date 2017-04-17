// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_ALLOCATOR_HPP
#define WATER_ALLOCATOR_HPP

/*

The main dynamic memory allocator used by water. Replace with your own by defining WATER_ALLOCATOR

This type of allocator classes are used throught water for dynamic memory allocation.

*/

#ifdef WATER_ALLOCATOR
namespace water {
using allocator = WATER_ALLOCATOR;
}

#elif defined(WATER_NO_STD)

#include <water/allocator_malloc.hpp>
namespace water {
using allocator = allocator_malloc;
}

#else

#include <water/water.hpp>
#include <new>
namespace water {

struct allocator {
	static void* allocate(size_t bytes) noexcept(false) {
		return ::operator new(bytes);
		}
	static void free(void *pointer, size_t = 0) noexcept {
		::operator delete(pointer);
		}
	template<typename type_> static
	 type_* allocate(size_t count = 1) noexcept(false) {
		return static_cast<type_*>(::operator new(sizeof(type_) * count));
		}
	template<typename type_> static
	 void free(void *pointer, size_t count = 1) noexcept {
		::operator delete(pointer);
		}
	};

}
#endif
#endif
