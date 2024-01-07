# water::xtr

*xtr* is short for *expression string* or *expression stream*

`water::xtr` does string formatting using << operators like `water::str` or `std::ostream`. But it
does this in a totally different way using expression templates.

Example:
    
    auto to = xtr::to_function([](char const* c) { fputs(c, stdout); });
    
    to << "hello world " << 123 << ' ' << xtr::base<16> << 1.23e4 << '\n'; // expression 1
    to << u"line " << U'2' << L'!' << '\n'; // expression 2

- All << operators of an expression are combined using `xtr::expression`.
- After that the entire expression is formatted as a UTF-8 string at once. This string is a char
  array on the stack. It's size is calculated at compile time, it is not dynamically allocated.
- Finally that string is, in this example, written to `stdout` using `fputs`. Two calls are made to
  `fputs`, one for each expression.

This example will write "hello world 123 0x1.806p13", a newline, then "line 2!" and a newline. The
double is written as hexadecimal because of the xtr::base<16> before it.

You can also write an expression directly into your own string, plus much more. A few examples:

    fputs(xtr::no << "hello world " << 123 << ' ' << xtr::base<16> << 1.23e4 << '\n' << xtr::string, stdout);
    
    std:string s;
    xtr::insert_at_end(s) << "hello";
    
    auto overwrite = str::to_begin_end(s.begin(), s.end());
    overwrite << "this is too long";
    if(overwrite.truncated()) {
        // ...
    }
    
    auto x = xtr::no << s << ' ' << xtr::base<16> << 1.23e4 << '\n' << xtr::string;
    s.assign(x.begin(), x.end());

Types with begin() and end() functions are seen as strings. This means `std::string` and `std::vector`
are seen as strings:
    
    std::string s{"la"};
    std::u16string u{u"la"};
    std::vector<char8_t> v;
    xtr::insert_at_end(v) << s << u;

`xtr::fold` is an alternative way of using xtr. Instead of the << operators just pass all arguments
to the `xtr::fold` function. Less like `std::ostream` and more like `printf` or `std::format` but
without a format-string:
    
    auto result = xtr::fold("hello world ", 123, ' ', xtr::base<16>, 1.23e4, '\n');
    
    fputs(result, stdout); // converts to zero-terminated C-string
    std::vector<char> copy{result.begin(), result.end()};


## None or minimal dependencies

`water/str/base.hpp` contains everything except floating-point, `xtr::fold`, UTF-16 and UTF-32 support.
This header is self contained, it does not use any code from anywhere else and does not include any
other header.

`water/xtr/xtr.hpp` includes everything from `base.hpp` and adds support for floating-point, UTF-16,
UTF-32 and `xtr::fold`. It depends on a few standard C library math functions, `std::numeric_limits` plus 
`water::unicode` and some other parts of `water`.

Note that `water/xtr/base.hpp` has UTF-8 support, and the generated string is always UTF-8. UTF-16
and UTF-32 characters and strings can only be written to the string.

The intention is that `water::xtr` should be useful in places where additional dependencies are
problems. It is used in the rest of the water library for asserts and logging with `water::trace`.
It can also be used on embedded systems with no standard C library.



## xtr::no

`xtr::no` is a way to build an `xtr::expression` whose result is not written anywhere

    xtr::no << "hello" << ' ' << 123; // does not write a string, it is just the expression, has no effect
    
This becomes useful when it's combined with things that can write the `xtr::expression`:

The `xtr::write` function that writes to iterators:

    std::vector<char> vector(123);
    
    auto at = write(xtr::no << "hello" << ' ' << 123, vector.begin(), vector.end());    
    
    vector.erase(at, vector.end()); // remove unused space
    
Or `xtr::string`, or `xtr::char_array`

    auto s1 = string(xtr::no << "hello" << ' ' << 123);
    auto s2 = xtr::no << "hello" << ' ' << 123 << xtr::string;
    xtr::char_array<123> s3{xtr::no <<  "hello" << ' ' << 123};
    
Here `s1`, `s2` and `s3` are all `xtr::char_array` objects. They automatically convert to a zero
terminated c-string, and also have `begin()`, `end()`, `size()` and subscript. 
    
    vector.assign(s1.begin(), s1.end());
    fputs(s1, stdout);
    
    for(auto c : s1)
       vector.push_back(c);


`xtr::no` has the same effect as using `xtr::expression<>{}` directly, it is just shorter to type.

    auto s1 = xtr::expression<>{} << "hello" << ' ' << 123 << xtr::string;
    auto s2 = xtr::no << "hello" << ' ' << 123 << xtr::string;



## xtr::to_function & xtr::line_to_function

`xtr::to_function` and `xtr::line_to_function` write the `xtr::expression` to a function. The only
difference is that `xtr::line_to_function` will add an newline `\n` to the end of the string if it
does not already end with one.

The function is called **once** with the entire expression formatted into a non-empty zero terminated
string. It should be something that can be called in one of these ways:

    void function(char const* begin, char const* end); // begin != end always, and *end == 0 always
    
    void function(char const* zero_terminated_non_empty_string);
    
Example:
    
    std::vector<char> vector;
    
    xtr::to_function([&vector](auto begin, auto end) { vector.assign(begin, end); }) << "hello world";



## xtr::to_function_unbuffered & xtr::line_to_function_unbuffered

`xtr::to_function_unbuffered` and `xtr::line_to_function_unbuffered` write the `xtr::expression` one
character at a time. The expression is written directly to the function.

This means the *string size calculation* described below does not matter, there is no need to know
the length of the string at compile time.

Example:
    
    std::vector<char> to;
    std::vector<char> a_lot_of_a(123456, 'a');
    
    xtr::to_function([&to](char c) { to.push_back(c); }) << "hello world " << a_lot_of_a;



## xtr::insert_at_end

Quick way to insert at the end of a container like `std::basic_string` or `std::vector`

    std::vector<char> vector;
    xtr::insert_at_end(vector) << "hello world";
    
    auto to = xtr::insert_at_end(vector);
    
    to << "hello again!";
    to << "hello again!";



## xtr::to_begin_end

Writes the expression directly to a pair of iterators, or an array. `end()` returns where the
written string ends. `truncated()` returns true if the string did not fit.

    std::vector<char> vector{123};
    
    auto to = xtr::to_begin_end(vector.begin(), vector.end());
    to << "hello world";
    if(to.truncated()) {
        // oh no
    }
    auto begin = to.begin(); // same as vector.begin()
    auto end = to.end(); // the end of the written string <= vector.end()

Array:
    
    char array[123];
    auto to = xtr::to_begin_end(array);
    to << "hello";
    char* end = to.end();

When the written string is not empty, and not truncated, it converts to true:

    char array[123];
    auto to = xtr::to_begin_end(array);
    to << "hello" << '\0'; // zero-terminated
    if(to)
        fputs(array, stdout);



## xtr::sout for writing to stdout

`xtr::sout` is in the `water/xtr/stdout.hpp` header, and writes to `stdout`. It will add a
linebreak at the end if the expression does not end with a linebreak. It can be used from multiple
threads at the same time.

    xtr::sout << "hello";



## Number formatting settings

These are similar to `water::str`, but fewer, and they are all compile time constants:

    using namespace water::xtr;
    
    expression<>{}
        << base<16> // 2 8 10 16 works
        << binary // same as base<2>
        << octal // same as base<8>
        << decimal // same as base<10>
        << hex // same as base<16>
        << digits<2> // 001 instead of 1, 234 instead of 1234. For floats this is the number of significant digits its rounded to
        << no_exponent_min_max<-10, 10> // when floating-point numbers use exponent form or not
        << exponent // always use floating-point exponent form
        << exponent_auto // numbers >= 1 up to a certain max do not use exponent form. 1234 instead of 1.234E3 (this is the default)
        << uppercase // 0XCAFE 1.23E5 0X1.CAFEP123 NAN TRUE
        << lowercase // 0xcafe 1.23e5 0x1.cafep123 nan true
        << mixedcase // 0xCAFE 1.23E5 0x1.CAFEp123 NaN true (this is the default)
        << show_base // 0xCAFE
        << hide_base // CAFE
        << show_plus // +123
        << hide_plus // 123

`no_exponent_min_max<min, max>` works like this:
- min: when the exponent is less than this value, exponent form is used. -3 means
  - 1.23E-3 is written as 0.00123
  - 1.23E-4 is written as 1.23E-4
- max: when the exponent is greater than this value, exponent form is used. 3 means:
  - 1.23E3 is written as 1230
  - 1.23E4 is written as 1.23E4
  
Floating-point numbers in base 2, 8 or 16 always use exponent form, the settings only affect base 10.




## Other << operations

    
    using namespace water::xtr;
    
    expression<>{}
        << reserve<123> // reserve this many additional characters in the string
        << bytes(pointer, size_in_bytes) // raw bytes as hex for debugging
        << string; // converts the expression into a xtr::char_array



## String size calculation

When the string is written to a temporary char array on the stack, the compile-time size for that
char array is calculated like this:

Each `xtr::expression` in the expression has a compile-time size.

The size of the char array is the sum of all `xtr::expression` sizes, plus an extra 80.

- For single characters the size is 1
- For char-arrays it's the array size
- For integers and floating-point numbers, it is the maximum possible length of that type with the
  current formatting settings. This is usually too large. For example a 64-bit integer can have 20
  digits, but if the number is 123 only 3 is used.
- For types without a compile-time size, a fixed value is used. This happens for char pointers or
  for example for std::string. That size is 40.

If you know the size is not big enough, use `<< reserve<123>` in the expression to manually add
that many extra characters.

Nothing bad happens if the array is too small, except that the string is truncated. It will not be
truncated in the middle of a UTF-8 sequence.

It is possible to tune this, and collect statistics. See the `base.hpp` header.



## Making your own << operators

It is more complicated to make a << operator for `water::xtr` than for `water::str`.

If a type already has a `water::str` << operator, it can be reused. See `water/xtr_operator_using_str.hpp`

This example is an operator for a `thing`.

You need to make a class that writes your type to the string, `xtr_write_thing` here. That class
must have the compile-time constant `size` that is the maximum number of characters needed. And the
function call operator for the writing.

The operator function should return a new `xtr::expression` that is the current expression plus the
writer class.

    class thing
    {
    public:
        size_t size();
        iterator begin();
        iterator end();
    };
    
    class xtr_write_thing
    {
        thing const *my;
    public:
        static unsigned constexpr size = 123;
    
        xtr_write_thing(thing const& a) :
            my{&a}
        {}
        
        template<typename write_>
        void operator()(write_&& write) const {
            auto b = my->begin();
            auto e = my->end();
            size_t s = my->size();
            
            // write has 3 different write functions:
            write('x');
            write(b, e);
            write(b, s);
        }
    };

    template<typename a_, typename b_>
    water::xtr::expression<
        water::xtr::expression<a_, b_>,
        xtr_write_thing
    >
    operator<<(water::xtr::expression<a_, b_>&& e, thing const& s) {
        return {e, xtr_write_thing{s}};
    }

Note that the writer class will only exist during the expression, it is 100% fine for it to keep a
pointer to the `thing` because that thing will always have a longer lifetime than the write class.



## Making your own "cout" like object using water::xtr

`xtr::expression` objects are temporary, and only used during the << expression. This example makes
a `sout` object that has a << operator that returns a `xtr::expression` that is written to `stdout`.

A feature is that it will always end the expression with a `\n`, using the `xtr::end_with_newline`.
    
    struct fputs_stdout {
        void operator()(char const* c) {
            fputs(c, stdout);
        }
    };

    constexpr xtr::create<fputs_stdout, xtr::end_with_newline> sout;

Use it like this:

    sout << "hello world" << 123 << '!';
    
    sout << "a newline\n";
    
A newline is added to the first expression, but not the second because it already ends with a newline.

Multiple threads can use the `sout` object at the same time without locks. Each thread writes one
line at a time, so the output is never mixed in the middle of a line. (`fputs` should be safe to
call from multiple threads at the same time.)

Using `std::cout` from multiple threads without locks is not a good idea.

This is exactly how `xtr::sout` in the `water/xtr/stdout.hpp` is made.



## Making your own "cout" again, but using xtr::expression directly

This example creates a `sout` that works exactly like the `sout` from the example above.

Instead of using `xtr::create` it uses `xtr::expression` directly. It shows how to make the initial
<< operator that creates the `xtr::expression`.

Instead of using `xtr::end_with_newline`, it uses `xtr::configuration` directly to make the exact
same thing as `xtr::end_with_newline`. You can configure the number format and more, look at the
`water/xtr/base.hpp` header.

    struct sout_type {};
    
    constexpr sout_type sout;
    
    struct fputs_stdout {
        void operator()(char const* c) {
            fputs(c, stdout);
        }
    };
    
    using sout_expression = xtr::expression<
        fputs_stdout,
        xtr::configuration<
            xtr::configuration_default::number_format,
            xtr::settings_end<xtr::configuration_default::settings, '\n'>
        >
    >;
    
    template<typename type_>
    auto operator<<(sout_type, type_&& a) -> decltype(sout_expression{} << a) {
        return sout_expression{} << a;
    }

