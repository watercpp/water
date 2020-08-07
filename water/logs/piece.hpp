// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_LOGS_PIECE_HPP
#define WATER_LOGS_PIECE_HPP
#include <water/logs/bits.hpp>
namespace water { namespace logs {

template<typename tag_>
class piece
{
public:
    using tag_type = tag_;

private:
    tag_type mytag;
    int mycapacity; // < 0 if first, save 4 bytes by not having a bool first??
    int mysize = 0;
    piece* mylist = 0;

public:
    piece(tag_type const& tag, size_t capacity) :
        mytag(tag),
        mycapacity(static_cast<int>(capacity))
    {}

    piece(piece const&) = delete;
    piece& operator=(piece const&) = delete;

    tag_type const& tag() const {
        return mytag;
    }

    piece* list() const {
        return mylist;
    }

    void list(piece* a) {
        mylist = a;
    }

    size_t capacity() const {
        return static_cast<size_t>(mycapacity < 0 ? -mycapacity : mycapacity);
    }

    size_t size() const {
        return static_cast<size_t>(mysize);
    }

    void size(size_t a) const {
        ___water_assert(a <= capacity());
        mysize = static_cast<int>(a);
    }

    char* begin() {
        return static_cast<char*>(static_cast<void*>(this + 1));
    }

    char const* begin() const {
        return static_cast<char const*>(static_cast<void const*>(this + 1));
    }

    char* end() {
        return begin() + size();
    }

    char const* end() const {
        return begin() + size();
    }

    void end(char const* a) {
        ___water_assert(begin() <= a && a <= begin() + capacity());
        mysize = static_cast<int>(a - begin());
    }

    void first(bool a) {
        if((a && mycapacity > 0) || (!a && mycapacity < 0))
            mycapacity = -mycapacity;
    }

    bool first() const {
        return mycapacity < 0;
    }
};

template<typename tag_>
piece<tag_>* reverse(piece<tag_>* a) noexcept {
    if(a) {
        auto *b = a->list();
        a->list(0);
        while(b) {
            auto s = a;
            a = b;
            b = a->list();
            a->list(s);
        }
    }
    return a;
}

}}
#endif
