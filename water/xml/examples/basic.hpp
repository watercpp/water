// Copyright 2018 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_XML_EXAMPLES_BASIC_HPP
#define WATER_XML_EXAMPLES_BASIC_HPP
#include <water/xml/xml.hpp>
#include <water/vector.hpp>
#include <water/numbers/numbers.hpp>
namespace water { namespace xml { namespace examples { namespace basic {

/*

Basic example of reading a list of locations from XML into a simple data structure,
and writing that data structure as XML text.

This example does not do any validation of the XML input. A real program would check
if the latitude and longitude values are valid, check if the XML was missing any
required data or contained any unrecognized data. And not use vectors like this.

*/

char constexpr text[] = u8R"###(<?xml version="1.1" encoding="UTF-8"?>

	<locations>
		<location latitude="90" longitude="0">
			<name>North Pole</name>
			<animal>Polar bear</animal>
			<animal>Ringed seal</animal>
		</location>
		<location latitude="-90" longitude="0">
			<name>South Pole</name>
			<animal>Penguin</animal>
		</location>
		<location latitude="17.7500" longitude="142.5000">
			<name>Mariana Trench</name>
			<animal>Giant squid</animal>
		</location>
		<location latitude="27.986065" longitude="86.922623">
			<name>Mount Everest</name>
			<animal>Snow Leopard</animal>
			<animal>Alpine chough</animal>
		</location>
		<location latitude="31.5590" longitude="35.4732">
			<name>Dead Sea</name>
		</location>
	</locations>

)###";



struct location {
	vector<char> name;
	vector<vector<char>> animals;
	double latitude = 0;
	double longitude = 0;
	};

template<typename text_> double number_from_text(text_ const& from);
inline vector<char> text_from_number(double);



inline vector<location> read_from_xml_text(char const* text_begin, char const* text_end) {
	xml::memory<> memory;
	auto root = read_to_memory(memory)(text_begin, text_end).nodes();
	
	vector<location> locations;
	if(root.name() == "locations") {
		auto from = root.nodes();
		while((from = from.find("location"))) {
			location to;
			auto name = from.nodes("name").first_value();
			to.name.assign(name.begin(), name.end());
			
			to.latitude = number_from_text(from.attributes("latitude").value());
			to.longitude = number_from_text(from.attributes("longitude").value());
			
			auto animal = from.nodes("animal");
			while(animal) {
				to.animals.emplace_back(animal.first_value().begin(), animal.first_value().end());
				animal = animal.next().find("animal");
				}
				
			locations.push_back(static_cast<location&&>(to));
			
			from = from.next();
			}
		}
	return locations;
	}


	
inline vector<char> write_as_xml_text(vector<location> const& locations) {
	xml::memory<> memory;
	auto root = memory.create().name("locations");
	
	for(auto const& from : locations) {
		auto to = root.push_back().name("location");
		to.push_back().name("name").first_value(from.name);
		to.attributes_or_add("latitude").value(text_from_number(from.latitude));
		to.attributes_or_add("longitude").value(text_from_number(from.longitude));
		for(auto const& animal : from.animals)
			to.push_back().name("animal").first_value(animal);
		}
	
	vector<char> text;
	write(
		[&text](char const* begin, char const* end) {
			text.insert(text.end(), begin, end);
			},
		root
		);
	return text;
	}



inline vector<char> example() {
	auto read = read_from_xml_text(text, text + sizeof(text) - 1);
	if(read.empty())
		return {};
	return write_as_xml_text(read);
	}



template<typename text_> double number_from_text(text_ const& from) {
	numbers::read<double> read;
	read(from.begin(), from.end());
	return read;
	}



inline vector<char> text_from_number(double from) {
	numbers::write<double> write{from};
	vector<char> to{write.template size<char>()};
	write(to.begin(), to.end());
	return to;
	}

}}}}
#endif
