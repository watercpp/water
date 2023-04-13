// Copyright 2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_NO_INITIALIZER_LIST_HPP
#define WATER_NO_INITIALIZER_LIST_HPP
#include <water/water.hpp>
#include <water/types.hpp>
#include <water/new_here.hpp>
namespace water {

/*

Normally, when WATER_NO_STD is not defined, std::initializer_list is used.

When WATER_NO_STD is defined no_initializer_list is used instead. It is a poor imitation, but it
makes code work as if it was std::initializer_list with.

water/initializer_list.hpp picks the standard or this.

Construction does not work like the real thing:

    class c {
        c(no_initializer_list<int>) {}
    };
    
    c{1, 2, 3}; // does not work
    c{{1, 2, 3}}; // works


Also, to avoid ambiguity with the copy assignment operator, use the initializer_list_std_or_not_empty:

    class c {
        c() = default;
        c& operator=(c const&) {}
        c& operator=(initializer_list_std_or_not_empty<int>) {}
    };
    
    class c;
    c = {}; // this will call the copy assign if WATER_NO_STD is defned, for std::initializer_list it would call initializer_list assign


And the lifetime is temporary, bound to the constructor arguments and not to the no_initializer_list object itself.
It must be used as an argument to a function and then only inside that function. Not like this:

    auto a = no_initializer_list<int>{1, 2, 3}; // do not do this
    int i = *a.begin(); // the lifetime of the temporary has ended


This does not seem to work with the Intel "classic" compiler. But it works with Intels newer Clang based compiler.
No one uses this + Intel classic compiler + no C++ standard library anyway?

*/



template<typename type_, size_t size_, typename = void>
class no_initializer_list_temporary
{
    unsigned char my[size_ * sizeof(type_)];
    type_ *myat;
    
public:
    
    no_initializer_list_temporary(type_ const& a) {
        myat = new(here(my)) type_{a};
        ++myat;
    }
    
    ~no_initializer_list_temporary() {
        while(myat != begin())
            (--myat)->~type_();
    }
    
    type_ const* begin() const {
        return static_cast<type_ const*>(static_cast<void const*>(my));
    }
    
    type_ const* end() const {
        return begin() + size_;
    }
    
    template<typename ...list_>
    void set(list_&&... l) {
        static_assert(sizeof...(list_) == size_ - 1, "");
        set_do(static_cast<list_&&>(l)...);
    }
    
private:

    void set_do() {
    }

    template<typename a_>
    void set_do(a_&& a) {
        new(here(myat)) type_{a};
        ++myat;
    }
    
    template<typename a_, typename b_, typename ...l_>
    void set_do(a_&& a, b_&& b, l_&&... l) {
        set_do(static_cast<a_&&>(a));
        set_do(static_cast<b_&&>(b), static_cast<l_&&>(l)...);
    }
    
    template<typename a_, typename b_, typename c_, typename d_, typename e_, typename ...l_>
    void set_do(a_&& a, b_&& b, c_&& c, d_&& d, e_&& e, l_&&... l) {
        // reduce recursion depth a little?
        set_do(static_cast<a_&&>(a));
        set_do(static_cast<b_&&>(b));
        set_do(static_cast<c_&&>(c));
        set_do(static_cast<d_&&>(d));
        set_do(static_cast<e_&&>(e), static_cast<l_&&>(l)...);
    }
    
};


template<typename type_, size_t size_>
class no_initializer_list_temporary<type_, size_, to_void<decltype(make_type<ifel<size_ != 1, type_&>>() = type_{})>>
{
    struct {
        type_ array[size_];
    } my;
    
public:
    
    constexpr no_initializer_list_temporary(type_ const& a) :
        my{a}
    {}
    
    type_ const* begin() const {
        return my.array;
    }
    
    type_ const* end() const {
        return my.array + size_;
    }
    
    template<typename ...list_>
    void set(list_&&... l) {
        static_assert(sizeof...(list_) == size_ - 1, "");
        my = {my.array[0], static_cast<list_&&>(l)...};
    }
};


template<typename type_>
class no_initializer_list_temporary<type_, 1, void>
{
    type_ const *my;
    
public:
    
    no_initializer_list_temporary(type_ const& a) :
        my{&a}
    {}
    
    type_ const* begin() const {
        return my;
    }
    
    type_ const* end() const {
        return my + 1;
    }
    
    void set() {
    }
};



template<typename type_>
class no_initializer_list
{
    type_ const
        *mybegin = 0,
        *myend = 0;
    
public:
    
    constexpr no_initializer_list()
    {}
    
    template<typename ...list_>
    no_initializer_list(no_initializer_list_temporary<type_, 1 + sizeof...(list_)>&& m, list_&&... l) :
        mybegin{m.begin()},
        myend{m.end()}
    {
        m.set(static_cast<list_&&>(l)...);
    }
    
    type_ const* begin() const noexcept {
        return mybegin;
    }
    
    type_ const* end() const noexcept {
        return myend;
    }
    
    size_t size() const noexcept {
        return static_cast<size_t>(myend - mybegin);
    }
};



template<typename type_>
class no_initializer_list_not_empty
{
    type_ const
        *mybegin,
        *myend;
    
public:
    
    template<typename ...list_>
    no_initializer_list_not_empty(no_initializer_list_temporary<type_, 1 + sizeof...(list_)>&& m, list_&&... l) :
        mybegin{m.begin()},
        myend{m.end()}
    {
        m.set(static_cast<list_&&>(l)...);
    }
    
    type_ const* begin() const noexcept {
        return mybegin;
    }
    
    type_ const* end() const noexcept {
        return myend;
    }
    
    size_t size() const noexcept {
        return static_cast<size_t>(myend - mybegin);
    }
};



}
#endif
