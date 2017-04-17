// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_DOWNGRADE_ITERATORS_RANDOM_ACCESS_HPP
#define WATER_DOWNGRADE_ITERATORS_RANDOM_ACCESS_HPP
#include <water/downgrade_iterators/bits.hpp>
namespace water { namespace downgrade_iterators {

template<typename iterator_, bool make_const_ = false, bool proxy_ = false> class
 random_access {
	static_assert(types::type_assert<is_random_access_iterator<iterator_>>::assert, "must be random access");
	using select_ = select_reference_pointer<iterator_, make_const_, proxy_>;
	public:
		using iterator_category = random_access_iterator_tag;
		using value_type = typename iterator_value_type<iterator_>::result;
		using reference = typename select_::reference_type;
		using pointer = typename select_::pointer_type;
		using difference_type = typename iterator_difference_type<iterator_>::result;
	private:
		iterator_ my {};
		difference_type myat = 0;
		bool myinitialized = false;
	public:
		random_access() = default;
		explicit random_access(iterator_ const& a) :
			my{a},
			myinitialized{true}
			{}
		reference operator*() const {
			___water_assert(myinitialized);
			return select_::reference(my);
			}
		pointer operator->() const {
			___water_assert(myinitialized);
			return select_::pointer(my);
			}
		reference operator[](difference_type const& d) const {
			___water_assert(myinitialized);
			return select_::reference(my + d);
			}
		random_access& operator++() {
			___water_assert(myinitialized);
			++my;
			++myat;
			return *this;
			}
		random_access operator++(int) {
			random_access r{*this};
			operator++();
			return r;
			}
		random_access& operator--() {
			___water_assert(myinitialized);
			--my;
			--myat;
			return *this;
			}
		random_access operator--(int) {
			random_access r{*this};
			operator--();
			return r;
			}
		random_access& operator+=(difference_type a) {
			___water_assert(myinitialized);
			my += a;
			myat += a;
			return *this;
			}
		random_access& operator-=(difference_type a) {
			___water_assert(myinitialized);
			my -= a;
			myat -= a;
			return *this;
			}
		random_access operator+(difference_type a) const {
			return random_access{*this} += a;
			}
		friend random_access operator+(difference_type a, random_access b) {
			return b += a;
			}
		random_access operator-(difference_type a) const {
			return random_access{*this} -= a;
			}
		difference_type operator-(random_access const& a) const {
			___water_assert(myinitialized && a.myinitialized);
			return my - a.my;
			}
		bool operator==(random_access const& a) const {
			___water_assert(myinitialized && a.myinitialized);
			return my == a.my;
			}
		bool operator!=(random_access const& a) const {
			___water_assert(myinitialized && a.myinitialized);
			return my != a.my;
			}
		bool operator<(random_access const& a) const {
			___water_assert(myinitialized && a.myinitialized);
			return my < a.my;
			}
		bool operator>(random_access const& a) const {
			___water_assert(myinitialized && a.myinitialized);
			return my > a.my;
			}
		bool operator<=(random_access const& a) const {
			___water_assert(myinitialized && a.myinitialized);
			return my <= a.my;
			}
		bool operator>=(random_access const& a) const {
			___water_assert(myinitialized && a.myinitialized);
			return my >= a.my;
			}
		friend iterator_ underlying(random_access const& a) {
			return a.my;
			}
		friend difference_type at(random_access const& a) {
			return a.myat;
			}
		friend bool initialized(random_access const& a) {
			return a.myinitialized;
			}
	};

template<typename iterator_> using random_access_const = random_access<iterator_, true, false>;
template<typename iterator_> using random_access_proxied = random_access<iterator_, false, true>;
template<typename iterator_> using random_access_proxied_const = random_access<iterator_, true, true>;

template<typename iterator_> random_access<iterator_> random_access_from(iterator_ const& a) {
	return random_access<iterator_>{a};
	}

template<typename iterator_> random_access_const<iterator_> random_access_const_from(iterator_ const& a) {
	return random_access_const<iterator_>{a};
	}

template<typename iterator_> random_access_proxied<iterator_> random_access_proxied_from(iterator_ const& a) {
	return random_access_proxied<iterator_>{a};
	}

template<typename iterator_> random_access_proxied_const<iterator_> random_access_proxied_const_from(iterator_ const& a) {
	return random_access_proxied_const<iterator_>{a};
	}

}}
#endif
