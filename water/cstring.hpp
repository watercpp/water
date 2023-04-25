// Copyright 2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_CSTRING_HPP
#define WATER_CSTRING_HPP
#include <water/begin_end.hpp>
#include <water/char8.hpp>
namespace water {

/*

Make a begin_end range from a C string, a zero terminated char pointer or array.

The purpose of this is to avoid C strings elsewhere.
And using this will explicitly say that something is a C string.

*/


template<typename char_>
class cstring_pointer
{
    char_ const *my;

public:
    constexpr cstring_pointer(char_ const *a) :
        my{a}
    {}

    char_ const* begin() const {
        return my;
    }

    char_ const* end() const {
        auto a = my;
        if(a) while(static_cast<bool>(*a)) ++a; // static_cast because it could be a not_char8
        return a;
    }
};


inline begin_end<char const*> cstring(cstring_pointer<char> a) {
    return {a.begin(), a.end()};
}

inline begin_end<unsigned char const*> cstring(cstring_pointer<unsigned char> a) {
    return {a.begin(), a.end()};
}

inline begin_end<char8_or_not const*> cstring(cstring_pointer<char8_or_not> a) {
    return {a.begin(), a.end()};
}

inline begin_end<char16_t const*> cstring(cstring_pointer<char16_t> a) {
    return {a.begin(), a.end()};
}

inline begin_end<char32_t const*> cstring(cstring_pointer<char32_t> a) {
    return {a.begin(), a.end()};
}

inline begin_end<wchar_t const*> cstring(cstring_pointer<wchar_t> a) {
    return {a.begin(), a.end()};
}


template<typename char_, size_t size_>
begin_end<char_ const*> cstring(char_ const(&a)[size_]) {
    // always use size_ - 1, the function is called cstring so the array should end with 0
    return {a, size_ - 1};
}

}
#endif
