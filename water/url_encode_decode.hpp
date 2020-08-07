// Copyright 2017-2018 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_URL_ENCODE_DECODE_HPP
#define WATER_URL_ENCODE_DECODE_HPP
#include <water/iterator.hpp> // only need this for forward_iterator_tag
namespace water {

/*

URL encode and decode

https://en.wikipedia.org/wiki/Percent-encoding
https://tools.ietf.org/html/rfc3986#section-2.1
https://www.w3.org/TR/html4/interact/forms.html#h-17.13.4.1
http://www.ietf.org/rfc/rfc1738.txt (x-www-form-urlencoded is based on this)

All bytes except the "unreserved" are encoded as %XX where XX is hexadecimal.
Hexadecimal should be uppercase, but can be lowercase.
It is not possible to encode anything beyond 8-bits. Encode unicode as UTF-8 bytes.

Unreserved by RFC 3986:
2d -
2e .
30-39 0-9
5f _
61-7a a-z
41-5a A-Z
7e ~

RFC 1738 says 7e ~ is "unsafe" and must be encoded. RFC 3986 says it's ok to encode anything, this will always encode 7e.

application/x-www-form-urlencoded says
- spaces are replaced with +
- line breaks are CR LF
- otherwise as rfc1738

This will always decode + to space, but url_encode_iterator encodes only if space_as_plus_ is true

*/

namespace _ {

    unsigned char constexpr url_encode_lookup[0x60] = {
        //0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, //0x00 0x0f
        //0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, //0x10 0x1f
        0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0, //0x20 0x2f
        1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0, //0x30 0x3f
        0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, //0x40 0x4f
        1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,1, //0x50 0x5f
        0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, //0x60 0x6f
        1,1,1,1,1,1,1,1,1,1,1,0,0,0,1,0  //0x70 0x7f
    };
    
    unsigned char constexpr url_encode_digits[0x10] = { '0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F' };
    
}

template<typename char_>
bool url_encode(char_ a) {
    // return true if a should be url encoded
    return a < 0x20 || 0x7a < a || _::url_encode_lookup[static_cast<unsigned>(a - 0x20)] == 0;
}

template<typename to_>
void url_encode(to_& t0, to_& t1, to_& t2, unsigned char f) {
    t0 = '%';
    t1 = _::url_encode_digits[(f >> 4) & 0xf];
    t2 = _::url_encode_digits[f & 0xf];
}

template<typename to_, typename from_>
void url_encode(to_& t0, to_& t1, to_& t2, from_ f) {
    url_encode(t0, t1, t2, static_cast<unsigned char>(f));
}


// URL encode iterator
//
// Tempalte space_as_plus_ means this will encode space as plus. otherwise as %20
//
// This converts to false when it reaches the end of the input range. After that it returns 0.
// A default consturcted compares equal to any at the end.
//
// Make sure the input is bytes (signed or unsigned char). This will cast away anything beyond 8 bits.
//
template<typename input_iterator_, bool space_as_plus_ = false>
class url_encode_iterator
{
public:
    using iterator_category = forward_iterator_tag;
    using value_type = unsigned char;
    using reference = value_type const&;
    using pointer = value_type const*;
    using difference_type = ptrdiff_t;

private:
    unsigned char my[4] = {}; // 4 so it can return 0 at the end
    unsigned myat = 3;
    input_iterator_
        myfrom {},
        myend {};

public:
    url_encode_iterator() = default;

    url_encode_iterator(input_iterator_ begin, input_iterator_ end) :
        myfrom{begin},
        myend{end}
    {
        next();
    }

    explicit operator bool() const {
        return myat != 3;
    }

    reference operator*() const {
        return my[myat];
    }

    pointer operator->() const {
        return my + myat;
    }

    url_encode_iterator& operator++() {
        next();
        return *this;
    }

    url_encode_iterator& operator++(int) {
        url_encode_iterator r{*this};
        ++*this;
        return r;
    }

    bool operator==(url_encode_iterator const& a) const {
        return myat == a.myat && (myat == 3 || myfrom == a.myfrom);
    }

    bool operator!=(url_encode_iterator const& a) const {
        return !this->operator==(a);
    }

private:
    void next() {
        if(++myat > 2 || !my[myat]) {
            if(myfrom == myend)
                myat = 3;
            else {
                myat = 0;
                my[0] = static_cast<unsigned char>(*myfrom);
                my[1] = 0;
                my[2] = 0;
                ++myfrom;
                // avoid warning C4127: conditional expression is constant
                encode_maybe(encode_space_as_plus<space_as_plus_>{});
            }
        }
    }

    template<bool>
    struct encode_space_as_plus {};
    void encode_maybe(encode_space_as_plus<true>) {
        if(my[0] == ' ')
            my[0] = '+';
        else
            encode_maybe(encode_space_as_plus<false>{});
    }

    void encode_maybe(encode_space_as_plus<false>) {
        if(url_encode(my[0]))
            url_encode(my[0], my[1], my[2], my[0]);
    }
    
};


template<typename input_iterator_>
url_encode_iterator<input_iterator_> url_encode_iterator_from(input_iterator_ begin, input_iterator_ end) {
    return {begin, end};
}

template<typename range_>
auto url_encode_iterator_from(range_ const& range) -> url_encode_iterator<decltype(range.begin())> {
    return {range.begin(), range.end()};
}


template<typename input_iterator_>
class url_encode_range
{
public:
    using iterator = url_encode_iterator<input_iterator_>;

private:
    input_iterator_
        mybegin {},
        myend {};

public:
    url_encode_range() = default;

    url_encode_range(input_iterator_ begin, input_iterator_ end) :
        mybegin{begin},
        myend{end}
    {}

    iterator begin() const {
        return {mybegin, myend};
    }

    iterator end() const {
        return {myend, myend};
    }
};


template<typename input_iterator_>
url_encode_range<input_iterator_> url_encode_range_from(input_iterator_ begin, input_iterator_ end) {
    return {begin, end};
}

template<typename range_>
auto url_encode_range_from(range_ const& range) -> url_encode_range<decltype(range.begin())> {
    return {range.begin(), range.end()};
}


namespace _ {

    template<typename forward_iterator_>
    bool url_decode_digit(unsigned char& to, forward_iterator_& from, forward_iterator_ const& from_end) {
        if(from == from_end)
            return false;
        unsigned char d;
        auto f = *from;
        if('0' <= f && f <= '9')
            d = static_cast<unsigned char>(f - '0');
        else if('A' <= f && f <= 'F')
            d = static_cast<unsigned char>(f - 'A') + 10;
        else if('a' <= f && f <= 'f')
            d = static_cast<unsigned char>(f - 'a') + 10;
        else
            return false;
        ++from;
        to = (to << 4) | d;
        return true;
    }
    
}

template<typename to_, typename forward_iterator_>
unsigned url_decode(to_& to, forward_iterator_& from, forward_iterator_ const& from_end) {
    // url decode 1 character.
    // returns the number of characters used in from, 1 or 3 or 0
    //
    // post
    // - to has been written to. 0 if return is 0
    // - from has moved the returned number of characters
    //
    // this will decode + to space
    to = 0;
    if(from == from_end)
        return 0;
    if(*from == '%') {
        unsigned char u = 0;
        auto f = from;
        ++f;
        if(_::url_decode_digit(u, f, from_end) && _::url_decode_digit(u, f, from_end)) {
            from = f;
            to = static_cast<to_>(u);
            return 3;
        }
        return 0;
    }
    if(*from == '+')
        to = ' ';
    else if(0x20 <= *from && *from <= 0x7e && _::url_encode_lookup[static_cast<unsigned>(*from - 0x20)])
        to = static_cast<to_>(*from);
    else
        return 0;
    ++from;
    return 1;
}


// URL decode iterator
//
// This will stop when it finds an error. The error() and at() functions can be used to see if and where the error was.
//
// This converts to false when it reaches the end of the input range or it finds an error. After that it returns 0.
// A default consturcted compares equal to any at the end/error.
//
template<typename forward_iterator_>
class url_decode_iterator
{
public:
    using iterator_category = forward_iterator_tag;
    using value_type = unsigned char;
    using reference = value_type const&;
    using pointer = value_type const*;
    using difference_type = ptrdiff_t;

private:
    unsigned char my = 0;
    bool myleft = false;
    forward_iterator_
        myfrom {},
        myend {};

public:
    url_decode_iterator() = default;

    url_decode_iterator(forward_iterator_ begin, forward_iterator_ end) :
        myleft{true},
        myfrom{begin},
        myend{end}
    {
        next();
    }

    explicit operator bool() const {
        return myleft;
    }

    bool error() const {
        return !myleft && myfrom != myend;
    }

    forward_iterator_ at() const {
        return myfrom;
    }

    reference operator*() const {
        return my;
    }

    pointer operator->() const {
        return &my;
    }

    url_decode_iterator& operator++() {
        next();
        return *this;
    }

    url_decode_iterator& operator++(int) {
        url_decode_iterator r{*this};
        ++*this;
        return r;
    }

    bool operator==(url_decode_iterator const& a) const {
        return myleft == a.myleft && (!myleft || (myfrom == a.myfrom));
    }

    bool operator!=(url_decode_iterator const& a) const {
        return !this->operator==(a);
    }

private:
    void next() {
        myleft = myleft && url_decode(my, myfrom, myend);
    }
};


template<typename forward_iterator_>
url_decode_iterator<forward_iterator_> url_decode_iterator_from(forward_iterator_ begin, forward_iterator_ end) {
    return {begin, end};
}

template<typename range_>
auto url_decode_iterator_from(range_ const& range) -> url_decode_iterator<decltype(range.begin())> {
    return {range.begin(), range.end()};
}


template<typename forward_iterator_>
class url_decode_range
{
public:
    using iterator = url_decode_iterator<forward_iterator_>;

private:
    forward_iterator_
        mybegin {},
        myend {};

public:
    url_decode_range() = default;

    url_decode_range(forward_iterator_ begin, forward_iterator_ end) :
        mybegin{begin},
        myend{end}
    {}

    iterator begin() const {
        return {mybegin, myend};
    }

    iterator end() const {
        return {myend, myend};
    }
};


template<typename forward_iterator_>
url_decode_range<forward_iterator_> url_decode_range_from(forward_iterator_ begin, forward_iterator_ end) {
    return {begin, end};
}

template<typename range_>
auto url_decode_range_from(range_ const& range) -> url_decode_range<decltype(range.begin())> {
    return {range.begin(), range.end()};
}

}
#endif
