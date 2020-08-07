// Copyright 2017-2018 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_XML_MEMORY_HPP
#define WATER_XML_MEMORY_HPP
#include <water/xml/node.hpp>
#include <water/xml_json/memory.hpp>
namespace water { namespace xml {

template<typename allocator_ = water::allocator>
struct memory :
    xml_json::memory<allocator_>
{
    memory() = default;

    memory(typename xml_json::memory<allocator_>::allocator_type const& a) :
        xml_json::memory<allocator_>{a}
    {}

    memory(memory&& a) :
        xml_json::memory<allocator_>{static_cast<xml_json::memory<allocator_>&&>(a)}
    {}

    memory(memory const& a) = delete;
    memory& operator=(memory const&) = delete;

    memory& operator=(memory&& a) {
        xml_json::memory<allocator_>::operator=(static_cast<xml_json::memory<allocator_>&&>(a));
        return *this;
    }

    template<typename char_>
    xml::node<char_, memory<allocator_>> create() {
        auto r = static_cast<memory_node<char_>*>(this->allocate(sizeof(memory_node<char_>), alignof(memory_node<char_>)));
        if(r) *r = {};
        return {*this, r};
    }

    xml::node<char, memory<allocator_>> create() {
        return this->template create<char>();
    }
};

template<typename allocator_>
void swap(memory<allocator_>& a, memory<allocator_>& b) {
    a.swap(b);
}

template<typename char_, typename allocator_>
node<char_, memory<allocator_>> create(memory<allocator_>& a) {
    return a.template create<char_>();
}

template<typename allocator_>
node<char, memory<allocator_>> create(memory<allocator_>& a) {
    return a.template create<char>();
}

}}
#endif
