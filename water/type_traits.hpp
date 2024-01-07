// Copyright 2017-2024 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_TYPE_TRAITS_HPP
#define WATER_TYPE_TRAITS_HPP
#include <water/water.hpp>
#include <water/types.hpp>

// deleted copy construct/assign is seen as trivial by the standard :(
namespace water {
    namespace _ {
        
        template<typename a_, typename = void>
        bool constexpr has_copy_constructor_do = false;
        
        template<typename a_>
        bool constexpr has_copy_constructor_do<a_, to_void<decltype(a_(make_type<a_ const&>()))>> = true;

        // visual c++ 2015 needed this extra indirection
        template<typename a_, typename = decltype(make_type<a_&>() = make_type<a_ const&>())>
        struct has_copy_assign_check {
            using result = void;
        };

        template<typename a_, typename = void>
        bool constexpr has_copy_assign_do = false;
        
        template<typename a_>
        bool constexpr has_copy_assign_do<a_, typename has_copy_assign_check<a_>::result> = true;
    }

    template<typename a_> bool constexpr has_copy_constructor = _::has_copy_constructor_do<a_>;
    template<typename a_> bool constexpr has_copy_assign = _::has_copy_assign_do<a_>;
}

#if !defined(WATER_NO_STD)

    #include <type_traits>
    namespace water {
        bool constexpr type_traits_exist = true;
        //template<typename a_> bool constexpr has_trivial_constructor      = std::is_trivially_constructible<a_>::value;
        template<typename a_> bool constexpr has_trivial_copy_assign      = has_copy_assign<a_> && std::is_trivially_copy_assignable<a_>::value;
        template<typename a_> bool constexpr has_trivial_copy_constructor = has_copy_constructor<a_> && std::is_trivially_copy_constructible<a_>::value;
        template<typename a_> bool constexpr has_trivial_destructor       = std::is_trivially_destructible<a_>::value;
        template<typename a_> bool constexpr has_nothrow_copy_assign      = has_copy_assign<a_> && std::is_nothrow_copy_assignable<a_>::value;
        template<typename a_> bool constexpr has_nothrow_copy_constructor = has_copy_constructor<a_> && std::is_nothrow_copy_constructible<a_>::value;
    }

#elif \
(defined(WATER_COMPILER_MICROSOFT) && WATER_COMPILER_MICROSOFT >= 1400) || \
(defined(WATER_COMPILER_GCC) && WATER_COMPILER_GCC >= 43) || \
defined(WATER_COMPILER_CLANG)

    // https://gcc.gnu.org/onlinedocs/gcc/Type-Traits.html
    // http://clang.llvm.org/docs/LanguageExtensions.html
    // https://msdn.microsoft.com/en-us/library/ms177194.aspx
    
    // clang has deprecated __has_something and replaced them with __is_something
    #if defined(WATER_COMPILER_CLANG) && __has_builtin(__is_trivially_assignable)
    
    #include <water/is_no_to.hpp>
    
    namespace water {
        bool constexpr type_traits_exist = true;
        //template<typename a_> bool constexpr has_trivial_constructor      = __is_trivially_constructible(a_);
        template<typename a_> bool constexpr has_trivial_copy_assign      = has_copy_assign<a_> && __is_trivially_assignable(to_reference<a_>, to_reference<to_const<a_>>);
        template<typename a_> bool constexpr has_trivial_copy_constructor = has_copy_constructor<a_> && __is_trivially_copyable(a_);
        template<typename a_> bool constexpr has_trivial_destructor       = __is_trivially_destructible(a_);
        template<typename a_> bool constexpr has_nothrow_copy_assign      = has_copy_assign<a_> && __is_nothrow_assignable(to_reference<a_>, to_reference<to_const<a_>>);
        template<typename a_> bool constexpr has_nothrow_copy_constructor = has_copy_constructor<a_> && __is_nothrow_constructible(a_, to_reference<to_const<a_>>);
    }
    
    #else
    
    namespace water {
        bool constexpr type_traits_exist = true;
        //template<typename a_> bool constexpr has_trivial_constructor      = __has_trivial_constructor(a_);
        template<typename a_> bool constexpr has_trivial_copy_assign      = has_copy_assign<a_> && __has_trivial_assign(a_);
        template<typename a_> bool constexpr has_trivial_copy_constructor = has_copy_constructor<a_> && __has_trivial_copy(a_);
        template<typename a_> bool constexpr has_trivial_destructor       = __has_trivial_destructor(a_);
        template<typename a_> bool constexpr has_nothrow_copy_assign      = has_copy_assign<a_> && __has_nothrow_assign(a_);
        template<typename a_> bool constexpr has_nothrow_copy_constructor = has_copy_constructor<a_> && __has_nothrow_copy(a_);
    }
    
    #endif

#else

    #include <water/is_no_to.hpp>

    namespace water {
        bool constexpr type_traits_exist = false;
        //template<typename a_> bool constexpr has_trivial_constructor    = !is_class_struct_union<a_>;
        template<typename a_> bool constexpr has_trivial_copy_assign      = !is_class_struct_union<a_>;
        template<typename a_> bool constexpr has_trivial_copy_constructor = !is_class_struct_union<a_>;
        template<typename a_> bool constexpr has_trivial_destructor       = !is_class_struct_union<a_>;
        template<typename a_> bool constexpr has_nothrow_copy_assign      = !is_class_struct_union<a_>;
        template<typename a_> bool constexpr has_nothrow_copy_constructor = !is_class_struct_union<a_>;
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
    
    static_assert(has_copy_constructor<int>, "");
    static_assert(has_copy_assign<int>, "");
    static_assert(has_trivial_copy_assign<int>, "");
    static_assert(has_trivial_copy_constructor<int>, "");
    static_assert(has_trivial_destructor<int>, "");
    static_assert(has_nothrow_copy_assign<int>, "");
    static_assert(has_nothrow_copy_constructor<int>, "");
    
    static_assert(has_copy_constructor<trivial>, "");
    static_assert(has_copy_assign<trivial>, "");
    static_assert(!type_traits_exist || has_trivial_copy_assign<trivial>, "");
    static_assert(!type_traits_exist || has_trivial_copy_constructor<trivial>, "");
    static_assert(!type_traits_exist || has_trivial_destructor<trivial>, "");
    static_assert(!type_traits_exist || has_nothrow_copy_assign<trivial>, "");
    static_assert(!type_traits_exist || has_nothrow_copy_constructor<trivial>, "");
    
    static_assert(has_copy_constructor<nontrivial>, "");
    static_assert(has_copy_assign<nontrivial>, "");
    static_assert(!type_traits_exist || !has_trivial_copy_assign<nontrivial>, "");
    static_assert(!type_traits_exist || !has_trivial_copy_constructor<nontrivial>, "");
    static_assert(!type_traits_exist || !has_trivial_destructor<nontrivial>, "");
    
    static_assert(!has_copy_constructor<deleted>, "");
    static_assert(!has_copy_assign<deleted>, "");
    static_assert(!has_trivial_copy_assign<deleted>, "");
    static_assert(!has_trivial_copy_constructor<deleted>, "");
    static_assert(!type_traits_exist || has_trivial_destructor<deleted>, "");
    static_assert(!has_nothrow_copy_assign<deleted>, "");
    static_assert(!has_nothrow_copy_constructor<deleted>, "");

}}}
#endif
