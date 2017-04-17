// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_ATOMIC_ATOMIC_OPS_COMPARE_AND_SWAP_HPP
#define WATER_ATOMIC_ATOMIC_OPS_COMPARE_AND_SWAP_HPP
#include <water/atomic/atomic_ops/bits.hpp>
namespace water { namespace atomic { namespace atomic_ops {

template<typename i_, barrier_t b_, typename x_ = i_, typename if_ = void> struct
 compare_and_swap : has<0> {
	static bool do_it(i_*, i_, i_);
	};

#ifdef AO_HAVE_compare_double_and_swap_double_full
	template<typename i_> struct
	 compare_and_swap<i_, full, ao_double_t, void> : has<has_compare_set> {
		static bool do_it(i_* a, i_ b, i_ c) {
			return AO_compare_double_and_swap_double_full(&a->my_, b.my_.AO_val1, b.my_.AO_val2, c.my_.AO_val1, c.my_.AO_val2) != 0;
			}
		};
#endif

#ifdef AO_HAVE_compare_double_and_swap_double_acquire
	template<typename i_> struct
	 compare_and_swap<i_, acquire, ao_double_t, void> : has<has_compare_set> {
		static bool do_it(i_* a, i_ b, i_ c) {
			return AO_compare_double_and_swap_double_acquire(&a->my_, b.my_.AO_val1, b.my_.AO_val2, c.my_.AO_val1, c.my_.AO_val2) != 0;
			}
		};
#endif
	
#ifdef AO_HAVE_compare_double_and_swap_double_release
	template<typename i_> struct
	 compare_and_swap<i_, release, ao_double_t, void> : has<has_compare_set> {
		static bool do_it(i_* a, i_ b, i_ c) {
			return AO_compare_double_and_swap_double_release(&a->my_, b.my_.AO_val1, b.my_.AO_val2, c.my_.AO_val1, c.my_.AO_val2) != 0;
			}
		};
#endif
	
#ifdef AO_HAVE_compare_double_and_swap_double
	template<typename i_> struct
	 compare_and_swap<i_, none, ao_double_t, void> : has<has_compare_set> {
		static bool do_it(i_* a, i_ b, i_ c) {
			return AO_compare_double_and_swap_double(&a->my_, b.my_.AO_val1, b.my_.AO_val2, c.my_.AO_val1, c.my_.AO_val2) != 0;
			}
		};
#endif

#define WATER_ATOMIC_ATOMIC_OPS_make(type, prefix, barrier, if_) \
	template<typename i_> struct \
	 compare_and_swap<i_, barrier, type, typename if_not_ao_t<if_>::result> : has<has_compare_set> { \
		static bool do_it(i_ *a, i_ b, i_ c) { return AO ## prefix ## compare_and_swap_ ## barrier(a, b, c) != 0; } \
		};
#define WATER_ATOMIC_ATOMIC_OPS_make_none(type, prefix, if_) \
	template<typename i_> struct \
	 compare_and_swap<i_, none, type, typename if_not_ao_t<if_>::result> : has<has_compare_set> { \
		static bool do_it(i_ *a, i_ b, i_ c) { return AO ## prefix ## compare_and_swap(a, b, c) != 0; } \
		};
	
// ao_t
#ifdef AO_HAVE_compare_and_swap
	WATER_ATOMIC_ATOMIC_OPS_make_none(ao_t, _, void)
	#endif
#ifdef AO_HAVE_compare_and_swap_acquire
	WATER_ATOMIC_ATOMIC_OPS_make(ao_t, _, acquire, void)
	#endif
#ifdef AO_HAVE_compare_and_swap_release
	WATER_ATOMIC_ATOMIC_OPS_make(ao_t, _, release, void)
	#endif
#ifdef AO_HAVE_compare_and_swap_full
	WATER_ATOMIC_ATOMIC_OPS_make(ao_t, _, full, void)
	#endif

// int
#ifdef AO_HAVE_int_compare_and_swap
	WATER_ATOMIC_ATOMIC_OPS_make_none(unsigned int, _int_, unsigned int)
	#endif
#ifdef AO_HAVE_int_compare_and_swap_acquire
	WATER_ATOMIC_ATOMIC_OPS_make(unsigned int, _int_, acquire, unsigned int)
	#endif
#ifdef AO_HAVE_int_compare_and_swap_release
	WATER_ATOMIC_ATOMIC_OPS_make(unsigned int, _int_, release, unsigned int)
	#endif
#ifdef AO_HAVE_int_compare_and_swap_full
	WATER_ATOMIC_ATOMIC_OPS_make(unsigned int, _int_, full, unsigned int)
	#endif

// short
#ifdef AO_HAVE_short_compare_and_swap
	WATER_ATOMIC_ATOMIC_OPS_make_none(unsigned short, _short_, unsigned short)
	#endif
#ifdef AO_HAVE_short_compare_and_swap_acquire
	WATER_ATOMIC_ATOMIC_OPS_make(unsigned short, _short_, acquire, unsigned short)
	#endif
#ifdef AO_HAVE_short_compare_and_swap_release
	WATER_ATOMIC_ATOMIC_OPS_make(unsigned short, _short_, release, unsigned short)
	#endif
#ifdef AO_HAVE_short_compare_and_swap_full
	WATER_ATOMIC_ATOMIC_OPS_make(unsigned short, _short_, full, unsigned short)
	#endif

// char
#ifdef AO_HAVE_char_compare_and_swap
	WATER_ATOMIC_ATOMIC_OPS_make_none(unsigned char, _char_, unsigned char)
	#endif
#ifdef AO_HAVE_char_compare_and_swap_acquire
	WATER_ATOMIC_ATOMIC_OPS_make(unsigned char, _char_, acquire, unsigned char)
	#endif
#ifdef AO_HAVE_char_compare_and_swap_release
	WATER_ATOMIC_ATOMIC_OPS_make(unsigned char, _char_, release, unsigned char)
	#endif
#ifdef AO_HAVE_char_compare_and_swap_full
	WATER_ATOMIC_ATOMIC_OPS_make(unsigned char, _char_, full, unsigned char)
	#endif

#undef WATER_ATOMIC_ATOMIC_OPS_make
#undef WATER_ATOMIC_ATOMIC_OPS_make_none

}}}
#endif
