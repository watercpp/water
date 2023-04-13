// Copyright 2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_XTR_OPERATOR_USING_STR_HPP
#define WATER_XTR_OPERATOR_USING_STR_HPP
#include <water/str/out.hpp>
#include <water/xtr/base.hpp>
namespace water {

/*

If there is a str::out << operator for some type, this is a way of using that for a xtr::expression << operator.

Example:

    template<typename>
    struct something {};

    template<typename o_, typename s_>
    water::str::out<o_>& operator<<(water::str::out<o_>& o, something<s_> s) {
        return o << "something";
    }

    template<typename a_, typename b_, typename s_>
    water::xtr::expression<
        water::xtr::expression<a_, b_>,
        water::xtr_operator_using_str<something<s_>>
    >
    operator<<(water::xtr::expression<a_, b_>&& e, something<s_> const& s) {
        return {e, s};
    }

*/


template<typename write_>
class xtr_write_from_str
{
    write_* my;

public:
    explicit xtr_write_from_str(write_& a) :
        my{&a}
    {}
    
    template<typename iterator_>
    void operator()(iterator_ b, iterator_ e) const {
        (*my)(b, e);
    }
};


template<typename type_, unsigned size_ = 0>
struct xtr_operator_using_str
{
    static constexpr unsigned size = size_ ? size_ : xtr::settings_default::string_size;

private:

    type_ const* my;

public:

    xtr_operator_using_str(type_ const& a) :
        my{&a}
    {}
    
    template<typename write_>
    void operator()(write_&& write) const {
        str::out<xtr_write_from_str<no_reference<write_>>>{write} << *my;
    }
};


}
#endif
