// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_TESTS_VECTORS_VALUE_MOVE_ONLY_HPP
#define WATER_TESTS_VECTORS_VALUE_MOVE_ONLY_HPP
#include <water/tests/vectors/bits.hpp>
namespace water { namespace tests { namespace vectors {

struct value_move_only_construct {};

class value_move_only {
	void const *myself; // this pointer, catch memcpy
	unsigned mymoves = 0;
	bool mymoved = false;
	public:
		value_move_only(value_move_only_construct const&) :
			myself(this)
			{}
		value_move_only(value_move_only&& a) :
			myself(this),
			mymoves(a.mymoves + 1)
			{
			a.mymoved = true;
			}
		~value_move_only() {
			___water_test(myself == this);
			}
		value_move_only& operator=(value_move_only&& a) {
			mymoves = a.mymoves + 1;
			mymoved = false;
			a.mymoved = true;
			return *this;
			}
		value_move_only(value_move_only const&) = delete;
		value_move_only& operator=(value_move_only const&) = delete;
		bool current() const {
			return mymoved == false;
			}
		bool valid() const {
			return myself == this;
			}
	};

}}}
#endif
