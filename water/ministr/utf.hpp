// Copyright 2017-2018 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_MINISTR_UTF_HPP
#define WATER_MINISTR_UTF_HPP
#include <water/ministr/out.hpp>
#include <water/unicode/utf_iterators.hpp>
#include <water/unicode/utf_length.hpp>
namespace water { namespace ministr {

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
        myend(cstring + length<false>(cstring))
    {}

    template<typename iterator2_>
    iterator2_ operator()(iterator2_ begin, iterator2_ end) const {
        unicode::utf_length<utf_> length(mybegin, myend);
        unicode::utf_iterator_with_end<8, iterator_, false, utf_> f(mybegin, myend);
        if(!length.utf8())
            return begin;
        unsigned s = static_cast<unsigned>(end - begin);
        if(s >= length.utf8())
            s = static_cast<unsigned>(length.utf8());
        else
            s = _::utf8_cut(f, s);
        end = begin + s;
        while(begin != end) {
            auto c = static_cast<no_const_reference<decltype(*begin)>>(*f);
            if(!c) break;
            *begin++ = c;
            ++f;
        }
        return begin;
    }
};



template<
    typename iterator_,
    bool if_ =
        unicode::utf_from_char<no_const_reference<decltype(*_::make<iterator_&>())>>::result == 16 ||
        unicode::utf_from_char<no_const_reference<decltype(*_::make<iterator_&>())>>::result == 32
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
        unicode::utf_from_char<no_const_reference<decltype(*_::make<iterator_&>())>>::result == 16 ? 16 : 32
    >;
};



template<typename char_, typename settings_, bool utf8_ = settings_::utf8>
class write_utf_char :
    public write_size<unicode::utf_from_char<char_>::result == 16 ? 3 : 4>
{
    static constexpr unsigned utf_ = unicode::utf_from_char<char_>::result;
    char my[4];
    unsigned mylength = 0;

public:
    write_utf_char(char_ a) {
        if(unicode::utf_from_char<char_>::result == 16 ? unicode::utf16_is_1_of_1(a) : unicode::utf32_verify(a)) {
            auto *m = static_cast<unsigned char*>(static_cast<void*>(my));
            mylength = unicode::utf8_encode_and_move(m, a);
        }
    }

    template<typename iterator_>
    iterator_ operator()(iterator_ begin, iterator_ end) const {
        if(static_cast<unsigned>(end - begin) < mylength) return begin;
        end = begin + mylength;
        char const *m = my;
        while(begin != end) *begin++ = *m++;
        return end;
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



template<typename char_>
struct utf_char_const_pointer {
    char_ const *pointer;
    utf_char_const_pointer(char_ const* a) : pointer{a} {}
};

unsigned constexpr wchar_utf = unicode::utf_from_char<wchar_t>::result == 16 ? 16 : 32;

template<typename p_, typename w_>
out<out<p_, w_>, write_utf_char<char16_t, typename out<p_, w_>::settings>> operator<<(
    out<p_, w_> const& o,
    char16_t a
) {
    return {o, a};
}

template<typename p_, typename w_>
out<out<p_, w_>, write_utf_char<char32_t, typename out<p_, w_>::settings>> operator<<(
    out<p_, w_> const& o,
    char32_t a
) {
    return {o, a};
}

template<typename p_, typename w_>
out<out<p_, w_>, write_utf_char<wchar_t, typename out<p_, w_>::settings>> operator<<(
    out<p_, w_> const& o,
    wchar_t a
) {
    return {o, a};
}

template<typename p_, typename w_, unsigned size_>
out<out<p_, w_>, write_utf_string<char16_t const*, typename out<p_, w_>::settings, 3 * size_, 16>> operator<<(
    out<p_, w_> const& o,
    char16_t const (&a)[size_]
) {
    return {o, {a, a + size_}};
}

template<typename p_, typename w_, unsigned size_>
out<out<p_, w_>, write_utf_string<char32_t const*, typename out<p_, w_>::settings, 4 * size_, 32>> operator<<(
    out<p_, w_> const& o,
    char32_t const (&a)[size_]
) {
    return {o, {a, a + size_}};
}

template<typename p_, typename w_, unsigned size_>
out<out<p_, w_>, write_utf_string<wchar_t const*, typename out<p_, w_>::settings, (wchar_utf == 16 ? 3 * size_ : 4 * size_), wchar_utf>> operator<<(
    out<p_, w_> const& o,
    wchar_t const (&a)[size_]
) {
    // keep parenthesis (wchar_utf == 16 ? 3 * size_ : 4 * size_) because vc++
    return {o, {a, a + size_}};
}

template<typename p_, typename w_>
out<out<p_, w_>, write_utf_string<char16_t const*, typename out<p_, w_>::settings, 0, 16> > operator<<(
    out<p_, w_> const& o,
    utf_char_const_pointer<char16_t> a
) {
    return {o, {a.pointer}};
}

template<typename p_, typename w_>
out<out<p_, w_>, write_utf_string<char32_t const*, typename out<p_, w_>::settings, 0, 32> > operator<<(
    out<p_, w_> const& o,
    utf_char_const_pointer<char32_t> a
) {
    return {o, {a.pointer}};
}

template<typename p_, typename w_>
out<out<p_, w_>, write_utf_string<wchar_t const*, typename out<p_, w_>::settings, 0, wchar_utf> > operator<<(
    out<p_, w_> const& o,
    utf_char_const_pointer<wchar_t> a
) {
    return {o, {a.pointer}};
}

}}
#endif
