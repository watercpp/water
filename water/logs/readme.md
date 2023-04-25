# water::logs

If a program does extensive logging, for example during debugging and testing, and the logging
slows the program down or makes otherwise unrelated threads wait for each other at the
logging-function, `water::logs` could be the solution.

`water::logs` is a highly concurrent buffered log.

`#include <water/logs/logs.hpp>`

Threads write their logs to a `water::logs::buffer` or a `water::logs::buffer_forever`, and then
one background thread will write the buffer to the actual logging destination.

`buffer` and `buffer_forever` have 3 template arguments:

    template<typename output_ = void, typename tag_ = void, bool memory_statistics_ = false>
    class buffer

- **output_** is the class that writes the log somewhere. The default `void` will use 
  `logs::output_to_trace` that writes to `water::trace`.

- **tag_** is a tag used to tag log messages. The default `void` uses `logs::tag_with_nothing`.
  There is also a `logs::tag_with_thread` for tagging with the thread id. The tag is just passed
  through the buffer to the output, and the output class is responsible for doing something with the tag.

- **memory_statistics_** enables memory statistics from the `water::fixed` memory allocator, useful
  for tuning memory use.

The log is written in UTF-8. You do not need to end each message with a line break, it is added
automatically.

    std::string s("hello");
    
    water::logs::buffer<> buffer;
    
    buffer("hello");
    buffer(s); // antyhing with begin() and end() works, not just std::string
    buffer(s.begin(), s.end());
    buffer(s.begin(), s.size());
    
    using str_type = water::str::out< water::logs::str_to_buffer< water::logs::buffer<> > >;
    
    str_type str{buffer};
    str << "hello" << u"UTF-16";
    
    water::xtr::to_function(buffer.write()) << "hello" << u"UTF-16";
    
With a tag and output class:

    class tag {...};
    class output {...};
    output o;
    std::string s("hello");
        
    water::logs::buffer<output, tag> buffer{o};
    
    buffer(tag{}, "hello");
    buffer(tag{}, s);
    buffer(tag{}, s.begin(), s.end());
    buffer(tag{}, s.begin(), s.size());
    
    using str_type = water::str::out< water::logs::str_to_buffer< water::logs::buffer<output, tag> > >;
    
    str_type str{buffer, tag{}};
    str << "hello" << u"UTF-16";
    
    water::xtr::to_function(buffer.write(tag{})) << "hello" << u"UTF-16";

The `buffer.write()` that is used with `xtr::to_function` above returns a light-weight
`logs::write_to_buffer` function object that can be moved or swapped (but not copied). The `buffer`
cannot be moved or copied.

When writing log messages to a `buffer` or `buffer_forever` it will *not* convert a message from
UTF-16 or UTF-32 to its own UTF-8. It expects everything to be UTF-8. Use `water::str` or
`water::xtr` to do UTF conversion. Look at `u"UTF-16"` in the code above.



## Example

Look at `water/logs/examples/global.hpp` and `water/logs/examples/global.cpp` for an example of a
global log object that can be used for the entire lifetime of the program. It writes to
`water::trace` and a file.

And it has examples of `water::str` and `water::xtr` that are easier to use.



## How it works

The `buffer` or `buffer_forever` uses `water::fixed`, a mostly lock free fixed-size memory
allocator. (The allocator uses a mutex only when it needs to allocate more memory from an underlying
allocator. That should happen only a few times. Maybe just once.)

The allocator is used to allocate `logs::piece` objects. A `piece` can hold a fixed number of
characters plus the tag, and many pieces can be linked together in a list.

Writing text to the `buffer` is done by allocating one or more `piece` objects from the `buffer`,
then writing the text to these pieces.

After that the pieces are passed back to the `buffer`. This last step adds them to a lock free
linked list.



## logs::buffer or logs::buffer_forever

These classes do the exact same thing, except that `buffer_forever` is meant to be used as a global
object that lives for the entire lifetime of the program. It can be used during global construction
and destruction. To make this possible, it will intentionally not destroy itself, it relies on the
system to reclaim all memory and resources when the process ends. It might look like a memory leak
to a memory leak detection tool.

You can pass an output object to the `buffer` constructor, but cannot to `buffer_forever`. The
output class needs to configure itself without any constructor-arguments for `buffer_forever`.

Also, `buffer` will start in *concurrent mode* by default, while `buffer_forever` will not.

A `buffer_forever` combined with a `flush_thread` in `main` will enable *concurrent mode* and flush
the buffer during `main`. Outside of `main` it will write directly to the output destination:

    #include <water/logs/logs.hpp>
    
    water::logs::buffer_forever<> log;
    
    void main() {
        water::logs::flush_thread flush{log};
        do_what_the_program_does();
    }



## Concurrent mode

The `buffer.concurrent(bool)` function switches concurrent mode on or off. In concurrent mode the
`buffer` must be flushed manually by calling `buffer.flush()`. When concurrent mode is off, each
write to the buffer will also flush:

    water::logs::buffer<> log;
    
    log.concurrent(true);
    
    log("hello"); // this does not flush
    log.flush();
    
    log.concurrent(false);
    
    log("world"); // this does flush

Concurrent mode is the whole point of using `water::logs`, when a background thread flushes the
buffer so the other threads don't have to. It is useful to switch concurrent mode off only when the
background thread is not running, usually before and after `main`. So the log gets written even
before and after the thread runs.



## logs::flush_thread

This is just a background thread that calls `buffer.flush()` a few times per second. It also
enables the buffers concurrent mode while the thread is running, and disables it after.



## Output class

The output class writes the log to some destination. And it can also prefix each line with a
prefix, usually with data from the tag. An output class should look like this:

    class output
    {
    public:
        char* prefix(char* begin, char *end, tag_type tag);
        void line(char const* begin, char const* end);
        void start();
        void stop();
    };

`buffer.flush()` uses the output class. The functions are always called while `buffer` holds a
mutex. So the output class does not need a mutex itself.

- `void start()`
    - Called when flush starts. It can be used for per-flush setup or to do one-time initialization
      of something like opening a file.

- `char* prefix(char* begin, char *end, tag_type tag)`
    - Write a line prefix to begin,end, return end of what was written.
    - For example if the tag has a thread-id or timestamp, write that as the prefix. 

- `void line(char const* begin, char const* end)`
    - Write a line to the output destination.
    - begin < end always
    - end[-1] == line break always
    - end[0] == 0 always (it's a zero-terminated C string)
    - If the tag is needed here, store it in `prefix` because it will be called before `line`.

- `stop()`
    - Called when flush is done, can be used to flush the output destination.
    - Called even if an exception is thrown after `start()`.
    - Not called when `start()` throws



## Tuning memory use

The `buffer` and `buffer_forever` constructors have:
    
    template<typename output_ = void, typename tag_ = void, bool memory_statistics_ = false>
    class buffer {
        buffer(size_t piece_size = 0, size_t memory_block_size = 0, bool concurrent = true)
    
Use `piece_size` to change how many characters each piece can hold. A larger `piece_size` will use
more memory, but each log write will allocate fewer pieces and that might be faster.

Use `memory_block_size` to change the `water::fixed` memory block size, in number of pieces. Fewer
and larger blocks should make allocation faster, but if the blocks are too large it will just waste
memory.

To get statistics of how memory is used, call the `buffer.statistics()` and
`buffer.memory_statistics()` functions at the end of the logs lifetime and examine the data. And
set the `memory_statistics_` template argument to `true`.