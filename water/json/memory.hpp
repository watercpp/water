// Copyright 2017-2018 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_JSON_MEMORY_HPP
#define WATER_JSON_MEMORY_HPP
#include <water/json/node.hpp>
#include <water/xml_json/memory.hpp>
namespace water { namespace json {

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

    node<memory> create() {
        auto n = static_cast<memory_node*>(this->allocate(sizeof(memory_node), alignof(memory_node)));
        if(n) *n = {};
        return {*this, n};
    }
};

template<typename allocator_>
void swap(memory<allocator_>& a, memory<allocator_>& b) {
    a.swap(b);
}

template<typename allocator_>
node<memory<allocator_>> create(memory<allocator_>& a) { // water::xml has this
    return a.create();
}

}}
#endif
