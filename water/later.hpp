// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_LATER_HPP
#define WATER_LATER_HPP
#include <water/swap.hpp>
namespace water {

/*

auto a = later([](){ trace() << "later"; });
trace() << "now";

output:
now
later

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
