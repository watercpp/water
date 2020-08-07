// Copyright 2018 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_JSON_EXAMPLES_CREATING_NODES_1_HPP
#define WATER_JSON_EXAMPLES_CREATING_NODES_1_HPP
#include <water/json/json.hpp>
#include <water/vector.hpp>
namespace water { namespace json { namespace examples { namespace creating_nodes_1 {

/*

This creates the JSON text below by programatically bulding the node tree structure.
Hopefully it gives an idea how a data structure from with a program can be converted to JSON text.

{
    "null": null,
    "bool_false": false,
    "bool_true": true,
    "integer": 123,
    "floatingpoint": -1.23e-45,
    "string": "hello world",
    "object": {
        "hello": "world"
    },
    "array_of_numbers": [
        1,
        2,
        3,
        4
    ]
}

json::node<json::memory<>> is a mutable node. it is possible to set its value and
use it to change the node tree structure.

json::node<> without the memory template argument is a constant node


*/

inline vector<char> example() {
    json::memory<> memory;
    
    // use memory to create a node
    json::node<json::memory<>> root = memory.create();
    
    // root is a mutable node, it has the memory as template argument
    // it can itslef create new nodes and its possible to set its value.
    
    // To make a node an object, use the object(123) function.
    // The integer is the initial capcity of the object (number of nodes inside the object)
    // Giving the object the needed capcity from the start can slightly improve memory use,
    // but using the wrong number is fine, the capcity will grow if needed.
    root.object(8);
    
    // Use push_back() push_front() insert(123) create and insert new nodes into root.
    // All functions return nodes, itself or the newly created one.
    // Nesting functions makes the code compact.
    
    root.push_front().name("null").null();
    root.insert(1).name("bool_false").boolean(false); // insert at the integer offset in the object
    root.push_back().name("bool_true").boolean(true);
    root.push_back().name("integer").number(123);
    root.push_back().name("floatingpoint").number(-1.23e-45);
    root.push_back().name("string").string("hello_world");
    root.push_back().name("object").object(1).push_back().name("hello").string("world");

    // The only thing left to reach the result JSON is "array_of_numbers".
    // To make an array, use array(123) just like object(123).
    // Here capacity is 0, the array will grow when nodes are added to it.
    auto array_of_numbers = memory.create().array(0);
    while(array_of_numbers.size() != 4)
        array_of_numbers.push_back().number(array_of_numbers.size());
    array_of_numbers.name("array_of_numbers");
    root.insert(root.size(), array_of_numbers);
    
    // write as indented JSON text to a vector
    vector<char> result;
    write(indent_to([&result](char a) { result.push_back(a); }), root);
    return result;
}

}}}}
#endif
