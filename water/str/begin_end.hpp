// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_STR_BEGIN_END_HPP
#define WATER_STR_BEGIN_END_HPP
#include <water/str/out.hpp>
namespace water { namespace str {

/*

Write to a begin,end range or an array

This can end in an incomplete utf-8 or utf-16 sequence, if the range ends in the middle of a sequence.

truncated() returns true if the range did not fit all characters.
If this happens, and you were writing multi-character utf-8 or utf-16 sequences, discard the
written string or validate that it is good UTF.

This converts to true if its not empty, and not truncated

Example:
    
    wchar_t array[123];
    auto out = str::out_begin_end(array);
    out << "hello world " << 123.45 << '\0';
    if(out) {
        // it is not truncated, and not empty
        fputws(array, stdout);
    }

*/

template<typename iterator_>
class begin_end
{
public:
    using iterator = iterator_;
    using char_type = typename types::no_const<types::no_reference<decltype(*types::make<iterator_&>())>>::result;

private:
    iterator
        mybegin {},
        myat {},
        myend {};
    bool mytruncated = false;

public:
    begin_end() = default;

    begin_end(iterator begin, iterator end) :
        mybegin{begin},
        myat{begin},
        myend{end}
    {}

    iterator begin() const {
        return mybegin;
    }

    iterator end() const {
        // the end of what has been written
        return myat;
    }
    
    bool truncated() const {
        return mytruncated;
    }

    explicit operator bool() const {
        return myat != mybegin && !mytruncated;
    }
    
    size_t size() const {
        return static_cast<size_t>(myat - mybegin);
    }

    template<typename iterator2_>
    void operator()(iterator2_ begin, iterator2_ end) {
        while(begin != end && myat != myend) {
            *myat = *begin;
            ++myat;
            ++begin;
        }
        if(begin != end)
            mytruncated = true;
    }
};


template<typename iterator_>
out<begin_end<iterator_>> to_begin_end(iterator_ begin, iterator_ end, settings s = {}) {
    return {s, begin, end};
}


template<typename iterator_>
out<begin_end<iterator_>> to_begin_end(iterator_ begin, size_t size, settings s = {}) {
    return {s, begin, begin + size};
}


template<typename char_, size_t size_>
out<begin_end<char_*>> to_begin_end(char_ (&array)[size_], settings s = settings{}) { // settings{} instead of just {} is a visual c++ workaround
    return {s, array + 0, array + size_};
}


}}
#endif
