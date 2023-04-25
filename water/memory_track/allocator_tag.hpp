// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_MEMORY_TRACK_ALLOCATOR_TAG_HPP
#define WATER_MEMORY_TRACK_ALLOCATOR_TAG_HPP
#include <water/memory_track/bits.hpp>
namespace water { namespace memory_track {

/*

Tag used to match alloacation and deallocation functions. 
For eample so delete[] is used after new[] and not plain delete.

Can be a string literal:

    allocator_tag{"hello"}

Or a pointer + function pointer, see allocator_base for example

*/


class allocator_tag
{
    void const *mypointer = 0;
    void (*myfunction)() = 0;

public:

    constexpr allocator_tag() = default;

    constexpr allocator_tag(void const *pointer, void (*function)()) noexcept :
        mypointer{pointer},
        myfunction{function}
    {}
    
    constexpr allocator_tag(char const* string_literal) noexcept :
        mypointer{string_literal},
        myfunction{&allocator_tag::string_literal}
    {}

    bool operator==(allocator_tag a) const noexcept {
        if(mypointer == a.mypointer && myfunction == a.myfunction)
            return true;
        if(myfunction != &allocator_tag::string_literal || a.myfunction != &allocator_tag::string_literal)
            return false;
        // compare string literals
        auto x = static_cast<char const*>(mypointer);
        auto y = static_cast<char const*>(a.mypointer);
        if(!x || !y)
            return false;
        while(*x == *y && *x) {
            ++x;
            ++y;
        }
        return *x == *y;
    }

    bool operator!=(allocator_tag a) const noexcept {
        return !operator==(a);
    }

private:

    static void string_literal() {}
};


}}
#endif
