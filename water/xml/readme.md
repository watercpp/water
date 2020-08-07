# water::xml

XML DOM parser and generator.

`water::xml` should be able to parse any UTF-8, UTF-16 or UTF-32 XML text into a DOM tree structure. It can convert input from UTF-8/16/32 to UTF-8/16/32. It can also be used to create a DOM tree in memory from scratch. It should parse XML version 1.0 and 1.1 including unicode 0x2028 and NEL 0x85 linebreaks.

It is similar to `water::json`.

`water::xml` is made as a simple and robust way of reading and writing commonly used XML. It does not know anything about DTDs, schemas, XML namespaces or other advanced XML features.

## xml::memory

This is the memory allocator that owns DOM tree structures.

## xml::read

The class used to parse XML text into a DOM tree structure of xml::node objects.

## xml::write

This function writes a xml::node tree as XML text

## xml::node

This is the DOM tree structure nodes. xml::memory can create new ones, and xml::read
creates them from XML-text. All xml::node objects are tied to a xml::memory object,
and the underlying memory is deallocated when the xml::memory is destroyed.

The xml::node object points to something in the xml::memory, like an iterator or pointer. Using the functions on the node it is possible to traverse the node tree in all directions, get or set the name and value of the node, and insert or remove new nodes.

There is no need to use recursive functions or keep a stack of xml::node objects, it is always possible to traverse the node tree in all directions from a single xml::node object.


    <?xml version="1.0" encoding="UTF-8"?>
    <tag1>
        text1
        <tag2 attribute1="1" attribute2="2">
            text2
            <tag3/>
        </tag2>
        <tag4>text3</tag4>
        <!-- comment -->
        text4
    </tag1>

This XML text are turned into a xml::node tree like this:

- tag1, tag2, tag3, tag4 are nodes. The `name()` is the tag name like "tag1", their `value()` is blank.
- text1, text2, text3, text4 are nodes. The `name()` is blank, their `value()` is the text like "text1".
- attribute1 and attribute2 are nodes. They have both `name()` and `value()`, like name "attribute1" and the value "1".
- The comment is ignored, xml::read does not keep comments

So attribute nodes have a name and value, tags have only a name, and text content have only a value.

#### node.next() and node.previous()

Nodes at the same level are connected like a linked list with functions `next()` and `previous()` returning the next or previous xml::node. The text1, tag2, tag4, text4 are connected like that. So are text2 and tag3. Attributes also, attribute1 and attribute2.

#### node.in()

Nodes that are contained inside other nodes are also connected. The text1, tag2, tag4, text4 nodes are inside tag1. The `in()` function on any of these return tag1. Same with text2 or tag3, they are inside tag2 and `in()` on text2 returns tag2. Attributes are contained inside their tag, so `in()` on attribute1 or attribute2 returns tag2.

#### node.nodes() and node.attributes()

Nodes that contain other nodes return the first of the contained nodes with the `nodes()` function, and the first of the contained attributes with the `attributes()` function. So tag1 `nodes()` return text1, tag2 `attributes()` return attribute1.

#### node.find("something"), node.nodes("something") node.attributes("something")

Nodes at the same level can be searched by `name()` using the `find("something")` function. If node `a` is text1, using `auto b = a.find("tag4")` will result in node `b` pointing to tag4. The find function will start searching at the current position, so `auto c = b.find("tag4")` will result in `b == c`. To find the next result use next and find together `auto d = b.next().find("tag4")`. In this case there is no other node with name "tag4", so `d` will point to nowhere and convert to false.

The `nodes("something")` and `attributes("something")` functions are shortcuts to find nodes contained inside a node:
    
    xml::node<> tag1 = ...;
    
    auto a = tag1.nodes("tag4");
    auto b = tag1.nodes().find("tag4"); // same result
    assert(a == b && a.name() == "tag4");
    
    a = tag1.nodes("tag2").attributes("attribute2");
    b = tag1.nodes().find("tag2").attributes().find("attribute2"); // same result
    assert(a.value() == b.value() && a.value() == "2");

There are overloads of find, nodes and attributes that take begin and end iterators, begin + size or any obect with begin() end() like std::string.

#### first_value()

The `first_value()` function returns the first value-only node from the nodes contained in a node. This is the first text content inside a tag. For tag4 the `first_value()` returns the text3 node. For tag1 it returns the text1 node.


#### Mutable and constant nodes

A mutable node has functions to set the name and value, insert or remove nodes and even create new nodes. A constant node is a read-only node. A mutable node converts to a constant node. Its the same xml::node class only the mutable node has the memory template argument set:
    
    xml::memory<> memory;
    
    xml::node<char, xml::memory<>> mutalbe_node = memory.create();
    xml::node<char> constant_node = mutalbe_node;
    
    mutalbe_node.name("hello").first_value("world");

The resulting XML from mutalbe_node is `<hello>world</hello>`, the first_value will create a new node for the text content.

Look at the class in node.hpp and the example code in the examples folder for more.

## xml::encoding

If the source document is not in UTF-8, UTF-16 or UTF-32 it should be possible to detect
the encoding using the xml::encoding class. If something else is used to convert
it from that encoding to UTF-8/16/32 it will be possible to parse it using xml::read.


## Example

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


## Example, create document from scratch

    xml::memory<> memory;
    auto root = create(memory);
    root.name("root");
    auto tag = root.push_back().name("tag");
    root.push_back().value("world");
    tag.attributes(root.create().name("attribute").value("value"));
    tag.create().value("hello").insert_before(tag);

This is the XML:

    <?xml version="1.0" encoding="UTF-8"?>
    <root>
        hello  
        <tag attribute="value"/>
        world
    </root>


## Intended use

Use `water::xml` temporarily to convert XML text to and from another data structure.

It can sometimes be used as a long lived data structure if the program does not remove nodes or make big changes to the existing nodes:

Each `xml::node` knows how much space it has reserved for the name and value. So if the value is changed, it will not allocate any memory if it fits within the reserved space. If it does not fit, it will allocate more memory *without* freeing the old memory.

Removing a `xml::node` from the tree does not free any memory. The `xml::memory.clear()` function will clear *all* memory so it can be used again (destorying every existing `xml::node` in that memory object). Memory is freed only when `xml::memory` is destoryed.


## Threads

**IMPORTANT!**

xml::read will by default use lazy decoding of text content. Decoding is when things like `&amp;` is decoded to plain `&`. It means text content is decoded when its first accessed using the `node.value()` function, not when the XML text is parsed. That is more efficient if the xml::node tree is used from a single thread and the program does not access all content of the XML. But it means only a single thread can use a xml::memory object and the xml::node objects connected to it at a time. 

To change it, use `read.lazy(false)` to disable lazy decoding before reading the XML text.

*Only* with lazy decoding off, multiple threads can access xml::node objects from the same xml::memory object at the same time, as long as no thread is modifying anything.

xml::memory objects are not connected to each other in any way, so separate threads can do anything with their own xml::memory objects along with the connected xml::node objects at the same time.


## Links

- http://www.w3.org/TR/2006/REC-xml11-20060816/
- http://en.wikipedia.org/wiki/XML