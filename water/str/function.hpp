// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_STR_FUNCTION_HPP
#define WATER_STR_FUNCTION_HPP
#include <water/str/out.hpp>
namespace water { namespace str {

/*

For functions that take begin,end as char pointers. This will convert any input that is not using a buffer

function(char_type const* begin, char_type const* end);

The begin,end range is *not* 0 terminated

out_function([](char16_t const* b, char16_t const *e) { ... }) << "hello";

char_ template is usually detected from the function_ type

*/

namespace _ {
    
    template<typename f_> char         function_test(...); // default to char
    template<typename f_> char         function_test(f_ *f, decltype((*f)(static_cast<char const*>(0), static_cast<char const*>(0)))*, int);
    template<typename f_> char16_t     function_test(f_ *f, decltype((*f)(static_cast<char16_t const*>(0), static_cast<char16_t const*>(0)))*, ...);
    template<typename f_> char32_t     function_test(f_ *f, decltype((*f)(static_cast<char32_t const*>(0), static_cast<char32_t const*>(0)))*, ...);
    template<typename f_> wchar_t      function_test(f_ *f, decltype((*f)(static_cast<wchar_t const*>(0), static_cast<wchar_t const*>(0)))*, ...);
    template<typename f_> char8_or_not function_test(f_ *f, decltype((*f)(static_cast<char8_or_not const*>(0), static_cast<char8_or_not const*>(0)))*, ...);

    template<
        typename function_,
        typename char_ = decltype(function_test<no_pointer<no_const<no_reference<function_>>>>(0, 0, 0))
    >
    struct function_char {
        using result = char_;
    };

    template<typename function_, typename char_>
    void function_call(function_& function, char_ const* begin, char_ const* end) {
        function(begin, end);
    }

    template<typename function_, typename char_>
    void function_call(function_*& function, char_ const* begin, char_ const* end) {
        (*function)(begin, end);
    }

}

template<typename function_, typename char_ = typename _::function_char<function_>::result>
class function
{
    using function_plain_ = no_const_or_reference<function_>;

public:
    using char_type = char_;
    using function_type = ifel<is_function<function_plain_>, function_plain_*,function_plain_>;

private:
    function_type my {};

public:

    constexpr function() = default;

    template<typename ...arguments_, typename not_copy_constructor<function, arguments_...>::result = 0>
    constexpr function(arguments_&&... a) :
        my{static_cast<arguments_&&>(a)...}
    {}

    void swap(function& a) {
        swap_from_swap(my, a.my);
    }

    void operator()(char_type const* begin, char_type const* end) {
        if(begin != end)
            _::function_call(my, begin, end);
    }

    void operator()(char_type* begin, char_type* end) {
        operator()(static_cast<char_type const*>(begin), static_cast<char_type const*>(end));
    }

    template<typename iterator_>
    void operator()(iterator_ begin, iterator_ end) {
        unsigned constexpr size = 256;
        char_type
            buffer[size],
            *b = buffer;
        while(begin != end) {
            if(b == buffer + size) {
                auto e = unicode::utf_adjust_end<unicode::utf_from_char<char_type>>(buffer + 0, b);
                operator()(static_cast<char_type const*>(buffer), static_cast<char_type const*>(e));
                auto b2 = buffer;
                while(e != b) *b2++ = *e++;
                b = b2;
            }
            *b++ = static_cast<char_type>(*begin);
            ++begin;
        }
        if(b != buffer)
            operator()(static_cast<char_type const*>(buffer), static_cast<char_type const*>(b));
    }
};

template<typename function_, typename char_>
void swap(function<function_, char_>& a, function<function_, char_>& b) {
    a.swap(b);
}

template<typename function_>
out<function<function_>> to_function(function_&& f, settings s = {}) {
    return out<function<function_>>(s, static_cast<function_&&>(f));
}

template<typename char_, typename function_>
out<function<function_, char_>> to_function(function_&& f, settings s = {}) {
    return out<function<function_, char_>>(s, static_cast<function_&&>(f));
}

}}
#endif
