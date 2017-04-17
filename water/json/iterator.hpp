// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_JSON_ITERATOR_HPP
#define WATER_JSON_ITERATOR_HPP
#include <water/json/bits.hpp>
#include <water/iterator.hpp>
namespace water { namespace json {

// this does not return a real reference, it returns a copy of a node

template<typename node_> class
 iterator {
	public:
		using iterator_category = random_access_iterator_tag;
		using difference_type = ptrdiff_t;
		using value_type = node_;
		using reference = value_type;
		class pointer {
			node_ my;
			public:
				pointer(node_ a) : my{a} {}
				node_* operator->() { return &my; }
			};
	private:
		node_ my;
		ptrdiff_t myat = 0;
	public:
		constexpr iterator() = default;
		iterator(node_ in, difference_type at) :
			my{in},
			myat{at}
			{}
		explicit operator bool() const {
			return my && static_cast<size_t>(myat) < my.size();
			}
		value_type operator*() const {
			return my.at(static_cast<size_t>(myat));
			}
		pointer operator->() const {
			return {my.at(static_cast<size_t>(myat))};
			}
		value_type operator[](difference_type a) const {
			return my.at(static_cast<size_t>(myat + a));
			}
		iterator& operator++() {
			++myat;
			return *this;
			}
		iterator& operator--() {
			--myat;
			return *this;
			}
		iterator operator++(int) {
			iterator r(*this);
			++myat;
			return r;
			}
		iterator operator--(int) {
			iterator r(*this);
			--myat;
			return r;
			}
		iterator& operator+=(difference_type a) {
			myat += a;
			return *this;
			}
		iterator& operator-=(difference_type a) {
			myat -= a;
			return *this;
			}
		iterator operator+(difference_type a) const {
			return iterator(*this) += a;
			}
		friend iterator operator+(difference_type a, iterator m) {
			return m += a;
			}
		iterator operator-(difference_type a) const {
			return iterator(*this) -= a;
			}
		difference_type operator-(iterator const& a) const {
			return myat - a.myat;
			}
		bool operator==(iterator const& a) const {
			return myat == a.myat;
			}
		bool operator!=(iterator const& a) const {
			return myat != a.myat;
			}
		bool operator<(iterator const& a) const {
			return myat < a.myat;
			}
		bool operator>(iterator const& a) const {
			return myat > a.myat;
			}
		bool operator<=(iterator const& a) const {
			return myat <= a.myat;
			}
		bool operator>=(iterator const& a) const {
			return myat >= a.myat;
			}
	};

}}
#endif
