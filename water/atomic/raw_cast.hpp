// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_ATOMIC_RAW_CAST_HPP
#define WATER_ATOMIC_RAW_CAST_HPP
#include <water/water.hpp>
#include <water/types/types.hpp>
namespace water { namespace atomic {
namespace _ {

	template<size_t s_> inline void
	 do_raw_cast_copy(unsigned char* t, unsigned char const* f);
	template<> inline void
	 do_raw_cast_copy<1>(unsigned char* t, unsigned char const* f) {
		t[0] = f[0];
		}
	template<> inline void
	 do_raw_cast_copy<2>(unsigned char* t, unsigned char const* f) {
		t[0] = f[0];
		t[1] = f[1];
		}
	template<> inline void
	 do_raw_cast_copy<3>(unsigned char* t, unsigned char const* f) {
		t[0] = f[0];
		t[1] = f[1];
		t[2] = f[2];
		}
	template<> inline void
	 do_raw_cast_copy<4>(unsigned char* t, unsigned char const* f) {
		t[0] = f[0];
		t[1] = f[1];
		t[2] = f[2];
		t[3] = f[3];
		}
	template<> inline void
	 do_raw_cast_copy<8>(unsigned char* t, unsigned char const* f) {
		t[0] = f[0];
		t[1] = f[1];
		t[2] = f[2];
		t[3] = f[3];
		t[4] = f[4];
		t[5] = f[5];
		t[6] = f[6];
		t[7] = f[7];
		}
	template<size_t s_> inline void
	 do_raw_cast_copy(unsigned char* t, unsigned char const* f) {
		do_raw_cast_copy<s_ / 2>(t, f);
		do_raw_cast_copy<s_ - (s_ / 2)>(t + (s_ / 2), f + (s_ / 2));
		}
	
	template<size_t s_> inline void
	 do_raw_cast_0(unsigned char *t);
	template<> inline void
	 do_raw_cast_0<0>(unsigned char *) {
		}
	template<> inline void
	 do_raw_cast_0<1>(unsigned char *t) {
		t[0] = 0;
		}
	template<> inline void
	 do_raw_cast_0<2>(unsigned char *t) {
		t[1] = t[0] = 0;
		}
	template<> inline void
	 do_raw_cast_0<3>(unsigned char *t) {
		t[2] = t[1] = t[0] = 0;
		}
	template<> inline void
	 do_raw_cast_0<4>(unsigned char *t) {
		t[3] = t[2] = t[1] = t[0] = 0;
		}
	template<size_t s_> inline void
	 do_raw_cast_0(unsigned char *t) {
		do_raw_cast_0<s_ / 2>(t);
		do_raw_cast_0<s_ - (s_ / 2)>(t + (s_ / 2));
		}
	
	// static_cast
	//   int <-> int
	//   bool <-> int
	//   pointer <-> pointer (via void*)
	//
	// reinterpret_cast
	//   pointer <-> int
	//
	// copy
	//   int <-> other, if sizeof(other) <= sizeof(int)
	//   char[size] <-> any, if sizeof(any) <= size
	
	template<
		typename t_,
		typename f_,
		int int_ =
			types::equal_plain<t_, f_>::result ? 22 :
			(types::is_int<t_>::result ? 10 : 0) + (types::is_int<f_>::result ? 1 : 0)
		> struct
	 do_raw_cast : types::true_result {
		static t_ do_it(f_ const& f) {
			t_ t;
			do_raw_cast_copy<sizeof(t_) <= sizeof(f_) ? sizeof(t_) : sizeof(f_)>(
				static_cast<unsigned char*>(static_cast<void*>(&t)),
				static_cast<unsigned char const*>(static_cast<void const*>(&f))
				);
			do_raw_cast_0<(sizeof(t_) > sizeof(f_) ? sizeof(t_) - sizeof(f_) : 0)>(
				static_cast<unsigned char*>(static_cast<void*>(&t)) + sizeof(f_)
				);
			return t;
			}
		};
	
	// same
	template<typename t_> struct
	 do_raw_cast<t_, t_, 22> : types::true_result {
		static t_ const& do_it(t_ const& f) { return f; }
		};
	
	// int/bool <-> int/bool
	template<typename t_, typename f_> struct
	 do_raw_cast<t_, f_, 11> : types::true_result {
		static t_ do_it(f_ f) { return static_cast<t_>(f); }
		};
	template<typename f_> struct
	 do_raw_cast<bool, f_, 01> : types::true_result {
		static bool do_it(f_ f) { return f ? true : false; } // visual c++ warns if static_cast
		};
	template<typename t_> struct
	 do_raw_cast<t_, bool, 10> :  do_raw_cast<t_, bool, 11>
		{};
		
	// pointer <-> int 
	template<typename t_, typename f_> struct
	 do_raw_cast<t_*, f_, 01> : types::true_result {
		static t_* do_it(f_ f) { return reinterpret_cast<t_*>(f); }
		};
	template<typename t_, typename f_> struct
	 do_raw_cast<t_, f_*, 10> : types::true_result {
		static t_ do_it(f_* f) { return reinterpret_cast<t_>(f); }
		};
	
	// pointer <-> pointer
	template<typename t_, typename f_> struct
	 do_raw_cast<t_*, f_*, 00> : types::bool_result<!types::is_const<f_>::result && !types::is_volatile<f_>::result> {
		static t_* do_it(f_* f) { return static_cast<t_*>(static_cast<void*>(f)); }
		};
	template<typename t_, typename f_> struct
	 do_raw_cast<t_ const*, f_*, 00> : types::bool_result<!types::is_volatile<f_>::result> {
		static t_ const* do_it(f_* f) { return static_cast<t_ const*>(static_cast<void const*>(f)); }
		};
	template<typename t_, typename f_> struct
	 do_raw_cast<t_ volatile*, f_*, 00> : types::bool_result<!types::is_const<f_>::result> {
		static t_ volatile* do_it(f_* f) { return static_cast<t_ volatile*>(static_cast<void volatile*>(f)); }
		};
	template<typename t_, typename f_> struct
	 do_raw_cast<t_ const volatile*, f_*, 00> : types::true_result {
		static t_ const volatile* do_it(f_* f) { return static_cast<t_ const volatile*>(static_cast<void const volatile*>(f)); }
		};

}

template<typename to_, typename form_> struct
 bad_raw_cast :
	types::type_assert<_::do_raw_cast<typename types::no_const<to_>::result, form_> >
		{};

template<typename to_, typename from_> to_
 raw_cast(from_ const& from) {
	// This is not an atomic operation. it is used by water::atomic to cast between the atomic_type
	// and the value_type of atomic::alias or other types with water::atomic_tag.
	//
	// when to_ and from_ are the same type, this just returns from.
	//
	// when to_ and from_ are different types, this will cast using:
	// - static_cast
	//   - integer to/from integer (integer means built-in integer type, not bool)
	//   - bool to/from integer_type
	//   - pointer to/from pointer (it will not cast away const or volatile, but it will cast to unrelated pointer types)
	// - reinterpret_cast
	//   - integer to/from pointer (it does not care if the pointer can be represented by the integer type)
	// - memcpy
	//   - to/from anything not above
	//   - if sizeof(to_) > sizeof(from_) the remaining bytes are set to 0
	//   - if sizeof(from_) < sizeof(to_) only sizeof(from_) bytes are copied
	//
	// This could maybe be useful to get/set the value of atomic::alias non-atomically
	//
	//   type t = ...;
	//   atomic::alias<type> a;
	//   a.atomic() = atomic::raw_cast<atomic::alias<type>::atomic_type>(t);
	//   t = atomic::raw_cast<type>(a.atomic());
	//
	static_cast<void>(bad_raw_cast<to_, from_>::assert);
	return _::do_raw_cast<typename types::no_const<to_>::result, from_>::do_it(from);
	}

}}
#endif
