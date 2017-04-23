// water::str
////////////////////////////////////////////////////////////////////////////////////////////////////

water::str::out has << operators to format things into a string, like std::ostream. Str is short for
string or stream.

	str::out<base_class>{} << "hello world " << 123 << ' ' << str::base(16) << 1.23e4 << str::el;

This example will write "hello world 123 0x1.806P13" followed by a newline. The double is written as
hexadecimal because of the str::base(16) before it. str::el is like std::endl, it writes a newline
then "flushes".

Numbers are formatted using water::numbers::write and the water::numbers::settings class is used to
control how number formatting works. All str::out objects hold a numbers::settings.

Strings are converted to the UTF encoding of the base_class class, from UTF-8, UTF-16 or UTF-32.

Types with begin() and end() functions are seen as strings. This means std::string or std::vector
are seen as strings:

	str::out<base_class> o;
	std::string s{"la"};
	std::u16string u{u"la"};
	o << s << u;

Char pointers are seen as C-strings, if they are of type char, char16_t, char32_t or wchar_t. But to
reduce the risk of buffer overruns there are separate << operators for char arrays, and classes that
implicitly convert to a char-pointer will not bee seen as C-strings at all.

For simply writing to stdout, use out_stdout from out_stdout.hpp

	str::out_stdout{} << "hello";
	str::out_stdout o;
	o << U"world";
	o << 123;

This will write:
	
	hello
	world123

This class will automatically add a newline when the string is written to stdout, unless the string
already ends in a newline. And it buffers output. If multiple threads write to stdout using it, each
thread will write complete lines every time so the output from different threads wont mix. (Provided
that writing to stdout from different threads at the same time is possible, it should be). Just use
separate out_stdout objects in different threads.


// The base class
////////////////////////////////////////////////////////////////////////////////////////////////////

You dont inherit from str::out, instead you specify the base class that str::out will inherit. It is
simple to make your own:

	class base_class {
		public:
			template<typename iterator_> void operator()(iterator_ begin, iterator_ end) {
				// write this range of characters to somewhere
				}
		};
	
str::out<base_class>() << "hello";

This class would recieve UTF-8 encoded strings to the function-call operator. If you want something
else have a char_type like this:

	class base_class_utf16 {
		public:
			using char_type = char16_t; // char, char16_t, char32_t or wchar_t should also work
			template<typename iterator_> void operator()(iterator_ begin, iterator_ end);
		};

You can also specify a locale and default settings for water::numbers in the base class.

See the out.hpp header.

// Pre-made base classes
////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename iterator_> class begin_end
For a pair of iterators.

template<typename function_, typename char_ = char, unsigned buffer_size_ = 0> class buffer_lines
Buffering and writing lines to function_, the one used by out_stdout

template<typename function_, typename char_ = char, unsigned buffer_size_ = 0> class buffer
Simpler buffering than buffer_lines.

template<typename container_> struct container
Inherit a container like std::string or std::vector. Ugly but useful.

The out_function function, for lambdas:

	std::vector<char16_T> v;
	str::out_function([&v](char16_t const* begin, char16_t const* end) { v.insert(v.end(), begin, end); }) << "test " << 123 << '!';


// Writing << operators
////////////////////////////////////////////////////////////////////////////////////////////////////

They look like this:

	template<typename base_> str::out<base_>& operator<<(str::out<base_>& o, point p) {
		o << p.x() << ',' << p.y();
		return o;
		}

If your class has a non-explicit constructor from a pointer or bool, you need to use the str::specific
template to avoid confusion. This is because of the "safer" c-string << operators.

	struct my_type {
		my_type(char const*);
		};
	
	template<typename base_> str::out<base_>& operator<<(str::out<base_>& o, str::specific<my_type const&> s) {
		my_type m = s.get();
		o << m...
		return o;
		}

The str::callback class and str::restore_settings can be useful for more complex tasks, look for
them in the out.hpp header.

