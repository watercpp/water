// Copyright 2017-2021 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_JSON_WRITE_HPP
#define WATER_JSON_WRITE_HPP
#include <water/json/node.hpp>
#include <water/json/write_number.hpp>
#include <water/json/write_string.hpp>
namespace water { namespace json {

template<typename to_>
void write_unbuffered(to_&& to, node<> nodes, bool escape_all = false) {
    // to(char)
    // to(char const*, char const*)
    //
    // esacpe_all
    // - true will write ascii, will be larger
    // - false will write utf-8, will be smaller, only things that must be esacped is
    using to_reference = typename types::to_reference<to_>::result;
    auto n = nodes;
    while(n) {
        bool next = true;
        if(n != nodes && n.in().type() == type::object) {
            to('"');
            auto s = n.name();
            if(s)
                write_string<to_reference>(
                    to,
                    static_cast<uchar_t const*>(static_cast<void const*>(s.begin())),
                    static_cast<uchar_t const*>(static_cast<void const*>(s.end())),
                    escape_all
                );
            to('"');
            to(':');
        }
        switch(n.type()) {
            case type::array:
            case type::object: {
                to(n.type() == type::array ? '[' : '{');
                if(auto nn = n.nodes()) {
                    n = nn;
                    next = false;
                }
                else
                    to(n.type() == type::array ? ']' : '}');
                break;
            }
            case type::string: {
                to('"');
                auto s = n.string();
                if(s)
                    write_string<to_reference>(
                        to,
                        static_cast<uchar_t const*>(static_cast<void const*>(s.begin())),
                        static_cast<uchar_t const*>(static_cast<void const*>(s.end())),
                        escape_all
                    );
                to('"');
                break;
            }
            case type::number: {
                write_number<to_reference>(to, n.number());
                break;
            }
            case type::boolean: {
                bool b = n.boolean();
                char const *c = b ? "true" : "false";
                to(c, c + (b ? 4 : 5));
                break;
            }
            default: {
                char const *c = "null";
                to(c, c + 4);
            }
        }
        if(next) {
            while(n != nodes && !n.next() && n.in()) {
                n = n.in();
                to(n.type() == type::array ? ']' : '}');
            }
            if(n != nodes && n.next()) {
                to(',');
                n = n.next();
            }
            else
                break;
        }
    }
}

unsigned constexpr write_buffer_size = 1024;

template<typename to_>
size_t write(to_&& to, node<> nodes, bool escape_all = false) {
    // to(char const* begin, char const* end)
    //
    // returns the number of characters written
    //
    // this writes to a temporary buffer of write_buffer_size. use write_unbuffered if that is not good.
    //
    struct buffer_
    {
        typename types::no_reference<to_>::result *to;
        unsigned at;
        size_t size;
        char buffer[write_buffer_size];
        
        void operator()(char a) {
            if(at == write_buffer_size)
                flush();
            buffer[at++] = a;
        }

        void operator()(char const *b, char const *e) {
            size_t s = static_cast<size_t>(e - b);
            if(at + s > write_buffer_size)
                flush();
            if(s > write_buffer_size) {
                (*to)(b, e);
                size += s;
            }
            else
                while(b != e)
                    buffer[at++] = *b++;
        }

        void flush() {
            if(!at)
                return;
            auto
                b = static_cast<char const*>(buffer),
                e1 = b + at,
                e2 = unicode::utf_adjust_end<8>(b, e1);
            if(b == e2)
                return;
            (*to)(b, e2);
            size += static_cast<size_t>(e2 - b);
            at = 0;
            while(e1 != e2)
                buffer[at++] = *e2++;
        }
    } b;
    b.to = &to;
    b.at = 0;
    b.size = 0;
    write_unbuffered(b, nodes, escape_all);
    b.flush();
    ___water_assert(b.at == 0);
    return b.size;
}

template<typename memory_>
size_t write_size(node<memory_> nodes, bool escape_all = false) {
    // return how many bytes write will write
    //
    // (memory_ is dummy to not compile this when not used)
    struct to_ {
        size_t size = 0;
        void operator()(char) { ++size; }
        void operator()(char const* b, char const* e) { size += static_cast<size_t>(e - b); }
    } to;
    write_unbuffered(to, nodes, escape_all);
    return to.size;
}

}}
#endif
