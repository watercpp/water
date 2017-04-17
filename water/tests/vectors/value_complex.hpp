// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_TESTS_VECTORS_VALUE_COMPLEX_HPP
#define WATER_TESTS_VECTORS_VALUE_COMPLEX_HPP
#include <water/tests/vectors/bits.hpp>
namespace water { namespace tests { namespace vectors {

struct value_complex_emplace {};

struct value_complex_count {
	ptrdiff_t
		count = 0,
		copies = 0;
	};

class value_complex {
	value_complex_count *my;
	void const *myself; // this pointer, catch memcpy
	unsigned mycopies = 0;
	bool mymoved = false;
	public:
		explicit value_complex(value_complex_count& a) :
			my(&a),
			myself(this)
			{
			++my->count;
			}
		value_complex(value_complex_count *a, value_complex_emplace&&, value_complex_emplace&&) :
			my(a),
			myself(this)
			{
			++my->count;
			}
		value_complex(value_complex const& a) :
			my(a.my),
			myself(this),
			mycopies(a.mycopies + 1)
			{
			if(my) {
				++my->count;
				++my->copies;
				}
			}
		value_complex(value_complex&& a) :
			my(a.my),
			myself(this),
			mycopies(a.mycopies + 1)
			{
			a.my = 0;
			a.mymoved = true;
			}
		~value_complex() {
			___water_test(myself == this);
			if(my) --my->count;
			}
		value_complex& operator=(value_complex const& a) {
			if(my)
				--my->count;
			my = a.my;
			if(my) {
				++my->count;
				++my->copies;
				}
			mycopies = a.mycopies + 1;
			mymoved = false;
			return *this;
			}
		value_complex& operator=(value_complex&& a) {
			if(my)
				--my->count;
			my = a.my;
			a.my = 0;
			mycopies = a.mycopies;
			mymoved = false;
			a.mymoved = true;
			return *this;
			}
		bool valid() const {
			return myself == this;
			}
	};

}}}
#endif
