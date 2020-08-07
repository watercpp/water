// Copyright 2017-2018 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_JSON_STRING_HPP
#define WATER_JSON_STRING_HPP
#include <water/json/bits.hpp>
namespace water { namespace json {

template<typename iterator_>
class string
{
public:
    using iterator = iterator_;
    using char_type = typename types::no_const<types::no_reference<decltype(*iterator{})>>::result;
    using value_type = char_type;
    using size_type = size_t;

private:
    iterator myb{}, mye{};

public:
    string() = default;

    string(iterator begin, iterator end) :
        myb(begin),
        mye(end)
    {}

    string(iterator begin, size_type size) :
        myb(begin),
        mye(begin + size)
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
string<iterator_> string_from(iterator_ begin, iterator_ end) {
    return {begin, end};
}

template<typename iterator_>
string<iterator_> string_from(iterator_ begin, size_t size) {
    return {begin, size};
}

template<typename a_, typename b_>
bool operator==(string<a_> const& a, string<b_> const& b) {
    if(a.size() != b.size())
        return false;
    auto ai = a.begin();
    auto bi = b.begin();
    while(ai != a.end() && string_compare_cast(*ai) == string_compare_cast(*bi)) {
        ++ai;
        ++bi;
    }
    return ai == a.end();
}

template<typename a_, typename b_>
bool operator!=(string<a_> const& a, string<b_> const& b) {
    return !(a == b);
}

template<typename iterator_, size_t size_>
bool operator==(string<iterator_> const& a, char const (&b)[size_]) {
    return a == string_from(b, size_ - 1);
}

template<typename iterator_, size_t size_>
bool operator==(char const (&a)[size_], string<iterator_> const& b) {
    return string_from(a, size_ - 1) == b;
}

template<typename iterator_, size_t size_>
bool operator!=(string<iterator_> const& a, char const (&b)[size_]) {
    return a != string_from(b, size_ - 1);
}

template<typename iterator_, size_t size_>
bool operator!=(char const (&a)[size_], string<iterator_> const& b) {
    return string_from(a, size_ - 1) != b;
}

}}
#endif
