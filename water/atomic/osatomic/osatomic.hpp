// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_ATOMIC_OSATOMIC_OSATOMIC_HPP
#define WATER_ATOMIC_OSATOMIC_OSATOMIC_HPP
#ifndef WATER_ATOMIC_OSATOMIC_TESTS_FAKE_HPP
	#include <libkern/OSAtomic.h>
#endif
#include <water/types/types.hpp>
#include <water/atomic/constants.hpp>
namespace water { namespace atomic { namespace osatomic {

// functions from <libkern/OSAtomic.h>, avalible in mac os x 10.4 or later
//
// full-barrier and no-barrier versions exsist. on 64-bit powerpc
// and any intel processor 64-bit versions exsist. all processors
// have 32-bit versions.

inline void barrier() {
	OSMemoryBarrier();
	}

template<typename t_> struct
 atomic_type_from :
	types::ifel<
		sizeof(t_) <= sizeof(int32_t),
		int32_t,
		#if !defined(WATER_CPU_POWERPC) || WATER_CPU_POWERPC > 32 // x86 has 64 bit always
		types::ifel<
			sizeof(t_) <= sizeof(int64_t),
			int64_t,
			void
			>
		#else
		void
		#endif
		> {};

typedef atomic_type_from<void*>::result int_t;

has_t const has_all = 
	has_compare_set |
	has_add_get |
	has_add1_get |
	has_subtract1_get;

template<typename t_, barrier_t b_, typename i_ = t_> struct
 atomic {
	enum has_e { has = 0 };
	};

template<typename i_> struct
 atomic<int32_t, none, i_> {
	enum has_e { has = has_all | has_get | has_set };
	static i_   get          (i_ const volatile* a) { return *a; }
	static void set          (i_ volatile* a, i_ b) { *a = b; }
	static bool compare_set  (i_* a, i_ b, i_ c)    { return OSAtomicCompareAndSwap32(b, c, a); }
	static i_   add_get      (i_* a, i_ b)          { return OSAtomicAdd32(b, a); }
	static i_   add1_get     (i_* a)                { return OSAtomicIncrement32(a); }
	static i_   subtract1_get(i_* a)                { return OSAtomicDecrement32(a); }
	};
	
template<typename i_> struct
 atomic<int32_t, full, i_> {
	enum has_e { has = has_all };
	static bool compare_set  (i_* a, i_ b, i_ c) { return OSAtomicCompareAndSwap32Barrier(b, c, a); }
	static i_   add_get      (i_* a, i_ b)       { return OSAtomicAdd32Barrier(b, a); }
	static i_   add1_get     (i_* a)             { return OSAtomicIncrement32Barrier(a); }
	static i_   subtract1_get(i_* a)             { return OSAtomicDecrement32Barrier(a); }
	};

#if !defined(WATER_CPU_POWERPC) || WATER_CPU_POWERPC > 32

	template<typename i_> struct
	 atomic<int64_t, none, i_> {
		enum has_e { has = has_all | (sizeof(void*) <= sizeof(i_) ? has_get | has_set : 0) };
		static i_   get          (i_ const volatile* a) { return *a; }
		static void set          (i_ volatile* a, i_ b) { *a = b; }
		static bool compare_set  (i_* a, i_ b, i_ c)    { return OSAtomicCompareAndSwap64(b, c, a); }
		static i_   add_get      (i_* a, i_ b)          { return OSAtomicAdd64(b, a); }
		static i_   add1_get     (i_* a)                { return OSAtomicIncrement64(a); }
		static i_   subtract1_get(i_* a)                { return OSAtomicDecrement64(a); }
		};
		
	template<typename i_> struct
	 atomic<int64_t, full, i_> {
		enum has_e { has = has_all };
		static bool compare_set  (i_* a, i_ b, i_ c) { return OSAtomicCompareAndSwap64Barrier(b, c, a); }
		static i_   add_get      (i_* a, i_ b)       { return OSAtomicAdd64Barrier(b, a); }
		static i_   add1_get     (i_* a)             { return OSAtomicIncrement64Barrier(a); }
		static i_   subtract1_get(i_* a)             { return OSAtomicDecrement64Barrier(a); }
		};
	
#endif //!defined(WATER_CPU_POWERPC) || WATER_CPU_POWERPC > 32

}}}
#endif
