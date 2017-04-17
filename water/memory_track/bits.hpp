// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_MEMORY_TRACK_BITS_HPP
#define WATER_MEMORY_TRACK_BITS_HPP
#include <water/water.hpp>
#include <water/int.hpp>
#include <water/throw_if.hpp>
#include <water/types/types.hpp>
#include <water/new_here.hpp>
namespace water { namespace memory_track {

using byte = unsigned char;

using large_t = uint_bits_at_least<64>;
// this will be at least 64 bits.
// if 1GB is allocated every second, it will overflow after 544 years
// used as a counter, it will overflow after 544 years if the count is incremented every nanosecond

using upointer_t = types::if_not_void<uint_size_at_least<sizeof(void*)>, size_t>::result;

inline upointer_t upointer(void const* a) noexcept {
	return reinterpret_cast<upointer_t>(a);
	}

struct exception {};

// string must be a literal! this keeps only a pointer to it.
class tag {
	char const *my;
	public:
		constexpr tag(char const *name = 0) noexcept :
			my(name)
			{}
		char const* name() const noexcept {
			return my;
			}
		bool operator==(tag t) const noexcept {
			char const
				*a = my,
				*b = t.my;
			if(a == b)
				return true;
			if(!a || !b)
				return false;
			while(*a == *b && *a) ++a, ++b;
			return *a == *b;
			}
		bool operator!=(tag t) const noexcept {
			return !operator==(t);
			}
	};

// used by allocators to tag what allocator + function was used to allocate
class allocator_tag {
	void const *mypointer = 0;
	void (*myfunction)() = 0;
	public:
		constexpr allocator_tag() = default;
		constexpr allocator_tag(void const *pointer, void (*function)()) noexcept :
			mypointer(pointer),
			myfunction(function)
			{}
		bool operator==(allocator_tag a) const noexcept {
			return mypointer == a.mypointer && myfunction == a.myfunction;
			}
		bool operator!=(allocator_tag a) const noexcept {
			return !operator==(a);
			}
	};


struct no_callback {
	template<typename cookie_> bool allocate(cookie_ const *cookie, size_t bytes_allocated_now) noexcept {
		return true; // return false to make allocation fail
		}
	template<typename cookie_> void free(cookie_ const *cookie) noexcept {
		}
	template<typename cookie_, typename tag_> bool free_error(cookie_ const *cookie, void *pointer, size_t bytes, tag_ const& tag, char const* error) noexcept {
		// cookie can be 0
		return true; // return true to breakpoint/assert, false is useful for testing
		}
	};

size_t constexpr pattern_size = sizeof(void*);

inline void pattern_set(void *pointer, size_t bytes) {
	if(pointer && bytes) {
		byte const dead[] = {0xde, 0xad};
		unsigned d = static_cast<unsigned>(upointer(pointer) & 1);
		auto
			i = static_cast<byte*>(pointer),
			e = i + bytes;
		do *i = dead[d = !d]; while(++i != e);
		}
	}

inline void const* pattern_check(void const* pointer, size_t bytes) {
	// return address where it does not match pattern, 0 if it matches
	if(!pointer || !bytes)
		return 0;
	byte const dead[] = {0xde, 0xad};
	unsigned d = static_cast<unsigned>(upointer(pointer) & 1);
	auto
		i = static_cast<byte const*>(pointer),
		e = i + bytes;
	do {
		if(*i != dead[d = !d])
			return i;
		} while(++i != e);
	return 0;
	}

template<typename unsigned_> void add(unsigned_& a, typename types::type<unsigned_>::result b) {
	if(a > static_cast<unsigned_>(-1) - b)
		a = static_cast<unsigned_>(-1);
	else
		a += b;
	}

template<typename unsigned_> void sub(unsigned_& a, typename types::type<unsigned_>::result b) {
	if(a < b)
		a = 0;
	else
		a -= b;
	}

template<typename unsigned_> void add(unsigned_& a) {
	if(a != static_cast<unsigned_>(-1))
		++a;
	}

template<typename unsigned_> void sub(unsigned_& a) {
	if(a)
		--a;
	}

}}
#endif
