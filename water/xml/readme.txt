XML DOM parser and generator.

http://www.w3.org/TR/2006/REC-xml11-20060816/
http://en.wikipedia.org/wiki/XML

xml::read
This should be able to parse any UTF-8, UTF-16 or UTF-32 XML text into a DOM
tree structure. It can convert input from UTF-8/16/32 to UTF-8/16/32.
It can also be used to create a DOM tree in memory from scratch. It should parse
XML version 1.0 and 1.1 including unicode 0x2028 and NEL 0x85 linebreaks.

xml::memory
This is the memory allocator that owns DOM tree structures.

xml::node
This is the DOM tree structure nodes. xml::memory can create new ones, and xml::read
creates them from XML-text. All xml::node objects are tied to a xml::memory object,
and the underlying memory is deallocated when the xml::memory is destroyed.

xml::write
This function writes a xml::node tree as XML text

xml::encoding
If the source document is not in UTF-8, UTF-16 or UTF-32 it should be possible to detect
the encoding using the xml::encoding class. If something else is used to convert
it to UTF-8/16/32 it can be parsed by xml::read.

Example:

  size_t file_size_in_bytes = ...;
  xml::memory<> memory;
  void *buffer = memory.allocate(file_size_in_bytes);
  read_the_whole_file_into_memory(buffer, file_size_in_bytes, file);
  auto nodes = read_to_memory(memory).parse_in_place(buffer, file_size_in_bytes).nodes();
  if(nodes)
      out << "parse success!!!\n";

  write(out, nodes); // output xml text to a water::str::out

  memory.clear(); // free all memory allocated with this memory object, including all nodes

Here the file is read into memory allocated by xml::memory, then read_to_memory is used
to create a xml::read parser that reads to the xml::memory object.
Then xml::read::parse_in_place is used to, when possible, re-use the buffer read from
the file and avoid extra allocations.

Example, create document from scratch:

  xml::memory<> memory;
  auto root = create(memory);
  root.name("root");
  auto tag = root.push_back().name("tag");
  root.push_back().value("world");
  tag.attributes(root.create().name("attribute").value("value"));
  tag.create().value("hello").insert_before(tag);

This is the XML: \code

  <?xml version="1.0" encoding="UTF-8"?>
  <root>
     hello  
     <tag attribute="value"/>
     world
  </root>
