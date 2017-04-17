// water::numbers
////////////////////////////////////////////////////////////////////////////////////////////////////

water::numbers::read reads a number from a string
water::numbers::write writes a number to a string
water::numbers::settings has settings for how the number is read and written

	#include <water/numbers/numbers.hpp>
	using water::numbers::read;
	using water::numbers::write;
	using water::numbers::settings;
	
	vector<char16_t> text; // UTF-16
	double d = 1.234e56;
	int i = 1234;
	
	write<double, 16> write_double(d); // 16 means UTF-16, 8 and 32 and 0 also works
	size_t write_size = write_double.size(); // measure
	text.resize(write_size);
	auto t = write_double(text.begin(), text.end()); // write, returns iterator to the end
	assert(t == text.end()); // written length will match measured write_size
	
	text.push_back(' ');
	
	write<int, 8> write_int(i, settings{}.base(16)); // settings to write hexadecimal (also works for floatingpoint). UTF-8 this time
	write_int.buffered([&text](char const* begin, size_t size) {
		text.insert(text.end(), begin, begin + size);
		});
	
	// text should now contain "1.234E56 0x4D2"
	
	read<int> read_int;
	t = read_int(text.begin() + write_size + 1, text.end());
	assert(t == text.end());
	if(read_int.error() || read_int.overflow()) {
		// oh no
		}
	i = read_int; // converts to int, will be 1234
	
	
	read<double> read_double{settings{}.base(10)}; // restrict reading to base 10, does not matter here
	t = read_double(text.begin(), text.end());
	assert(t == text.begin() + write_size);
	if(read_double.error() || read_double.inexact() || read_double.nan()) {
		// do something?
		}
	d = read_double; // this should be very close to 1.234e56
	

Each time a floating point number is read or written, some precision might be lost. The loss is
very small, but if perfect roundtrip conversion is needed do not use this. This is made to produce
output for humans where the floating point numbers are usually rounded.

Look at water::numbers::settings to see the different formatting and parsing settings.

water::numbers::locale is the default locale. It is hard coded for english. You can create your own
locale and use with ther read and write classes. There is a proof of concept in test/word_locale.hpp

You can use the classes that read and write uses for more complex formatting or parsing. For example
if the exponent is in superscript you need to write and read the exponent separatley with a locale
that has superscript digits and symbols.

Reading numbers that could be in one of many locales is possible by using multiple locales. Read the
number with each locale separatley, select the one that used the most input (probably?) as the best
match and use that resulting number.