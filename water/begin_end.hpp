// Copyright 2022-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_BEGIN_END_HPP
#define WATER_BEGIN_END_HPP
#include <water/water.hpp>
#include <water/types.hpp>
namespace water {



namespace _ {

    template<typename begin_end_, typename iterator_, typename = size_t>
    struct begin_end_size_maybe
    {};

    template<typename begin_end_, typename iterator_>
    struct begin_end_size_maybe<begin_end_, iterator_, decltype(static_cast<size_t>(make_type<iterator_ const&>() - make_type<iterator_ const&>()))>
    {
        using size_type = size_t;
        
        size_t size() const {
            return static_cast<size_t>(static_cast<begin_end_ const*>(this)->end() - static_cast<begin_end_ const*>(this)->begin());
        }
    };

}



template<typename iterator_>
struct begin_end : _::begin_end_size_maybe<begin_end<iterator_>, iterator_>
{
    using iterator = iterator_;
    
private:

    iterator
        mybegin{},
        myend{};

public:
    
    constexpr begin_end() = default;
    
    constexpr begin_end(iterator begin, iterator end) :
        mybegin{begin},
        myend{end}
    {}
    
    constexpr begin_end(iterator begin, size_t size) :
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
    
    template<typename int_>
    auto operator[](int_ a) const -> decltype(begin()[a]) {
        return mybegin[a];
    }
};



template<typename iterator_>
begin_end<iterator_> begin_end_from(iterator_ begin, iterator_ end) {
    return {begin, end};
}

template<typename iterator_>
begin_end<iterator_> begin_end_from(iterator_ begin, size_t size) {
    // avoid begin_end_from(iterator_ const& begin because that will not decay arrays to pointers
    return {begin, size};
}

template<typename type_, size_t size_>
begin_end<type_*> begin_end_from(type_ (&array)[size_]) {
    return {array, size_};
}



namespace _ {

    template<typename a_, typename b_, typename = bool>
    struct begin_end_size_is_different {
        static constexpr bool do_it(a_ const&, b_ const&) {
            return false;
        }
    };
    
    template<typename a_, typename b_>
    struct begin_end_size_is_different<a_, b_, decltype(make_type<a_ const&>().size() != make_type<b_ const&>().size())> {
        static bool do_it(a_ const& a, b_ const& b) {
            return a.size() != b.size();
        }
    };
    
}


template<typename a_, typename b_>
auto operator==(begin_end<a_> const& a, begin_end<b_> const& b) -> decltype(static_cast<bool>(*a.begin() == *b.begin())) {
    if(_::begin_end_size_is_different<begin_end<a_>, begin_end<b_>>::do_it(a, b))
        return false;
    auto ai = a.begin();
    auto bi = b.begin();
    while(ai != a.end() && bi != b.end()) {
        if(!(*ai == *bi))
            return false;
        ++ai;
        ++bi;
    }
    return true;
}

template<typename a_, typename b_>
auto operator!=(begin_end<a_> const& a, begin_end<b_> const& b) -> decltype(static_cast<bool>(*a.begin() == *b.begin())) {
    return !(a == b);
}



}
#endif
