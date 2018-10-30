// Copyright 2018 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_JSON_EXAMPLES_CREATING_NODES_2_HPP
#define WATER_JSON_EXAMPLES_CREATING_NODES_2_HPP
#include <water/json/json.hpp>
#include <water/vector.hpp>
namespace water { namespace json { namespace examples { namespace creating_nodes_2 {

/*

This creates the same JSON text as the creating_nodes_1 example.
It does it in a overcomplicated way to illustrate different ways of creating the node tree.

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

*/

inline vector<char> example() {
	json::memory<> memory;
	
	auto root = memory.create().object(8);
	
	// this adds the "integer" and "floatingpoint" nodes by reading them from JSON text.
	// this is totally fine as long as the same memory object is used for all nodes.
	// it is possible to combine parts from different JSON documents without copying. 
	char const numbers_text[] = "{\"integer\":123, \"floatingpoint\": -1.23e-45}";
	auto numbers = read_to_memory(memory)(numbers_text, sizeof(numbers_text) - 1).nodes();
	root.push_back(numbers["integer"]);
	root.push_back(numbers["floatingpoint"]);
	// the nodes are moved, not copied, so numbers is an empty object now
	if(numbers.size())
		return {};
	
	// numbers are set using the json::number class, it has convertion constructors for all
	// integer and floatingpoint types. these 3 lines are doing the same thing:
	root["floatingpoint"].number(-1.23e-45);
	root["floatingpoint"].number(json::number{-1.23e-45});
	root["floatingpoint"].number(json::number{-123, -47});
	
	// doing something wrong will return a node pointing to nowhere, that converts to false
	json::node<json::memory<>>
		nothing, // this node points to nowhere
		result_of_wrongdoing = root.push_back(nothing); // this is wrong, cannot push back node to nowhere
	if(result_of_wrongdoing)
		return {}; // impossible
	if(nothing != result_of_wrongdoing)
		return {}; // impossible
	
	// operations on nodes pointing to nowhere do nothing except return a node that converts to nowhere.
	// they do not crash or throw exceptions. this makes it possible to 
	result_of_wrongdoing = nothing.push_back().object(123).insert(0).name("hello").string("world");
	if(result_of_wrongdoing)
		return {}; // impossible
	
	
	root.push_front().name("null").null();
	root.insert(1).name("bool_false").boolean(false);
	root.insert(2, memory.create().name("bool_true").boolean(true));
	
	// string() and name() has overloads with iterators and iterator + size
	// this adds "string": "hello world"
	char const
		*name_begin = "string",
		*name_end = name_begin + sizeof("string") - 1,
		*value_begin = "hello_world";
	size_t value_size = sizeof("hello_world") - 1;
	root.push_back().name(name_begin, name_end).string(value_begin, value_size);
	
	// this adds the "object": {"hello": "world"} plus adds and removes a node
	auto object = root.push_back().name("object").object(1);
	object.push_back().name("to_be_removed").array(0);
	object.push_back().name("hello").string("world");
	auto removed = object.pop_front();
	if(removed.name() != "to_be_removed")
		return {};

	
	auto array_of_numbers = memory.create().array(0);
	while(array_of_numbers.size() != 4)
		array_of_numbers.push_back().number(array_of_numbers.size());
	
	array_of_numbers.name("array_of_numbers");
	root.insert(0, array_of_numbers);
	
	// remove it
	removed = root.remove(0);
	if(removed != array_of_numbers)
		return {};
	
	// insert it again, this time using push_back
	root.push_back(array_of_numbers);
	
	// insert it again, this time using push_front. it will remove itself first
	root.push_front(array_of_numbers);

	// remove using "remove myself" function on the array_of_numbers object
	array_of_numbers.remove();
	
	// add it again
	root.push_back(array_of_numbers);
	
	
	// write as indented JSON text to a vector
	vector<char> result;
	write(indent_to([&result](char a) { result.push_back(a); }), root);
	return result;
	}

}}}}
#endif
