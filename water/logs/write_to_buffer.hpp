// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_LOGS_WRITE_TO_BUFFER_HPP
#define WATER_LOGS_WRITE_TO_BUFFER_HPP
#include <water/logs/bits.hpp>
#include <water/swap.hpp>
namespace water { namespace logs {

/*

write one log entry from a single thread to buffer.

ok = write_to_buffer(buffer, tag)(begin, end);

*/

template<typename buffer_>
class write_to_buffer
{
public:
    using piece_type = typename buffer_::piece_type;
    using tag_type = typename buffer_::tag_type;

private:
    buffer_ *mybuffer = 0;
    tag_type mytag {};
    piece_type *mypiece = 0;
    char
        *myat = 0,
        *myend = 0;
    bool
        myerror = false,
        myhalfway = false; // true if writing range failed/threw

public:
    write_to_buffer() = default;
    write_to_buffer(write_to_buffer const&) = delete;
    write_to_buffer& operator=(write_to_buffer const&) = delete;

    write_to_buffer(buffer_ *a, tag_type const& tag = {}) :
        mybuffer(a),
        mytag(tag)
    {}

    write_to_buffer(buffer_& a, tag_type const& tag = {}) :
        mybuffer(&a),
        mytag(tag)
    {}

    ~write_to_buffer() {
        if(mypiece) mybuffer->dont_write(mypiece);
    }

    write_to_buffer(write_to_buffer&& a) :
        mybuffer(a.mybuffer),
        mytag(a.mytag),
        mypiece(a.mypiece),
        myat(a.myat),
        myend(a.myend),
        myerror(a.myerror),
        myhalfway(a.myhalfway)
    {
        a.mybuffer = 0;
        a.mypiece = 0;
        a.myat = 0;
        a.myend = 0;
    }

    write_to_buffer& operator=(write_to_buffer&& a) {
        swap(a);
        return *this;
    }

    explicit operator bool() const {
        return mybuffer && !myerror && !myhalfway;
    }

    void swap(write_to_buffer& a) {
        swap_from_swap(mybuffer, a.mybuffer);
        swap_from_swap(mytag, a.mytag);
        swap_from_swap(mypiece, a.mypiece);
        swap_from_swap(myat, a.myat);
        swap_from_swap(myend, a.myend);
        swap_from_swap(myerror, a.myerror);
        swap_from_swap(myhalfway, a.myhalfway);
    }

    write_to_buffer& tag(tag_type const& tag) {
        mytag = tag;
        return *this;
    }

    void dont() {
        if(mypiece) mybuffer->dont_write(mypiece);
        reset();
    }

    bool write() {
        bool r = false;
        if(mypiece) {
            if(!myerror && !myhalfway) {
                mypiece->end(myat);
                mybuffer->write(mypiece);
                r = true;
            }
            else
                mybuffer->dont_write(mypiece);
        }
        reset();
        return r;
    }

    bool operator()(char a) {
        // adds 1 character. keep doing this, then call write()
        return !myhalfway && one(a);
    }

    template<typename iterator_>
    bool operator()(iterator_ begin, iterator_ end) {
        if(!*this)
            return false;
        if(begin == end)
            return true;
        myhalfway = true;
        do one(static_cast<char>(*begin)); while(++begin != end);
        myhalfway = false;
        return write();
    }

    template<typename iterator_>
    bool operator()(iterator_ begin, size_t size) {
        if(!*this)
            return false;
        if(!size)
            return true;
        myhalfway = true;
        do {
            one(static_cast<char>(*begin));
            ++begin;
        } while(--size);
        myhalfway = false;
        return write();
    }

    bool operator()(char const* cstring) {
        if(!*this)
            return false;
        if(!cstring || !*cstring)
            return true;
        do one(*cstring); while(*++cstring);
        return write();
    }

    template<
        typename range_,
        typename = decltype(make_type<range_ const&>().begin() == make_type<range_ const&>().end())    
    >
    bool operator()(range_ const& a) {
        return (*this)(a.begin(), a.end());
    }

private:
    bool one(char a) {
        // write even if myhalfway
        if(myat != myend) {
            *myat++ = a;
            return true;
        }
        else if(mybuffer && !myerror) {
            if(auto p = mybuffer->piece(mytag, mypiece)) {
                if(mypiece) mypiece->end(myat);
                mypiece = p;
                myat = mypiece->begin();
                myend = myat + mypiece->capacity();
                *myat++ = a;
                return true;
            }
            else
                myerror = true;
        }
        return false;
    }

    void reset() {
        mypiece = 0;
        myend = myat = 0;
        myerror = myhalfway = false;
    }
    
};

template<typename buffer_>
void swap(write_to_buffer<buffer_>& a, write_to_buffer<buffer_>& b) {
    a.swap(b);
}

}}
#endif
