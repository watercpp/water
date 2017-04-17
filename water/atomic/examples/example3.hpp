// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_ATOMIC_EXAMPLES_EXAMPLE3_HPP
#define WATER_ATOMIC_EXAMPLES_EXAMPLE3_HPP
#include <water/atomic/constants.hpp>
#include <water/types/types.hpp>
namespace water { namespace atomic { namespace example3 {

/*

This example only has the atomic::add_get operation

water::atomic can use this to make get and the various add and subtract functions. it cannot make set,
get_set or any of the compare_set functions.

*/

inline unsigned atomic_add_get(unsigned* atomic, unsigned add) {
	// pretend this function is atomic and full barrier
	return *atomic += add;
	}

typedef unsigned int_t;

template<typename type_, barrier_t barrier_> struct
 atomic {
	enum { has = 0 };
	};

template<> struct
 atomic<unsigned, full> {
	enum {
		has = has_add_get
		};
	static unsigned add_get(unsigned* atomic, unsigned add) {
		return atomic_add_get(atomic, add);
		}
	};
	
template<typename type_> struct
 atomic_type_from :
	types::ifel<
		sizeof(type_) <= sizeof(unsigned),
		unsigned,
		void
		> {};
	
inline void barrier() {
	unsigned a;
	atomic_add_get(&a, 0); // is full barrier
	}

}}}
#endif
