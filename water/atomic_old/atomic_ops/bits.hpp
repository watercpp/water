// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_ATOMIC_OLD_ATOMIC_OPS_BITS_HPP
#define WATER_ATOMIC_OLD_ATOMIC_OPS_BITS_HPP
#ifndef WATER_ATOMIC_OLD_ATOMIC_OPS_TESTS_FAKE_HPP
	#include <atomic_ops.h>
#endif
#include <water/types/types.hpp>
#include <water/atomic_old/constants.hpp>
namespace water { namespace atomic_old { namespace atomic_ops {

template<has_t has_> struct
 has : types::integer<has_t, has_>
	{};
	
typedef AO_t ao_t, int_t; // always sizeof(void*) and unsigned, maybe always unsigned long

#ifdef AO_HAVE_compare_double_and_swap_double_full	

	struct ao_double_t {
		AO_double_t my_;
		bool operator==(ao_double_t a) const {
			return my_.AO_val1 == a.my_.AO_val1 && my_.AO_val2 == a.my_.AO_val2;
			}
		bool operator!=(ao_double_t a) const {
			return !this->operator==(a);
			}
		};
	
	enum { ao_double_assert = types::if_assert<sizeof(ao_double_t) == sizeof(AO_double_t)>::assert };
		
#else
	
	struct ao_double_t { ao_t my_[2]; };

#endif

template<typename if_> struct
 if_not_ao_t {
	typedef void result;
	};
template<> struct
 if_not_ao_t<ao_t> {
	typedef ao_t result;
	};
 
}}}
#endif
