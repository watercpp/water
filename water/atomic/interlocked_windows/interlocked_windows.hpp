// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_ATOMIC_INTERLOCKED_WINDOWS_INTERLOCKED_WINDOWS_HPP
#define WATER_ATOMIC_INTERLOCKED_WINDOWS_INTERLOCKED_WINDOWS_HPP
#include <water/types/types.hpp>
#include <water/int.hpp>
#include <water/atomic/constants.hpp>
#ifndef WATER_ATOMIC_INTERLOCKED_WINDOWS_TESTS_FAKE_HPP
	#include <water/windows.hpp>
	#ifdef WATER_NO_WINDOWS_H
		#error sorry, dont know how to declare interlocked functions without windows.h
		#endif
	#endif
namespace water { namespace atomic { namespace interlocked_windows {

/*

Interlocked* funtions from windows api

The functions avalible depend on WATER_WINDOWS_VERSION

if WATER_WINDOWS_VERSION >= WATER_WINDOWS_2003 64-bit functions exist, even for 32-bit windows.

if WATER_WINDOWS_VERSION < WATER_WINDOWS_2003 and windows is 64-bit InterlockedExchangePointer and
InterlockedCompareExchangePointer is used to make the other functions for 64-bit. int_t is 32-bit.

*/

typedef LONG i32_t;
typedef water::int_size<sizeof(void*)> iptr_t;

#if defined(WATER_WINDOWS_VERSION) && WATER_WINDOWS_VERSION >= WATER_WINDOWS_2003
	
	typedef LONGLONG i64_t;
	typedef types::ifel<sizeof(void*) == sizeof(i32_t), i32_t, i64_t>::result int_t;
	
#else

	typedef i32_t int_t;

#endif

template<typename t_> struct
 atomic_type_from :
	types::ifel<
		sizeof(t_) <= sizeof(i32_t),
		i32_t,
		typename types::ifel<
			#if defined(WATER_WINDOWS_VERSION) && WATER_WINDOWS_VERSION >= WATER_WINDOWS_2003
				sizeof(t_) <= sizeof(i64_t),
				i64_t,
			#else
				sizeof(t_) <= sizeof(void*),
				iptr_t,
			#endif
			void
			>
		> {};
		
template<typename i_> struct
 get_and_set_volatile {
	enum has_ { has = has_get | has_set };
	static i_   get(i_ const volatile* a) { return *a; }
	static void set(i_ volatile* a, i_ b) { *a = b; }
	};

template<typename t_, barrier_t b_, typename i_ = t_, bool if_ = true> struct
 atomic {
	enum has_ { has = 0 };
	};

template<typename i_> struct
 atomic<i32_t, none, i_, true> :
	get_and_set_volatile<i_> 
		{};

template<typename i_> struct
 atomic<i32_t, full, i_, true> {	
	// CompareExchange
	// Decrement
	// Exchange
	// ExchangeAdd
	// Increment
	enum has_ { has = 
		has_add1_get |
		has_get_add |
		has_get_compare_set |
		has_get_set |
		has_subtract1_get
		};
	static i_ add1_get     (i_ *a)               { return InterlockedIncrement(a); }
	static i_ get_add      (i_ *a, i_ b)         { return InterlockedExchangeAdd(a, b); }
	static i_ get_compare_set(i_ *a, i_ b, i_ c) { return InterlockedCompareExchange(a, c, b); }
	static i_ get_set      (i_ *a, i_ b)         { return InterlockedExchange(a, b); }
	static i_ subtract1_get     (i_ *a)          { return InterlockedDecrement(a); }
	};

#if defined(WATER_WINDOWS_VERSION) && WATER_WINDOWS_VERSION >= WATER_WINDOWS_2003

	template<typename i_> struct
	 atomic<i64_t, none, i_, sizeof(i64_t) <= sizeof(void*)> :
		get_and_set_volatile<i_> 
			{};

	template<typename i_> struct
	 atomic<i64_t, full, i_, true> {	
		// CompareExchange64 2003
		// Decrement64       2003
		// Exchange64        2003
		// ExchangeAdd64     2003
		// Increment64       2003
		enum has_ { has = 
			has_add1_get |
			has_get_add |
			has_get_compare_set |
			has_get_set |
			has_subtract1_get
			};
		static i_ add1_get     (i_ *a)               { return InterlockedIncrement64(a); }
		static i_ get_add      (i_ *a, i_ b)         { return InterlockedExchangeAdd64(a, b); }
		static i_ get_compare_set(i_ *a, i_ b, i_ c) { return InterlockedCompareExchange64(a, c, b); }
		static i_ get_set      (i_ *a, i_ b)         { return InterlockedExchange64(a, b); }
		static i_ subtract1_get     (i_ *a)          { return InterlockedDecrement64(a); }
		};
	
#else //defined(WATER_WINDOWS_VERSION) && WATER_WINDOWS_VERSION >= WATER_WINDOWS_2003

	template<typename i_> struct
	 atomic<iptr_t, none, i_, sizeof(iptr_t) != sizeof(int_t)> :
		get_and_set_volatile<iptr_t>
			{};

	template<typename i_> struct
	 atomic<iptr_t, full, i_, sizeof(iptr_t) != sizeof(int_t)> {	
		// CompareExchangePointer
		// ExchangePointer
		static i_ get_compare_set(i_ *a, i_ b, i_ c) {
			return InterlockedCompareExchangePointer(
				static_cast<void**>(static_cast<void*>(a)),
				reinterpret_cast<void*>(c),
				reinterpret_cast<void*>(b)
				);
			}
		static i_ get_set(i_ *a, i_ b) {
			return InterlockedExchange(
				static_cast<void**>(static_cast<void*>(a)),
				reinterpret_cast<void*>(b)
				);
			}
		};

#endif //defined(WATER_WINDOWS_VERSION) && WATER_WINDOWS_VERSION >= WATER_WINDOWS_2003

inline void barrier() {
	i32_t i;
	InterlockedExchange(&i, 0);
	}

}}}
#endif
