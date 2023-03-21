# water::str

*str* is short for *string* or *stream*.

`water::str::out` has << operators to format things into a string, like `std::ostream` or `water::xtr`.

    str::out<base_class> out;
    out << "hello world " << 123 << ' ' << str::base(16) << 1.23e4 << '\n';
    out << u"line " << U'2' << L'!' << str::el;

This example will write "hello world 123 0x1.806p13", a newline, then "line 2!" and a newline. The
double is written as hexadecimal because of the str::base(16) before it.

Numbers are formatted using `water::numbers::write` and the `water::numbers::settings` class is used to
control how number formatting works. All `str::out` objects hold a `numbers::settings`.

`str::el` is like `std::endl`: It writes a newline and then "flushes". This is useful if the str::out
writes to a base_class that is buffered. Otherwise there is no need to flush or use `str::el`.

Strings are converted to the UTF encoding of the base_class class, from UTF-8, UTF-16 or UTF-32.

Types with begin() and end() functions are seen as strings. This means `std::string` and `std::vector`
are seen as strings:

    str::out<base_class> o;
    std::string s{"la"};
    std::u16string u{u"la"};
    o << s << u;

Char pointers are seen as C-strings, if they are of type `char`, `char8_t`, `char16_t`, `char32_t` or
`wchar_t`. But to reduce the risk of buffer overruns there are separate << operators for char arrays,
and classes that implicitly convert to a char-pointer will not bee seen as C-strings at all.


#### Example: Writing to stdout

For simply writing to `stdout`, use `out_stdout` from `water/str/out_stdout.hpp`

    str::out_stdout{} << "hello";
    str::out_stdout o;
    o << U"world";
    o << 123;

This will write:
    
    hello
    world123

This class will automatically add a newline when the string is written to `stdout`, unless the string
already ends in a newline. And it buffers output. If multiple threads write to `stdout` using it, each
thread will write complete lines every time so the output from different threads wont mix. (Provided
that writing to stdout from different threads at the same time is possible, it should be). Just use
separate `out_stdout` objects in different threads.


#### Example: Writing to a char array

Use `str::to_begin_end(begin, end)` to write to a pair of iterators, or `str::to_begin_end(array)`
to write to an array:

    char16_t array[123];
    auto out = str::to_begin_end(array);
    out << "hello" << 123.45 << '\0';
    if(!out.truncated()) {
        // array now contains a zero terminated UTF-16 string
        std::vector<char16_t> v{out.begin(), out.end()};
    }

Notice that the zero termination is done by `<< '\n'`, it is not done automatically.


#### Example: Writing to a lambda

Use `str::to_function` to write to something that can be called like a function:

    auto out = str::to_function([](char32_t const* begin, char32_t const* end) {});
    out << "hello" << 123.45;

The function will be called multiple times, each time with a new part of the formatted string to
append. You can use `str::buffer` if this is not good.


#### Example: Writing to the end of a std::string

Use `str::insert_at_end` to insert at the end of a container like `std::basic_string` or `std::vector`:

    std::string s;
    str::insert_at_end(s) << "hello";


## The base class

You don't inherit from `str::out`, instead you specify the base class that `str::out` will inherit. It is
simple to make your own:

    class base_class {
    public:
        template<typename iterator_> void operator()(iterator_ begin, iterator_ end) {
            // write this range of characters to somewhere
        }
    };
    
    water::str::out<base_class>() << "hello";

This class would receive UTF-8 encoded strings to the function-call operator. If you want something
else have a `char_type` like this:

    class base_class_utf16 {
    public:
        using char_type = char16_t; // char, char16_t, char32_t or wchar_t should also work
        template<typename iterator_> void operator()(iterator_ begin, iterator_ end);
    };

You can also specify a locale and default settings for `water::numbers` in the base class. And if it
has a `flush()` function it will be called from `str::el`.

See the `water/str/out.hpp` header for more details.

Because the class is the base class of `str::out` instead of just a data member, it can add useful
functions and operators directly to the `str::out` object. The `str::to_begin_end` for example has
the `truncated()` function.



## Pre-made base classes

    template<typename iterator_> class begin_end
    // Use the to_begin_end functions to make objects of this type. Unbuffered.
    
    template<typename function_, typename char_ = *auto-detected*, unsigned buffer_size_ = 0> class buffer_lines
    // Buffering and writing lines to function_, the one used by out_stdout

    template<typename function_, typename char_ = *auto-detected*, unsigned buffer_size_ = 0> class buffer
    // Simpler buffering than buffer_lines.

    template<typename function_, typename char_ = *auto-detected*> class function;
    // Use the to_function functions to make objects of this type. Unbuffered.
    
    template<typename container_> class insert_at_end_of_container;
    // Use the insert_at_end functions to make objects of this type. Unbuffered.

The functions:

    char array[123];
    std::vector<char> vector;
    auto begin = vector.begin();
    auto end = vector.end();
    auto size = vector.size();
    void function(char const* begin, char const* end) {}
    
    str::to_begin_end(array) << "hello";
    str::to_begin_end(begin, end) << "hello";
    str::to_begin_end(begin, size) << "hello";
    
    str::to_function(function) << "hello";
    
    str::insert_at_end(vector) << "hello";



## Writing << operators

They look like this:

    template<typename base_> str::out<base_>& operator<<(str::out<base_>& o, point p) {
        o << p.x() << ',' << p.y();
        return o;
    }

If your class has a non-explicit constructor from a pointer or bool, you need to use the `str::specific`
template to avoid confusion. This is because of the "safer" c-string << operators.

    struct my_type {
        my_type(char const*);
    };
    
    template<typename base_> str::out<base_>& operator<<(str::out<base_>& o, str::specific<my_type const&> s) {
        my_type m = s.get();
        o << m...
        return o;
    }

The `str::callback` class and `str::restore_settings` can be useful for more complex tasks, look for
them in the `water/str/out.hpp` header.



## Number formatting settings

Most of these are exactly the same as the `water::numbers::settings` functions.

    using namespace water::str;
    out<...> o;
    
    o << restore_settings // at the end of the expression, the settings will be restored to what they are now
      << settings{}
      << number(123.45, settings{}.base(16)) // a number with settings for formatting just that number
      << base(2)
      << binary // same as base(2)
      << octal // same as base(8)
      << decimal // same as base(10)
      << hex // same as base(16)
      << base_prefix_suffix(false)
      << show_base // same as base_prefix_suffix(true)
      << hide_base // same as base_prefix_suffix(false)
      << bool_text(false)
      << digts(3)
      << fraction_digits(4)
      << group(true)
      << exponent
      << no_exponent
      << no_exponent_min_max(-5, 5)
      << plus(true)
      << show_plus // same as plus(true)
      << hide_plus // same as plus(false)
      << precision(5)
      << scientific(true)
      << trailing_zeros(true);
    
    
## Other << operations

    using namespace water::str;
    out<...> o;
    
    o << flush
      << el // newline + flush
      << repeat('?', 5) // repeat something a number of times, it does not have to be a char
      << string(begin, end)
      << bytes(pointer, size_in_bytes)
      << hexdump(pointer, size_in_bytes)
      << type_name(some_type_of_thing);
      
    