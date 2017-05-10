// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_ATOMIC_OLD_EXAMPLES_EXAMPLE2_HPP
#define WATER_ATOMIC_OLD_EXAMPLES_EXAMPLE2_HPP
#include <water/atomic_old/constants.hpp>
#include <water/types/types.hpp>
namespace water { namespace atomic_old { namespace example2 {

/*

This example has a special atomic_old::compare_set operation that can atomically modify something with sizeof(long[2]).

*/

inline bool compare_and_swap(long* atomic, long compare, long swap) {
	// pretend this is atomic and full barrier
	if(*atomic != compare) return false;
	*atomic = swap;
	return true;
	}

inline bool compare_and_swap_two(
	long* atomic,
	long compare0,
	long compare1, 
	long swap0,
	long swap1
	) {
		// pretend this is atomic and full barrier
		if(atomic[0] != compare0 || atomic[1] != compare1) return false;
		atomic[0] = swap0;
		atomic[1] = swap1;
		return true;
		}

inline void read_write_barrier() {
	// pretend this is a full barrier
	}

typedef long int_t; // pretend that sizeof(long) == sizeof(void*)

struct two {
	long my_[2];
	bool operator==(two a) const {
		return my_[0] == a.my_[0] && my_[1] == a.my_[1];
		}
	bool operator!=(two a) const {
		return !this->operator==(a);
		}
	};

// water::atomic_old cannot use arrays. this struct is needed to make compare_and_swap_two work with water::atomic_old.
//
// water::atomic_old needs structs like this to...
// - have == and != operators
// - be a pod struct
// - not be padded, in this case sizeof(two) must be sizeof(long[2])

int const two_assert = types::if_assert<sizeof(two) == sizeof(long[2])>::assert;

template<typename type_, barrier_t barrier_> struct
 atomic {
	enum { has = 0 };
	};

template<> struct
 atomic<long, none> {
	enum {
		has = has_get | has_set
		};
	static long get(long const volatile* atomic) {
		return *atomic;
		}
	static void set(long volatile* atomic, long set) {
		*atomic = set;
		}
	// this assumes reading and writing something with the same size a
	// pointer is atomic, and that the volatile qualifier on the atomic
	// is enough to make the compiler do the right thing
	};

template<> struct
 atomic<long, full> {
	enum has_ {
		has = has_compare_set
		};
	static bool compare_set(long* atomic, long compare, long set) {
		return compare_and_swap(atomic, compare, set);
		}
	};

template<> struct
 atomic<two, full> {
	enum {
		has = has_compare_set
		};
	static bool compare_set(two* atomic, two compare, two set) {
		return compare_and_swap_two(atomic->my_, compare.my_[0], compare.my_[1], set.my_[0], set.my_[1]);
		}
	};


// atomic_type_from<type_>::result is
// - long if sizeof(type_) <= sizeof(long)
// - two if sizeof(type_) <= sizeof(two)
// - else void
template<typename type_> struct
 atomic_type_from :
	types::ifel<
		sizeof(type_) <= sizeof(long),
		long,
		types::ifel<
			sizeof(type_) <= sizeof(two),
			two,
			void
			>
		> {};


inline void barrier() {
	read_write_barrier();
	}

}}}
#endif
