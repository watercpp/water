// ministr
///////////////////////////////////////////////////////////////////////////////////////////////////

water::ministr is a mini version of water::str. the << operator is used to format things into a
string, like water::str::out or std::ostream, but it is implemented in a completly different way
using expression templates.

water::ministr is made to be small, simple, portable and to never fail. It is not meant to do
everything water::str::out or std::ostream can. It should be useful for debugging and logging.

The expression results in a class template for the entire expression:

out<function>() << "hello world " << 123 << base<16> << 1.23e4 << '\n';

- the size of the resulting string is calculated at compile time from the different parts of the
  expression.
- the destructor creates a char array on the stack, writes the string to it, and then writes that
  to the function.

The different parts can be divided in 3 categories: 

1 Known fixed sizes:
- "hello world" is an array, the size is known
- '\n' has size 1

2 Known maximum sizes:
- 123 and 1.23e4 have known maximum sizes, integers have a maximum number of digits, and the
  floatinpoint formatter also has a maximum number of characters.

3 Unknown sizes:
- String pointers that are not arrays and string ranges have no known size at compile time.
  They use space like this:
  - Each string with unknown size reseves 64 characters
  - The entire expression reserves 128 characters
  - Leftover characters from previous written parts, like numbers or strings that did not use
    all reserved characters

It is possible to reserve more space like this:
out<function>() << size<1024> << long_string_with_unknown_size << '\n';

And it is possible to configure the default reseved size, see ministr::settings

// Headers
///////////////////////////////////////////////////////////////////////////////////////////////////

out.hpp
Has no dependency at all.
Contains the ministr::out class and all operators for UTF-8 strings and integer types.

float.hpp
Optional floating point formatting, depends on cmath / math.h and numeric_limits

utf.hpp
Optional UTF-16 and UTF-32, depends on water::unicode
Include this to write char16_t, char32_t, wchar_t and UTF-16 and UTF-32 ranges.

// Configuration
///////////////////////////////////////////////////////////////////////////////////////////////////

look at WATER_MINISTR_CONFIGURATION in out.hpp

// out 
///////////////////////////////////////////////////////////////////////////////////////////////////

template<typename function_ = void, typename configuration_ = void> class out;

function_ is optional, 

	struct my_function {
		void operator()(char const *begin, char const *end) const noexcept {
			// called from ministr::out destructor, it must never throw. it must be const
			// this is always a non-emty 0-terminated string
			// begin < end always
			// *end == 0 always
			fputs(begin, stdout);
			}
		};

when function_ is void the cstring() functions can be used to write a char array 
	
configuration_ is optional

Example:

	out<my_function>() << "hello world " << 123 << '\n';

	auto cstring1 = (out<>() << "cstring member function").cstring();
	auto cstring2 = cstring(out<>() << "cstring free function");

the cstring1 / cstring2 types are char arrays, they convert to a cstring
pointer and also have begin/end/size

	fputs(cstring1, stdout);
	for(auto c : cstring2)
		write_character(c);


two shortcut functions create out that write to functions

	out_function([](char const* b, char const*){ fputs(b, stdout); }) << "hello\n";
	out_line_function([](char const* b, char const*){ fputs(b, stdout); }) << "hello"; // appends newline always


out_begin_end creates an out that writes to iterators. It has a reference to the begin iterator so
it is possible to see how much it has written. It will not zero terminate the string.

	iterator at = something.begin();
	out_begin_end(at, something.end()) << "hello";
	auto length = at - something.begin();


// Number formatting 
///////////////////////////////////////////////////////////////////////////////////////////////////

base<base> can be 2, 8, 10, 16. For integers and floating point

	out<my_function> o;
	o << base<16> << 127 << '\n'; // 0x7f
	o << base<16> << -123.4567890123456 << '\n'; // -0x1.edd3c07fb4c99p6

digits sets the number of digits. -1 means maximum for that type
	
	o << digits<-1> << 123; // 0000000123 maybe
	o << digits<2> << 123; // 23
	o << digits<2> << 1; // 01
	
uppercase makes everything uppercase, including TRUE FALSE

	o << base<16> << upperase << -123.4567890123456 << '\n'; // -0X1.EDD3C07FB4C99P6
	
lowercase makes everything lowercase

	o << uppercase << 1e50 << lowercase << 1e50 << '\n'; // 1E50 1e50
	
mixedcase is the default. it uses uppercase exponent and hex digits
	
	o << mixedcase << 1e50 << base<16> << 0xcafe132 << ' ' << true << '\n'; // 1E50 0xCAFE123 true

no_exponent_min_max<min, max> to change when base 10 floating point numbers should use exponent form
and not. When max is -1 exponent form is used when exponent is < 0 or greater than the number of
digits that is written. This is the default.

show_plus shows plus, hide_plus does not

	o << show_plus << 123; // +123

show_base shows base prefix 0b 0o 0x, hide_base does not

	o << hide_base << base<16> << 0xff; // ff

// byte hexdump 
///////////////////////////////////////////////////////////////////////////////////////////////////

The various functions called bytes are used to hexdump memory

	class something { ... };
	something s;
	o << bytes(s); // writes all bytes of s
	
	void const *pointer = ...;
	size_t size = ...;
	o << bytes(pointer, size); // writes up to string_length bytes, or size if smaller
	o << bytes<128>(pointer, size); // writes up to 128 bytes, or size if smaller
	o << bytes<128>(pointer); // writes exactly 128 bytes

// Operators 
///////////////////////////////////////////////////////////////////////////////////////////////////

1 make a write class for a type:

	class write_my_type {
		public:
			static constexpr unsigned size = 123;
			// size reseves this many characters for writing. you can get more
			// than this in the function call operator but never less
		
			char* operator()(char* begin, char* end) const {
				// end - begin >= size
				// return one past the end of what you wrote, where the next write will begin
				*begin++ = '?';
				return begin;
				}
		
			write_my_type(my_type a);
		};
	
2 make the operator:

	template<typename a_, typename b_> out<out<a_, b_>, write_my_type>
	 operator<<(out<a_, b_> const& o, my_type m) {
		return {o, write_my_type{m}};
		}
 	
the operator must look like this, except that the write_my_type constructor can take any arguments

// Errors
///////////////////////////////////////////////////////////////////////////////////////////////////

When settings::utf8 is true this will verify all strings to be valid UTF-8 (or 16 32 with utf.hpp).
If verification fails nothing of that string is written.

You can still write bad UTF-8 to this by writing one character at a time.

Single char16_t char32_t wchar_t characters are validated. UTF-16 pair characters are discarded.
