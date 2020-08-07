// Copyright 2018 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_JSON_EXAMPLES_NODE_TREE_2_HPP
#define WATER_JSON_EXAMPLES_NODE_TREE_2_HPP
#include <water/json/json.hpp>
#include <water/vector.hpp>
#include <water/ministr/out.hpp>
namespace water { namespace json { namespace examples { namespace node_tree_2 {


/*

Loop through the node tree and trun it into a flat list.

This shows how to go through the entire tree with a single node object without recursive functions.
This happens in the loop function below.

The line_from_node function below creates a line of text from each node.
It is there to do something with each node, it produces output like this:

null:null
bool_false:false
bool_true:true
integer:123
floatingpoint:-1.23e-45
string:"hello world"
object.hello:"world"
object.world:1
object.bool:true
object.nested_object.field:"value"
object.nested_object.nested_nested_object.hello:"world"
object.nested_array.0.number:1
object.nested_array.1.number:2
array_of_integers.0:0
array_of_integers.1:1
array_of_integers.2:2
array_of_integers.3:3
array_of_integers.4:4
array_of_integers.5:5
array_of_integers.6:6
array_of_integers.7:7
array_of_integers.8:8
array_of_arrays.0:"a"
array_of_arrays.1.0:"b"
array_of_arrays.1.1.0:"c"
array_of_arrays.1.1.1.0:"d"
array_of_arrays.1.1.1.1.0:"e"
array_of_arrays.1.1.1.1.1:[]
another_object.name1:"value1"
another_object.name2:"value2"

*/

char constexpr text[] = u8R"###({
    "null": null,
    "bool_false": false,
    "bool_true": true,
    "integer": 123,
    "floatingpoint": -1.23e-45,
    "string": "hello world",
    "object": {
        "hello": "world",
        "world": 1,
        "bool": true,
        "nested_object": {
            "field":"value",
            "nested_nested_object": {
                "hello": "world"
            }
        },
        "nested_array": [
            {
                "number":1
            },
            {
                "number":2
            }
        ]
    },
    "array_of_integers": [
        0,
        1,
        2,
        3,
        4,
        5,
        6,
        7,
        8
    ],
    "array_of_arrays": [
        "a",
        [
            "b",
            [
                "c",
                [
                    "d",
                    [
                        "e",
                        []
                    ]
                ]
            ]
        ]
    ],
    "another_object": {
        "name1": "value1",
        "name2": "value2"
    }
})###";

inline void line_from_node(vector<char>& to, node<> from);


inline vector<char> loop() {
    json::memory<> memory;
    auto node = read_to_memory(memory)(text, sizeof(text) - 1).nodes();
    
    vector<char> result;
    
    while(node) {
        // if this node contains nodes (its an array or object) go into them
        while(node.nodes())
            node = node.nodes();
        
        line_from_node(result, node);
        
        // if there is no node after this, go up one level to to the node that this is in
        while(!node.next() && node.in())
            node = node.in();
        // go to the next node
        node = node.next();
    }
    
    return result;
}


inline void line_from_node(vector<char>& to, node<> from) {
    
    // add the path like object.nested_object.field
    size_t depth = 0;
    auto path = from;
    while(path.in()) {
        path = path.in();
        ++depth;
    }
    while(depth) {
        path = from;
        size_t d = depth;
        while(--d)
            path = path.in();
        if(path.in().type() == type::array) {
            // its in an array, use the integer position in the array as path. ministr to convert integer to string
            auto at = cstring(ministr::out<>{} << path.at_position());
            to.insert(to.end(), at.begin(), at.end());
        }
        else
            to.insert(to.end(), path.name().begin(), path.name().end());
        --depth;
        if(depth)
            to.push_back('.');
    }
    
    // a : between the path an the value
    if(from.in())
        to.push_back(':');
    
    // add the value using json::write
    write(
        [&to](char const *begin, char const* end) {
            to.insert(to.end(), begin, end);
        },
        from
    );
    
    // finish with newline
    to.push_back('\n');
}

}}}}
#endif
