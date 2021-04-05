// Copyright 2017-2021 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_UNICODE_BITS_HPP
#define WATER_UNICODE_BITS_HPP
#include <water/water.hpp>
#include <water/types/types.hpp>
#include <water/iterator.hpp>
#include <water/char8.hpp>
namespace water { namespace unicode {

using size_t = decltype(sizeof(0));

using uchar_t = unsigned char;
    
template<typename>
struct to_void {
    using result = void;
};

template<typename iterator_, typename result_, typename category_ = typename iterator_category<iterator_>::result>
struct if_random_access
{};

template<typename iterator_, typename result_>
struct if_random_access<iterator_, result_, random_access_iterator_tag> {
    using result = result_;
};

template<typename iterator_, typename result_, typename category_ = typename iterator_category<iterator_>::result>
struct if_not_random_access {
    using result = result_;
};

template<typename iterator_, typename result_>
struct if_not_random_access<iterator_, result_, random_access_iterator_tag>
{};

template<typename char_>
inline char32_t cast(char_ a) { return static_cast<char32_t>(a); }
inline char16_t cast(char16_t a) { return a; }
inline uchar_t cast(uchar_t a) { return a; }
inline uchar_t cast(char a) { return static_cast<uchar_t>(a); }
inline uchar_t cast(signed char a) { return static_cast<uchar_t>(a); }
inline uchar_t cast(char8_or_not a) { return static_cast<uchar_t>(a); } // the normal utf-8 type for this namespace is still unsigned char


template<typename iterator_>
class begin_end
{
public:
    using iterator = iterator_;

private:
    iterator
        mybegin,
        myend;

public:
    begin_end() :
        mybegin{},
        myend{}
    {}
    
    begin_end(iterator begin, iterator end) :
        mybegin{begin},
        myend{end}
    {}
    
    explicit operator bool() const {
        return mybegin != myend;
    }
    
    iterator begin() const {
        return mybegin;
    }
    
    iterator end() const {
        return myend;
    }
};

}}
#endif
