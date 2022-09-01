// Copyright 2018-2022 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_XML_EXAMPLES_WRITING_TO_TEXT_HPP
#define WATER_XML_EXAMPLES_WRITING_TO_TEXT_HPP
#include <water/xml/xml.hpp>
#include <water/vector.hpp>
namespace water { namespace xml { namespace examples { namespace writing_to_text {

/*

Writing xml::node tree as XML text is done with the xml::write function.

xml::write_size can be used to determine how many characters xml::write will write.

write (and write_size) has 2 optional arguments:

bool xml_declaration (default true)
When true, writes <?xml ....?>

unsigned indent_spaces_or_tab (default '\t', integer value 9)
When its '\t', indents the XML with tab
When its 0, does not indent the XML at all. More compact but less readable for humans.
When its anything else, indents the XML with that many spaces.


example1 will use the defaults and write the XML like this:
    
    <?xml version="1.0" encoding="UTF-8"?>
    <hello>
        world
    </hello>

example2 will not indent and not write the XML delcaration, like this:
    
    <hello>world</hello>

*/

char constexpr text[] = "<hello>world</hello>";
size_t constexpr text_size = sizeof(text) - 1;


inline bool example1() {
    xml::memory<> memory;
    auto nodes = read_to_memory(memory)(text, text_size).nodes();
    
    vector<char> destination;
    
    write(
        [&destination](char const* begin, char const* end) {
            destination.insert(destination.end(), begin, end);
        },
        nodes
    );
    
    return !destination.empty();
}



// Here write_size is used to reserve memory in the vector before writing.
// And the xml_declaration plus indent_spaces_or_tab arguments are used.

inline bool example2() {
    xml::memory<> memory;
    auto nodes = read_to_memory(memory)(text, text_size).nodes();
    bool xml_declaration = false;
    unsigned indent_spaces_or_tab = 0; // do not indent
    
    size_t size = write_size(nodes, xml_declaration, indent_spaces_or_tab);
    
    vector<char> destination;
    destination.reserve(size);
    
    size_t size_written = write(
        [&destination](char const* begin, char const* end) {
            destination.insert(destination.end(), begin, end);
        },
        nodes,
        xml_declaration,
        indent_spaces_or_tab
    );
    
    return size_written == size && destination.size() == size;
}

}}}}
#endif
