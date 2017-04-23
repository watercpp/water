// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_ASCII_LOWERCASE_ITERATOR_HPP
#define WATER_ASCII_LOWERCASE_ITERATOR_HPP
#include <water/ascii_lower_upper.hpp>
#include <water/iterator.hpp>
#include <water/types/types.hpp>
namespace water {

/*

Iterator that converts another iterators ASCII output to lowercase.
Non-ASCII values are not changed, it is safe to use on UTF-8, UTF-16 and UTF-32.

*/

template<typename iterator_> class
 ascii_lowercase_iterator {
	struct no_difference_type;
	using difference_if = typename types::ifel_type<is_random_access_iterator<iterator_>, iterator_difference_type<iterator_>, no_difference_type>::result;
	public:
		using iterator_category = typename water::iterator_category<iterator_>::result;
		using value_type = typename iterator_value_type<iterator_>::result;
		using reference = value_type;
		using pointer = void;
		using difference_type = typename iterator_difference_type<iterator_>::result;
	private:
		iterator_ my {};
	public:
		ascii_lowercase_iterator() = default;
		ascii_lowercase_iterator(iterator_ a) :
			my{a}
			{}
		reference operator*() const {
			return ascii_to_lower(*my);
			}
		pointer operator->() const {
			// is this dumb?
			}
		reference operator[](difference_if d) const {
			return ascii_to_lower(my[d]);
			}
		ascii_lowercase_iterator& operator++() {
			++my;
			return *this;
			}
		ascii_lowercase_iterator operator++(int) {
			ascii_lowercase_iterator r(*this);
			++my;
			return r;
			}
		ascii_lowercase_iterator& operator--() {
			--my;
			return *this;
			}
		ascii_lowercase_iterator operator--(int) {
			ascii_lowercase_iterator r(*this);
			--my;
			return r;
			}
		ascii_lowercase_iterator& operator+=(difference_if d) {
			my += d;
			return *this;
			}
		ascii_lowercase_iterator& operator-=(difference_if d) {
			my -= d;
			return *this;
			}
		ascii_lowercase_iterator operator+(difference_if d) const {
			return ascii_lowercase_iterator(*this) += d;
			}
		friend ascii_lowercase_iterator operator+(difference_if d, ascii_lowercase_iterator b) {
			return b += d;
			}
		ascii_lowercase_iterator operator-(difference_if d) const {
			return ascii_lowercase_iterator(*this) -= d;
			}
		difference_if operator-(ascii_lowercase_iterator const& b) const {
			return my - b.my;
			}
		bool operator==(ascii_lowercase_iterator const& b) const {
			return my == b.my;
			}
		bool operator!=(ascii_lowercase_iterator const& b) const {
			return my != b.my;
			}
		bool operator<(ascii_lowercase_iterator const& b) const {
			return my < b.my;
			}
		bool operator>(ascii_lowercase_iterator const& b) const {
			return my > b.my;
			}
		bool operator<=(ascii_lowercase_iterator const& b) const {
			return my <= b.my;
			}
		bool operator>=(ascii_lowercase_iterator const& b) const {
			return my >= b.my;
			}
	};

template<typename iterator_> ascii_lowercase_iterator<iterator_> ascii_lowercase_iterator_from(iterator_ a) { return {a}; }

}
#endif
