// Copyright 2018 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_JSON_EXAMPLES_WRITING_TO_TEXT_HPP
#define WATER_JSON_EXAMPLES_WRITING_TO_TEXT_HPP
#include <water/json/json.hpp>
#include <water/vector.hpp>
namespace water { namespace json { namespace examples { namespace writing_to_text {

/*

Writing json::node tree as JSON text is done with the json::write function.

json::write_size can be used to determine how many characters json::write will write.

write (and write_size) has an optional bool argument "esacpe_all". When true, all characters
that can be escaped are escaped, the JSON text will only contain ASCII characrers.

*/

char constexpr text[] = "{\"hello\":\"world\"}";
size_t constexpr text_size = sizeof(text) - 1;


inline bool basic() {
    json::memory<> memory;
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
// And the escape_all argument is used

inline bool escape_all_and_measure_size() {
    json::memory<> memory;
    auto nodes = read_to_memory(memory)(text, text_size).nodes();
    bool escape_all = true;
    
    size_t size = write_size(nodes, escape_all);
    
    vector<char> destination;
    destination.reserve(size);
    
    size_t size_written = write(
        [&destination](char const* begin, char const* end) {
            destination.insert(destination.end(), begin, end);
        },
        nodes,
        escape_all
    );
    
    return size_written == size && destination.size() == size;
}


}}}}
#endif
