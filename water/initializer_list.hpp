// Copyright 2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_INITIALIZER_LIST_HPP
#define WATER_INITIALIZER_LIST_HPP
#include <water/water.hpp>
#ifndef WATER_NO_STD
    #include <initializer_list>
#endif
namespace water {

/*

Normally, when WATER_NO_STD is not defined, std::initializer_list is used.

When WATER_NO_STD is defined no_initializer_list is used instead. It is a poor imitation, but it
makes some things work. Construction does not work like the real thing:

    class c {
        c(no_initializer_list<int> const&) {}
    };
    
    c{1, 2, 3}; // does not work
    c{{1, 2, 3}}; // works

*/

template<typename type_>
class no_initializer_list
{
    static size_t constexpr capacity = sizeof(type_) < 4 ? 128 / sizeof(type_) : 32; 
    
    type_ my[capacity];
    size_t mysize = 0;
    
public:
    
    no_initializer_list() = default;
    
    template<typename ...list_>
    no_initializer_list(list_&&... l) :
        my{static_cast<list_&&>(l)...},
        mysize{sizeof...(list_)}
    {}
    
    type_ const* begin() const noexcept {
        return my;
    }
    
    type_ const* end() const noexcept {
        return my + mysize;
    }
    
    size_t size() const noexcept {
        return mysize;
    }
};

#ifdef WATER_NO_STD

template<typename type_>
using initializer_list = no_initializer_list<type_>;

#else

using ::std::initializer_list;

#endif

}
#endif
