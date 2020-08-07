// Copyright 2017-2018 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_XML_TESTS_BITS_HPP
#define WATER_XML_TESTS_BITS_HPP
#include <water/xml/xml.hpp>
#include <water/test.hpp>
#include <water/unicode/utf_iterators.hpp>
namespace water { namespace xml { namespace tests {

inline char32_t equal_decode(char32_t a) {
    // this wont translate crlf to lf
    switch(a) {
        case 0xd:
        case 0x85:
        case 0x2028: return 0xa;
    }
    return a;
}

template<typename iterator1_, typename iterator2_>
bool equal_ranges(iterator1_ b1, iterator1_ e1, iterator2_ b2, iterator2_ e2) {
    auto i1 = unicode::utf_iterator_from<32>(b1, e1);
    auto i2 = unicode::utf_iterator_from<32>(b2, e2);
    while(i1 && i2 && equal_decode(*i1) == equal_decode(*i2)) {
        ++i1;
        ++i2;
    }
    return !i1 && !i2;
}

template<typename a_, typename b_>
bool equal_ranges(a_ const& a, b_ const& b) {
    return equal_ranges(a.begin(), a.end(), b.begin(), b.end());
}

template<typename left_, typename right_>
struct equal_return {
    left_ left;
    right_ right;
    explicit operator bool() const {
        return !left && !right;
    }
};

template<typename cl_, typename ml_, typename cr_, typename mr_>
equal_return<node<cl_, ml_>, node<cr_, mr_>> equal(node<cl_, ml_> l, node<cr_, mr_> r) {
    // compare the whole tree of left and right
    // if any nodes are not equal, they are returned in equal_return
    while(l) {
        if(!r)
            break;
        auto la = l.attributes();
        auto ra = r.attributes();
        while(la) {
            if(!equal_ranges(la.name(), ra.name()) || !equal_ranges(la.value(), ra.value()))
                break;
            la = la.next();
            ra = ra.next();
        }
        if(la || ra)
            return {l, r}; // return nodes not the attributes
        if(!equal_ranges(l.name(), r.name()) || !equal_ranges(l.value(), r.value()))
            break;
        // next
        if(l.nodes()) {
            l = l.nodes();
            r = r.nodes();
        }
        else if(r.nodes())
            break;
        else if(l.next()) {
            l = l.next();
            r = r.next();
        }
        else if(r.next())
            break;
        else {
            l = l.in().next();
            r = r.in().next();
        }
    }
    return {l, r};
}

}}}
#endif
