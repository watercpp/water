// Copyright 2024 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_STR_LIST_HPP
#define WATER_STR_LIST_HPP
#include <water/str/out.hpp>
namespace water { namespace str {

/*

str::out<...> o;

o << list(myvector).settings(s).separator(", ");


*/

namespace _ {

    template<typename char_>
    ifel<is_char<char_>, ifel<is_unsigned<char_>, unsigned long long, long long>> list_int_if_char(char_ a) {
        return a;
    }

    template<typename type_>
    ifel<!is_char<type_>, type_&&> list_int_if_char(type_&& a) {
        return static_cast<type_&&>(a);
    }

    template<typename a_>
    a_& list_separator(a_* a) {
        return *a;
    }
    
    using list_separator_type = char[2];
    constexpr list_separator_type default_list_separator {static_cast<char>(u','), static_cast<char>(u' ')};
    
    inline list_separator_type const& list_separator(void*) {
        return default_list_separator;
    }

}


template<typename list_, typename separator_ = void>
class list_write :
    public callback<list_write<list_, separator_>>
{
    list_ my;
    separator_ *myseparator = 0;
    str::settings mysettings;
    bool mysettings_set = false;

public:

    list_write(list_ const& l) :
        my{l}
    {}
    
    list_write(list_ const& list, separator_ *separator, str::settings settings, bool settings_set) :
        my{list},
        myseparator{separator},
        mysettings{settings},
        mysettings_set{settings_set}
    {}
    
    list_write& settings(str::settings a) {
        mysettings = a;
        mysettings_set = true;
        return *this;
    }
    
    template<typename separator2_>
    list_write<list_, no_reference<separator2_>> separator(separator2_&& a) {
        return {my, &a, mysettings, mysettings_set};
    }

    template<typename o_>
    void operator()(out<o_>& o) {
        auto restore = o.restore_settings();
        if(mysettings_set)
            o << mysettings;
        my(
            [&o, first = true, s = myseparator] (auto&& a) mutable {
                if(first)
                    first = false;
                else
                    o << _::list_separator(s);
                o << _::list_int_if_char(a);
            }
        );
    }
};


template<typename a_>
auto list(a_&& a) {
    auto list = [p = &a] (auto&& to) {
        for(auto&& v : *p)
            to(v);
    };
    return list_write<decltype(list)>{list};
}


template<typename iterator_>
auto list(iterator_ begin, first<iterator_> end) {
    auto list = [begin, end] (auto&& to) mutable {
        while(begin != end) {
            to(*begin);
            ++begin;
        }
    };
    return list_write<decltype(list)>{list};
}


template<typename iterator_>
auto list(iterator_ begin, size_t size) {
    auto list = [begin, size] (auto&& to) mutable {
        while(size) {
            to(*begin);
            ++begin;
            --size;
        }
    };
    return list_write<decltype(list)>{list};
}


}}
#endif
