// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_UNICODE_UTF_ITERATORS_HPP
#define WATER_UNICODE_UTF_ITERATORS_HPP
#include <water/unicode/utf.hpp>
namespace water { namespace unicode {

// utf_iterator_with_end
/////////////////////////////////////////////////////////////////////
// template
// - to_ 8 16 32
// - iterator_ the iterator to read from, can be input iterator
// - verify_ true will verify and stop on first error, output will only be valid codepoints
// - from_ 8 16 32, can be same as to_ to just verify
//
// this default constructed will be == end. use end(*this) to make end
//
// it is safe to ++ or read this past the end, it will return 0. this also means it will work as a 0-terminated cstring
//
template<unsigned to_, typename iterator_, bool verify_ = false, unsigned from_ = utf_from_iterator<iterator_>::result> class
 utf_iterator_with_end {
	static_assert(to_ == 8 || to_ == 16 || to_ == 32, "to_ must be 8, 16, 32");
	static_assert(from_ == 8 || from_ == 16 || from_ == 32, "from_ must be 8, 16, 32");
	public:
		using iterator_category = forward_iterator_tag;
		using value_type = typename types::ifel<to_ == 8, char8_t, types::ifel<to_ == 16, char16_t, char32_t>>::result;
		using reference = value_type const&;
		using pointer = value_type const*;
		using difference_type = ptrdiff_t;
	private:
		constexpr static unsigned size_ = to_ == 8 ? 4 : to_ == 16 ? 2 : 1;
		using select_ = typename
			types::ifel<to_ == 8, char8_t,
			types::ifel<to_ == 16, char16_t,
			types::ifel<to_ == 32, char32_t
			> > >::result;
	private:
		iterator_ myfrom, myend;
		value_type my[size_ + 1] {};
		value_type myat = size_;
	public:
		utf_iterator_with_end() = default;
		utf_iterator_with_end(iterator_ begin, iterator_ end) :
			myfrom{begin},
			myend{end}
			{
			next(static_cast<select_*>(0));
			}
		explicit operator bool() const {
			return myat != size_;
			}
		reference operator*() const {
			return my[myat];
			}
		pointer operator->() const {
			return my + myat;
			}
		utf_iterator_with_end& operator++() {
			if(++myat >= size_)
				next(static_cast<select_*>(0));
			return *this;
			}
		utf_iterator_with_end operator++(int) {
			// forward iterators must return iterator from this, would be more efficient with proxy
			utf_iterator_with_end r{*this};
			++*this;
			return r;
			}
		bool operator==(utf_iterator_with_end const& a) const {
			return myat == a.myat && (myat == size_ || myfrom == a.myfrom); // means end will compare true to default constructed
			}
		bool operator!=(utf_iterator_with_end const& a) const {
			return !this->operator==(a);
			}
	private:
		void next(char8_t*) {
			myat = 4;
			char32_t f;
			if(!utf_decode_verify_and_move<from_, verify_>(f, myfrom, myend))
				myfrom = myend;
			else if(f <= 0x7f)
				my[myat = 3] = static_cast<char8_t>(f);
			else if(f <= 0x7ff)
				utf8_pack(my[myat = 2], my[3], f);
			else if(f <= 0xffff)
				utf8_pack(my[myat = 1], my[2], my[3], f);
			else
				utf8_pack(my[myat = 0], my[1], my[2], my[3], f);
			}
		void next(char16_t*) {
			myat = 2;
			char32_t f;
			if(!utf_decode_verify_and_move<from_, verify_>(f, myfrom, myend))
				myfrom = myend;
			else if(f <= 0xffff)
				my[myat = 1] = static_cast<char16_t>(f);
			else
				utf16_pack(my[myat = 0], my[1], f);
			}
		void next(char32_t*) {
			myat = 1;
			if(utf_decode_verify_and_move<from_, verify_>(my[0], myfrom, myend))
				myat = 0;
			else
				myfrom = myend;
			}
	};

template<unsigned to_, typename iterator_, bool verify_, unsigned from_>
 utf_iterator_with_end<to_, iterator_, verify_, from_>
	end(utf_iterator_with_end<to_, iterator_, verify_, from_> const&) {
		return {};
		}

// utf_iterator_with_size
/////////////////////////////////////////////////////////////////////
// template
// - to_ 8 16 32
// - iterator_ the iterator to read from, can be input iterator
// - verify_ true will verify and stop on first error, output will only be valid codepoints
// - from_ 8 16 32, can be same as to_ to just verify
//
// this default constructed will be == end. use end(*this) to make end
//
// it is safe to ++ or read this past the end, it will return 0. this also means it will work as a 0-terminated cstring
//
template<unsigned to_, typename iterator_, bool verify_ = false, unsigned from_ = utf_from_iterator<iterator_>::result> class
 utf_iterator_with_size {
	static_assert(to_ == 8 || to_ == 16 || to_ == 32, "to_ must be 8, 16, 32");
	static_assert(from_ == 8 || from_ == 16 || from_ == 32, "from_ must be 8, 16, 32");
	public:
		using iterator_category = forward_iterator_tag;
		using value_type = typename types::ifel<to_ == 8, char8_t, types::ifel<to_ == 16, char16_t, char32_t>>::result;
		using reference = value_type const&;
		using pointer = value_type const*;
		using difference_type = ptrdiff_t;
	private:
		constexpr static unsigned size_ = to_ == 8 ? 4 : to_ == 16 ? 2 : 1;
		using select_ = typename
			types::ifel<to_ == 8, char8_t,
			types::ifel<to_ == 16, char16_t,
			types::ifel<to_ == 32, char32_t
			> > >::result;
	private:
		iterator_ myfrom;
		size_t mysize = 0;
		value_type my[size_ + 1] {};
		value_type myat = size_;
	public:
		utf_iterator_with_size() = default;
		utf_iterator_with_size(iterator_ begin, size_t size) :
			myfrom{begin},
			mysize{size}
			{
			next(static_cast<select_*>(0));
			}
		explicit operator bool() const {
			return myat != size_;
			}
		reference operator*() const {
			return my[myat];
			}
		pointer operator->() const {
			return my + myat;
			}
		utf_iterator_with_size& operator++() {
			if(++myat >= size_)
				next(static_cast<select_*>(0));
			return *this;
			}
		utf_iterator_with_size operator++(int) {
			// forward iterators must return iterator from this, would be more efficient with proxy
			utf_iterator_with_size r{*this};
			++*this;
			return r;
			}
		bool operator==(utf_iterator_with_size const& a) const {
			return mysize == a.mysize && myat == a.myat; // means end will compare true to default constructed
			}
		bool operator!=(utf_iterator_with_size const& a) const {
			return mysize != a.mysize || myat != a.myat;
			}
	private:
		void next(char8_t*) {
			myat = 4;
			char32_t f;
			if(!utf_decode_verify_and_move<from_, verify_>(f, myfrom, mysize))
				mysize = 0;
			else if(f <= 0x7f)
				my[myat = 3] = static_cast<char8_t>(f);
			else if(f <= 0x7ff)
				utf8_pack(my[myat = 2], my[3], f);
			else if(f <= 0xffff)
				utf8_pack(my[myat = 1], my[2], my[3], f);
			else
				utf8_pack(my[myat = 0], my[1], my[2], my[3], f);
			}
		void next(char16_t*) {
			myat = 2;
			char32_t f;
			if(!utf_decode_verify_and_move<from_, verify_>(f, myfrom, mysize))
				mysize = 0;
			else if(f <= 0xffff)
				my[myat = 1] = static_cast<char16_t>(f);
			else
				utf16_pack(my[myat = 0], my[1], f);
			}
		void next(char32_t*) {
			myat = 1;
			if(utf_decode_verify_and_move<from_, verify_>(my[0], myfrom, mysize))
				myat = 0;
			else
				mysize = 0;
			}
	};
	
template<unsigned to_, typename iterator_, bool verify_, unsigned from_>
 utf_iterator_with_size<to_, iterator_, verify_, from_>
	end(utf_iterator_with_size<to_, iterator_, verify_, from_> const&) {
		return {};
		}

// utf_iterator_from functions
/////////////////////////////////////////////////////////////////////

template<unsigned to_, typename char_> utf_iterator_with_end<to_, char_ const*>
 utf_iterator_from_cstring(char_ const* cstring) {
	auto end = cstring;
	if(end) while(*end) ++end;
	return {cstring, end};
	}

template<unsigned to_, typename iterator_> utf_iterator_with_end<to_, iterator_>
 utf_iterator_from(iterator_ begin, iterator_ end) {
	return {begin, end};
	}

template<unsigned to_, typename iterator_> utf_iterator_with_size<to_, iterator_>
 utf_iterator_from(iterator_ begin, size_t size) {
	return {begin, size};
	}

// utf_range_from
/////////////////////////////////////////////////////////////////////

template<unsigned to_, typename char_> begin_end<utf_iterator_with_end<to_, char_ const*> >
 utf_range_from_cstring(char_ const* cstring) {
	return {utf_iterator_from_cstring<to_>(cstring), {}};
	}

template<unsigned to_, typename iterator_> begin_end<utf_iterator_with_end<to_, iterator_> >
 utf_range_from(iterator_ begin, iterator_ end) {
	return {utf_iterator_from<to_>(begin, end), {}} ;
	}

template<unsigned to_, typename iterator_> begin_end<utf_iterator_with_size<to_, iterator_> >
 utf_range_from(iterator_ begin, size_t size) {
	return {utf_iterator_from<to_>(begin, size), {}};
	}

}}
#endif
