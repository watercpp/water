// Copyright 2017-2018 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_STR_BUFFER_HPP
#define WATER_STR_BUFFER_HPP
#include <water/str/out.hpp>
namespace water { namespace str {

/*

Buffer text and write to function_

struct function_ {
	void operator()(char_ const *begin, char_ const *end) {
		fputs(begin, stdout);
		}
	};

*end == 0 always. end[-1] is linefeed always. its a non-empty cstring.

Unlike str::buffer_lines this does *not* write complete lines every time. This means it will not cut
off long lines. Instead it can write partial lines, leading to different problems: If many str::out
objects write to the same destination (from one or many threads) the output of them can be mixed.

The destrucor will flush(), that will call function_. If it throws bad things happen.
If you always flush() so the destructor does not have to, the function_ can throw and it wont hurt.

*/

template<typename function_, typename char_ = char, unsigned buffer_size_ = 0> class
 buffer {
	static_assert(!buffer_size_ || buffer_size_ > 8, "");
	public:
		using function_type = function_;
		using char_type = char_;
		static unsigned constexpr buffer_size = buffer_size_ ? buffer_size_ : 512;
	private:
		function_type myfunction;
		char_type my[buffer_size];
		unsigned mysize;
	public:
		buffer() : // constructors not default because visual c++ 2015
			myfunction{},
			mysize{0}
			{}
		buffer(buffer const& a) :
			myfunction{a.myfunction}
			{
			copy(a);
			}
		buffer(buffer&& a) :
			myfunction{static_cast<function_type&&>(a.myfunction)}
			{
			copy(a);
			a.mysize = 0;
			}
		template<typename ...arguments_, typename not_copy_constructor<buffer, arguments_...>::result = 0>
		 buffer(arguments_&&... a) :
			myfunction{static_cast<arguments_&&>(a)...},
			mysize{0}
			{}
		~buffer() {
			flush();
			}
		buffer& operator=(buffer const& a) {
			myfunction = a.myfunction;
			copy(a);
			return *this;
			}
		buffer& operator=(buffer&& a) {
			myfunction = static_cast<function_type&&>(a.myfunction);
			copy(a);
			a.mysize = 0;
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
				unsigned size = static_cast<unsigned>(unicode::utf_adjust_end<unicode::utf_from_char<char_type>::result>(my + 0, my + mysize) - my);
				if(!size)
					return;
				// if myfunction thorws, behave as if it did not. discard the buffer
				struct move {
					char_type
						*buffer,
						zeroed;
					unsigned
						flush_size,
						size;
					~move() {
						if(flush_size != size) {
							auto
								t = buffer,
								f = buffer + flush_size,
								fe = buffer + size;
							*f = zeroed;
							do *t++ = *f++; while(f != fe);
							}
						}
					};
				move move_later { my, my[size], size, mysize };
				my[size] = 0;
				mysize = mysize - size;
				myfunction(my + 0, my + size); // could throw
				}
			}
		template<typename iterator_> void operator()(iterator_ begin, iterator_ end) {
			char_type c;
			while(begin != end && (c = static_cast<char_type>(*begin))) {
				if(mysize == buffer_size - 1)
					flush();
				my[mysize++] = c;
				++begin;
				}
			}
	private:
		void copy(buffer const& a) {
			mysize = 0;
			while(mysize != a.mysize) {
				my[mysize] = a.my[mysize];
				++mysize;
				}
			}
	};

}}
#endif
