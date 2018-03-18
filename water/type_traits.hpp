// Copyright 2017-2018 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_TYPE_TRAITS_HPP
#define WATER_TYPE_TRAITS_HPP
#include <water/water.hpp>
#include <water/types/types.hpp>

// deleted copy construct/assign is seen as trivial by the standard :(
namespace water {
	namespace _ {
		template<typename a_, typename = void> struct
		 has_copy_constructor_do :
			types::bool_result<false>
				{};
		template<typename a_> struct
		 has_copy_constructor_do<a_, types::to_void<decltype(a_(types::make<a_ const&>()))>> :
			types::bool_result<true>
				{};

		// visual c++ 2015 needed this extra indirection
		template<typename a_, typename = decltype(types::make<a_&>() = types::make<a_ const&>())> struct
		 has_copy_assign_check {
			using result = void;
			};

		template<typename a_, typename = void> struct
		 has_copy_assign_do :
			types::bool_result<false>
				{};
		template<typename a_> struct
		 has_copy_assign_do<a_, typename has_copy_assign_check<a_>::result> :
			types::bool_result<true>
				{};
		}

	template<typename a_> struct has_copy_constructor : _::has_copy_constructor_do<a_> {};
	template<typename a_> struct has_copy_assign : _::has_copy_assign_do<a_> {};
	}

#if !defined(WATER_NO_STD)
	#include <type_traits>
	namespace water {
		bool constexpr type_traits_exist = true;
		//template<typename a_> struct has_trivial_constructor :    types::bool_result<std::is_trivially_constructible<a_>::value> {};
		template<typename a_> struct has_trivial_copy_assign :      types::bool_result<has_copy_assign<a_>::result && std::is_trivially_copy_assignable<a_>::value> {};
		template<typename a_> struct has_trivial_copy_constructor : types::bool_result<has_copy_constructor<a_>::result && std::is_trivially_copy_constructible<a_>::value> {};
		template<typename a_> struct has_trivial_destructor :       types::bool_result<std::is_trivially_destructible<a_>::value> {};
		template<typename a_> struct has_nothrow_copy_assign :      types::bool_result<has_copy_assign<a_>::result && std::is_nothrow_copy_assignable<a_>::value> {};
		template<typename a_> struct has_nothrow_copy_constructor : types::bool_result<has_copy_constructor<a_>::result && std::is_nothrow_copy_constructible<a_>::value> {};
	}

#elif \
(defined(WATER_COMPILER_MICROSOFT) && WATER_COMPILER_MICROSOFT >= 1400) || \
(defined(WATER_COMPILER_GCC) && WATER_COMPILER_GCC >= 43) || \
defined(WATER_COMPILER_CLANG)
	// https://gcc.gnu.org/onlinedocs/gcc/Type-Traits.html
	// http://clang.llvm.org/docs/LanguageExtensions.html
	// https://msdn.microsoft.com/en-us/library/ms177194.aspx
	namespace water {
			bool constexpr type_traits_exist = true;
			//template<typename a_> struct has_trivial_constructor :    types::bool_result<__has_trivial_constructor(a_)> {};
			template<typename a_> struct has_trivial_copy_assign :      types::bool_result<has_copy_assign<a_>::result && __has_trivial_assign(a_)> {};
			template<typename a_> struct has_trivial_copy_constructor : types::bool_result<has_copy_constructor<a_>::result && __has_trivial_copy(a_)> {};
			template<typename a_> struct has_trivial_destructor :       types::bool_result<__has_trivial_destructor(a_)> {};
			template<typename a_> struct has_nothrow_copy_assign :      types::bool_result<has_copy_assign<a_>::result && __has_nothrow_assign(a_)> {};
			template<typename a_> struct has_nothrow_copy_constructor : types::bool_result<has_copy_constructor<a_>::result && __has_nothrow_copy(a_)> {};
	}

#else
	namespace water {
		namespace _ {
			template<typename a_> struct
			 type_traits :
				types::bool_number<!types::is_class_struct_union<a_>::result>
					{};
		}
		bool constexpr type_traits_exist = false;
		//template<typename a_> struct has_trivial_constructor :    _::type_traits<a_> {};
		template<typename a_> struct has_trivial_copy_assign :      _::type_traits<a_> {};
		template<typename a_> struct has_trivial_copy_constructor : _::type_traits<a_> {};
		template<typename a_> struct has_trivial_destructor :       _::type_traits<a_> {};
		template<typename a_> struct has_nothrow_copy_assign :      _::type_traits<a_> {};
		template<typename a_> struct has_nothrow_copy_constructor : _::type_traits<a_> {};
	}

#endif

// test
namespace water { namespace tests { namespace type_traits {

	struct trivial { int i; double d; void* v; };
	
	class nontrivial {
		nontrivial *me, *copy;
		public:
			nontrivial() : me(this), copy(0) {}
			nontrivial(nontrivial const& a) : me(this), copy(a.me) {}
			nontrivial& operator=(nontrivial const& a) { copy = a.me; return *this; }
			~nontrivial() { if(copy) copy->copy = 0; } // that would crash
		};
		
	struct deleted {
		deleted() = default;
		deleted(deleted const&) = delete;
		deleted& operator=(deleted const&) = delete;
		};
	
	static_assert(has_copy_constructor<int>::result, "");
	static_assert(has_copy_assign<int>::result, "");
	static_assert(has_trivial_copy_assign<int>::result, "");
	static_assert(has_trivial_copy_constructor<int>::result, "");
	static_assert(has_trivial_destructor<int>::result, "");
	static_assert(has_nothrow_copy_assign<int>::result, "");
	static_assert(has_nothrow_copy_constructor<int>::result, "");
	
	static_assert(has_copy_constructor<trivial>::result, "");
	static_assert(has_copy_assign<trivial>::result, "");
	static_assert(!type_traits_exist || has_trivial_copy_assign<trivial>::result, "");
	static_assert(!type_traits_exist || has_trivial_copy_constructor<trivial>::result, "");
	static_assert(!type_traits_exist || has_trivial_destructor<trivial>::result, "");
	static_assert(!type_traits_exist || has_nothrow_copy_assign<trivial>::result, "");
	static_assert(!type_traits_exist || has_nothrow_copy_constructor<trivial>::result, "");
	
	static_assert(has_copy_constructor<nontrivial>::result, "");
	static_assert(has_copy_assign<nontrivial>::result, "");
	static_assert(!type_traits_exist || !has_trivial_copy_assign<nontrivial>::result, "");
	static_assert(!type_traits_exist || !has_trivial_copy_constructor<nontrivial>::result, "");
	static_assert(!type_traits_exist || !has_trivial_destructor<nontrivial>::result, "");
	
	static_assert(!has_copy_constructor<deleted>::result, "");
	static_assert(!has_copy_assign<deleted>::result, "");
	static_assert(!has_trivial_copy_assign<deleted>::result, "");
	static_assert(!has_trivial_copy_constructor<deleted>::result, "");
	static_assert(!type_traits_exist || has_trivial_destructor<deleted>::result, "");
	static_assert(!has_nothrow_copy_assign<deleted>::result, "");
	static_assert(!has_nothrow_copy_constructor<deleted>::result, "");

}}}
#endif
