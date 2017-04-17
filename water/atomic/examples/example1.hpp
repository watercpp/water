// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_ATOMIC_EXAMPLES_EXAMPLE1_HPP
#define WATER_ATOMIC_EXAMPLES_EXAMPLE1_HPP

/*

This file tries to document how to make an underlying implementation for water::atomic. it is written in plain c++, it is not atomic.

*/

#include <water/atomic/constants.hpp>
// the <water/atomic/constants.hpp> header contains
// - atomic::barrier_t and the none, acquire, release, full barriers
// - atomic::has_* flags

namespace water { namespace atomic { namespace example1 {

typedef long int_t;
// int_t should be a built-in integer type that always works with this atomic implementation. it can be signed or unsigned.


/*

The atomic class contains the atomic operations as static member functions

In this example, atomic operations exist only for int_t. The unspecialzied template is for type_ != int_t
The specialization is for int_t

The atomic class must have:
- template arguments:
  - typename type_, the type the functions use
  - barrier_t barrier_
- A compile-time constant has, an or-combination of water::atomic has flags
  - This tells water::atomic what functions exist
  - If there are no functions, has is 0

Atomic can have more template arguments, as long as they have default values. water::atomic will only
use the first 2

The functions are similar to the water::atomic functions except the first argument is a pointer
instead of a reference.
- The functions must never throw exceptions
- get_compare_set and compare_set can fail, the other functions cannot fail

In this example, atomic operations exist only for int_t
- The unspecialzied template is for type_ != int_t
- The specialization is for int_t, it has all possible functions

*/

template<typename type_, water::atomic::barrier_t barrier_> struct
 atomic {
	enum has_ { has = 0 };
	};

template<water::atomic::barrier_t barrier_> struct
 atomic<int_t, barrier_> {
	enum has_ {
		has =
			has_get |
			has_set |
			has_get_set |
			has_compare_set |
			has_get_compare_set |
			has_add |
			has_add_get |
			has_get_add |
			has_add1 |
			has_add1_get |
			has_get_add1 |
			has_subtract |
			has_subtract_get |
			has_get_subtract |
			has_subtract1 |
			has_subtract1_get |
			has_get_subtract1
		};
	static int_t get(int_t *atomic) {
		return *atomic;
		}
	static void set(int_t *atomic, int_t set) {
		*atomic = set;
		}
	static int_t get_set(int_t *atomic, int_t set) {
		int_t get = *atomic;
		*atomic = set;
		return get;
		}
	static bool compare_set(int_t *atomic, int_t compare, int_t set) {
		if(*atomic == compare) {
			*atomic = set;
			return true;
			}
		return false;
		}
	static int_t get_compare_set(int_t *atomic, int_t compare, int_t set) {
		int_t get = *atomic;
		if(*atomic == compare) *atomic = set;
		return get;
		}
	static void add(int_t *atomic, int_t add) {
		*atomic += add;
		}
	static int_t add_get(int_t *atomic, int_t add) {
		*atomic += add;
		return *atomic;
		}
	static int_t get_add(int_t *atomic, int_t add) {
		int_t get = *atomic;
		*atomic += add;
		return get;
		}
	static void add1(int_t *atomic) {
		*atomic += 1;
		}
	static int_t add1_get(int_t *atomic) {
		*atomic += 1;
		return *atomic;
		}
	static int_t get_add1(int_t *atomic) {
		int_t get = *atomic;
		*atomic += 1;
		return get;
		}
	static void subtract(int_t *atomic, int_t sub) {
		*atomic -= sub;
		}
	static int_t subtract_get(int_t *atomic, int_t sub) {
		*atomic -= sub;
		return *atomic;
		}
	static int_t get_subtract(int_t *atomic, int_t sub) {
		int_t get = *atomic;
		*atomic -= sub;
		return get;
		}
	static void subtract1(int_t *atomic) {
		*atomic -= 1;
		}
	static int_t subtract1_get(int_t *atomic) {
		*atomic -= 1;
		return *atomic;
		}
	static int_t get_subtract1(int_t *atomic) {
		int_t get = *atomic;
		*atomic -= 1;
		return get;
		}
	};

/*

atomic_type_from is a template containing a result type. it results
- a type with at least the same size as type_, that works with this atomic implementation (the atomic template above)
- void if no such type exists

water::atomic will only use the first template argument, type_, any additional template arguments,
like select_ above, must have default values.

This example only has operations for int_t, the result is int_t or void

  // if sizeof(float) <= sizeof(int_t), type is int_t
  typedef atomic_type_from<float>::result type;

*/

template<typename type_, bool select_ = sizeof(type_) <= sizeof(int_t)> struct
 atomic_type_from {
	typedef void result;
	};
template<typename type_> struct
 atomic_type_from<type_, true> {	
	typedef int_t result;
	};


inline void barrier() {
	// do someting to cause full barrier. cannot fail or throw exceptions
	}

}}}
#endif
