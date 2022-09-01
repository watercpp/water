# water::json

The `water::json` namespace contains classes and functions to read and write JSON, based on a tree structure that makes it easy to work with and create JSON format data. It is similar to `water::xml`.

Parsing is strict, and should detect any errors in the JSON data. You can check if the parsing of numbers caused overflow or underflow, or when the full precision of a floating point number could not be preserved.

The `node` class is the tree structure. It is a kind of linked list, where you can move not only to the next and previous node but also to the parent and child nodes. This means you never have to use recursive functions when working with the tree structure, and the parser is not recursive.

It can write only UTF-8 or ASCII format JSON, but it can read UTF-32, UTF-16 and UTF-8.


## Example code

The water/json/examples folder contains example code.


## json::read

Use the `read` class to parse JSON text into a structure of `node` objects. All `node` objects belong to a `memory` object, that `memory` object must exist for as long as the nodes are used. The `read_to_memory` function creates a `read` object that uses a `memory` object.

    std::vector<char> input = download_json_as_text_from_the_internet();
    
    water::json::memory<> memory;
    auto read = read_to_memory(memory);
    read(input.begin(), input.tend());
    if(!read)
        trace() << "not valid JSON";
    auto nodes = read.nodes();

The `read` parser can parse memory in place. This is the most memory-efficient way to use it. In the example above the data from the input vector is copied. Below, a file is read into a buffer allocated from the `memory` object, then `read.parse_in_place` is used to parse that memory. This will modify the contents of the buffer.

    auto file = open_file_with_json_text();
    size_t size = file.size();
    
    water::json::memory<> memory;
    void *buffer = memory.allocate(size);
    file.read(buffer, size);
    auto read = read_to_memory(memory);
    read.parse_in_place(buffer, size);
    if(!read)
        trace() << "not valid JSON";
    auto nodes = read.nodes();

The `memory` object will free all memory when its destroyed. After that, nodes that belong to the memory cannot be used.

    
## json::write

Use the `write` function to write the `node` structure as JSON text

    std::vector<char> output;
    
    write(
        [&output] (char const* begin, char const* end) {
            output.insert(output.end(), begin, end);
        },
        nodes
    );

It is possible to find out the size `write` will write using the `write_size` function. This makes it possible to allocate the space needed to write the JSON data before writing:

    size_t size = write_size(nodes);
    output.reserve(size);
    
    write(
        [&output] (char const* begin, char const* end) {
            output.insert(output.end(), begin, end);
        },
        nodes,
        escape::all // see below
    );


#### Writing UTF-8 or ASCII

The `write` function has a third argument, it can be `json::escape::all` or `json::escape::minimal`. It is `minimal` by default, meaning it will escape only what it must, resulting in UTF-8 output. If `all`, it will escape all special characters resulting in ASCII output. (It will always escape </ and 0x2028 and 0x2029, the JSON is always safe to use inside HTML or JavaScript)


## json::indent

The `indent` class can format and indent JSON text to make it easier to read. The `write` function does not indent the JSON text, it will always write a single line. The `indent_to` function creates an `indent` object.

    std::vector<char> input = download_json_as_text_from_the_internet();
    water::str::out_stdout output;
    
    auto indent = water::json::indent_to(output, 4); // 4 means indent with 4 spaces per tab
    indent(input.begin(), input.end());

The thing indent writes to must behave like a function taking a single char like `void function(char a)`. A `water::str::out` does that. It always writes a single char at a time, you might want to buffer the output.

Indent `node` objects by combining `write` and `indent`:

    water::json::node<> nodes = ...;
    water::str::out_stdout output;
    
    auto indent = water::json::indent_to(output, `\t`); // indent with tab
    write(indent, nodes);
    

## json::memory

The `memory` memory allocator works exactly like the `water::xml::memory` allocator, it uses the same code. It allocates larger blocks of memory from an underlying allocator (`water::allocator` by default) and then makes smaller allocations inside those chunks. It is compact and efficient, but also dumb: Its not possible to free a single allocation, you can only clear the entire memory.

    water::json::memory<water::allocator_nothrow> memory;
    
    water::json::node<water::json::memory<water::allocator_nothrow>> node1 = memory.create();
    auto node2 = memory.create();

    memory.clear(); // do not use node1 and node2 after this!


## json::node

`node` objects are lightweight. Similar to pointers or iterators they only point to somewhere in the tree structure. You can copy them by value. No data is destroyed when they go out of scope. The data they point to is owned by the `memory` object.

Unlike pointers or iterators, they contain many functions to access, insert and modify the tree structure. Like functions to create new nodes.

Look at the example code for more details.

## json::number

`number` is the type used to represent numbers. It uses a 64-bit integer mantissa plus a 32-bit integer exponent. It converts to and from all built in integer and floating point types, so you don't have to care about this if you don't want to.

64-bit signed integer values are preserved, and the precision and range is greater than a 64-bit floating point type.

Since it does not use a floating point type, rounding errors with floating point math is avoided when parsing from text and writing it back to text. But if you convert between a `number` and a floating point type, it uses floating point math.

The `imprecise()` and `overflow()` functions can be used to check if the number was truncated when parsed. Even if that did not happen, it might not be possible to covert to a integer or floatingpoint type without overflow/underflow. This can aslo be detected, the `to_int()` and `to_double()` functions return 0 if it happens.

## Intended use

Use `water::json` temporarily to convert JSON text to and from another data structure.

If the data does not change at all, it is fine to keep and use the `json::node` structure for as long as needed in the program. But do not use it as a long lived data structure in a program for something that chages. That will waste memory.

## Threads

Connected objects should be modified from a single thread at a time. You can read data from multiple threads at the same time, as long as no thread modifies any data. For example, a `node` is connected to a `memory` object.

Independent objects can be used from separate threads. For example two different `memory` objects, or different `node` objects that are connected to different `memory` objects.