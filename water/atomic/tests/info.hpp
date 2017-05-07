// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#if !defined(WATER_ATOMIC_TESTS_INFO_HPP) || defined(WATER_ATOMIC_INLINE)
#ifndef WATER_ATOMIC_INLINE
#define WATER_ATOMIC_TESTS_INFO_HPP
#include <water/atomic/tests/bits.hpp>
#include <water/types/link.hpp>
#include <water/str/str.hpp>
#include <water/str/type_name.hpp>
namespace water { namespace atomic { namespace tests {
#else
namespace str = water::str;
namespace types = water::types;
using water::atomic::tests::barrier_name;
#endif

/*

info() function at the bottom outputs the types the underlying can use and all functions avalible for each type

*/

inline char const* barrier_name(barrier_t b) {
	return
		b == none ? "none" :
		b == acquire ? "acquire" :
		b == release ? "release" :
		"full";
	}

template<typename a_, barrier_t b_> struct
 info_functions_do {
	template<typename o_>
	 static unsigned do_it(str::out<o_>& o, unsigned s) {
		if(has_t const h = underlying::atomic<a_, b_>::has) {
			o << str::repeat(' ', s) << str::type_name<a_>() << ", " << barrier_name(b_) << '\n';
			struct has_name {
				has_t has;
				char const* name;
				} x[] = {
					h & has_get, "get",
					h & has_set, "set",
					h & has_get_set, "get_set",
					h & has_compare_set, "compare_set",
					h & has_get_compare_set, "get_compare_set",
					h & has_add, "add",
					h & has_add_get, "add_get",
					h & has_get_add, "get_add",
					h & has_add1, "add1",
					h & has_add1_get, "add1_get",
					h & has_get_add1, "get_add1",
					h & has_subtract, "subtract",
					h & has_subtract_get, "subtract_get",
					h & has_get_subtract, "get_subtract",
					h & has_subtract1, "subtract1",
					h & has_subtract1_get, "subtract1_get",
					h & has_get_subtract1, "get_subtract1"
					};
			size_t const xs = sizeof(x) / sizeof(has_name); 
			size_t i = 0;
			do {
				if(x[i].has) o << str::repeat(' ', s + 2) << x[i].name << '\n';
				} while(++i != xs);
			o << str::el;
			return 1;
			}
		return 0;
		}
	};
	
template<typename a_, bool b_ = types::is_result<a_>::result> struct
 info_functions {
	template<typename o_> static
	 unsigned do_it(str::out<o_>& o, unsigned s) {
		unsigned r = 0;
		r += info_functions_do<typename a_::result, none>::do_it(o, s);
		r += info_functions_do<typename a_::result, acquire>::do_it(o, s);
		r += info_functions_do<typename a_::result, release>::do_it(o, s);
		r += info_functions_do<typename a_::result, full>::do_it(o, s);
		return r + info_functions<types::next<a_> >::do_it(o, s);
		}
	};
template<typename a_> struct
 info_functions<a_, false> {
	template<typename o_> static
	 unsigned do_it(str::out<o_> const&, unsigned) {
		return 0;
		}
	};
	
template<typename a_, bool b_ = types::is_result<a_>::result> struct
 info_types {
	template<typename o_> static
	 unsigned do_it(str::out<o_>& o, unsigned s) {
		o << str::repeat(' ', s) << str::type_name<typename a_::result>() << ", size=" << sizeof(typename a_::result) << '\n';
		return 1u + info_types<types::next<a_> >::do_it(o, s);
		}
	};
template<typename a_> struct
 info_types<a_, false> {
	template<typename o_> static
	 unsigned do_it(str::out<o_> const&, unsigned) {
		return 0;
		}
	};

template<
	typename a_ = typename underlying::atomic_type_from<char[1]>::result,
	typename r_ = types::link<>,
	bool pointer_ = types::is_pointer<a_>::result
	> struct
 info_other :
	info_other<
		typename underlying::atomic_type_from<char[sizeof(a_) + 1]>::result,
		typename types::ifel_plain<
			types::is_int<a_>::result,
			r_,
			types::insert<r_, a_, types::size<r_>::result>
			>::result,
		pointer_ || types::is_pointer<a_>::result
		> {};
template<typename r_> struct
 info_other<void, r_, false> :
	types::ifel_plain<
		types::is_pointer<typename underlying::atomic_type_from<void*>::result>::result,
		types::insert<r_, typename underlying::atomic_type_from<void*>::result, types::size<r_>::result>,
		r_
		>::result {};
template<typename r_> struct
 info_other<void, r_, true> :
	r_
		{};
		
template<typename o_> void
 info(str::out<o_>& o, unsigned s = 0) {
	
	o << str::repeat(' ', s) << "water::atomic::tests::info\n\n";
	s += 2;
	
	o << str::repeat(' ', s) << "int_types\n";
	unsigned i = info_types<int_types>::do_it(o, s + 2);
	if(!i) o << str::repeat(' ', s + 2) << "no int types" << str::el;
	o << '\n';
	
	o	<< str::repeat(' ', s) << "other types\n";
	unsigned t = info_types<info_other<> >::do_it(o, s + 2);
	if(!t) o << str::repeat(' ', s + 2) << "no other types" << str::el;
	o << '\n';
	
	o << str::repeat(' ', s) << "int_t ................ " << str::type_name<int_t>() << '\n'
		<< str::repeat(' ', s) << "uint_t ............... " << str::type_name<uint_t>() << '\n'
		<< str::repeat(' ', s) << "alias_type_from<int*>  " << str::type_name<typename alias_type_from<int*>::result>() << '\n'
		<< str::repeat(' ', s) << "alias_type_from<bool>  " << str::type_name<typename alias_type_from<bool>::result>() << '\n'
		<< str::repeat(' ', s) << "alias_type_from<float> " << str::type_name<typename alias_type_from<float>::result>() << "\n\n";

	o << str::repeat(' ', s) << "functions in the underlying implementation:\n\n";
	if(i || t) {
		i = info_functions<int_types>::do_it(o, s + 2);
		t = info_functions<info_other<> >::do_it(o, s + 2);
		}
	if(!i && !t)
		o << str::repeat(' ', s + 2) << "none\n\n";
	
	o << str::flush;
	}

#ifndef WATER_ATOMIC_INLINE
}}}
#endif
#endif
