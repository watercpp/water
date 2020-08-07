// Copyright 2018 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_JSON_EXAMPLES_READING_FROM_TEXT_HPP
#define WATER_JSON_EXAMPLES_READING_FROM_TEXT_HPP
#include <water/json/json.hpp>
namespace water { namespace json { namespace examples { namespace reading_from_text {

/*

Reading JSON into a json::node tree:

Create a json::memory object. This will own the json::node tree.
The tree is destroyed when the memory object is destroyed.

Use json::read to parse JSON text. The function json::read_to_memory creates a json::read object.

*/

char constexpr text[] = u8R"###(
{
    "object": {
        "nested_object": {
            "nested_nested_object": {
                "hello": "world"
            }
        }
    },
    "array_of_arrays": [
        [
            [
                [
                    [
                        123
                    ]
                ]
            ]
        ]
    ]
}
)###";

char const*const text_begin = text;
char const*const text_end = text + sizeof(text) - 1;
    


inline bool basic1() {
    json::memory<> memory;
    auto nodes = read_to_memory(memory)(text_begin, text_end).nodes();
    if(!nodes)
        return false; // if it was not valid json
    
    // use the json::node tree
    auto world = nodes["object"]["nested_object"]["nested_nested_object"]["hello"];
    auto does_not_exist = nodes["does"]["not"]["exist"];
    return world.string() == "world" && !does_not_exist;
}



// The exact same thing, but without the auto keyword and read_to_memory function

inline bool basic2() {
    json::memory<> memory;
    json::read<json::memory<>> read{memory};
    read(text_begin, text_end);
    if(!read)
        return false; // if it was not valid json
    json::node<> nodes = read.nodes();
    
    // use the json::node tree
    json::node<> world = nodes["object"]["nested_object"]["nested_nested_object"]["hello"];
    json::node<> does_not_exist = nodes["does"]["not"]["exist"];
    return world.string() == "world" && !does_not_exist;
}



// basic1() and basic2() will copy the text. If the JSON text is read from a file or from the internet it is
// possible to avoid the copy by reading directly into a memory buffer allocated with json::memory and
// using json::read.parse_in_place.

inline void copy_memory(void *to, void const *from, size_t bytes) {
    auto t = static_cast<unsigned char*>(to);
    auto f = static_cast<unsigned char const*>(from);
    while(bytes--) *t++ = *f++;
}

inline bool parse_in_place() {
    json::memory<> memory;
    
    // allocate a buffer large enough to fit the text. it will be freed when the memory object is destroyed
    size_t const bytes = static_cast<size_t>(text_end - text_begin);
    void *buffer = memory.allocate(bytes);
    // copy the text into the buffer. imagine it was reading from a file instead.
    copy_memory(buffer, text_begin, bytes);
    
    // parse_in_place will modify the buffer
    auto nodes = read_to_memory(memory).parse_in_place(buffer, bytes).nodes();
    
    // use the json::node tree
    return nodes["array_of_arrays"][0][0][0][0][0].number().to_int() == 123;
}


// even when json::read sucessfully parses JSON text, it is possible that a number could not be fully
// represented by the json::number type, or that a string was so long it was cut off.
// when this matters, json::read has functions to check if that happened.

inline bool pedantic_error_checking() {
    json::memory<> memory;
    auto read = read_to_memory(memory)(text_begin, text_end);
    if(!read)
        return false; // if it was not valid json
    
    // pedantic_error_checking
    if(
        read.imprecise_numbers() || // a number was very close to zero, or had many digits
        read.numbers_did_overflow() || // a number was really huge
        read.string_was_cut() || // a string was longer than 4294967295 characters (0xffffffff)
        read.object_name_was_cut() // an objects name was longer than 65535 characters (0xffff)
    )
        return false;
    
    // object name lengths are limited to 16 bits, and string lengths are limited to 32 bits. even on 64-bit.
    // this checks for the super unlikely event that a object name or string was cut to fit these limits
    if(read.string_was_cut() || read.object_name_was_cut())
        return false;
    
    auto nodes = read.nodes();
    // use the json::node tree
    auto world = nodes["object"]["nested_object"]["nested_nested_object"]["hello"];
    auto does_not_exist = nodes["does"]["not"]["exist"];
    return world.string() == "world" && !does_not_exist;
}


}}}}
#endif
