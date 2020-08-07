// Copyright 2018 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_JSON_EXAMPLES_NODE_TREE_1_HPP
#define WATER_JSON_EXAMPLES_NODE_TREE_1_HPP
#include <water/json/json.hpp>
#include <water/vector.hpp>
#include <water/ministr/out.hpp>
namespace water { namespace json { namespace examples { namespace node_tree_1 {


/*

json::node is similar to an iterator or pointer, it points to a position in the tree structure.

A node knows how to move to the next and previous nodes at the same level in the tree.
It also how to move to the node that it is inside of.
And if its an array or object, how to move to the nodes it contains.

This makes it possible to traverse the tree with a single node object, and without recursive functions.

This example show the different functions for moving around the tree structure.

*/

char constexpr text[] = u8R"###({
    "zero": true,
    "one": 123,
    "two": 12.34E45,
    "three": "value",
    "four": [
        0,
        1,
        2,
        3
    ],
    "five": {
        "same": 0,
        "same": 1,
        "same": 2,
        "same": 3
    }
})###";

template<typename a_>
inline void unused(a_ const&) {}


inline bool example() {
    json::memory<> memory;
    json::node<> root = read_to_memory(memory)(text, sizeof(text) - 1).nodes();
    
    // nodes() returns the first node of an object or array
    auto node = root.nodes();
    
    // nodes inside an object has a name()
    if(node.name() != "zero")
        return false;
    
    // in() returns the node that the current node is inside
    if(node.in() != root)
        return false;
    
    // next() and previous() moves to the next or previous node
    auto node2 = node.next();
    if(node2.name() != "one")
        return false;
    if(node2.previous() != node)
        return false;
    
    // first() and last() returns the first or last node
    if(node2.first() != node)
        return false;
    if(node2.last().name() != "five")
        return false;
    
    // moving outside of the tree returns a default constructed node, it converts to false
    if(
        root.in() || // root is not inside another node
        node.nodes() || // the "zero" is a bool, not an object or array
        node.previous() || // "zero" is the first
        node.last().next() || // nothing after the last
        node.last().next().nodes().previous() // its fine to try operations on a node that points nowhere
    )
        return false;
    
    // simple loop using next()
    for(auto n = node; n; n = n.next()) {
        // do something here
        unused(n);
    }
    
    // size() returns the size of an object or array
    size_t size = root.size();
    if(size != 6)
        return false;
    
    // at(123) returns the node at that position in an object or array
    auto five = root.at(5);
    if(five.name() != "five")
        return false;
    
    // at_position() returns the position a node has in the object or array
    if(five.at_position() != 5)
        return false;
    
    // simple loop using at and size
    for(size_t i = 0; i != five.size(); ++i) {
        auto n = five.at(i);
        unused(n);
    }
    
    // subscript works just like at(5)
    five = root[5];
    
    // subscript also works with strings, to get nodes in an object
    five = root["five"];
    
    // find("text") will find the next node in an object with a specfic name, starting from the current node
    five = node.find("five");
    if(five != five.find("five")) // find starts at the current node, so it finds itself
        return false;
    
    // subscript looks at the nodes the current node contains
    // find looks at the nodes at the same level as the current node
    // these 2 lines do the same thing:
    five = root["five"];
    five = root.nodes().find("five");
    
    // the "five" object contains many nodes with the same name.
    // subscript will only find the first, the find() function can find all
    size_t same_count = 0;
    for(auto n = five.nodes().find("same"); n; n = n.next().find("same"))
        ++same_count;
    if(same_count != 4)
        return false;
    
    // subsctipt and find has overloads for ranges
    json::string<char const*> name{"four", 4};
    json::node<> four;
    four = root["four"];
    four = root[name];
    four = root[four.name()];
    four = node.find("four");
    four = node.find(name);
    four = node.find(name.begin(), name.end());
    four = node.find(name.begin(), name.size());
    
    // the begin and end functions return iterators to iterate over the nodes in an object or array
    // "five" is an object, this iterates over that object nodes
    auto i = five.begin();
    while(i != five.end()) {
        json::node<> n = *i;
        if(n.name() != "same")
            return false;
        ++i;
    }
    
    // same thing using a for loop
    for(json::node<> n : five)
        if(n.name() != "same")
            return false;
    
    return true;
}


}}}}
#endif
