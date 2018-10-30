// Copyright 2018 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_JSON_EXAMPLES_VALUES_HPP
#define WATER_JSON_EXAMPLES_VALUES_HPP
#include <water/json/json.hpp>
namespace water { namespace json { namespace examples { namespace values {

/*

Examples of how to read booleans, numbers and strings from nodes and how to determine the value type of a node.

*/

char constexpr text[] = u8R"###({
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
		0,
		1,
		2,
		3
	]
})###";

template<typename ...a_> inline void unused(a_ const&...) {}


inline bool nulls(json::node<> root) {
	
	bool is_null = root["null"].is_null();
	if(!is_null)
		return false;
	if(root["object"].is_null())
		return false;
	
	return true;
	}


inline bool booleans(json::node<> root) {
	bool boolean = root["bool_false"].boolean();
	if(boolean)
		return false;
	boolean = root["bool_true"].boolean();
	if(!boolean)
		return false;
	
	// nodes that are not booleans will return false (numbers are not casted to bool)
	if(root["integer"].boolean())
		return false;
	
	// is_boolean() will return true if the value is a boolean
	if(!root["bool_false"].is_boolean())
		return false;
	
	return true;
	}


inline bool numbers(json::node<> root) {
	// json::number is a class with a signed int64 integer value and an int32 exponent
	json::number
		integer = root["integer"].number(),
		floatingpoint = root["floatingpoint"].number();
	if(integer.integer() != 123 || integer.exponent() != 0)
		return false;
	// -1.23e-45 is represented as -123e-47 in the number object
	if(floatingpoint.integer() != -123 || floatingpoint.exponent() != -47)
		return false;
	
	// static cast them to integer or floating point types
	json::int64_t int64_value = integer.to_int();
	double double_value = floatingpoint.to_double();
	
	// if the number could not be represented by a int64_t or double it will be 0 (see json::number)
	// for security it can be worth checking that the conversion was successful
	if(!int64_value && integer)
		return false;
	if(!double_value && integer)
		return false;
	
	// number will static_cast to any integer or floatingpoint type, but only int64 and double
	// are fully checked against overflow, so use only int64 and double when security matters
	int int_value = static_cast<int>(integer);
	float float_value = static_cast<float>(floatingpoint);
	unused(int_value, float_value);
	
	// if json::number was not able to represent the number from the JSON text imprecise() will return true
	// it could be because the number was huge, then overflow() also returns true
	// it can be that the number was very close to 0, and was truncated to 0
	// or the most likely: that it had more digits than the integer() can represent so it was rounded
	if(floatingpoint.imprecise()) {
		if(floatingpoint.overflow())
			return false;
		}
	
	// nodes that are not numbers will return the number zero (booleans are not casted to number)
	if(root["string"].number())
		return false;
	if(root["string"].number().integer() || root["string"].number().exponent())
		return false;
	
	// is_number() is true when it is a number
	if(!root["integer"].is_number())
		return false;
	
	return true;
	}


inline bool strings(json::node<> root) {
	// the string type is a simple range with begin() and end() iterators.
	// the iterators point to memory in the json::memory object, the string does not do any memory allocation
	// and its cheap to copy.
	json::string<char const*> string = root["string"].string();
	
	// non-empty string converts to true
	if(!string)
		return false;
	
	// begin and end return iterators (they are char const*)
	json::string<char const*>::iterator
		begin = string.begin(),
		end = string.end();
	while(begin != end)
		++begin;
		
	// size return the size
	size_t size = string.size();
	for(char c : string) {
		--size;
		unused(c);
		}
	if(size)
		return false;
	
	// the same string type is used for node names
	json::string<char const*> name = root["string"].name();
	if(name != "string")
		return false;
	
	// a node that is not a string returns a empty string 
	if(root["integer"].string())
		return false;
	
	return true;
	}


inline bool type_switching(json::node<> root) {
	
	// the json::type enum and node.type() function makes it possible to do type switching
	bool any = false;
	switch(root["array_of_numbers"].type()) {
		case json::type::null:
		case json::type::array:
		case json::type::object:
		case json::type::string:
		case json::type::number:
		case json::type::boolean:
			any = true;
			break;
		}
	if(!any)
		return false; // this is impossible
	
	// the is_something functions
	if(!root["null"].is_null())
		return false;
	if(!root["bool_false"].is_boolean())
		return false;
	if(!root["floatingpoint"].is_number())
		return false;
	if(!root["string"].is_string())
		return false;
	if(!root["object"].is_object())
		return false;
	if(!root["array_of_numbers"].is_array())
		return false;
	
	// a node that points to nowhere is null
	json::node<> nothing;
	if(nothing.type() != json::type::null)
		return false;
	if(!nothing.is_null())
		return false;
	// but the node itself will convert to false
	if(nothing)
		return false;
	// a node that points somewhere converts to true even if the value is null
	if(!root["null"])
		return false;
	
	return true;
	}	


inline bool example() {
	json::memory<> memory;
	json::node<> root = read_to_memory(memory)(text, sizeof(text) - 1).nodes();
	return
		nulls(root) &&
		booleans(root) &&
		numbers(root) &&
		strings(root) &&
		type_switching(root);
	}


}}}}
#endif
