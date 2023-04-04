# water::numbers

`water::numbers::read` reads a number from a string.

`water::numbers::write` writes a number to a string.

`water::numbers::settings` has settings for how the number is read and written.

The classes are made to work with UTF-8, UTF-16 and UTF-32. `read` can read from forward iterators,
and has functions to detect if any error or problem happend when converting the string to a number.
`write` can write to output iterators or a function object. It can measure how many characters it
will write before writing. `settings` allows unusual formats like binary floating point.

`#include <water/numbers/numbers.hpp>` to use them.



## Writing numbers as text

### Writing to a function

    numbers::write<double> write{1.234e56};
    write.buffered(
        [](char16_t const* begin, char16_t const* end) {
            //...
        }
    );

The number is written as UTF-16, detected from the function. If the char type cannot be detected it
will default to `char` and UTF-8. You can specify the UTF with the second template argument:

    numbers::write<int, 32> write_int_as_utf_32{1234};
    write.buffered(
        [](auto const* begin, auto const* end) {
            // auto is char32_t
        }
    );

The number is formatted depending on what `numbers::settings` are used. Those can be passed to the
constructor:

    numbers::settings s;
    s.base(16);
    numbers::write<double> write_hexfloat{1.234e56, s};

If extreme settings are used, the formatted number could be very long, and then the function used
to write the number would be called more than once.


### Writing to iterators

    std::vector<char> to;
    
    numbers::write<double> write{1.234e56};
    size_t size = write.size<char>(); // measure, not required
    to.resize(size);
    auto end = write(to.begin(), to.end());


If the UTF is specified with the second template argument, the size function does not need the
char type template argument:
    
    numbers::write<bool, 16> write_bool_as_utf_16{true};
    size_t size = write.size();



## Reading numbers from text

    std::vector<char> text = ....;
    
    numbers::read<double> read;
    auto end = read(text.begin(), text.end());
    if(read.any_problem()) {
        // it failed, or the number was infinity or NaN
    }
    double value = read.value();

How the numbers are read depend on the `numbers::settings`. For example the default settings does
not read a number that is separated into groups (using thousands separators) as one number, so
`1 000 000` would read as 1 instead of 1000000. To change that:

    numbers::settings s;
    s.group(true);
    numbers::read<unsigned> read{s};
    


## Settings

Look at `water::numbers::settings` in `water/numbers/settings.hpp` to see the different formatting
and parsing settings.



## Locales

A locale can be passed as the last argument to the reading and writing functions:

    numbers::locale my_locale;
    
    numbers::read<int> read;
    read(begin, end, my_locale);
    
    numbers::write<float> write{1.23e4f};
    size_t size = write.size<char16_t>(my_locale);
    write(begin, end, my_locale);
    write([](char const*, char const*), my_locale);

`water::numbers::locale` is the default locale. It is hard coded for english. You can create your
own locale and use with the read and write classes.

The locale specifies the characters or strings that make up the different parts of the number.
Digits, plus, minus, decimal point and so on. Digits do not have to be the ASCII 0-9. It also
specifies how numbers are grouped (usually 3 digits, thousands separators). Or it could specify that
hex is written as `#AB12` or `AB12h` or `ab12(Hex)` instead of `0xAB12`.

It should hopefully be possible to use any number format from the Unicode CLDR Project https://cldr.unicode.org/
or from the locale data of the operating system with water::numbers. But that data needs to be
converted to the water::numbers locale format and that code does not exist (yet?).

There is a proof of concept of an unusual locale in `water/numbers/test/word_locale.hpp`



## Building blocks

You can use the classes that `numbers::read` and `numbers::write` use for more complex formatting
or parsing. For example if the exponent is in superscript you need to write and read the exponent
separately with a locale that has superscript digits and symbols.

Reading numbers that could be in one of many locales is possible by using multiple locales. Read the
number with each locale separately, select the one that used the most input (probably?) as the best
match and use that resulting number.



## Floating point conversion accuracy

Each time a floating point number is read or written in decimal format, some precision might be lost.
The loss is usually minimal, close to `numeric_limits<type>::epsilon()`.

Some accuracy is lost from the calculations when formatting or parsing the number. For example the
`pow` function will not have the same accuracy on all platforms, and that will affect the result.

The default `numbers::settings` will round floating point numbers in decimal format so they look good
to humans. The precision is usually 7 digits for `float` and 14 digits for `double` and `long double`,
but it depends on `numeric_limits`.
This prevents 12345 to be written as 12344.9999999999 even if that might be closer to the real value
of the IEEE 754 / IEC 60559 float.

If the numbers will be read by computers, increase the precision used for `numbers::write`
    
    numbers::settings s;
    s.precision(std::numeric_limits<double>::digits10 + 2);
    numbers::write<double> w{s};

Hexfloats will probably be 100% accurate, and they are not rounded with the default `numbers::settings`.

If perfect roundtrip conversion to/from decimal format is needed, there are specialized libraries
that do just that.



## Octal numbers

The default locale will write octal numbers as `0o1234`, prefixed with 0 followed by the letter O.
When reading numbers this will see `01234` as a base 10 decimal number and `0o1234` as octal. If
that is a problem, it is possible to use `numbers::settings` and make a `numbers::read` that will
read all numbers as octal:

    numbers::settings s;
    s.base(8);
    numbers::read<unsigned> read_octal{s};
