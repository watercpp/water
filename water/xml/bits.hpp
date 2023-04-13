// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_XML_BITS_HPP
#define WATER_XML_BITS_HPP
#include <water/water.hpp>
#include <water/types.hpp>
#include <water/is_no_to.hpp>
#include <water/unicode/utf.hpp>
#include <water/swap.hpp>
namespace water { namespace xml {



template<typename char_>
struct memory_node
{
    using char_type = if_not_void<to_unsigned<char_>, char_>; // must have unsigned char
    
    memory_node
        *previous,
        *next,
        *in,
        *nodes,
        *attributes;
    char_type
        *name_begin,
        *name_end,
        *name_end_memory, // name_begin to name_end_memory can be used, name_begin to name_end is used now
        *value_begin,
        *value_end,
        *value_end_memory;
};



namespace _ {

    template<typename iterator_, typename = size_t>
    struct size_iterators {
        static size_t do_it(iterator_ begin, iterator_ end) {
            size_t r = 0;
            while(begin != end) {
                ++begin;
                ++r;
            }
            return r;
        }
    };
    
    template<typename iterator_>
    struct size_iterators<iterator_, decltype(static_cast<size_t>(make_type<iterator_&>() - make_type<iterator_&>()))> {
        static size_t do_it(iterator_ begin, iterator_ end) {
            auto s = end - begin;
            return s > 0 ? static_cast<size_t>(s) : 0;
        }
    };
    
}

template<typename iterator_>
size_t size(iterator_ begin, iterator_ end) {
    return _::size_iterators<iterator_>::do_it(begin, end);
}



namespace _ {

    template<typename range_, typename = size_t>
    struct range_size {
        static size_t do_it(range_ const& a) {
            return xml::size(a.begin(), a.end());
        }
    };
    
    template<typename range_>
    struct range_size<range_, decltype(static_cast<size_t>(make_type<range_ const&>().size()))> {
        static size_t do_it(range_ const& a) {
            return static_cast<size_t>(a.size());
        }
    };
    
}

template<typename range_>
size_t range_size(range_ const& a) {
    return _::range_size<range_>::do_it(a);
}



// this only checks for begin(), because range_size below will use size() and not end() when it can
template<typename range_>
using void_if_range = to_void<decltype(make_type<range_ const&>().begin() == make_type<range_ const&>().begin())>;



template<typename iterator_>
class text
{
public:
    using iterator = iterator_;
    using char_type = no_const_or_reference<decltype(*iterator{})>;
    using value_type = char_type;
    using size_type = size_t;

private:
    iterator myb{}, mye{};

public:
    text() = default;

    text(iterator begin, iterator end) :
        myb{begin},
        mye{end}
    {}

    text(iterator begin, size_type size) :
        myb{begin},
        mye{begin + size}
    {}

    explicit operator bool() const {
        return myb != mye;
    }

    iterator begin() const {
        return myb;
    }

    iterator end() const {
        return mye;
    }

    size_type size() const {
        return static_cast<size_type>(mye - myb);
    }

    decltype(iterator{}[0]) operator[](decltype(iterator{} - iterator{}) a) {
        return myb[a];
    }
};


template<typename iterator_>
text<iterator_> text_from(iterator_ begin, iterator_ end) {
    return { begin, end };
}

template<typename iterator_>
text<iterator_> text_from(iterator_ begin, size_t size) {
    return { begin, size };
}



template<typename char_>
inline char32_t      text_compare_cast(char_ a)         { return static_cast<char32_t>(a); }
inline char16_t      text_compare_cast(char16_t a)      { return a; }
inline unsigned char text_compare_cast(unsigned char a) { return a; }
inline unsigned char text_compare_cast(signed char a)   { return static_cast<unsigned char>(a); }
inline unsigned char text_compare_cast(char a)          { return static_cast<unsigned char>(a); }



template<typename iterator1_, typename iterator2_>
bool equal(iterator1_ b1, iterator1_ e1, iterator2_ b2, iterator2_ e2) {
    if(e1 - b1 != e2 - b2)
        return false;
    while(b1 != e1 && text_compare_cast(*b1) == text_compare_cast(*b2)) {
        ++b1;
        ++b2;
    }
    return b1 == e1;
}

template<typename iterator_, typename char_, size_t size_>
bool equal(char_ const (&a)[size_], iterator_ b, iterator_ e) {
    return equal(a + 0, a + size_ - 1, b, e);
}



template<typename a_, typename b_>
bool operator==(text<a_> const& a, text<b_> const& b) {
    return equal(a.begin(), a.end(), b.begin(), b.end());
}

template<typename a_, typename b_>
bool operator!=(text<a_> const& a, text<b_> const& b) {
    return !(a == b);
}

template<typename iterator_, size_t size_>
bool operator==(text<iterator_> const& a, char const (&b)[size_]) {
    return a == text_from(b, size_ - 1);
}

template<typename iterator_, size_t size_>
bool operator==(char const (&a)[size_], text<iterator_> const& b) {
    return text_from(a, size_ - 1) == b;
}

template<typename iterator_, size_t size_>
bool operator!=(text<iterator_> const& a, char const (&b)[size_]) {
    return a != text_from(b, size_ - 1);
}

template<typename iterator_, size_t size_>
bool operator!=(char const (&a)[size_], text<iterator_> const& b) {
    return text_from(a, size_ - 1) != b;
}



}}
#endif
