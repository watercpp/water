// Copyright 2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_MEMORY_TRACK_NAMES_HPP
#define WATER_MEMORY_TRACK_NAMES_HPP
#include <water/memory_track/container.hpp>
#include <water/cstring.hpp>
namespace water { namespace memory_track {



class name_unique
{
    unsigned mysize = 0;
    // char array[mysize] after this
    
    name_unique() noexcept = default;
    
    ~name_unique() noexcept {
        pattern_set(this, sizeof(name_unique) + mysize);
    }
    
public:

    name_unique(name_unique const&) = delete;
    name_unique& operator=(name_unique const&) = delete;
    
    template<typename iterator_>
    static name_unique* create(iterator_ begin, size_t size) noexcept {
        size_t constexpr max_size = 1000;
        if(size > max_size)
            size = max_size;
        void *p = underlying_allocator_nothrow{}.allocate(sizeof(name_unique) + size);
        if(!p)
            return 0;
        auto r = new(here(p)) name_unique;
        if(size) {
            auto c = static_cast<char*>(static_cast<void*>(r + 1));
            do {
                *c = static_cast<char>(*begin);
                ++c;
                ++begin;
            } while(++r->mysize != size);
        }
        return r;
    }
    
    void destroy() noexcept {
        this->~name_unique();
        underlying_allocator_nothrow{}.free(this, sizeof(name_unique) + mysize);
    }
    
    char const* begin() const noexcept {
        return static_cast<char const*>(static_cast<void const*>(this + 1));
    }
    
    char const* end() const noexcept {
        return begin() + mysize;
    }
    
    unsigned size() const noexcept {
        return mysize;
    }
};


class names;


class name
{
    friend class names;
    
    name_unique* my = 0;
    
    constexpr explicit name(name_unique* a) noexcept :
        my{a}
    {}

public:

    constexpr name() noexcept = default;
    
    explicit operator bool() const noexcept {
        // returns true even if name_unique is a blank name
        return my != 0;
    }
    
    char const* begin() const noexcept {
        return my ? my->begin() : 0;
    }
    
    char const* end() const noexcept {
        return my ? my->end() : 0;
    }
    
    unsigned size() const noexcept {
        return my ? my->size() : 0;
    }
    
    bool operator==(name const& a) const noexcept {
        return my == a.my; // they should all be from the same names object
    }
    
    bool operator!=(name const& a) const noexcept {
        return my != a.my;
    }
};



template<typename iterator_>
struct name_find_return {
    iterator_ found;
    iterator_ insert_before;
};

template<typename name_iterator_, typename char_iterator_, typename name_from_iterator_>
name_find_return<name_iterator_> name_find(name_iterator_ begin, name_iterator_ end, char_iterator_ find, size_t size, name_from_iterator_&& get) {
    if(begin == end)
        return {end, end};
    auto
        i = begin,
        e = end;
    while(i != e) {
        auto x = i + (e - i) / 2;
        if(get(x).size() < size)
            ++(i = x);
        else
            e = x;
    }
    while(i != end && get(i).size() == size) {
        auto n = get(i).begin();
        auto ne = get(i).end();
        auto f = find;
        while(n != ne && *n == static_cast<char>(*f)) {
            ++f;
            ++n;
        }
        if(n == ne)
            return {i, i}; // found exact match
        if(*n < static_cast<char>(*f))
            break;
        ++i;
    }
    return {end, i}; // found where to insert
}



template<typename range_, typename = decltype(make_type<range_ const&>().begin() != make_type<range_ const&>().end())>
bool name_is_empty(range_ const& a) noexcept {
    return a.end() == a.begin();
}
    
inline bool name_is_empty(cstring_pointer<char> a) noexcept {
    return a.begin() == a.end();
}



class names
{
    container<name> my;
    
public:

    constexpr names() = default;
    
    names(names const&) = delete;
    names& operator=(names const&) = delete;
    
    ~names() {
        for(auto n : my)
            n.my->destroy();
    }
    
    template<typename range_, typename = decltype(make_type<range_ const&>().begin() != make_type<range_ const&>().end())>
    name find(range_ const& a) const noexcept {
        return find(a.begin(), static_cast<size_t>(a.end() - a.begin()));
    }
    
    name find(cstring_pointer<char> a) const noexcept {
        return find(a.begin(), static_cast<size_t>(a.end() - a.begin()));
    }
    
    template<typename range_, typename = decltype(make_type<range_ const&>().begin() != make_type<range_ const&>().end())>
    name find_or_add(range_ const& a) noexcept {
        return find_or_add(a.begin(), static_cast<size_t>(a.end() - a.begin()));
    }
    
    name find_or_add(cstring_pointer<char> a) noexcept {
        return find_or_add(a.begin(), static_cast<size_t>(a.end() - a.begin()));
    }

private:
    
    template<typename iterator_>
    name find(iterator_ begin, size_t size) const noexcept {
        auto f = name_find(my.begin(), my.end(), begin, size, [](name const* a) { return *a; }).found;
        if(f != my.end())
            return *f;
        return {};
    }
    
    template<typename iterator_>
    name find_or_add(iterator_ begin, size_t size) noexcept {
        auto f = name_find(my.begin(), my.end(), begin, size, [](name *a) { return *a; });
        if(f.found != my.end())
            return *f.found;
        name n{name_unique::create(begin, size)};
        if(n && my.insert(f.insert_before, n))
            return n;
        if(n)
            n.my->destroy();
        return {};
    }
};



}}
#endif
