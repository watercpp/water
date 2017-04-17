// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_XML_BITS_HPP
#define WATER_XML_BITS_HPP
// no dependency on anything
namespace water { namespace xml {

using size_t = decltype(sizeof(0));

template<typename range_, typename result_, typename iterator_ = typename range_::iterator> struct
 if_range {
	using result = result_;
	};

namespace _ {
	template<typename iterator_, typename = size_t> struct
	 size_iterators {
		static size_t do_it(iterator_ begin, iterator_ end) {
			size_t r = 0;
			while(begin != end) {
				++begin;
				++r;
				}
			return r;
			}
		};
	template<typename iterator_> struct
	 size_iterators<iterator_, decltype(static_cast<size_t>(iterator_{} - iterator_{}))> {
		static size_t do_it(iterator_ begin, iterator_ end) {
			return static_cast<size_t>(end - begin);
			}
		};
	}
	
template<typename iterator_> size_t size(iterator_ begin, iterator_ end) {
	return _::size_iterators<iterator_>::do_it(begin, end);
	}

namespace _ {
	template<typename range_, typename = size_t> struct
	 range_size {
		static size_t do_it(range_ const& a) {
			return size(a.begin(), a.end());
			}
		};
	template<typename range_> struct
	 range_size<range_, decltype(static_cast<size_t>(typename range_::size_type{}))> {
		static size_t do_it(range_ const& a) {
			return static_cast<size_t>(a.size());
			}
		};
}
template<typename range_> size_t
 range_size(range_ const& a) {
	return _::range_size<range_>::do_it(a);
	}

// do not have swap_from_swap in this namespace baecause that will cause argument dependant look up
namespace _ {
	namespace hidden_from_name_lookup {
		template<typename t_> void
		 swap(t_& a, t_& b) {
			// anything will be more specialized than this
			t_ s(static_cast<t_&&>(a));
			a = static_cast<t_&&>(b);
			b = static_cast<t_&&>(s);
			}
		template<typename t_, size_t s_> void
		 swap(t_ (&a)[s_], t_ (&b)[s_]) {
			size_t i = 0;
			do swap(a[i], b[i]); while(++i != s_);
			}
		}
	template<typename a_> void
	 swap_from_swap(a_& a, a_& b) {
		using hidden_from_name_lookup::swap;
		swap(a, b);
		}
}

namespace _ {
	template<typename type_> struct text_char_type { using result = type_; };
	template<typename type_> struct text_char_type<type_&> { using result = type_; };
	template<typename type_> struct text_char_type<type_ const> { using result = type_; };
	template<typename type_> struct text_char_type<type_ const&> { using result = type_; };
	}

template<typename iterator_> class
 text {
	public:
		using iterator = iterator_;
		using char_type = typename _::text_char_type<decltype(*iterator{})>::result;
		using value_type = char_type;
		using size_type = size_t;
	private:
		iterator myb{}, mye{};
	public:
		text() = default;
		text(iterator begin, iterator end) :
			myb{begin},
			mye{end}
			{}
		text(iterator begin, size_type size) :
			myb{begin},
			mye{begin + size}
			{}
		explicit operator bool() const {
			return myb != mye;
			}
		iterator begin() const {
			return myb;
			}
		iterator end() const {
			return mye;
			}
		size_type size() const {
			return static_cast<size_type>(mye - myb);
			}
		decltype(iterator{}[0]) operator[](decltype(iterator{} - iterator{}) a) {
			return myb[a];
			}
	};
	
template<typename iterator_> text<iterator_>
 text_from(iterator_ begin, iterator_ end) {
 	return { begin, end };
 	}
template<typename iterator_> text<iterator_>
 text_from(iterator_ begin, size_t size) {
 	return { begin, size };
 	}
	
template<typename char_> inline char32_t text_compare_cast(char_ a) { return static_cast<char32_t>(a); }
inline char16_t      text_compare_cast(char16_t a)      { return a; }
inline unsigned char text_compare_cast(unsigned char a) { return a; }
inline unsigned char text_compare_cast(signed char a)   { return static_cast<unsigned char>(a); }
inline unsigned char text_compare_cast(char a)          { return static_cast<unsigned char>(a); }
 
template<typename a_, typename b_> bool
 operator==(text<a_> const& a, text<b_> const& b) {
	if(a.size() != b.size())
		return false;
	auto ai = a.begin();
	auto bi = b.begin();
	while(ai != a.end() && text_compare_cast(*ai) == text_compare_cast(*bi)) ++ai, ++bi;
	return ai == a.end();
	}

template<typename a_, typename b_> bool
 operator!=(text<a_> const& a, text<b_> const& b) {
	return !(a == b);
	}

template<typename iterator_, size_t size_> bool
 operator==(text<iterator_> const& a, char const (&b)[size_]) {
	return a == text_from(b, size_ - 1);
	}
template<typename iterator_, size_t size_> bool
 operator==(char const (&a)[size_], text<iterator_> const& b) {
	return text_from(a, size_ - 1) == b;
	}
template<typename iterator_, size_t size_> bool
 operator!=(text<iterator_> const& a, char const (&b)[size_]) {
	return a != text_from(b, size_ - 1);
	}
template<typename iterator_, size_t size_> bool
 operator!=(char const (&a)[size_], text<iterator_> const& b) {
	return text_from(a, size_ - 1) != b;
	}

}}
#endif
