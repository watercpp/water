// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_ALLOCATOR_NOTHROW_HPP
#define WATER_ALLOCATOR_NOTHROW_HPP

// The main non-throwing dynamic memory allocator used by water. Replace with your own by defining WATER_ALLOCATOR_NOTHROW

#ifdef WATER_ALLOCATOR_NOTHROW
namespace water {
using allocator_nothrow = WATER_ALLOCATOR_NOTHROW;
}

#elif defined(WATER_NO_STD)

#include <water/allocator_malloc.hpp>
namespace water {
using allocator_nothrow = allocator_malloc_nothrow;
}

#else

#include <water/water.hpp>
#include <new>
namespace water {

struct allocator_nothrow {
	static void* allocate(size_t bytes) noexcept {
		return ::operator new(bytes, std::nothrow);
		}
	static void free(void *pointer, size_t = 0) noexcept {
		::operator delete(pointer, std::nothrow);
		}
	template<typename type_> static
	 type_* allocate(size_t count = 1) noexcept {
		return static_cast<type_*>(::operator new(sizeof(type_) * count, std::nothrow));
		}
	template<typename type_> static
	 void free(void *pointer, size_t count = 1) noexcept {
		::operator delete(pointer, std::nothrow);
		}
	};

}

#endif
#endif
