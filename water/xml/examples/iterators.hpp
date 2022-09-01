// Copyright 2022 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_XML_EXAMPLES_ITERATOR_HPP
#define WATER_XML_EXAMPLES_ITERATOR_HPP
#include <water/xml/xml.hpp>
#include <water/vector.hpp>
#include <water/numbers/numbers.hpp>
namespace water { namespace xml { namespace examples { namespace iterators {

/*

Basic example of using the xml::iterator and related functions from iterator.hpp.

The iterators could be easier to use than traversing the nodes with the xml::node functions directly.

xml::iterator is a forward iterator that iterates over and returns xml::node values,
like using xml::node.next() to go to the next node.

It can also iterate over nodes with a specifict name, like using xml::node.find("name")

*/

char constexpr text[] = R"###(<?xml version="1.1" encoding="UTF-8"?>

    <root one="1" two="2" three="3">
        <animal legs="4">
            Anteater
        </animal>
        <fruit taste="good">
            Avocado
        </fruit>
        <animal legs="6">
            Ant
        </animal>
        <animal legs="2">
            Bat
        </animal>
        <fruit taste="good">
            Banana
        </fruit>
        <fruit taste="good">
            Cherry
        </fruit>
        <animal legs="4">
            Cat
        </animal>
    </root>

)###";


template<typename ...a_>
inline void unused(a_&&...) {
    // avoid warnigns
}


inline bool iterator_ranges() {
    xml::memory<> memory;
    auto root = read_to_memory(memory)(text, text + sizeof(text) - 1).nodes();
    
    unsigned all = 0;
    unsigned animals = 0;
    unsigned fruits = 0;
    
    // traverse all nodes inside <root>
    for(auto node : nodes_in(root)) {
        ++all;
        if(node.name() == "animal")
            ++animals;
        if(node.name() == "fruit")
            ++fruits;
    }
    
    // only the animal nodes
    for(auto node : nodes_in(root, "animal")) {
        if(!node.attributes("legs")) // never happens, all the animals have legs
            break;
        ++animals;
    }
    
    // only the fruit nodes, with a while loop
    auto range = nodes_in(root, "fruit");
    auto
        begin = range.begin(),
        end = range.end();
    while(begin != end) {
        if(!begin->attributes("taste")) // never happens, all the fruits have taste
            break;
        ++fruits;
        ++begin;
    }
    
    // the attributes of <root>
    unsigned attributes = 0;
    for(auto node : attributes_in(root)) {
        unused(node);
        ++attributes;
    }
    
    // nodes_in(x) will iterate over x.nodes().
    // nodes(x) will iterate over x and the nodes after it.
    for(auto node : nodes(root.nodes(), "animal")) {
        unused(node);
    }
    
    return all == 7 && animals == 4 * 2 && fruits == 3 * 2 && attributes == 3;
}



inline bool plain_iterators() {
    xml::memory<> memory;
    auto root = read_to_memory(memory)(text, text + sizeof(text) - 1).nodes();
    
    using node_type = node<char, xml::memory<>>;
    
    unsigned all = 0;
    unsigned animals = 0;
    unsigned fruits = 0;
    unsigned attributes = 0;
    
    // creating iterators directly with the constructor or iterator_from functions
    iterator<node_type>
        b1 = iterator<node_type>(root.nodes()),
        b2 = iterator<node_type>(root.nodes(), "animal"),
        b3 = iterator_from(root.nodes(), "fruit"),
        b4 = iterator_from(root.attributes());
        
    // the end iterator is a default constructed iterator
    iterator<node_type> end;
    
    // the value is a node
    node_type node = *b1;
    bool same = node.name() == b1->name();
    unused(same);
    
    // the iterator will convert to false when it goes beyond the last node
    while(b1++)
        ++all;
    
    // the default constructed also converts to false, and compares equal to the one beyond the last node
    if(!end && b1 == end)
        b1 = root.nodes();
    
    while(b1 != end) {
        if(b1->name() == "animal")
            ++animals;
        if(b1->name() == "fruit")
            ++fruits;
        ++b1;
    }
    
    while(b2 != end) {
        ++b2;
        ++animals;
    }
    
    do ++fruits; while(++b3);
    
    while(b4++)
        ++attributes;
    
    return all == 7 && animals == 4 * 2 && fruits == 3 * 2 && attributes == 3;
}

}}}}
#endif
