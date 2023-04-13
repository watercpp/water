// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_XTR_UTF_HPP
#define WATER_XTR_UTF_HPP
#include <water/xtr/base.hpp>
#include <water/unicode/utf_iterators.hpp>
#include <water/unicode/utf_length.hpp>
namespace water { namespace xtr {

// utf16 and utf32 using water::unicode

template<typename iterator_, typename settings_, unsigned size_, unsigned utf_>
class write_utf_string :
    public write_size<size_ ? size_ : settings_::string_size>
{
    iterator_ mybegin, myend;

public:
    write_utf_string(iterator_ begin, iterator_ end) :
        mybegin(begin),
        myend(end)
    {}

    write_utf_string(iterator_ cstring) :
        mybegin(cstring),
        myend(_::cstring_end(cstring))
    {}

    template<typename write_>
    void operator()(write_&& write) const {
        unicode::utf_iterator_with_end<8, iterator_, false, utf_> f(mybegin, myend);
        write(f, end(f));
    }
};



template<
    typename iterator_,
    bool if_ =
        unicode::utf_from_char<_::no_const_reference<decltype(*_::make<iterator_&>())>> == 16 ||
        unicode::utf_from_char<_::no_const_reference<decltype(*_::make<iterator_&>())>> == 32
>
struct write_range_select_utf;

template<typename iterator_>
struct write_range_select_utf<iterator_, true> {
    using result = void;
};

template<typename iterator_, typename settings_, unsigned size_>
struct write_range_select<iterator_, settings_, size_, typename write_range_select_utf<iterator_>::result> {
    using result = write_utf_string<
        iterator_,
        settings_,
        size_,
        unicode::utf_from_char<_::no_const_reference<decltype(*_::make<iterator_&>())>> == 16 ? 16 : 32
    >;
};



template<typename char_, typename settings_, bool utf8_ = settings_::utf8>
class write_utf_char :
    public write_size<unicode::utf_from_char<char_> == 16 ? 3 : 4>
{
    static constexpr unsigned utf_ = unicode::utf_from_char<char_>;
    char my[4];
    unsigned mylength = 0;

public:
    write_utf_char(char_ a) {
        if(unicode::utf_from_char<char_> == 16 ? unicode::utf16_is_1_of_1(a) : unicode::utf32_verify(a)) {
            auto *m = static_cast<unsigned char*>(static_cast<void*>(my));
            mylength = unicode::utf8_encode_and_move(m, a);
        }
    }

    template<typename write_>
    void operator()(write_&& write) const {
        // if the char_ was 0, it should be written. write(my, mylegnth) will stop on the first zero
        if(mylength == 1)
            write(my[0]);
        else
            write(my, mylength);
    }
};

template<typename char_, typename settings_>
class write_utf_char<char_, settings_, false> :
    public write_char
{
public:
    write_utf_char(char_ a) :
        write_char(static_cast<char>(a))
    {}
};



unsigned constexpr wchar_utf = unicode::utf_from_char<wchar_t> == 16 ? 16 : 32;

template<typename p_, typename w_>
auto operator<<(expression<p_, w_>&& x, char16_t a) -> expression<expression<p_, w_>, write_utf_char<char16_t, typename expression<p_, w_>::settings>> {
    return {x, a};
}

template<typename p_, typename w_>
auto operator<<(expression<p_, w_>&& x, char32_t a) -> expression<expression<p_, w_>, write_utf_char<char32_t, typename expression<p_, w_>::settings>> {
    return {x, a};
}

template<typename p_, typename w_>
auto operator<<(expression<p_, w_>&& x, wchar_t a) -> expression<expression<p_, w_>, write_utf_char<wchar_t, typename expression<p_, w_>::settings>> {
    return {x, a};
}

template<typename p_, typename w_, size_t size_>
auto operator<<(expression<p_, w_>&& x, char16_t const (&a)[size_]) -> expression<expression<p_, w_>, write_utf_string<char16_t const*, typename expression<p_, w_>::settings, static_cast<unsigned>(3 * size_), 16>> {
    return {x, {a, a + size_}};
}

template<typename p_, typename w_, size_t size_>
auto operator<<(expression<p_, w_>&& x, char32_t const (&a)[size_]) -> expression<expression<p_, w_>, write_utf_string<char32_t const*, typename expression<p_, w_>::settings, static_cast<unsigned>(4 * size_), 32>> {
    return {x, {a, a + size_}};
}

template<typename p_, typename w_, size_t size_>
auto operator<<(expression<p_, w_>&& x, wchar_t const (&a)[size_]) -> expression<expression<p_, w_>, write_utf_string<wchar_t const*, typename expression<p_, w_>::settings, static_cast<unsigned>(wchar_utf == 16 ? 3 * size_ : 4 * size_), wchar_utf>> {
    // keep parenthesis (wchar_utf == 16 ? 3 * size_ : 4 * size_) because vc++
    return {x, {a, a + size_}};
}

template<typename p_, typename w_>
auto operator<<(expression<p_, w_>&& x, char_const_pointer<char16_t> a) -> expression<expression<p_, w_>, write_utf_string<char16_t const*, typename expression<p_, w_>::settings, 0, 16>> {
    return {x, {a.pointer}};
}

template<typename p_, typename w_>
auto operator<<(expression<p_, w_>&& x, char_const_pointer<char32_t> a) -> expression<expression<p_, w_>, write_utf_string<char32_t const*, typename expression<p_, w_>::settings, 0, 32>> {
    return {x, {a.pointer}};
}

template<typename p_, typename w_>
auto operator<<(expression<p_, w_>&& x, char_const_pointer<wchar_t> a) -> expression<expression<p_, w_>, write_utf_string<wchar_t const*, typename expression<p_, w_>::settings, 0, wchar_utf>> {
    return {x, {a.pointer}};
}

}}
#endif
