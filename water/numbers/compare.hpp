// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_NUMBERS_COMPARE_HPP
#define WATER_NUMBERS_COMPARE_HPP
#include <water/numbers/bits.hpp>
namespace water { namespace numbers {

/*

used by parsers. compares with utf conversion, and keeps track of how many characters that was used

*/

class compare_and_move
{
    size_t myused = 0;

public:
    template<typename iterator1_, typename iterator2_, typename transform_>
    compare_and_move& operator()(iterator1_& begin1, iterator1_ end1, iterator2_ begin2, iterator2_ end2, transform_&& transform) {
        // if begin1,end1 starts with begin2,end2 move begin1 to after it and return how many characters it was moved.
        // transform is used on both ranges
        // begin1,end1 is converted to utf-32
        // begin2,end2 in not converted, it has to be utf-32, ascii, or utf-16 <= 0xffff
        myused = 0;
        size_t u = 0;
        auto i1 = begin1;
        while(i1 != end1 && begin2 != end2) {
            char32_t c;
            size_t n = unicode::utf_decode_and_move<unicode::utf_from_iterator<iterator1_>>(c, i1, end1);
            if(!n || transform(c) != transform(unicode::cast(*begin2)))
                return *this;
            u += n;
            ++begin2;
        }
        if(u && begin2 == end2) {
            begin1 = i1;
            myused = u;
        }
        return *this;
    }

    template<typename iterator_, typename begin_end_, typename transform_>
    compare_and_move& operator()(iterator_& begin, iterator_ end, begin_end_ const& range, transform_&& transform) {
        return operator()(begin, end, range.begin(), range.end(), transform);
    }

    template<typename iterator_, typename begin_end_>
    compare_and_move& operator()(iterator_& begin, iterator_ end, begin_end_ const& range) {
        return operator()(begin, end, range.begin(), range.end(), [](char32_t a) { return a; });
    }

    explicit operator bool() const {
        return myused != 0;
    }

    size_t used() const {
        return myused;
    }
};



// compare to an array of strings, or an array of characters
class compare_one_of_and_move
{
    size_t
        myused = 0,
        mywhich = 0;

public:
    template<typename iterator1_, typename iterator2_, typename transform_>
    compare_one_of_and_move& operator()(iterator1_& begin1, iterator1_ end1, iterator2_ begin2, iterator2_ end2, transform_&& transform) {
        myused = mywhich = 0;
        do_it(begin1, end1, begin2, end2, transform);
        return *this;
    }

    template<typename iterator_, typename begin_end_, typename transform_>
    compare_one_of_and_move& operator()(iterator_& begin, iterator_ end, begin_end_ const& range, transform_&& transform) {
        return operator()(begin, end, range.begin(), range.end(), transform);
    }

    template<typename iterator_, typename begin_end_>
    compare_one_of_and_move& operator()(iterator_& begin, iterator_ end, begin_end_ const& range) {
        return operator()(begin, end, range.begin(), range.end(), [](char32_t a) { return a; });
    }

    explicit operator bool() const {
        return myused != 0;
    }

    size_t used() const {
        return myused;
    }

    size_t which() const {
        return mywhich;
    }

private:
    template<typename iterator1_, typename iterator2_, typename transform_>
    void do_it(iterator1_& begin1, iterator1_ end1, iterator2_ begin2, iterator2_ end2, transform_& transform, ifel<!has_begin_end<decltype(*begin2)>, int*> = {}) {
        // compare first character of begin1,end1 against range of characters
        auto i1 = begin1;
        char32_t c;
        size_t u = unicode::utf_decode_and_move<unicode::utf_from_iterator<iterator1_>>(c, i1, end1);
        if(u && c) {
            c = transform(c);
            size_t w = 0;
            while(begin2 != end2) {
                if(transform(unicode::cast(*begin2)) == c) {
                    myused = u;
                    mywhich = w;
                    begin1 = i1;
                    return;
                }
                ++begin2;
                ++w;
            }
        }
    }

    template<typename iterator1_, typename iterator2_, typename transform_>
    void do_it(iterator1_& begin1, iterator1_ end1, iterator2_ begin2, iterator2_ end2, transform_& transform, ifel<has_begin_end<decltype(*begin2)>, int*> = {}) {
        // compare begin1,end1 against range of strings
        size_t w = 0;
        while(begin2 != end2) {
            size_t u = compare_and_move{}(begin1, end1, *begin2, transform).used();
            if(u) {
                myused = u;
                mywhich = w;
                return;
            }
            ++begin2;
            ++w;
        }
    }
};

}}
#endif
