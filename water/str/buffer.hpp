// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_STR_BUFFER_HPP
#define WATER_STR_BUFFER_HPP
#include <water/str/out.hpp>
namespace water { namespace str {

/*

Buffer text and write to function_. Similar to str::buffer_lines

function_ is something that can be called in one of these ways:

function(char_type const *begin, char_type const *end) noexcept;
function(char_type const *zero_terminated) noexcept;

The function is never called with an empty string. For the begin end variant *end == 0 always.

char_ template is usually detected from the function_ type can can be left as void.

The buffer is written to the function when the buffer is full, when flush() is called (for example
via str::el) or when this is destroyed (so the function should not throw exceptions).

Unlike str::buffer_lines this does *not* write complete lines every time. This means it will not cut
off long lines. Instead it can write partial lines, leading to different problems: If many str::out
objects write to the same destination (from one or many threads) the output of them can be mixed in
the middle of a line.

*/

namespace _ {

    template<typename f_> char         buffer_function_char_test(...); // default to char
    template<typename f_> char         buffer_function_char_test(f_* f, decltype((*f)(static_cast<char const*>(0), static_cast<char const*>(0)))*, int, int);
    template<typename f_> char16_t     buffer_function_char_test(f_* f, decltype((*f)(static_cast<char16_t const*>(0), static_cast<char16_t const*>(0)))*, int, ...);
    template<typename f_> char32_t     buffer_function_char_test(f_* f, decltype((*f)(static_cast<char32_t const*>(0), static_cast<char32_t const*>(0)))*, int, ...);
    template<typename f_> wchar_t      buffer_function_char_test(f_* f, decltype((*f)(static_cast<wchar_t const*>(0), static_cast<wchar_t const*>(0)))*, int, ...);
    template<typename f_> char8_or_not buffer_function_char_test(f_* f, decltype((*f)(static_cast<char8_or_not const*>(0), static_cast<char8_or_not const*>(0)))*, int, ...);
    template<typename f_> char         buffer_function_char_test(f_* f, decltype((*f)(static_cast<char const*>(0)))*, ...);
    template<typename f_> char16_t     buffer_function_char_test(f_* f, decltype((*f)(static_cast<char16_t const*>(0)))*, ...);
    template<typename f_> char32_t     buffer_function_char_test(f_* f, decltype((*f)(static_cast<char32_t const*>(0)))*, ...);
    template<typename f_> wchar_t      buffer_function_char_test(f_* f, decltype((*f)(static_cast<wchar_t const*>(0)))*, ...);
    template<typename f_> char8_or_not buffer_function_char_test(f_* f, decltype((*f)(static_cast<char8_or_not const*>(0)))*, ...);

    template<typename function_, typename char_>
    struct buffer_function_char :
        types::type_plain<char_>
    {};
    
    template<typename function_>
    struct buffer_function_char<function_, void> :
        types::type_plain<
            decltype(buffer_function_char_test<typename types::no_pointer<function_>::result>(0, 0, 0, 0))
        >
    {};

    template<typename char_, typename function_> char_ const* buffer_function_end_test(function_ *f, decltype((*f)(static_cast<char_ const*>(0), static_cast<char_ const*>(0)))*);
    template<typename char_, typename function_> void  const* buffer_function_end_test(function_ *f, ...);

    template<typename function_, typename char_>
    void buffer_function_call_end_or_not(function_& function, char_ const* begin, char_ const* end) {
        function(begin, end);
    }

    template<typename function_, typename char_>
    void buffer_function_call_end_or_not(function_& function, char_ const* begin, void const*) {
        function(begin);
    }

    template<typename function_, typename char_>
    void buffer_function_call(function_& function, char_ const* begin, char_ const* end) {
        buffer_function_call_end_or_not(function, begin, static_cast<decltype(buffer_function_end_test<char_>(&function, 0))>(end));
    }

    template<typename function_, typename char_>
    void buffer_function_call(function_*& function, char_ const* begin, char_ const* end) {
        buffer_function_call_end_or_not(*function, begin, static_cast<decltype(buffer_function_end_test<char_>(function, 0))>(end));
    }

}



template<typename function_, typename char_ = void, unsigned buffer_size_ = 0>
class buffer
{
    static_assert(!buffer_size_ || buffer_size_ > 8, "");

public:
    using function_type = typename types::ifel_type<types::is_function<function_>, function_*, function_>::result;
    using char_type = typename _::buffer_function_char<function_type, char_>::result;
    static unsigned constexpr buffer_size = buffer_size_ ? buffer_size_ : 512;

private:
    function_type myfunction {};
    char_type my[buffer_size];
    unsigned mysize = 0;

public:
    buffer() = default;

    buffer(buffer const& a) :
        myfunction{a.myfunction}
    {
        copy(a);
    }

    buffer(buffer&& a) :
        myfunction{static_cast<function_type&&>(a.myfunction)}
    {
        copy(a);
        a.mysize = 0;
    }

    template<typename ...arguments_, typename not_copy_constructor<buffer, arguments_...>::result = 0>
    buffer(arguments_&&... a) :
        myfunction{static_cast<arguments_&&>(a)...},
        mysize{0}
    {}

    ~buffer() {
        flush();
    }

    buffer& operator=(buffer const& a) {
        myfunction = a.myfunction;
        copy(a);
        return *this;
    }

    buffer& operator=(buffer&& a) {
        myfunction = static_cast<function_type&&>(a.myfunction);
        copy(a);
        a.mysize = 0;
        return *this;
    }

    function_type& function() {
        return myfunction;
    }

    function_type const& function() const {
        return myfunction;
    }

    void flush() {
        if(mysize) {
            unsigned size = static_cast<unsigned>(unicode::utf_adjust_end<unicode::utf_from_char<char_type>::result>(my + 0, my + mysize) - my);
            if(!size)
                return;
            // if myfunction throws, behave as if it did not. discard the buffer
            struct move {
                char_type
                    *buffer,
                    zeroed;
                unsigned
                    flush_size,
                    size;
                ~move() {
                    if(flush_size != size) {
                        auto
                            t = buffer,
                            f = buffer + flush_size,
                            fe = buffer + size;
                        *f = zeroed;
                        do *t++ = *f++; while(f != fe);
                    }
                }
            };
            move move_later { my, my[size], size, mysize };
            my[size] = 0;
            mysize = mysize - size;
            _::buffer_function_call(myfunction, my + 0, my + size); // could throw
        }
    }

    template<typename iterator_>
    void operator()(iterator_ begin, iterator_ end) {
        while(begin != end) {
            auto c = static_cast<char_type>(*begin);
            if(!c)
                break;
            if(mysize == buffer_size - 1)
                flush();
            my[mysize++] = c;
            ++begin;
        }
    }

private:
    void copy(buffer const& a) {
        mysize = 0;
        while(mysize != a.mysize) {
            my[mysize] = a.my[mysize];
            ++mysize;
        }
    }
    
};


}}
#endif
