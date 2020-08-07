// Copyright 2018 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_JSON_EXAMPLES_INDENTING_HPP
#define WATER_JSON_EXAMPLES_INDENTING_HPP
#include <water/json/json.hpp>
#include <water/vector.hpp>
namespace water { namespace json { namespace examples { namespace indenting {

/*

JSON text can be indented or minified with json::indent.

json::indent operates on text, but it can be combined with json::write to
indent a json::node tree.

The json::indent_to function creates a json::indent function object.

*/

char constexpr text[] = "{\"hello\":\"world\"}";
char const*const text_begin = text;
char const*const text_end = text + sizeof(text) - 1;


inline bool basic() {
    vector<char> destination;
    
    unsigned spaces_or_tab = 4; // indent with 4 spaces
    auto indent = indent_to(
        [&destination](char a) { destination.push_back(a); },
        spaces_or_tab
    );
    
    indent(text_begin, text_end);
    
    return !destination.empty();
}



// To write a json::node tree as indented JSON text, combine indent with json::write

inline bool indent_nodes() {
    json::memory<> memory;
    auto nodes = read_to_memory(memory)(text_begin, text_end).nodes();
    
    vector<char> destination;
    
    unsigned spaces_or_tab = '\t'; // indent with tab
    auto indent = indent_to(
        [&destination](char a) { destination.push_back(a); },
        spaces_or_tab
    );
    
    write(indent, nodes);
    
    return !destination.empty();
}



// It can be used in a loop, the result is the same as indenting all at once.
// When spaces_or_tab is 0 it will minify everything to a single line instead of indent.

inline bool loop_and_minify() {
    vector<char> destination;
    
    unsigned spaces_or_tab = 0; // minify
    auto indent = indent_to(
        [&destination](char a) { destination.push_back(a); },
        spaces_or_tab
    );
    
    for(auto t = text_begin; t != text_end; ++t)
        indent(t, t + 1);
    
    return !destination.empty();
}


}}}}
#endif
