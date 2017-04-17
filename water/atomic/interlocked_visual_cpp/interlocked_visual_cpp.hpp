// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_ATOMIC_INTERLOCKED_VISUAL_CPP_INTERLOCKED_VISUAL_CPP_HPP
#define WATER_ATOMIC_INTERLOCKED_VISUAL_CPP_INTERLOCKED_VISUAL_CPP_HPP
#include <water/types/types.hpp>
#include <water/atomic/constants.hpp>
#include <water/atomic/interlocked_visual_cpp/intrinsic.hpp>
namespace water { namespace atomic { namespace interlocked_visual_cpp {

// visual c++ 2005 _Interlocked* intrinsic funtions
// - atomic operations for 16, 32 and 64-bit values always exist
// - visual c++ 2008 X86 64-bit has 128-bit atomic operations

#ifndef WATER_ATOMIC_INTERLOCKED_VISUAL_CPP_TESTS_FAKE_HPP

	typedef short i16_t;
	typedef long i32_t;
	typedef __int64 i64_t;
	
	#if (defined(_M_X64) || defined(_M_AMD64)) && _MSC_VER >= 1500
		
		__declspec(align(16)) struct a128 { //_InterlockedCompareExchange128
			i64_t my_[2];
			bool operator==(a128 a) const {
				return my_[0] == a.my_[0] && my_[1] == a.my_[1];
				}
			bool operator!=(a128 a) const {
				return my_[0] != a.my_[0] || my_[1] != a.my_[1];
				}
			};
	
	#endif
#endif

inline void
 barrier() {
	_ReadWriteBarrier();
	}

template<typename t_> struct
 atomic_type_from :
	types::ifel<sizeof(t_) <= sizeof(i16_t), i16_t,
	types::ifel<sizeof(t_) <= sizeof(i32_t), i32_t,
	types::ifel<sizeof(t_) <= sizeof(i64_t), i64_t,
	#if (defined(_M_X64) || defined(_M_AMD64)) && _MSC_VER >= 1500
	types::ifel<sizeof(t_) <= sizeof(a128), a128, void>
	#else
	void
	#endif
	> > > {};

typedef atomic_type_from<void*>::result int_t;

template<typename i_, barrier_t b_, typename x_ = i_> struct
 atomic {
	enum has_ { has = 0 };
	};

has_t const has_all =
	has_add1_get |
	has_get_add |
	has_get_compare_set |
	has_get_set |
	has_subtract1_get;

template<typename i_> struct
 get_and_set_volatile {
	enum has_ { has = has_get | has_set };
	static i_   get(i_ const volatile* a) { return *a; }
	static void set(i_ volatile* a, i_ b) { *a = b; }
	};

template<typename i_> struct
 atomic<i_, none, i16_t> : get_and_set_volatile<i16_t>
	{};
	
template<typename i_> struct
 atomic<i_, full, i16_t> {
	enum has_ { has = has_add1_get | has_subtract1_get | has_get_compare_set };
	static i_ get_compare_set(i_* a, i_ b, i_ c) { return _InterlockedCompareExchange16(a, c, b); }
	static i_ add1_get(i_* a)                    { return _InterlockedIncrement16(a); }
	static i_ subtract1_get(i_* a)               { return _InterlockedDecrement16(a); }
	};

template<typename i_> struct
 atomic<i_, none, i32_t> : get_and_set_volatile<i32_t>
	{};

template<typename i_> struct
 atomic<i_, full, i32_t> {
	enum has_ { has = has_all };
	static i_ add1_get     (i_ *a)               { return _InterlockedIncrement(a); }
	static i_ get_add      (i_ *a, i_ b)         { return _InterlockedExchangeAdd(a, b); }
	static i_ get_compare_set(i_ *a, i_ b, i_ c) { return _InterlockedCompareExchange(a, c, b); }
	static i_ get_set      (i_ *a, i_ b)         { return _InterlockedExchange(a, b); }
	static i_ subtract1_get     (i_ *a)          { return _InterlockedDecrement(a); }
	};

#if defined(_M_X64) || defined(_M_AMD64)

	template<typename i_> struct
	 atomic<i_, none, i64_t> : get_and_set_volatile<i64_t>
		{};
		
	template<typename i_> struct
	 atomic<i_, full, i64_t> {
		enum has_ { has = has_all };
		static i_ add1_get     (i_ *a)               { return _InterlockedIncrement64(a); }
		static i_ get_add      (i_ *a, i_ b)         { return _InterlockedExchangeAdd64(a, b); }
		static i_ get_compare_set(i_ *a, i_ b, i_ c) { return _InterlockedCompareExchange64(a, c, b); }
		static i_ get_set      (i_ *a, i_ b)         { return _InterlockedExchange64(a, b); }
		static i_ subtract1_get     (i_ *a)          { return _InterlockedDecrement64(a); }
		};

#else //defined(_M_X64) || defined(_M_AMD64)

	template<typename i_> struct
	 atomic<i_, full, i64_t> {
		enum has_ { has = has_get_compare_set };
		static i_ get_compare_set(i_ *a, i_ b, i_ c) { return _InterlockedCompareExchange64(a, c, b); }
		};

#endif //defined(_M_X64) || defined(_M_AMD64)

#if (defined(_M_X64) || defined(_M_AMD64)) && _MSC_VER >= 1500

	template<typename i_> struct
	 atomic<i_, full, a128> {
		enum has_ { has = has_get_compare_set | has_compare_set };
		static i_ get_compare_set(i_ *a, i_ b, i_ c) {
			_InterlockedCompareExchange128(a->my_, c.my_[1], c.my_[0], b.my_);
			return b;
			}
		static bool compare_set(i_ *a, i_ b, i_ c) {
			return _InterlockedCompareExchange128(a->my_, c.my_[1], c.my_[0], b.my_) != 0;
			}
		};

#endif //(defined(_M_X64) || defined(_M_AMD64)) && _MSC_VER >= 1500

}}}
#endif
