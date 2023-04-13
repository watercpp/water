// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_XTR_TESTS_CHAR_POINTER_HPP
#define WATER_XTR_TESTS_CHAR_POINTER_HPP
#include <water/xtr/tests/bits.hpp>
namespace water { namespace xtr { namespace tests {

namespace char_pointers {

    struct left {};

    struct char_const_pointer {
        char const *pointer;
        char_const_pointer(char const* a) : pointer{a} {}
    };

    template<typename type_>
    struct void_const_pointer_if { using result = void_const_pointer_if; };
    
    template<>
    struct void_const_pointer_if<char> {};
    
    struct void_const_pointer {
        void const *pointer;
        template<typename type_>
        void_const_pointer(type_ const* a, typename void_const_pointer_if<type_>::result = {}) : pointer{a} {}
    };

    inline char const* operator<<(left const&, char_const_pointer) {
        return 0;
    }

    inline void const* operator<<(left const&, void_const_pointer) {
        return 0;
    }

    template<unsigned size_>
    inline unsigned operator<<(left const&, char const (&)[size_]) {
        return 0;
    }
    
    // these below do not work
    // char const* is selected over the array version
    /*
    template<unsigned size_>
    inline unsigned operator<<(left const&, char (&)[size_]) {
        return 0;
    }
    
    inline char const* operator<<(left const&, char const*const&) {
        return 0;
    }
    
    inline void const* operator<<(left const&, void const*const&) {
        return 0;
    }
    */

}

struct char_pointer {
    char_pointer() {
        char not_const[] = "hello";
        using char_pointers::left;
        static_assert(water::equal<decltype(left() << static_cast<char const*>(0)), char const*>, "");
        static_assert(water::equal<decltype(left() << static_cast<char*>(0)), char const*>, "");
        static_assert(water::equal<decltype(left() << "hello"), unsigned>, "");
        static_assert(water::equal<decltype(left() << not_const), unsigned>, "");
        static_assert(water::equal<decltype(left() << this), void const*>, "");
        static_assert(water::equal<decltype(left() << static_cast<void const*>(0)), void const*>, "");
        not_const[0] = 0; // avoid unused warning
    }
};

}}}
#endif
