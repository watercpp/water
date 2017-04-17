// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_ATOMIC_OSATOMIC_TESTS_FAKE_HPP
#define WATER_ATOMIC_OSATOMIC_TESTS_FAKE_HPP
#include <water/types/types.hpp>
#include <water/int.hpp>
#include <water/atomic/tests/fake/fake.hpp>
namespace water { namespace atomic { namespace osatomic {

#undef WATER_CPU_POWERPC
#define WATER_CPU_POWERPC 64

namespace fake = atomic::tests::fake;

typedef types::if_not_void<atomic::int_bits<32>, water::int_bits_at_least<32> >::result int32_t;
typedef types::if_not_void<atomic::int_bits<64>, water::int_bits_at_least<64> >::result int64_t;

inline void OSMemoryBarrier() { fake::barrier(); }
	
inline bool OSAtomicCompareAndSwap32(int32_t comp, int32_t swap, int32_t *a) {
	return fake::compare_set<none>(a, comp, swap);
	}
inline int32_t OSAtomicAdd32(int32_t add, int32_t *a) {
	return fake::add_get<none>(a, add);
	}
inline int32_t OSAtomicIncrement32(int32_t *a) {
	return fake::add1_get<none>(a);
	}
inline int32_t OSAtomicDecrement32(int32_t *a) {
	return fake::subtract1_get<none>(a);
	}
	
inline bool OSAtomicCompareAndSwap32Barrier(int32_t comp, int32_t swap, int32_t *a) {
	return fake::compare_set<full>(a, comp, swap);
	}
inline int32_t OSAtomicAdd32Barrier(int32_t add, int32_t *a) {
	return fake::add_get<full>(a, add);
	}
inline int32_t OSAtomicIncrement32Barrier(int32_t *a) {
	return fake::add1_get<full>(a);
	}
inline int32_t OSAtomicDecrement32Barrier(int32_t *a) {
	return fake::subtract1_get<full>(a);
	}

inline bool OSAtomicCompareAndSwap64(int64_t comp, int64_t swap, int64_t *a) {
	return fake::compare_set<none>(a, comp, swap);
	}
inline int64_t OSAtomicAdd64(int64_t add, int64_t *a) {
	return fake::add_get<none>(a, add);
	}
inline int64_t OSAtomicIncrement64(int64_t *a) {
	return fake::add1_get<none>(a);
	}
inline int64_t OSAtomicDecrement64(int64_t *a) {
	return fake::subtract1_get<none>(a);
	}
	
inline bool OSAtomicCompareAndSwap64Barrier(int64_t comp, int64_t swap, int64_t *a) {
	return fake::compare_set<full>(a, comp, swap);
	}
inline int64_t OSAtomicAdd64Barrier(int64_t add, int64_t *a) {
	return fake::add_get<full>(a, add);
	}
inline int64_t OSAtomicIncrement64Barrier(int64_t *a) {
	return fake::add1_get<full>(a);
	}
inline int64_t OSAtomicDecrement64Barrier(int64_t *a) {
	return fake::subtract1_get<full>(a);
	}

}}}
#endif
