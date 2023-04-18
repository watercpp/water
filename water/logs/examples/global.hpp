// Copyright 2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_LOGS_EXAMPLES_GLOBAL_HPP
#define WATER_LOGS_EXAMPLES_GLOBAL_HPP
#include <water/logs/buffer_forever.hpp>
#include <water/logs/tag_with_thread.hpp>
#include <water/logs/str_to_buffer.hpp>
namespace water { namespace logs { namespace examples {

/*

This example (this file + global.cpp) has a global water::logs::buffer_forever log that is written
to a file and to water::trace. Each line is prefixed with the thread ID.

Also water::str and water::xtr that write to the log.

To use it:

    global("hello world");

    global_str s;
    s << "hello world " << 123 << "!\n";
    s << "more text!";

    global_xtr << "hello world " << 123 << '!';

*/

struct global_output
{
    void start();
    char* prefix(char* begin, char* end, water::logs::tag_with_thread const& tag);
    void line(char const* begin, char const* end);
    void stop();
};

using global_buffer = water::logs::buffer_forever<global_output, water::logs::tag_with_thread>;

extern global_buffer global; // this is the global log



// water::str

struct str_to_global : water::logs::str_to_buffer<global_buffer>
{
    str_to_global() :
        water::logs::str_to_buffer<global_buffer>(&global)
    {}
};

using global_str = water::str::out<str_to_global>;



// water::xtr

struct xtr_to_global
{
    void operator()(char const* begin, char const* end) const noexcept {
        global(begin, end);
    }    
};

struct global_xtr_class {};

constexpr global_xtr_class global_xtr;

template<typename a_>
auto operator<<(global_xtr_class, a_ && a) -> decltype(water::xtr::expression<xtr_to_global>{} << a) {
    return water::xtr::expression<xtr_to_global>{} << a;
}



/*

Example use, will write:

   One
   Two 2
   Four 4
   Three 3

Three comes after Four because it is written when s is destroyed

*/

inline void use_global() {
    global("One");
    global_str s;
    s << "Two " << 2 << str::el; // el writes the string to the log now
    s << "Three " << 3; // witout el, it writes the string when s is destroyed
    global_xtr << "Four " << 4;
}


}}}
#endif
