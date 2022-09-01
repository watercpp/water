// Copyright 2022 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_XML_ITERATOR_HPP
#define WATER_XML_ITERATOR_HPP
#include <water/xml/node.hpp>
#include <water/iterator.hpp>
#include <water/begin_end.hpp>
namespace water { namespace xml {


// forward iterator for xml::node


template<typename node_>
struct iterator;

template<typename char_, typename memory_>
struct iterator<node<char_, memory_>>
{
    using iterator_category = forward_iterator_tag;
    using difference_type = void;
    using value_type = node<char_, memory_>;
    using reference = value_type;
    class pointer {
        value_type my;
    public:
        pointer(value_type a) : my{a} {}
        value_type* operator->() { return &my; }
    };
    
private:
    value_type my;
    bool myname = false;

public:
    
    constexpr iterator() = default;
    
    iterator(value_type in) :
        my{in},
        myname{false}
    {}
    
    template<typename name_>
    iterator(value_type in, name_&& name) :
        my{in.find(name)},
        myname{true}
    {}
    
    explicit operator bool() const {
        return static_cast<bool>(my);
    }
    
    value_type operator*() const {
        return my;
    }

    pointer operator->() const {
        return my;
    }
    
    iterator& operator++() {
        if(myname)
            my = my.next().find(my.name());
        else
            my = my.next();
        return *this;
    }
    
    iterator operator++(int) {
        iterator r(*this);
        ++*this;
        return r;
    }
    
    bool operator==(iterator const& a) const {
        return my == a.my;
    }

    bool operator!=(iterator const& a) const {
        return my != a.my;
    }
};



template<typename char_, typename memory_>
iterator<node<char_, memory_>> iterator_from(node<char_, memory_> a) {
    return a;
}

template<typename char_, typename memory_, typename name_>
iterator<node<char_, memory_>> iterator_from(node<char_, memory_> a, name_&& name) {
    return {a, name};
}



template<typename char_, typename memory_>
begin_end<iterator<node<char_, memory_>>> nodes(node<char_, memory_> a) {
    return {a, iterator<node<char_, memory_>>{}};
}

template<typename char_, typename memory_, typename name_>
begin_end<iterator<node<char_, memory_>>> nodes(node<char_, memory_> a, name_&& name) {
    return {iterator<node<char_, memory_>>{a, name}, iterator<node<char_, memory_>>{}};
}



template<typename char_, typename memory_>
begin_end<iterator<node<char_, memory_>>> nodes_in(node<char_, memory_> a) {
    return nodes(a.nodes());
}

template<typename char_, typename memory_, typename name_>
begin_end<iterator<node<char_, memory_>>> nodes_in(node<char_, memory_> a, name_&& name) {
    return nodes(a.nodes(), name);
}



template<typename char_, typename memory_>
begin_end<iterator<node<char_, memory_>>> attributes_in(node<char_, memory_> a) {
    return nodes(a.attributes());
}



}}
#endif
