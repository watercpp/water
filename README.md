# Water C++ Library

A open source C++ library of building blocks that could be useful in any C++ program. It is licensed
under the MIT License.

- Main page https://watercpp.com
- Source code https://github.com/watercpp/water

This is a header only library, made up almost entirely of C++ templates. It contains various
components, building blocks, that can be used together or independently. They share common design
patterns that should make them portable, robust and efficient. Beware that example code and
documentation is lacking at this point.



# Getting Started

Get the source code from github.com/watercpp/water and add it to your compilers include path so that
`#include <water/water.hpp>` works. The library is header only so that is enough.

Test program:

    #include <water/trace.hpp>
    
    int main(int argc, char **argv) {
        water::trace << "Hello world " << 1234 << '!';
        return 0;
    }

The entire library is in the water namespace. Larger components have their own subnamespace, smaller
are declared directly in the water namespace. Header files are structured like the namespaces:
Everything in the water folder is in the water namespace, and subfolders correspond to subnamespaces
with the same name.

Documentation is sometimes in the header files themselves, sometimes in readme files next to the
header files. Many components do not have documentation.

Example code, if it exists, is in a examples folder next to the header files.

Tests are next to the header files in a folder called tests.



# Dependencies and Portability

The code should compile on any compiler supporting at least the 2014 C++ standard.

Water depends on the standard C library, and it uses the standard C++ library if it exists. You can
configure it to work without the C++ standard library, exceptions, or RTTI.

A few parts depend on operating system specific functionality. These should work on Android, iOS,
Linux, macOS, Windows and any posix compatible operating system.

In theory it should build on any system, even where bytes are not 8 bits. In reality it is being
used for projects on Android, iOS, macOS and Windows and tested on Linux. It is being compiled with
recent versions of Clang, GCC, Intel C++ and Visual C++.



# Changelog

The CHANGELOG.md should be updated when major or breaking changes are made to the code. The changes
are ordered by date, and a tag is set on the code with the same date.

Small changes and bugfixes will usually not be added to the changelog. These changes should not
break existing code, and hopefully the code commits explain what was done.

The CHANGELOG.md does not go back very far in time, it was added recently.



# Components

Some of the included components are:


### water::numbers

Number-to-string and string-to-number conversion with unicode support and advanced formatting. Can
use any unicode strings or codepoints as digits and symbols. Namespace in `water/numbers`.


### water::str

String formatting using << operators like iostreams but with unicode support. Uses water::numbers
for number formatting. Creating your own destination where the string is written is a lot simpler
than with iostreams. Automatically converts input from UTF-8, UTF-16, UTF-32 to whatever UTF the
destination requires. Namespace in `water/str`.


### water::xtr

Similar to water::str but implemented with minimal dependencies using expression templates to
generate the string on the stack. The library itself uses it when WATER_DEBUG is defined to write
output to the debugger, see water::trace. Namespace in `water/xtr`, water::trace in `water/trace.hpp`.


### water::threads

Portable thread, mutex, condition variable, read write lock, semaphore, barrier and call once. An
unusual feature is statistics for mutexes, read write locks, semaphores and barriers. You can use it
to see how frequently threads block on a particular mutex, to find concurrency bottlenecks.
Namespace in `water/threads`.


### water::logs

Concurrent logging classes. Use this with water::str for a highly concurrent alternative to
std::cout. Multiple threads write text to the logging-buffer with minimal synchronization.
Implemented using a lock free queue and mostly lock free memory allocator to prevent threads from
blocking. Namespace in `water/logs`.


### water::fixed::memory_atomic

Mostly lock-free fixed size memory allocator useful for lock free data structures. Used by
water::logs::buffer. In `water/fixed/memory_atomic.hpp`.


### water::memory_track

Dynamic memory allocation tracking for portable error checking and statistics. Useful during
development and testing to gain insight in how a program uses dynamic memory and to find bugs.
Namespace in `water/memory_track`.


### water::temporary

Tools for using temporary memory. If you repeatedly need dynamic memory for short term allocations
but want to avoid the global dynamic memory allocator. Namespace in `water/temporary`.


### water::unicode

Unicode UTF conversion that is strict, robust and easy to use. Namespace in `water/unicode`.


### water::json

JSON parser and generator with robust parsing and efficient memory use. Namespace in `water/json`.


### water::xml

XML DOM parser and generator with robust parsing and efficient memory use. Namespace in `water/xml`.


### water::later

Trivial but useful way of running a lambda or other function object later, when the water::later
object goes out of scope. In `water/later.hpp`.


### water/types.hpp and water::types

Template meta programming primitives.

`water/types.hpp` have fundamental basic things that are used all over the library. Documented in
`water/types.md`.

The `water::types` namespace contains things for more complex metaprogramming. If you are interested
in the topic, this is probably worth a look. Namespace and `readme.md` in `water/types`.


### water/int.hpp

Templates to select integer types with specific number of bits, bytes or digits. In `water/int.hpp`.


### water::base64

Simple base 64 implementation. Namespace in `water/base64`.


### water/url_encode_decode.hpp

Simple URL encoding and decoding. In `water/url_encode_decode.hpp`.


---

There is more, but you need to explore the header files to see what is actually there.