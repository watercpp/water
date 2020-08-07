# water::unicode

Tools for converting between UTF-8, UTF-16 and UTF-32. The most useful are:

### Detecting UTF type of an iterator or char type:

    #include <water/unicode/unicode.hpp>
    unsigned constexpr a = water::unicode::utf_from_iterator<char const*>::result; // is 8
    unsigned constexpr b = water::unicode::utf_from_char<char16_t>::result; // is 16


### UTF conversion iterators to convert to another UTF encoding:

    std::vector<sometype> text;

    // 16 means convert to UTF-16. 8 and 32 also works
    auto r1 = water::unicode::utf_range_from<16>(text.begin(), text.end());
    auto i = r1.begin();
    while(i != r1.end()) {
        do_something(*i);
        ++i;
    }
    
    // 32 means convert to UTF-32. Notice text.size() not text.end()
    auto r2 = water::unicode::utf_range_from<32>(text.begin(), text.size());
    auto j = r2.begin();
    while(j != r2.end()) {
        do_something(*j);
        ++j;
    }

### Verifying that text is valid UTF, and getting the length it would have converted to other UTF encodings:

    // 8 means text is UTF-8, use 16 or 32 for UTF-16 or UTF-32
    water::unicode::utf_length<8> verify(text.begin(), text.begin());
    if(!verify)
        trace() << "not valid utf-8!";
    else {
        size_t
            legth_as_utf8 = verify.utf8(),
            length_as_utf16 = verify.utf16(),
            length_as_utf32 = verify.utf32();
    }

You can use the `utf_length_from` function to detect the UTF of the input text from text's character type:

    auto verify = water::unicode::utf_length_from(text.begin(), text.end());
    
## More

`water/unicode/utf.hpp` contains more low-level building blocks to convert one codepoint at a time
or convert a range of characters when you know the destination has enough space.
