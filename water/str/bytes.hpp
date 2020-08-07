// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_STR_BYTES_HPP
#define WATER_STR_BYTES_HPP
#include <water/str/out.hpp>
namespace water { namespace str {

/*

Write bytes as hex. 4 bytes then space
01234567 89abcdef 01234567 89abcdef

*/

class bytes : public callback<bytes>
{
    void const *my;
    size_t mybytes;

public:
    bytes(bytes const&) = default;
    bytes(bytes&&) = default;

    template<typename type_>
    explicit bytes(type_ const& a) :
        my{&a},
        mybytes{sizeof(a)}
    {}

    bytes(void const *pointer, size_t bytes) :
        my{pointer},
        mybytes{bytes}
    {}

    bytes(void const *begin, void const *end) :
        my{begin},
        mybytes{static_cast<size_t>(static_cast<char const*>(end) - static_cast<char const*>(begin))}
    {}

    template<typename o_>
    void operator()(out<o_>& o) {
        auto r = o.restore_settings();
        o << settings{}.base(16).base_prefix_suffix(false).digits(numbers::max_digits<unsigned char>(16));
        auto
            i = static_cast<unsigned char const*>(my),
            e = i + mybytes;
        unsigned n = 0;
        while(i != e) {
            if(n == 4) {
                o << static_cast<char>(u' ');
                n = 0;
            }
            o << *i;
            ++i;
            ++n;
        }
    }
};

}}
#endif
