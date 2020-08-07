// Copyright 2017-2018 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_STR_CONTAINER_HPP
#define WATER_STR_CONTAINER_HPP
#include <water/str/out.hpp>
namespace water { namespace str {

/*

Inherit a container like a vector or std::string. Inserts to the end.

*/

template<typename container_>
struct container : public container_
{
    using char_type = typename container_::value_type;
    
    container() : // constructors not default because visual c++ 2015
        container_{}
    {}

    container(container const& a) :
        container_{static_cast<container_ const&>(a)}
    {}

    container(container&& a) :
        container_{static_cast<container_&&>(a)}
    {}

    template<typename ...arguments_, typename not_copy_constructor<container, arguments_...>::result = 0>
    container(arguments_&&... a) :
        container_{static_cast<arguments_&&>(a)...}
    {}

    container& operator=(container const& a) {
        static_cast<container_&>(*this) = static_cast<container_ const&>(a);
        return *this;
    }

    container& operator=(container&& a) {
        static_cast<container_&>(*this) = static_cast<container_&&>(a);
        return *this;
    }

    void swap(container& a) {
        swap_from_swap(static_cast<container_&>(*this), static_cast<container_&>(a));
    }

    template<typename iterator_>
    void operator()(iterator_ begin, iterator_ end) {
        this->insert(this->end(), begin, end);
    }
};

template<typename container_>
void swap(container<container_>& a, container<container_>& b) {
    a.swap(b);
}

}}
#endif
