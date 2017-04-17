// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_LOGS_STR_TO_BUFFER_HPP
#define WATER_LOGS_STR_TO_BUFFER_HPP
#include <water/logs/write_to_buffer.hpp>
namespace water { namespace logs {

/*

str::out base class for writing to a logs::buffer

using my_buffer_type = water::logs::buffer...
my_buffer_type my_buffer;

water::str::out<water::logs::str_to_buffer<my_buffer_type>> out(my_buffer);
out << "hello";


*/

template<typename buffer_> class
 str_to_buffer {
 	public:
 		using char_type = char;
 	private:
	 	write_to_buffer<buffer_> my;
 	public:
 		str_to_buffer(buffer_& b, typename buffer_::tag_type const& tag = {}) :
 			my(b, tag)
 			{}
 		str_to_buffer(buffer_ *b, typename buffer_::tag_type const& tag = {}) :
 			my(b, tag)
 			{}
 		~str_to_buffer() {
 			flush();
 			}
 		void flush() {
			my.write();
			}
		template<typename iterator_> void operator()(iterator_ begin, iterator_ end) {
			while(begin != end) {
				auto c = *begin;
				if(c) my(c);
				++begin;
				}
			}
 	};

}}
#endif
