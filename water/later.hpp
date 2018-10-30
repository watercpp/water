// Copyright 2017-2018 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_LATER_HPP
#define WATER_LATER_HPP
#include <water/swap.hpp>
namespace water {

/*

Run a lambda or other function object when the water::later object goes out of scope.
Although the class is very simple, it has been really useful.

example 1:

	auto later = water::later([](){ trace() << "later"; });
	trace() << "now";

output:

	now
	later
	

example 2:

	type *pointer = new type;
	auto delete_later = water::later([pointer] { delete pointer; });
	pointer->do_something();


example 3:

	gpu_buffer_struct buffer{};
	if(gpu_texture_lock_texture_memory(texture, &buffer)) {
		auto unlock_later = water::later([texture, &buffer] {
			gpu_texture_unlock_texture_memory(texture, &buffer);
			});
		do_something(buffer.pointer, buffer.width, buffer.height);
		}


example 4:
	
	auto clenaup_if_something_goes_wrong = water::later([] { cleanup(); });
	if(operaton_1() == success) {
		do_something();
		if(operation_2() == success) {
			// all good, no need to cleanup
			clenaup_if_something_goes_wrong.dont();
			}
		}

*/

template<typename function_> class
 later_move {
 	function_ my;
 	bool mydo = false;
 	public:
 		later_move() = default; // makes no sense?
 		explicit later_move(function_&& a) :
 			my{static_cast<function_&&>(a)},
 			mydo{true}
 			{}
 		later_move(later_move const& a) = delete;
 		later_move(later_move&& a) :
 			my{a.my},
 			mydo{a.mydo}
 			{
 			a.mydo = false;
 			}
 		~later_move() {
 			if(mydo) my();
 			}
 		later_move& operator=(later_move const& a) = delete;
 		later_move& operator=(later_move&& a) {
 			swap(a);
 			return *this;
 			}
 		void swap(later_move& a) {
 			swap_from_swap(a.my, my);
 			swap_from_swap(a.mydo, mydo);
 			}
 		void dont() {
 			mydo = false;
 			}
 		void now() {
 			if(mydo) my();
 			mydo = false;
 			}
 	};

template<typename function_> later_move<function_> later(function_&& a) {
	return later_move<function_>{static_cast<function_&&>(a)};
	}

template<typename function_> void swap(later_move<function_>& a, later_move<function_>& b) {
	a.swap(b);
	}

}
#endif
