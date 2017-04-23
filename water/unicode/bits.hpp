// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_UNICODE_BITS_HPP
#define WATER_UNICODE_BITS_HPP
#include <water/water.hpp>
#include <water/types/types.hpp>
#include <water/iterator.hpp>
namespace water { namespace unicode {

using size_t = decltype(sizeof(0));

using char8_t = unsigned char;
	
template<typename> struct
 to_void {
	using result = void;
	};
	
template<typename iterator_, typename result_, typename category_ = typename iterator_category<iterator_>::result> struct
 if_random_access
	{};
template<typename iterator_, typename result_> struct
 if_random_access<iterator_, result_, random_access_iterator_tag> {
	using result = result_;
	};
 
template<typename iterator_, typename result_, typename category_ = typename iterator_category<iterator_>::result> struct
 if_not_random_access {
	using result = result_;
	};
template<typename iterator_, typename result_> struct
 if_not_random_access<iterator_, result_, random_access_iterator_tag>
	{};
	
template<typename iterator_> typename if_random_access<iterator_, size_t>::result
 size(iterator_ begin, iterator_ end) {
	return static_cast<size_t>(end - begin);
	}

template<typename iterator_> typename if_not_random_access<iterator_, size_t>::result
 size(iterator_ begin, iterator_ end) {
	size_t r = 0;
	while(begin != end) {
		++begin;
		++r;
		}
	return r;
	}

template<typename iterator_> class
 begin_end {
	public:
		using iterator = iterator_;
	private:
		iterator mybegin, myend;
	public:
		begin_end() : mybegin{}, myend{} {}
		begin_end(iterator begin, iterator end) : mybegin{begin}, myend{end} {}
		explicit operator bool() const { return mybegin != myend; }
		iterator begin() const { return mybegin; }
		iterator end() const { return myend; }
	};

template<typename iterator_> class
 begin_size {
	public:
		using iterator = iterator_;
		using size_type = size_t;
	private:
		iterator_ mybegin;
		size_type mysize;
	public:
		begin_size() : mybegin{}, mysize{} {}
		begin_size(iterator begin, size_type size) : mybegin{begin}, mysize{size} {}
		explicit operator bool() const { return mysize != 0; }
		iterator begin() const { return mybegin; }
		size_type size() const { return mysize; }
	};

template<typename char_> inline char32_t cast(char_ a) { return static_cast<char32_t>(a); }
inline char16_t cast(char16_t a) { return a; }
inline char8_t cast(char8_t a) { return a; }
inline char8_t cast(char a) { return static_cast<char8_t>(a); }
inline char8_t cast(signed char a) { return static_cast<char8_t>(a); }

}}
#endif
