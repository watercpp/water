// Copyright 2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_STR_INSERT_AT_END_HPP
#define WATER_STR_INSERT_AT_END_HPP
#include <water/str/out.hpp>
namespace water { namespace str {

/*

Use this to write to the end of a std::basic_string, std::vector or similar container type

std::u16string s;
str::insert_at_end(s) << "hello world " << 123 << '!';

Calls container.insert(container.end(), begin, end) where begin and end are iterators of any type.
The container_ type needs only the insert and end functions, nothing else.

*/

template<typename container_>
class insert_at_end_of_container
{
    using char_type = no_const_or_reference<decltype(*make_type<container_*>()->end())>;

private:
    container_ *my;

public:

    insert_at_end_of_container(container_& a) :
        my{&a}
    {}
    
    insert_at_end_of_container(container_* a) :
        my{a}
    {}

    template<typename iterator_>
    void operator()(iterator_ begin, iterator_ end) {
        my->insert(my->end(), begin, end);
    }
};

template<typename container_>
out<insert_at_end_of_container<container_>> insert_at_end(container_& c, settings s = {}) {
    return out<insert_at_end_of_container<container_>>{s, c};
}

template<typename container_>
auto insert_at_end(container_&& c, settings s = {}) -> decltype(insert_at_end(c, s)) {
    return insert_at_end(c, s);
}


}}
#endif
