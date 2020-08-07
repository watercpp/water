// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_NEW_HERE_HPP
#define WATER_NEW_HERE_HPP

/*

This exists only so water does not depend on the c++ standard library placement new from <new>

char buffer[sizeof(type)];
type* t = new(water::here(buffer)) type;

*/

namespace water {

    struct here {
        void* pointer;
        explicit here(void *pointer) noexcept : pointer(pointer) {}
    };

}

inline void* operator new(decltype(sizeof(0)), water::here const& a) noexcept {
    return a.pointer;
}

inline void operator delete(void*, water::here const&) noexcept
{}

#endif
