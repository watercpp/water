// Copyright 2024 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_STR_CREATE_HPP
#define WATER_STR_CREATE_HPP
#include <water/str/out.hpp>
namespace water { namespace str {

/*

str::create creates a temporary str::out for every expression. Avoids typing () or {}

    constexpr str::create<str::out_stdout> sout;

    sout << "hello";

same effect as:
    
    str::out_stdout{} << "hello";

*/


template<typename out_>
class create_on_copy
{
    out_ *my = 0;

protected:

    constexpr create_on_copy() = default;

public:
    
    create_on_copy(create_on_copy const&, out_&& temporary = {}) : // this temporary exists during the << expression
        my{&temporary}
    {}
    
    constexpr create_on_copy& operator=(create_on_copy const&) = default;
    
    out_* out() const {
        return my;
    }
};


// workaround for visual c++, see xtr::expression_return
template<typename out_, typename type_>
using create_return_type = decltype(make_type<out_&>() << make_type<type_&&>());


template<typename out_, typename type_>
auto operator<<(create_on_copy<out_> c, type_&& a) -> create_return_type<out_, type_&&> {
    return *c.out() << a;
}


template<typename out_>
struct create :
    create_on_copy<out_>
{   
    constexpr create() = default;
    constexpr create(create const&) : create_on_copy<out_>{} {} // does not create a temporary
    constexpr create& operator=(create const&) = default;
};


}}
#endif
