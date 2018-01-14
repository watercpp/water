// Copyright 2017-2018 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_STR_BUFFER_LINES_HPP
#define WATER_STR_BUFFER_LINES_HPP
#include <water/str/out.hpp>
namespace water { namespace str {

/*

Buffer text and write 1 or more lines at a time to function_

struct function_ {
	void operator()(char_ const *begin, char_ const *end) {
		fputs(begin, stdout);
		}
	};

*end == 0 always. end[-1] is linefeed always. its a non-empty cstring.

Important: buffer_size_ - 2 is the maximum line length. Longer lines will be truncated.

The destrucor will flush(), that will call function_. If it throws bad things happen.
If you always flush() so the destructor does not have to, the function_ can throw and it wont hurt.

flush() will add a newline if the buffer does not end with one:

	using my_out = str::out<str::buffer_lines<my_function>>;
	
	my_out{} << "hello"; // newline added here
	my_out{} << "world"; // newline added here

	// output:
	// hello
	// world

*/

template<typename function_, typename char_ = char, unsigned buffer_size_ = 0> class
 buffer_lines {
	static_assert(!buffer_size_ || buffer_size_ > 8, "");
	public:
		using function_type = function_;
		using char_type = char_;
		static unsigned constexpr buffer_size = buffer_size_ ? buffer_size_ : 512;
	private:
		function_type myfunction;
		char_type my[buffer_size];
		unsigned
			mysize,
			myline; // if not 0 my[myline - 1] is '\n'
	public:
		buffer_lines() : // constructors not default because visual c++ 2015
			myfunction{},
			mysize{0},
			myline{0}
			{}
		buffer_lines(buffer_lines const& a) :
			myfunction{a.myfunction},
			myline{a.myline}
			{
			copy(a);
			}
		buffer_lines(buffer_lines&& a) :
			myfunction{static_cast<function_type&&>(a.myfunction)},
			myline{a.myline}
			{
			copy(a);
			a.mysize = a.myline = 0;
			}
		template<typename ...arguments_, typename not_copy_constructor<buffer_lines, arguments_...>::result = 0>
		 buffer_lines(arguments_&&... a) :
			myfunction{static_cast<arguments_&&>(a)...},
			mysize{0},
			myline{0}
			{}
		~buffer_lines() {
			flush();
			}
		buffer_lines& operator=(buffer_lines const& a) {
			myfunction = a.myfunction;
			copy(a);
			return *this;
			}
		buffer_lines& operator=(buffer_lines&& a) {
			myfunction = static_cast<function_type&&>(a.myfunction);
			copy(a);
			a.mysize = a.myline = 0;
			return *this;
			}
		function_type& function() {
			return myfunction;
			}
		function_type const& function() const {
			return myfunction;
			}
		void flush() {
			if(mysize) {
				unsigned s = mysize; // if function throws, set mysize to 0 first
				myline = mysize = 0;
				if(my[s - 1] != static_cast<char_type>(u'\n')) {
					auto end = unicode::utf_adjust_end<unicode::utf_from_char<char_type>::result>(my + 0, my + s);
					s = static_cast<unsigned>(end - my);
					my[s++] = static_cast<char_type>(u'\n');
					}
				my[s] = 0;
				myfunction(my + 0, my + s); // could throw
				}
			}
		template<typename iterator_> void operator()(iterator_ begin, iterator_ end) {
			while(begin != end) {
				char_type c = static_cast<char_type>(*begin);
				if(!c)
					return;
				++begin;
				if(mysize == buffer_size - 2) {
					if(c == static_cast<char_type>(u'\n')) {
						flush();
						return;
						}
					if(!myline) // discard until newline or flush()
						return;
					// flush old lines to make room
					unsigned
						s = mysize,
						l = myline;
					myline = mysize = 0; // if exception
					auto keep = my[l];
					my[l] = 0;
					myfunction(my + 0, my + l); // could throw
					my[l] = keep;
					// move
					while(l != s) my[mysize++] = my[l++];
					}
				my[mysize++] = c;
				if(c == static_cast<char_type>(u'\n'))
					myline = mysize;
				}
			}
	private:
		void copy(buffer_lines const& a) {
			mysize = 0;
			while(mysize != a.mysize) {
				my[mysize] = a.my[mysize];
				++mysize;
				}
			}
	};

}}
#endif
