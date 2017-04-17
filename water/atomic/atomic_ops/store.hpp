// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_ATOMIC_ATOMIC_OPS_STORE_HPP
#define WATER_ATOMIC_ATOMIC_OPS_STORE_HPP
#include <water/atomic/atomic_ops/bits.hpp>
namespace water { namespace atomic { namespace atomic_ops {

template<typename i_, barrier_t b_, typename x_ = i_, typename if_ = void> struct
 store : has<0> {
	static void do_it(i_*, i_);
	};

#define WATER_ATOMIC_ATOMIC_OPS_make(type, prefix, barrier, if_) \
	template<typename i_> struct \
	 store<i_, barrier, type, typename if_not_ao_t<if_>::result> : has<has_set> { \
		static void do_it(i_ *a, i_ b) { AO ## prefix ## store_ ## barrier(a, b); } \
		};
#define WATER_ATOMIC_ATOMIC_OPS_make_none(type, prefix, if_) \
	template<typename i_> struct \
	 store<i_, none, type, typename if_not_ao_t<if_>::result> : has<has_set> { \
		static void do_it(i_ *a, i_ b) { AO ## prefix ## store(a, b); } \
		};
	
// ao_t
#ifdef AO_HAVE_store
	WATER_ATOMIC_ATOMIC_OPS_make_none(ao_t, _, void)
	#endif
#ifdef AO_HAVE_store_acquire
	WATER_ATOMIC_ATOMIC_OPS_make(ao_t, _, acquire, void)
	#endif
#ifdef AO_HAVE_store_release
	WATER_ATOMIC_ATOMIC_OPS_make(ao_t, _, release, void)
	#endif
#ifdef AO_HAVE_store_full
	WATER_ATOMIC_ATOMIC_OPS_make(ao_t, _, full, void)
	#endif

// int
#ifdef AO_HAVE_int_store
	WATER_ATOMIC_ATOMIC_OPS_make_none(unsigned int, _int_, unsigned int)
	#endif
#ifdef AO_HAVE_int_store_acquire
	WATER_ATOMIC_ATOMIC_OPS_make(unsigned int, _int_, acquire, unsigned int)
	#endif
#ifdef AO_HAVE_int_store_release
	WATER_ATOMIC_ATOMIC_OPS_make(unsigned int, _int_, release, unsigned int)
	#endif
#ifdef AO_HAVE_int_store_full
	WATER_ATOMIC_ATOMIC_OPS_make(unsigned int, _int_, full, unsigned int)
	#endif

// short
#ifdef AO_HAVE_short_store
	WATER_ATOMIC_ATOMIC_OPS_make_none(unsigned short, _short_, unsigned short)
	#endif
#ifdef AO_HAVE_short_store_acquire
	WATER_ATOMIC_ATOMIC_OPS_make(unsigned short, _short_, acquire, unsigned short)
	#endif
#ifdef AO_HAVE_short_store_release
	WATER_ATOMIC_ATOMIC_OPS_make(unsigned short, _short_, release, unsigned short)
	#endif
#ifdef AO_HAVE_short_store_full
	WATER_ATOMIC_ATOMIC_OPS_make(unsigned short, _short_, full, unsigned short)
	#endif

// char
#ifdef AO_HAVE_char_store
	WATER_ATOMIC_ATOMIC_OPS_make_none(unsigned char, _char_, unsigned char)
	#endif
#ifdef AO_HAVE_char_store_acquire
	WATER_ATOMIC_ATOMIC_OPS_make(unsigned char, _char_, acquire, unsigned char)
	#endif
#ifdef AO_HAVE_char_store_release
	WATER_ATOMIC_ATOMIC_OPS_make(unsigned char, _char_, release, unsigned char)
	#endif
#ifdef AO_HAVE_char_store_full
	WATER_ATOMIC_ATOMIC_OPS_make(unsigned char, _char_, full, unsigned char)
	#endif

#undef WATER_ATOMIC_ATOMIC_OPS_make
#undef WATER_ATOMIC_ATOMIC_OPS_make_none

}}}
#endif
