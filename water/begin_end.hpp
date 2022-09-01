// Copyright 2022 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_BEGIN_END_HPP
#define WATER_BEGIN_END_HPP
#include <water/water.hpp>
namespace water {



template<typename iterator_>
struct begin_end
{
    using iterator = iterator_;
    
private:

    iterator
        mybegin{},
        myend{};

public:
    
    begin_end() = default;
    
    begin_end(iterator const& begin, iterator const& end) :
        mybegin{begin},
        myend{end}
    {}
    
    begin_end(iterator const& begin, size_t size) :
        mybegin{begin},
        myend{begin + size}
    {}
    
    explicit operator bool() const {
        return mybegin != myend;
    }
    
    iterator const& begin() const {
        return mybegin;
    }
    
    iterator const& end() const {
        return myend;
    }
    
    size_t size() const {
        return static_cast<size_t>(myend - mybegin);
    }
    
    template<typename int_>
    auto operator[](int_ a) const -> decltype(begin()[a]) {
        return mybegin[a];
    }
};



template<typename iterator_>
begin_end<iterator_> begin_end_from(iterator_ const& begin, iterator_ const& end) {
    return {begin, end};
}

template<typename iterator_>
begin_end<iterator_> begin_end_from(iterator_ const& begin, size_t size) {
    return {begin, size};
}



}
#endif
