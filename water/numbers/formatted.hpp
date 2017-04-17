// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_NUMBERS_FORMATTED_HPP
#define WATER_NUMBERS_FORMATTED_HPP
#include <water/numbers/bits.hpp>
namespace water { namespace numbers {

// a formatted number for writing as a string

template<typename iterator_> class
 formatted {
	public:
		using iterator = iterator_;
	private:
		iterator mybegin {};
		unsigned mydigits = 0;
		unsigned mybase = 10;
		unsigned mypoint_at = 0;
		int mysign = 0;
		int mybool = 0;
		bool mybase_prefix_suffix = false;
		bool mynan = false;
		bool myinfinity = false;
	public:
		formatted() = default;
		formatted(iterator begin, unsigned digits, unsigned base) :
			mybegin(begin),
			mydigits(digits),
			mybase(base)
			{}
		void begin(iterator a) {
			mybegin = a;
			}
		void digits(unsigned a) {
			mydigits = a;
			}
		void point_at(unsigned a) {
			mypoint_at = a;
			}
		void sign(int a) {
			mysign = a;
			}
		void base(unsigned base) {
			___water_assert(2 <= base && base <= 16);
			mybase = base;
			}
		void base_prefix_suffix(bool a) {
			mybase_prefix_suffix = a;
			}
		void nan(bool a) {
			mynan = a;
			}
		void infinity(bool a) {
			myinfinity = a;
			}
		void bool_set(bool a) {
			mybool = a ? 1 : -1;
			}
	public:
		iterator begin() const {
			return mybegin;
			}
		unsigned digits() const {
			// digits. can be 0 if this is exponent, and exponent should not be shown
			return mydigits;
			}
		unsigned base() const {
			return mybase;
			}
		bool point() const {
			return mypoint_at != 0;
			}
		unsigned point_at() const {
			// 0 means no, 1 means after first digit
			return mypoint_at;
			}
		bool minus() const {
			return mysign < 0;
			}
		bool plus() const {
			return mysign > 0;
			}
		bool base_prefix_suffix() const {
			return mybase_prefix_suffix;
			}
		bool nan() const {
			return mynan;
			}
		bool infinity() const {
			return myinfinity;
			}
		bool bool_false() const {
			return mybool == -1;
			}
		bool bool_true() const {
			return mybool == 1;
			}
	};

}}
#endif
