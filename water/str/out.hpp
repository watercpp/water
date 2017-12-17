// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_STR_OUT_HPP
#define WATER_STR_OUT_HPP
#include <water/int.hpp>
#include <water/numbers/write.hpp>
#include <water/unicode/utf.hpp>
#include <water/unicode/utf_iterators.hpp>
#include <water/swap.hpp>
namespace water { namespace str {

using numbers::settings;

class restore_settings_move {
	settings
		*mypointer = 0,
		mysettings;
	public:
		restore_settings_move(restore_settings_move const&) = delete;
		restore_settings_move& operator=(restore_settings_move const&) = delete;
		restore_settings_move(restore_settings_move* (&)())
			{}
		explicit restore_settings_move(settings& a) :
			mypointer{&a},
			mysettings{a}
			{}
		restore_settings_move(restore_settings_move&& a) :
			mypointer{a.mypointer},
			mysettings{a.mysettings}
			{
			a.mypointer = 0;
			}
		~restore_settings_move() {
			if(mypointer) *mypointer = mysettings;
			}
		restore_settings_move& operator=(restore_settings_move&& a) {
			swap_from_swap(mypointer, a.mypointer);
			swap_from_swap(mysettings, a.mysettings);
			return *this;
			}
	};

namespace _ {

	template<typename a_, typename = void> struct
	 locale {
		static numbers::locale from(a_&) {
			return {};
			}
		};
	template<typename a_> struct
	 locale<a_, types::to_void<decltype(types::make<a_&>().locale())>> {
		static auto from(a_& a) -> decltype(a.locale()) {
			return a.locale();
			}
		};

	template<typename a_, typename = void> struct
	 default_settings {
		#ifdef WATER_STR_DEFAULT_SETTINGS
		static auto from(a_ const&) -> decltype(WATER_STR_DEFAULT_SETTINGS) {
			return WATER_STR_DEFAULT_SETTINGS;
			}
		#else
		static constexpr unsigned from(a_ const&) { // means out constructor can still be constexpr
			return 0;
			}
		#endif
		};
	template<typename a_> struct
	 default_settings<a_, types::to_void<decltype(types::make<a_&>().default_settings())>> {
		static auto from(a_& a) -> decltype(a.default_settings()) {
			return a.default_settings();
			}
		};

	template<typename a_, typename = void> struct
	 flush {
	 	static void do_it(a_&) {}
	 	};
	template<typename a_> struct
	 flush<a_, types::to_void<decltype(types::make<a_&>().flush())>> {
	 	static void do_it(a_& a) { a.flush(); }
	 	};

	template<typename a_, typename = void> struct char_type_of : types::type_plain<char> {};
	template<typename a_> struct char_type_of<a_, types::to_void<typename a_::char_type>> : types::type_plain<typename a_::char_type> {};

}
template<typename a_> using char_type_of = typename _::char_type_of<a_>::result;

template<typename write_> class
 out : public write_ {
	public:
		using char_type = char_type_of<write_>;
		static unsigned constexpr utf = unicode::utf_from_char<char_type>::result;
	private:
		str::settings mysettings;
	public:
		out() : // not default beccause visual c++ seems to ignore it when the tempalte list constructor exists
			write_{}, // this is also needed
			mysettings{_::default_settings<write_>::from(*this)} // base is constructed
			{}
		out(out const& a) :
			write_{static_cast<write_ const&>(a)}, // visual c++
			mysettings{a.mysettings}
			{}
		out(out&& a):
			write_{static_cast<write_&&>(a)}, // visual c++
			mysettings{static_cast<str::settings&&>(a.mysettings)}
			{}
		template<typename ...arguments_> out(arguments_&&... a) :
			write_{static_cast<arguments_&&>(a)...},
			mysettings{_::default_settings<write_>::from(*this)}
			{}
		template<typename ...arguments_> out(str::settings s, arguments_&&... a) :
			write_{static_cast<arguments_&&>(a)...},
			mysettings{s}
			{}
		str::settings& settings() {
			return mysettings;
			}
		str::settings const& settings() const {
			return mysettings;
			}
		friend restore_settings_move restore_settings(out<write_>& a) {
			return restore_settings_move{a.settings()};
			}
		template<typename iterator_> out& operator()(iterator_ begin, iterator_ end) {
			if(begin != end)
				write(begin, end, typename types::ifel<utf == unicode::utf_from_iterator<iterator_>::result, short*, long*>::result{});
			return *this;
			}
		template<typename char_> out& operator()(char_ *begin, char_ *end) {
			___water_assert(begin == end || (begin && begin < end));
			if(begin && begin < end)
				write(begin, end, typename types::ifel<utf == unicode::utf_from_iterator<char_*>::result, short*, long*>::result{});
			return *this;
			}
		template<typename char_> out& operator()(char_ *cstring) { // not char_ const* because then non-const pointers go to single char_ operator
			if(cstring && *cstring) {
				auto end = cstring;
				while(*++end);
				write(cstring, end, typename types::ifel<utf == unicode::utf_from_char<char_>::result, short*, long*>::result{});
				}
			return *this;
			}
		template<typename char_> out& operator()(char_ *cstring, size_t max_size) {
			// for char arrays
			___water_assert((cstring || !max_size) && max_size < static_cast<size_t>(-1) / 2);
			if(cstring && max_size && *cstring) {
				auto
					stop = cstring + max_size,
					end = cstring;
				while(++end != stop && *end);
				write(cstring, end, typename types::ifel<utf == unicode::utf_from_char<char_>::result, short*, long*>::result{});
				}
			return *this;
			}
		template<typename char_> out& operator()(char_ a) {
			write(a, typename types::ifel<utf >= unicode::utf_from_char<char_>::result, short*, long*>::result{});
			return *this;
			}
		template<typename number_> out& number(number_ a) {
			numbers::write<number_, utf> write(a, mysettings);
			write.buffered(*this, locale());
			return *this;
			}
		auto locale() -> decltype(_::locale<write_>::from(types::make<write_&>())) {
			return _::locale<write_>::from(*this);
			}
		out& flush() {
			_::flush<write_>::do_it(*this);
			return *this;
			}
		void swap(out& a) {
			swap_from_swap(mysettings, a.mysettings);
			swap_from_swap(static_cast<write_&>(*this), static_cast<write_&>(a));
			}
	private:
		template<typename iterator_> void write(iterator_ begin, iterator_ end, short*) {
			(*static_cast<write_*>(this))(begin, end);
			}
		template<typename iterator_> void write(iterator_ begin, iterator_ end, long*) {
			unicode::utf_iterator_with_end<utf, iterator_> b{begin, end}, e;
			(*static_cast<write_*>(this))(b, e);
			}
		template<typename char_> void write(char_ a, short*) {
			(*static_cast<write_*>(this))(&a, &a + 1);
			}
		template<typename char_> void write(char_ a, long*) {
			char_type x[4];
			unsigned n = unicode::utf_encode<utf>(x, unicode::cast(a));
			(*static_cast<write_*>(this))(x + 0, x + n);
			}
	};

template<typename write_> void swap(out<write_>& a, out<write_>& b) {
	a.swap(b);
	}

#if 0

class write_minimal {
	public:
		template<typename iterator_> void operator()(iterator_ begin, iterator_ end) {
			// write this range of characters to somewhere
			}
	};

class write_complete {
	public:
		using char_type = some_type; // used to select utf. default is char
	public:
		template<typename iterator_> void operator()(iterator_ begin, iterator_ end) {
			// write this range of characters to somewhere
			}
		void flush() {
			// if this buffers output, flush the buffer
			}
		str::settings default_settings() {
			// used to set settings in the constructor
			}
		locale_type const& locale() {
			// return a locale that works with water::numbers
			}
	};

#endif

// restore settings at the end of the scope:
//
// auto r = restore_settings(out)
// out << base(16) << 123;
// out << base(2) << 123;
//
// restore settings at the end of the expression:
//
// out << restore_settings << base(16) << 123;

// this is a friend inside out because visual c++ did not like the << restore_settings operator otherwise
// template<typename write_> restore_settings_move restore_settings(out<write_>& o) {
//     return restore_settings_move{o.settings()};
//     }

inline restore_settings_move* restore_settings() {
	return 0;
	}

template<typename write_> out<write_>& operator<<(out<write_>& o, restore_settings_move&& a) {
	a = restore_settings_move{o.settings()};
	return o;
	}

// operator<<(out, char const(&)[size_])
// operator<<(out, char const*)
// is solved by:
// specific<char const*> means the array operator is prefered over char const*
// void_const_pointer is needed to not make char const* go to void const*
// then void_const_pointer_if disables the constructor for char const*
// after that if_bool is needed, a plain bool operator would be used for pointers

// useful to avoid implicit conversions for the << operators
template<typename type_> class specific {
	type_ my;
	public:
		specific(type_ a) : my{a} {}
		type_ get() { return my; }
	};

template<typename write_, typename type_> out<write_>& operator<<(out<write_>&& o, type_ const& t) {
	return o << t;
	}

// char

template<typename write_> out<write_>& operator<<(out<write_>& o, char a) {
	return o(a);
	}

template<typename write_> out<write_>& operator<<(out<write_>& o, char16_t a) {
	return o(a);
	}

template<typename write_> out<write_>& operator<<(out<write_>& o, char32_t a) {
	return o(a);
	}

template<typename write_> out<write_>& operator<<(out<write_>& o, wchar_t a) {
	return o(a);
	}

// numbers

template<typename if_, typename result_> struct if_bool;
template<typename result_> struct if_bool<bool, result_> { using result = result_; };

template<typename write_, typename bool_> typename if_bool<bool_, out<write_>&>::result operator<<(out<write_>& o, bool_ a) {
	return o.number(a);
	}

template<typename write_> out<write_>& operator<<(out<write_>& o, int a) {
	return o.number(a);
	}

template<typename write_> out<write_>& operator<<(out<write_>& o, long a) {
	return o.number(a);
	}

template<typename write_> out<write_>& operator<<(out<write_>& o, long long a) {
	return o.number(a);
	}

template<typename write_> out<write_>& operator<<(out<write_>& o, unsigned int a) {
	return o.number(a);
	}

template<typename write_> out<write_>& operator<<(out<write_>& o, unsigned long a) {
	return o.number(a);
	}

template<typename write_> out<write_>& operator<<(out<write_>& o, unsigned long long a) {
	return o.number(a);
	}

template<typename write_> out<write_>& operator<<(out<write_>& o, float a) {
	return o.number(a);
	}

template<typename write_> out<write_>& operator<<(out<write_>& o, double a) {
	return o.number(a);
	}

template<typename write_> out<write_>& operator<<(out<write_>& o, long double a) {
	return o.number(a);
	}

// pointer

template<typename type_, typename = decltype(static_cast<void const*>(0) == static_cast<type_ const*>(0))> struct void_const_pointer_if { using result = void_const_pointer_if; };
template<> struct void_const_pointer_if<char, bool> {};
template<> struct void_const_pointer_if<char16_t, bool> {};
template<> struct void_const_pointer_if<char32_t, bool> {};
template<> struct void_const_pointer_if<wchar_t, bool> {};

struct void_const_pointer {
	void const *pointer;
	template<typename type_> void_const_pointer(type_ const* pointer, typename void_const_pointer_if<type_>::result = {}) : pointer{pointer} {}
	};

template<typename write_> out<write_>& operator<<(out<write_>& o, void_const_pointer a) {
	auto r = restore_settings(o);
	if(!o.settings().base())
		o.settings().base(16);
	return o.number(reinterpret_cast<typename types::if_not_void<uint_size_at_least<sizeof(void*)>, size_t>::result>(a.pointer));
	}

// cstrings

template<typename write_> out<write_>& operator<<(out<write_>& o, specific<char const*> a) {
	return o(a.get());
	}

template<typename write_> out<write_>& operator<<(out<write_>& o, specific<char16_t const*> a) {
	return o(a.get());
	}

template<typename write_> out<write_>& operator<<(out<write_>& o, specific<char32_t const*> a) {
	return o(a.get());
	}

template<typename write_> out<write_>& operator<<(out<write_>& o, specific<wchar_t const*> a) {
	return o(a.get());
	}

// arrays

template<typename write_, size_t size_> out<write_>& operator<<(out<write_>& o, char const (&a)[size_]) {
	return o(a, size_);
	}

template<typename write_, size_t size_> out<write_>& operator<<(out<write_>& o, char16_t const (&a)[size_]) {
	return o(a, size_);
	}

template<typename write_, size_t size_> out<write_>& operator<<(out<write_>& o, char32_t const (&a)[size_]) {
	return o(a, size_);
	}

template<typename write_, size_t size_> out<write_>& operator<<(out<write_>& o, wchar_t const (&a)[size_]) {
	return o(a, size_);
	}

// begin end range

template<typename write_, typename begin_end_> numbers::if_begin_end<begin_end_, out<write_>&> operator<<(out<write_>& o, begin_end_ const& a) {
	return o(a.begin(), a.end());
	}

// settigns

template<typename write_> out<write_>& operator<<(out<write_>& o, settings a) {
	o.settings() = a;
	return o;
	}

// callback

template<typename derived_> class callback {};

template<typename write_, typename function_> out<write_>& operator<<(out<write_>& o, callback<function_>& a) {
	static_cast<function_&>(a)(o);
	return o;
	}

template<typename write_, typename function_> out<write_>& operator<<(out<write_>& o, callback<function_>&& a) {
	static_cast<function_&>(a)(o);
	return o;
	}

template<typename write_, typename function_> out<write_>& operator<<(out<write_>& o, callback<function_> const& a) {
	static_cast<function_ const&>(a)(o);
	return o;
	}

// callbacks

struct flush_do : callback<flush_do> {
	template<typename write_> void operator()(out<write_>& o) const { o.flush(); };
	} constexpr flush {};

struct el_do : callback<el_do> {
	template<typename write_> void operator()(out<write_>& o) const {
		o(static_cast<char>(u'\n'));
		o.flush();
		};
	} constexpr el {};

template<typename type_> class
 repeat_do : public callback<repeat_do<type_>> {
	type_ my;
	size_t mytimes;
	public:
		repeat_do(type_ const& value, size_t times) :
			my{value},
			mytimes{times}
			{}
		template<typename write_> void operator()(out<write_>& o) const {
			size_t times = mytimes;
			while(times) {
				o << my;
				--times;
				}
			}
	};

template<typename type_> repeat_do<type_> repeat(type_ const& value, size_t times) {
	return {value, times};
	}

template<typename number_> class
 number_with_settings : public callback<number_with_settings<number_>> {
	number_ my;
	settings mysettings;
	public:
		number_with_settings(number_ n, settings s) :
			my{n},
			mysettings{s}
			{}
		template<typename write_> void operator()(out<write_>& o) const {
			auto r = restore_settings(o);
			o.settings() = mysettings;
			o.number(my);
			}
	};

template<typename number_> number_with_settings<number_> number(number_ t, settings s = {}) { return {t, s}; }

template<typename iterator_> class
 string_do : public callback<string_do<iterator_>> {
	iterator_ mybegin, myend;
	public:
		string_do(iterator_ begin, iterator_ end) :
			mybegin{begin},
			myend{end}
			{}
		template<typename write_> void operator()(out<write_>& o) const {
			o(mybegin, myend);
			}
	};

template<typename iterator_> string_do<iterator_> string(iterator_ begin, iterator_ end) {
	return {begin, end};
	}

// setting callbacks

class base : public callback<base> {
	unsigned my;
	public:
		explicit constexpr base(unsigned a) : my{a} {}
		template<typename write_> void operator()(out<write_>& o) const { o.settings().base(my); }
	};

class base_prefix_suffix : public callback<base_prefix_suffix> {
	bool my;
	public:
		explicit constexpr base_prefix_suffix(bool a) : my{a} {}
		template<typename write_> void operator()(out<write_>& o) const { o.settings().base_prefix_suffix(my); }
	};

class bool_text : public callback<bool_text> {
	bool my;
	public:
		explicit constexpr bool_text(bool a) : my{a} {}
		template<typename write_> void operator()(out<write_>& o) const { o.settings().bool_text(my); }
	};

class digits : public callback<digits> {
	unsigned my;
	public:
		explicit constexpr digits(unsigned a) : my{a} {}
		template<typename write_> void operator()(out<write_>& o) const { o.settings().digits(my); }
	};

struct exponent_do : public callback<exponent_do> {
	template<typename write_> void operator()(out<write_>& o) const { o.settings().exponent(); }
	} constexpr exponent {};

class fraction_digits : public callback<fraction_digits> {
	unsigned my;
	public:
		explicit constexpr fraction_digits(unsigned a) : my{a} {}
		template<typename write_> void operator()(out<write_>& o) const { o.settings().fraction_digits(my); }
	};

class group : public callback<group> {
	bool my;
	public:
		explicit constexpr group(bool a) : my{a} {}
		template<typename write_> void operator()(out<write_>& o) const { o.settings().group(my); }
	};

struct no_exponent_do : public callback<no_exponent_do> {
	template<typename write_> void operator()(out<write_>& o) const { o.settings().no_exponent(); }
	} constexpr no_exponent {};

class no_exponent_min_max : public callback<no_exponent_min_max> {
	int mymin;
	int mymax;
	public:
		explicit constexpr no_exponent_min_max(int min, int max) : mymin{min}, mymax{max} {}
		template<typename write_> void operator()(out<write_>& o) const { o.settings().no_exponent_min_max(mymin, mymax); }
	};

class plus : public callback<plus> {
	bool my;
	public:
		explicit constexpr plus(bool a) : my{a} {}
		template<typename write_> void operator()(out<write_>& o) const { o.settings().plus(my); }
	};

class precision : public callback<precision> {
	unsigned my;
	public:
		explicit constexpr precision(unsigned a) : my{a} {}
		template<typename write_> void operator()(out<write_>& o) const { o.settings().precision(my); }
	};

class scientific : public callback<scientific> {
	bool my;
	public:
		explicit constexpr scientific(bool a) : my{a} {}
		template<typename write_> void operator()(out<write_>& o) const { o.settings().scientific(my); }
	};

class trailing_zeros : public callback<trailing_zeros> {
	bool my;
	public:
		explicit constexpr trailing_zeros(bool a) : my{a} {}
		template<typename write_> void operator()(out<write_>& o) const { o.settings().trailing_zeros(my); }
	};

}}
#endif
