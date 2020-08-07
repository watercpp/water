// Copyright 2017-2018 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_STATISTICS_NAME_HPP
#define WATER_THREADS_STATISTICS_NAME_HPP
#include <water/threads/statistics/bits.hpp>
namespace water { namespace threads { namespace statistics {

// cstring, always 0 terminated
class name
{
    char my[64] = {};
    unsigned mysize = 0;

public:
    constexpr name() noexcept = default;

    template<typename iterator_>
    name(iterator_ begin, iterator_ end) noexcept {
        assign(begin, end);
    }

    name(char const* cstring) noexcept {
        assign(cstring);
    }

    template<typename iterator_>
    void assign(iterator_ begin, iterator_ end) noexcept {
        mysize = 0;
        while(begin != end && mysize != sizeof(my) - 1) {
            my[mysize++] = static_cast<char>(*begin);
            ++begin;
        }
        my[mysize] = 0;
    }

    template<typename iterator_>
    void assign(iterator_ begin, size_t size) noexcept {
        if(size > sizeof(my) - 1)
            size = sizeof(my) - 1;
        mysize = 0;
        while(mysize != size) {
            my[mysize++] = static_cast<char>(*begin);
            ++begin;
        }
        my[mysize] = 0;
    }

    void assign(char const* cstring) noexcept {
        mysize = 0;
        if(cstring)
            while(*cstring && mysize != sizeof(my) - 1)
                my[mysize++] = static_cast<char>(*cstring++);
        my[mysize] = 0;
    }

    char const* begin() const noexcept {
        return my;
    }

    char const* end() const noexcept {
        return my + mysize;
    }

    unsigned size() const noexcept {
        return mysize;
    }

    explicit operator bool() const noexcept {
        return mysize != 0;
    }
};

inline bool operator==(name const& a, name const& b) noexcept {
    if(a.size() != b.size())
        return false;
    auto
        ai = a.begin(),
        ae = a.end(),
        bi = b.begin();
    while(ai != ae && *ai == *bi) {
        ++ai;
        ++bi;
    }
    return ai == ae;
}

inline bool operator!=(name const& a, name const& b) noexcept {
    return !(a == b);
}

class name_assign
{
    name *my = 0;

public:
    name_assign() noexcept = default;
    
    name_assign(name& a) noexcept :
        my(&a)
    {}
    
    template<typename iterator_>
    void operator()(iterator_ begin, iterator_ end) const noexcept {
        if(my)
            my->assign(begin, end);
    }
    
    template<typename iterator_>
    void operator()(iterator_ begin, size_t size) const noexcept {
        if(my)
            my->assign(begin, size);
    }
    
    void operator()(char const* cstring) const noexcept {
        if(my)
            my->assign(cstring);
    }
};

}}}
#endif
