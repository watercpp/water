XML DOM parser and generator.

http://www.w3.org/TR/2006/REC-xml11-20060816/
http://en.wikipedia.org/wiki/XML

xml::document
This should be able to parse any UTF-8 or UTF-16 xml document into a DOM tree
structure. It can convert input from UTF-8 to UTF-16 or from UTF-16 to UTF-8.
It can also be used to create a DOM tree in memory from scratch. It should parse
XML version 1.0 and 1.1 including unicode 0x2028 and NEL 0x85 linebreaks.

xml::node
This is the DOM tree structure nodes. xml::document provides access to these,
and can also create new ones. All xml::node objects are tied to a xml::document

xml::write_indented
This function writes an xml::document or xml::node structure as XML text

xml::encoding
If the source document is not in UTF-8 or UTF-16 it should be possible to detect
the encoding using the xml::encoding class. If something else is used to convert
it to UTf-8 or UTF-16 it can be parsed by xml::document.

Example:

  size_t file_size_in_bytes = ...;
  xml::document<char, water::allocator_nothrow> document;
  void *buffer = document.allocate(file_bytes);
  read_the_whole_file_into_memory(buffer, file_size_in_bytes, file);
  auto nodes = document.parse_in_place(buffer, file_size_in_bytes);
  if(nodes)
    out << "parse success!!!\n";

  write_indented(xml::write_to_str(out), nodes); // output xml text to a water::str::out

  document.clear(); // free all memory allocated from document, including all nodes


Example, create document from scratch:

  xml::document<char, water::allocator_nothrow> document;
  document.nodes(document.node().name("root"));
  auto tag = document.node().name("tag");
  document.node().value("hello").insert_after(tag);
  tag.attributes(document.node().name("attribute").value("value"));
  document.nodes().nodes(tag);

  bool error = document.allocate_has_failed(); // the only reason the above could have failed

This is the XML: \code

  <?xml version="1.0" encoding="UTF-8"?>
  <root>
     <tag attribute="value"/>
     hello
  </root>

