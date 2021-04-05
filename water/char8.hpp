// Copyright 2021 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_CHAR8_HPP
#define WATER_CHAR8_HPP
namespace water {

/*

char8_t

Use char8_or_not to have function overloads for char8_t. When compiling without char8_t
char8_or_not is an enum of unsigned char. That means it is always possible to cast char8_or_not
to char or unsigned char, and because char and unsigned char can alias anything it is also possible
to cast pointers.

    void x(char a) {
        //...
    }
    
    void x(char8_or_not a) {
        x(static_cast<char>(a));
    }
    
    
    void y(char const* a) {
        //...
    }
    
    void y(char8_or_not const* a) {
        y(static_cast<char const*>(static_cast<void const*>(a)));
    }

Use char8_or_char for u8 string arrays. It is char8_t when compiling with char8_t otherwise char.

    char8_or_char constexpr a[] = u8"hello";

*/

enum class not_char8 : unsigned char;

namespace _ {

    template<typename char8_ = decltype(*u8"")>
    struct char8_select;

    template<typename char8_>
    struct char8_select<char8_ const&> {
        static bool constexpr exists = true;
        using char8_or_char = char8_;
        using char8_or_not = char8_;
    };
    
    template<>
    struct char8_select<char const&> {
        static bool constexpr exists = false;
        using char8_or_char = char;
        using char8_or_not = not_char8;
    };

}

using char8_or_char = _::char8_select<>::char8_or_char;

using char8_or_not = _::char8_select<>::char8_or_not;

bool constexpr char8_exists = _::char8_select<>::exists;

}
#endif
