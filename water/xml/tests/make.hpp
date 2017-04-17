// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_XML_TESTS_MAKE_HPP
#define WATER_XML_TESTS_MAKE_HPP
#include <water/xml/xml.hpp>
#include <water/str/str.hpp>
namespace water { namespace xml { namespace tests {

/*

<?xml version="1.0" encoding="UTF-8"?>
<root>
	<tag attribute="value with &lt;&gt;&apos;&quot; that must be encoded">
		changed first value
		<empty/>
		Text with &lt;&gt;&amp;'" characters that must be encoded. More text in separate node
	</tag>
	hello world!
</root>

*/

template<typename document_> void
 make(document_& document) {
	document.nodes(document.node().name("root"));
	auto tag = document.node().name("tag");
	document.nodes().nodes(tag);
	document.node().value("hello world!").insert_after(tag);
	tag.attributes(document.node().name("attribute").value("value with <>'\" that must be encoded"));
	tag.first_value("first value");
	tag.nodes(
		document.node().name("empty") <<
		document.node().value("Text with <>&'\" characters that must be encoded.") <<
		document.node().value("More text in separate node")
		);
	tag.first_value("changed first value");
	}

template<typename document_> void
 make2(document_& document) {
	// xml::document<char, water::allocator_nothrow> document;
	auto root = document.node(); // create a node
	document.nodes(root); // this is now the root node
	root.name("root");
	auto tag = document.node();
	tag.name("tag");
	auto text = document.node();
	text.value("hello");
	text.insert_after(tag);
	root.nodes(tag);
	auto attribute = document.node();
	attribute.name("attribute");
	attribute.value("value");
	tag.attributes(attribute);
	}
	
template<typename document_> void
 make3(document_& document) {
	// xml::document<char, water::allocator_nothrow> document;
	document.nodes(document.node().name("root"));
	auto tag = document.node().name("tag");
	document.node().value("hello").insert_after(tag);
	tag.attributes(document.node().name("attribute").value("value"));
	document.nodes().nodes(tag);
	}

template<typename o_> void
 make(str::out<o_>& o) {
	document<char, water::allocator_nothrow> dc;
	make(dc);
	o << "water::xml::tests::make char" << str::el;
	write_indented(write_to_str(o), dc);
	o << str::el;
	
	dc.clear();
	make2(dc);
	o << "water::xml::tests::make2 char" << str::el;
	write_indented(write_to_str(o), dc);
	o << str::el;
	
	dc.clear();
	make3(dc);
	o << "water::xml::tests::make2 char" << str::el;
	write_indented(write_to_str(o), dc);
	o << str::el;
	
	document<wchar_t, water::allocator_nothrow> dw;
	make(dw);
	o << "water::xml::tests::make wchar_t" << str::el;
	write_indented(write_to_str(o), dw);
	o << str::el;
	}

}}}
#endif
