// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_ATOMIC_OLD_ATOMIC_OPS_FETCH_AND_ADD_HPP
#define WATER_ATOMIC_OLD_ATOMIC_OPS_FETCH_AND_ADD_HPP
#include <water/atomic_old/atomic_ops/bits.hpp>
namespace water { namespace atomic_old { namespace atomic_ops {

template<typename i_, barrier_t b_, typename x_ = i_, typename if_ = void> struct
 fetch_and_add : has<0> {
	static i_ do_it(i_*, i_);
	};

#define WATER_ATOMIC_OLD_ATOMIC_OPS_make(type, prefix, barrier, if_) \
	template<typename i_> struct \
	 fetch_and_add<i_, barrier, type, typename if_not_ao_t<if_>::result> : has<has_get_add> { \
		static i_ do_it(i_ *a, i_ b) { return AO ## prefix ## fetch_and_add_ ## barrier(a, b); } \
		};
#define WATER_ATOMIC_OLD_ATOMIC_OPS_make_none(type, prefix, if_) \
	template<typename i_> struct \
	 fetch_and_add<i_, none, type,  typename if_not_ao_t<if_>::result> : has<has_get_add> { \
		static i_ do_it(i_ *a, i_ b) { return AO ## prefix ## fetch_and_add(a, b); } \
		};
	
// ao_t
#ifdef AO_HAVE_fetch_and_add
	WATER_ATOMIC_OLD_ATOMIC_OPS_make_none(ao_t, _, void)
	#endif
#ifdef AO_HAVE_fetch_and_add_acquire
	WATER_ATOMIC_OLD_ATOMIC_OPS_make(ao_t, _, acquire, void)
	#endif
#ifdef AO_HAVE_fetch_and_add_release
	WATER_ATOMIC_OLD_ATOMIC_OPS_make(ao_t, _, release, void)
	#endif
#ifdef AO_HAVE_fetch_and_add_full
	WATER_ATOMIC_OLD_ATOMIC_OPS_make(ao_t, _, full, void)
	#endif

// int
#ifdef AO_HAVE_int_fetch_and_add
	WATER_ATOMIC_OLD_ATOMIC_OPS_make_none(unsigned int, _int_, unsigned int)
	#endif
#ifdef AO_HAVE_int_fetch_and_add_acquire
	WATER_ATOMIC_OLD_ATOMIC_OPS_make(unsigned int, _int_, acquire, unsigned int)
	#endif
#ifdef AO_HAVE_int_fetch_and_add_release
	WATER_ATOMIC_OLD_ATOMIC_OPS_make(unsigned int, _int_, release, unsigned int)
	#endif
#ifdef AO_HAVE_int_fetch_and_add_full
	WATER_ATOMIC_OLD_ATOMIC_OPS_make(unsigned int, _int_, full, unsigned int)
	#endif

// short
#ifdef AO_HAVE_short_fetch_and_add
	WATER_ATOMIC_OLD_ATOMIC_OPS_make_none(unsigned short, _short_, unsigned short)
	#endif
#ifdef AO_HAVE_short_fetch_and_add_acquire
	WATER_ATOMIC_OLD_ATOMIC_OPS_make(unsigned short, _short_, acquire, unsigned short)
	#endif
#ifdef AO_HAVE_short_fetch_and_add_release
	WATER_ATOMIC_OLD_ATOMIC_OPS_make(unsigned short, _short_, release, unsigned short)
	#endif
#ifdef AO_HAVE_short_fetch_and_add_full
	WATER_ATOMIC_OLD_ATOMIC_OPS_make(unsigned short, _short_, full, unsigned short)
	#endif

// char
#ifdef AO_HAVE_char_fetch_and_add
	WATER_ATOMIC_OLD_ATOMIC_OPS_make_none(unsigned char, _char_, unsigned char)
	#endif
#ifdef AO_HAVE_char_fetch_and_add_acquire
	WATER_ATOMIC_OLD_ATOMIC_OPS_make(unsigned char, _char_, acquire, unsigned char)
	#endif
#ifdef AO_HAVE_char_fetch_and_add_release
	WATER_ATOMIC_OLD_ATOMIC_OPS_make(unsigned char, _char_, release, unsigned char)
	#endif
#ifdef AO_HAVE_char_fetch_and_add_full
	WATER_ATOMIC_OLD_ATOMIC_OPS_make(unsigned char, _char_, full, unsigned char)
	#endif

#undef WATER_ATOMIC_OLD_ATOMIC_OPS_make
#undef WATER_ATOMIC_OLD_ATOMIC_OPS_make_none

}}}
#endif
