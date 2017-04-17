// water::encodings
////////////////////////////////////////////////////////////////////////////////////////////////////

Tools for converting between UTF-8, UTF-16 and UTF-32. The most useful are:

Detecting UTF type of an iterator or char type:

	#include <water/encodings/encodings.hpp>
	unsigned constexpr a = water::encodings::utf_from_iterator<char const*>::result; // is 8
	unsigned constexpr b = water::encodings::utf_from_char<char16_t>::result; // is 16


UTF conversion iterators to convert to another UTF encoding:

	std::vector<sometype> text;

	auto r1 = water::encodings::utf_range_from<16>(text.begin(), text.end()); // 16 means UTF-16. 8 and 32 also works
	auto i = r1.begin();
	while(i != r1.end()) {
		do_something(*i);
		++i;
		}
	
	auto r2 = water::encodings::utf_range_from<32>(text.begin(), text.size()); // 32 means UTF-32. Notice text.size() not text.end()
	auto j = r2.begin();
	while(j != r2.end()) {
		do_something(*j);
		++j;
		}

Verifying that text is valid UTF, and getting the length it would have converted to other UTF encodings:

	water::encodings::utf_length<8> verify(text.begin(), text.begin()); // 8 means UTF-8, use 16 or 32 for UTF-16 or UTF-32
	if(!verify)
		trace() << "not valid utf-8!";
	else {
		size_t
			legth_as_utf8 = verify.utf8(),
			length_as_utf16 = verify.utf16(),
			length_as_utf32 = verify.utf32();
		}

Notice that you could use utf_from_iterator or utf_from_char above to detect the UTF of text

	water::encodings::utf_length<water::encodings::utf_from_iterator<decltype(text.begin())>::result> verify(text.begin(), text.begin());


water/encodings/utf.hpp contains more low-level building blocks to convert one codepoint at a time
or convert a range of characters when you know the output has enough space. 