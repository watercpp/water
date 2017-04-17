// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_NUMBERS_FLOAT_ITERATOR_HPP
#define WATER_NUMBERS_FLOAT_ITERATOR_HPP
#include <water/numbers/bits.hpp>
namespace water { namespace numbers {

/*

iterate over float digits

fmod the float with 10 or 2 before this if needed

leading_zeros makes this return that many 0 before the number
digits limits the number of digits, after that this retirns 0
round_up will round the last digit up

*/

template<typename float_> class
 float_iterator {
	public:
		using iterator_category = forward_iterator_tag;
		using value_type = unsigned;
		using pointer = value_type const*;
		using reference = value_type const&;
		using difference_type = ptrdiff_t;
	private:
		float_ my = 0;
		unsigned
			mybase = 10,
			mydigit = 0,
			myat = 0,
			my0 = 0,
			mylast = static_cast<unsigned>(-1);
		bool myround = false;
	public:
		float_iterator() = default;
		float_iterator(float_ a, unsigned base) :
			my(a),
			mybase(base)
			{
			digit();
			}
		float_iterator(float_ a, unsigned base, unsigned leading_zeros, unsigned digits, bool round_up) :
			my(a),
			mybase(base),
			my0(leading_zeros),
			mylast(digits && digits <= static_cast<unsigned>(-1) - leading_zeros ? leading_zeros + digits - 1 : static_cast<unsigned>(-1)),
			myround(round_up)
			{
			if(!my0) digit();
			}
		explicit float_iterator(unsigned at) : // use this to make end
			myat(at)
			{}
		reference operator*() const {
			return mydigit;
			}
		pointer operator->() const {
			return &mydigit;
			}
		float_iterator& operator++() {
			mydigit = 0;
			++myat; // wrap around does not matter?
			if(my0 <= myat && myat <= mylast) {
				if(myat != my0)
					my = fmod(my, static_cast<float_>(1)) * static_cast<float_>(mybase);
				digit();
				}
			return *this;
			}
		float_iterator operator++(int) {
			float_iterator r(*this);
			operator++();
			return r;
			}
		bool operator==(float_iterator const& a) {
			return myat == a.myat;
			}
		bool operator!=(float_iterator const& a) {
			return myat != a.myat;
			}
	private:
		void digit() {
			if(my >= 1)
				mydigit = static_cast<unsigned>(my);
			if(myround && myat == mylast)
				++mydigit;
			if(mydigit >= mybase)
				mydigit = mybase - 1;
			}
	};

}}
#endif
