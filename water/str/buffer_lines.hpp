// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_STR_BUFFER_LINES_HPP
#define WATER_STR_BUFFER_LINES_HPP
#include <water/str/buffer.hpp>
#include <water/later.hpp>
namespace water { namespace str {

/*

Buffer text and write one or more lines at a time to function_. Similar to str::buffer.

function_ is something that can be called in one of these ways:

function(char_type const *begin, char_type const *end) noexcept;
function(char_type const *zero_terminated) noexcept;

The function is never called with an empty string. For the begin end variant *end == 0 always.

char_ template is usually detected from the function_ type can can be left as void.

Important: buffer_size_ - 2 is the maximum line length. Longer lines will be truncated.

The buffer is written to the function when the buffer is full, when flush() is called (for example
via str::el) or when this is destroyed (so the function should not throw exceptions).

A linebreak is added when the buffer is written to the function, if the buffer does not already end
with a linebreak:

    using my_out = str::out<str::buffer_lines<my_function>>;
    
    my_out{} << "hello"; // linebreak added here
    my_out{} << "world\n"; // no linebreak added
    my_out{} << 123; // linebreak added

    output:

    hello
    world
    213 

*/

template<typename function_, typename char_ = void, unsigned buffer_size_ = 0>
class buffer_lines
{
    static_assert(!buffer_size_ || buffer_size_ > 8, "");

public:
    using function_type = typename types::ifel_type<types::is_function<function_>, function_*, function_>::result;
    using char_type = typename _::buffer_function_char<function_type, char_>::result;
    static unsigned constexpr buffer_size = buffer_size_ ? buffer_size_ : 512;

private:
    function_type myfunction {};
    char_type my[buffer_size];
    unsigned
        mysize = 0,
        myline = 0; // if not 0 my[myline - 1] is '\n'

public:

    buffer_lines() = default;

    buffer_lines(buffer_lines const& a) :
        myfunction{a.myfunction}
    {
        copy(a);
    }

    buffer_lines(buffer_lines&& a) :
        myfunction{static_cast<function_type&&>(a.myfunction)},
        myline{a.myline}
    {
        copy(a);
        a.mysize = a.myline = 0;
    }

    template<typename ...arguments_, typename not_copy_constructor<buffer_lines, arguments_...>::result = 0>
    buffer_lines(arguments_&&... a) :
        myfunction{static_cast<arguments_&&>(a)...},
        mysize{0},
        myline{0}
    {}

    ~buffer_lines() {
        flush();
    }

    buffer_lines& operator=(buffer_lines const& a) {
        myfunction = a.myfunction;
        copy(a);
        return *this;
    }

    buffer_lines& operator=(buffer_lines&& a) {
        myfunction = static_cast<function_type&&>(a.myfunction);
        copy(a);
        a.mysize = a.myline = 0;
        return *this;
    }

    function_type& function() {
        return myfunction;
    }

    function_type const& function() const {
        return myfunction;
    }

    void flush() {
        if(mysize)
            flush(false);
    }

    template<typename iterator_>
    void operator()(iterator_ begin, iterator_ end) {
        while(begin != end) {
            char_type c = static_cast<char_type>(*begin);
            if(!c)
                return;
            ++begin;
            if(mysize != buffer_size - 2) {
                my[mysize++] = c;
                if(c == static_cast<char_type>(u'\n'))
                    myline = mysize;
            }
            else if(c == static_cast<char_type>(u'\n'))
                flush(true);
            else if(myline) {
                // flush old lines to make room. if the function throws discard what was written like flush() does
                auto l = my[myline];
                my[myline] = 0;
                auto after = water::later(
                    [this, l, c] {
                        my[myline] = l;
                        auto s = mysize;
                        mysize = 0;
                        while(myline != s)
                            my[mysize++] = my[myline++];
                        myline = 0;
                        my[mysize++] = c;
                    }
                );
                _::buffer_function_call(myfunction, my + 0, my + myline); // could throw
            }
        }
    }

private:

    void flush(bool add_line_break) {
        unsigned s = mysize; // if function throws, set mysize to 0 first
        myline = mysize = 0;
        if(add_line_break || my[s - 1] != static_cast<char_type>(u'\n')) {
            auto end = unicode::utf_adjust_end<unicode::utf_from_char<char_type>::result>(my + 0, my + s);
            s = static_cast<unsigned>(end - my);
            my[s++] = static_cast<char_type>(u'\n');
        }
        my[s] = 0;
        _::buffer_function_call(myfunction, my + 0, my + s); // could throw
    }

    void copy(buffer_lines const& a) {
        mysize = 0;
        myline = a.myline;
        while(mysize != a.mysize) {
            my[mysize] = a.my[mysize];
            ++mysize;
        }
    }
    
};

}}
#endif
