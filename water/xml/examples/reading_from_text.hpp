// Copyright 2018 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_XML_EXAMPLES_READING_FROM_TEXT_HPP
#define WATER_XML_EXAMPLES_READING_FROM_TEXT_HPP
#include <water/xml/xml.hpp>
namespace water { namespace xml { namespace examples { namespace reading_from_text {

/*

Reading XML into a xml::node tree:

Create a xml::memory object. This will own the xml::node tree.
The tree is destroyed when the memory object is destroyed.

Use xml::read to parse XML text. The function xml::read_to_memory creates a xml::read object.


*/

char constexpr text[] = u8R"###(<?xml version="1.1" encoding="UTF-8"?>

    <room>
        text
        <thing upside_down="yes">
            <description>butter sandwich</description>
            <location>floor</location>
        </thing>
        more text
    </room>

)###";

char const*const text_begin = text;
char const*const text_end = text + sizeof(text) - 1;



inline bool basic1() {
    xml::memory<> memory;
    auto nodes = read_to_memory(memory)(text_begin, text_end).nodes();
    if(!nodes)
        return false; // if it was not valid XML
    
    // use the xml::node tree
    return
        nodes.nodes("thing").nodes("description").first_value() == "butter sandwich" &&
        nodes.nodes("thing").attributes("upside_down").value() == "yes";
}



// The exact same thing, but without the auto keyword and read_to_memory function

inline bool basic2() {
    xml::memory<> memory;
    xml::read<char, xml::memory<>> read{memory};
    read(text_begin, text_end);
    if(!read)
        return false; // if it was not valid xml
    xml::node<> nodes = read.nodes();
    
    // use the xml::node tree
    xml::node<> thing_description = nodes.nodes("thing").nodes("description");
    xml::node<> thing_upside_down_attribute = nodes.nodes("thing").attributes("upside_down");
    xml::text<char const*> description = thing_description.first_value();
    xml::text<char const*> upside_down = thing_upside_down_attribute.value();
    return
        description == "butter sandwich" &&
        upside_down == "yes";
}



// basic1() and basic2() will copy the text. If the XML text is read from a file or from the internet it is
// possible to avoid the copy by reading directly into a memory buffer allocated with xml::memory and
// using xml::read.parse_in_place.

inline void copy_memory(void *to, void const *from, size_t bytes) {
    auto t = static_cast<unsigned char*>(to);
    auto f = static_cast<unsigned char const*>(from);
    while(bytes--) *t++ = *f++;
}

inline bool parse_in_place() {
    xml::memory<> memory;
    
    // allocate a buffer large enough to fit the text. it will be freed when the memory object is destroyed
    size_t const bytes = static_cast<size_t>(text_end - text_begin);
    void *buffer = memory.allocate(bytes);
    // copy the text into the buffer. imagine it was reading from a file instead.
    copy_memory(buffer, text_begin, bytes);
    
    // parse_in_place will modify the buffer
    auto nodes = read_to_memory(memory).parse_in_place(buffer, bytes).nodes();
    
    // use the xml::node tree
    return
        nodes.first_value() == "text" &&
        nodes.nodes().find("thing").next().value() == "more text";
}



}}}}
#endif
