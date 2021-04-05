// Copyright 2017-2021 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_MINISTR_OUT_HPP
#define WATER_MINISTR_OUT_HPP
// no dependency!
namespace water { namespace ministr {

// settings
//
// string_size_
// the size reserved for strings when no size is specified
//
// extra_size_
// extra characters in addition to the known size. the extra space is used if a string is longer
// than it's reserved size. the leftover space from numbers or strings that did not use all of the
// reseved space can also be used.
//
// utf8_
// when true all strings are expected to be utf-8. all strings are verfied up to the first 0
// character and discarded if they are not valid. if a string needs to be cut when written it will
// not be cut in the middle of a utf-8 sequence.
// writing 1 char at a time is not validated, it is possible to write bad utf-8
//
// end_
// if not 0, each write ends with this. set this to lf to always end with a linebreak. it will be
// added to the end unless it already ends with it.
// this means it does not matter if the string ends with a linebreak or not. and it is possible to
// make it end with two linebreaks, just write 2 linebreaks.
//
template<
    unsigned string_size_ = 64,
    unsigned extra_size_ = 128,
    bool utf8_ = true,
    char end_ = 0
>
struct settings {
    static constexpr unsigned
        string_size = string_size_,
        extra_size = extra_size_;
    static constexpr bool utf8 = utf8_;
    static constexpr char end = end_;
};

template<typename settings_, unsigned string_size_>
using settings_string_size = settings<
    string_size_,
    settings_::extra_size,
    settings_::utf8,
    settings_::end
>;

template<typename settings_, unsigned extra_size_>
using settings_extra_size = settings<
    settings_::string_size,
    extra_size_,
    settings_::utf8,
    settings_::end
>;

template<typename settings_, bool utf8_>
using settings_utf8 = settings<
    settings_::string_size,
    settings_::extra_size,
    utf8_,
    settings_::end
>;

template<typename settings_, char end_>
using settings_end = settings<
    settings_::string_size,
    settings_::extra_size,
    settings_::utf8,
    end_
>;

unsigned constexpr
    number_uppercase = 1 << 0,
    number_lowercase = 1 << 1,
    number_mixedcase = number_uppercase | number_lowercase, // also 0
    number_show_plus = 1 << 2,
    number_hide_base = 1 << 3;

// number_format
//
// base_
// can be 2 8 10 16
// pointers are always written in base 16
//
// digits_
// 0: use as many digits as needed
// -1: always use the maximum number of digits for that type (same as 0 for floatingpoint)
// 1...n: use exactly this many digits. floatingpoint will not round, and with no_exponent_max_ > 0 might write more digits
//
// no_exponent_min_ <= 0 (only used for base 10 floating point)
// 0: always use exponent when exponent < 0
// -1 or less: no exponent when exponent < 0 && exponent >= no_exponent_min_ (0.00123 instead of 1.23e-3)
//
// no_exponent_max_ >= -1 (only used for base 10 floating point)
// 0: always use exponent when exponent >= 0
// 1 or more: no exponent when exponent >= 0 && exponent <= no_exponent_max_
// -1: no exponent when exponent >= 0 && exponent <= mantissa digits. means 123.45 instead of 1.2345e2
//
// flags_
// number_uppercase: uppercase is used for all letters in a number including true false nan infinity
// number_lowercase: lowercase is used for all letters in a number
// number_mixedcase: (default) will use uppercase hex-digits, exponent EP, nan as NaN, the rest lowercase
// number_show_plus: write +
// number_hide_base: do not write 0x 0o 0b
//
template<
    unsigned base_ = 0,
    int digits_ = 0,
    int no_exponent_min_ = 0,
    int no_exponent_max_ = -1,
    unsigned flags_ = 0
>
struct number_format {
    static constexpr unsigned
        base = base_ == 2 || base_ == 8 || base_ == 16 ? base_ : 10,
        flags = flags_;
    static constexpr int
        no_exponent_min = no_exponent_min_ < 0 ? no_exponent_min_ : 0,
        no_exponent_max = no_exponent_max_ > -1 ? no_exponent_max_ : -1, // -1 means use exponent when it is withing the types signifcant digits
        digits = digits_ < 0 ? -1 : digits_;
    static constexpr bool
        uppercase = (flags & number_mixedcase) == number_uppercase,
        lowercase = (flags & number_mixedcase) == number_lowercase,
        mixedcase = !uppercase && !lowercase,
        show_plus = (flags & number_show_plus) != 0,
        hide_base = (flags & number_hide_base) != 0;
};

template<typename number_format_, unsigned base_>
using number_format_base = number_format<
    base_,
    number_format_::digits,
    number_format_::no_exponent_min,
    number_format_::no_exponent_max,
    number_format_::flags
>;

template<typename number_format_, int digits_>
using number_format_digits = number_format<
    number_format_::base,
    digits_,
    number_format_::no_exponent_min,
    number_format_::no_exponent_max,
    number_format_::flags
>;

template<typename number_format_, int min_, int max_>
using number_format_no_exponent_min_max = number_format<
    number_format_::base,
    number_format_::digits,
    min_,
    max_,
    number_format_::flags
>;

template<typename number_format_, unsigned flags_>
using number_format_flags = number_format<
    number_format_::base,
    number_format_::digits,
    number_format_::no_exponent_min,
    number_format_::no_exponent_max,
    flags_
>;

template<typename number_format_, int case_>
using number_format_case = number_format_flags<
    number_format_,
    (number_format_::flags & ~number_mixedcase) | (case_ > 0 ? number_uppercase : case_ < 0 ? number_lowercase : 0)
>;

template<typename number_format_, bool show_>
using number_format_show_plus = number_format_flags<
    number_format_,
    show_ ? number_format_::flags | number_show_plus : number_format_::flags & ~number_show_plus
>;

template<typename number_format_, bool hide_>
using number_format_hide_base = number_format_flags<
    number_format_,
    hide_ ? number_format_::flags | number_hide_base : number_format_::flags & ~number_hide_base
>;

template<typename number_format_ = number_format<>, typename settings_ = settings<>>
struct configuration {
    using settings = settings_;
    using number_format = number_format_;
};

using configuration_default =
    #ifdef WATER_MINISTR_CONFIGURATION
    WATER_MINISTR_CONFIGURATION;
    #else
    configuration<>;
    #endif

using number_format_default = configuration_default::number_format;
using settings_default = configuration_default::settings;




template<typename function_ = void, typename configuration_ = void>
class out;

namespace _ {

    // array version of...
    // template<typename function_, typename configuration_, unsigned size_>
    //  void function(out<function_, configuration_> const& o, char const (&array)[size_], unsigned length) {
    // ...caused comile times on clang to explode.
    
    template<typename configuration_>
    void function(out<void, configuration_> const&, char const* /*begin*/, char const* /*end*/) {
    }
    
    template<typename function_, typename configuration_>
    void function(out<function_, configuration_> const& o, char const *begin, char const *end) {
        o(begin, end);
    }
    
    template<typename previous1_, typename previous2_, typename write_>
    void function(out<out<previous1_, previous2_>, write_> const& o, char const *begin, char const *end) {
        function(o.previous(), begin, end);
    }

    template<typename function_, typename configuration_>
    char* write(out<function_, configuration_> const& /*o*/, char* /*begin*/, char *at) {
        return at;
    }
    
    template<typename previous1_, typename previous2_, typename write_>
    char* write(out<out<previous1_, previous2_>, write_> const& o, char *begin, char *at) {
        at = write(o.previous(), begin, at);
        char *end = begin + out<out<previous1_, previous2_>, write_>::size + out<out<previous1_, previous2_>, write_>::settings::extra_size;
        return o.write(at, end);
    }

    template<bool>
    struct write_if {
        template<typename previous_, typename write_>
        static void do_it(out<previous_, write_> const& o) {
            char array[out<previous_, write_>::size + out<previous_, write_>::settings::extra_size + (out<previous_, write_>::settings::end ? 1 : 0) + 1];
            char *end = write(o, array, array);
            if(end != array) {
                char end_with = out<previous_, write_>::settings::end; // avoid "warning C4127: conditional expression is constant"
                if(end_with && end[-1] != end_with)
                    *end++ = end_with;
                *end = 0;
                function(o, array, end);
            }
        }
    };
    
    template<>
    struct write_if<false> {
        template<typename previous_, typename write_>
        static void do_it(out<previous_, write_> const&) {}
    };
    
    template<typename previous_, typename write_>
    void write(out<previous_, write_> const& o) {
        // avoid "warning C4127: conditional expression is constant" on if(out<previous_, write_>::size)
        write_if<out<previous_, write_>::size != 0>::do_it(o);
    }

}

template<bool> struct char_array_if { using result = char_array_if; };
template<> struct char_array_if<false> {};



// this is a zero terminated char array.
// it converts to char const*, cstring
// and it's a range with begin/end/size
template<unsigned capacity_>
class char_array
{
public:
    static unsigned constexpr capacity = capacity_; // without 0 at end
    using size_type = unsigned;
    using value_type = char;
    using iterator = char const*;
    using const_iterator = char const*;

private:
    unsigned mysize = 0;
    char my[capacity + 1];

public:
    char_array() {
        my[0] = 0;
    }

    template<typename p_, typename w_>
    char_array(out<p_, w_> const& o, typename char_array_if<out<p_, w_>::size + (out<p_, w_>::settings::end ? 1 : 0) <= capacity>::result = {}) {
        o.dont();
        char *end = _::write(o, my, my);
        // write end even if empty string, becuase someone called cstring() and will probably use this
        if(out<p_, w_>::settings::end)
            *end++ = out<p_, w_>::settings::end;
        *end = 0;
        mysize = static_cast<unsigned>(end - my);
    }

    operator char const*() const {
        return my;
    }

    iterator begin() const {
        return my;
    }

    iterator end() const {
        return my + mysize;
    }

    unsigned size() const {
        return mysize;
    }

    template<typename write_>
    void write(write_ const& a) {
        // write_int / write_float
        mysize = static_cast<unsigned>(a(my, my + capacity) - my);
    }
};



template<typename configuration_> struct configuration_or_default : configuration_ {};
template<> struct configuration_or_default<void> : configuration_default {};



template<typename function_, typename configuration_>
class out
{
public:
    static constexpr unsigned size = 0;
    using number_format = typename configuration_or_default<configuration_>::number_format;
    using settings = typename configuration_or_default<configuration_>::settings;
    using char_array = ministr::char_array<settings::end ? 1 : 0>; // visual c++ needs this for the ministr::cstring return type to work

private:
    function_ my;

public:
    out() = default;

    out(function_ a) :
        my(a)
    {}

    void dont() const
    {}

    void operator()(char const* begin, char const* end) const {
        // always non empty 0-terminated string. begtin < end and *end == 0
        my(begin, end);
    }

    char_array cstring() const {
        return *this;
    }
};



template<typename configuration_>
class out<void, configuration_>
{
public:
    static constexpr unsigned size = 0;
    using number_format = typename configuration_or_default<configuration_>::number_format;
    using settings = typename configuration_or_default<configuration_>::settings;
    using char_array = ministr::char_array<settings::end ? 1 : 0>;

public:
    void dont() const
    {}

    char_array cstring() const {
        return *this;
    }
};



template<typename previous1_, typename previous2_, typename write_>
class out<out<previous1_, previous2_>, write_>
{
public:
    static constexpr unsigned size = out<previous1_, previous2_>::size + write_::size;
    using number_format = typename out<previous1_, previous2_>::number_format;
    using settings = typename out<previous1_, previous2_>::settings;
    using char_array = ministr::char_array<size + (size ? settings::extra_size : 0) + (settings::end ? 1 : 0)>;

private:
    out<previous1_, previous2_> const* myprevious;
    write_ mywrite;
    mutable bool mydo = true;

public:
    out(out<previous1_, previous2_> const& p, write_ w) :
        myprevious(&p),
        mywrite(w)
    {
        p.dont();
    }

    out(out const& a) :
        myprevious(a.myprevious),
        mywrite(a.mywrite)
    {
        a.mydo = false;
    }

    ~out() {
        if(mydo)
            _::write(*this);
    }

    void dont() const {
        mydo = false;
    }

    out<previous1_, previous2_> const& previous() const {
        return *myprevious;
    }

    char* write(char* begin, char* end) const {
        return mywrite(begin, end);
    }

    char_array cstring() const {
        return *this;
    }
};



template<typename format_>
struct set_number_format { static constexpr unsigned size = 0; };



template<typename previous1_, typename previous2_, typename number_format_>
class out<out<previous1_, previous2_>, set_number_format<number_format_>>
{
public:
    static constexpr unsigned size = out<previous1_, previous2_>::size;
    using number_format = number_format_;
    using settings = typename out<previous1_, previous2_>::settings;
    using char_array = ministr::char_array<size + (size ? settings::extra_size : 0) + (settings::end ? 1 : 0)>;

private:
    out<previous1_, previous2_> const* myprevious;
    mutable bool mydo = true;

public:
    out(out<previous1_, previous2_> const& p) :
        myprevious(&p)
    {
        p.dont();
    }

    out(out const& a) :
        myprevious(a.myprevious)
    {
        a.mydo = false;
    }

    ~out() {
        if(mydo)
            _::write(previous());
    }

    void dont() const {
        mydo = false;
    }

    out<previous1_, previous2_> const& previous() const {
        return *myprevious;
    }

    char* write(char* begin, char* /*end*/) const {
        return begin;
    }

    char_array cstring() const {
        return *this;
    }
};



template<typename p_, typename w_>
typename out<p_, w_>::char_array cstring(out<p_, w_> const& o) {
    return o;
}



template<typename function_>
out<function_> out_function(function_ a) {
    return a;
}

template<typename function_>
out<function_, configuration<typename configuration_default::number_format, settings_end<typename configuration_default::settings, static_cast<char>(u'\n')>>>
out_line_function(function_ a) {
    return a;
}



template<typename iterator_>
class out_to_iterator_reference
{
    iterator_ *my, myend;

public:
    out_to_iterator_reference(iterator_& begin, iterator_ end) :
        my(&begin),
        myend(end)
    {}

    void operator()(char const* begin, char const *end) const {
        while(*my != myend && begin != end) {
            **my = *begin++;
            ++*my;
        }
    }
};

template<typename iterator_>
out<out_to_iterator_reference<iterator_>> out_begin_end(iterator_& begin, iterator_ end) {
    // will move begin
    // will not zero terminate
    return out_to_iterator_reference<iterator_>{begin, end};
}



namespace _ {
    
    template<typename type_> type_ make();
    
    template<typename a_> struct no_const_reference_do { using result = a_; };
    template<typename a_> struct no_const_reference_do<a_&> { using result = a_; };
    template<typename a_> struct no_const_reference_do<a_ const&> { using result = a_; };
    template<typename a_> struct no_const_reference_do<a_ const> { using result = a_; };
    
    template<typename a_> using no_const_reference = typename _::no_const_reference_do<a_>::result;
    
    using char8_or_char = no_const_reference<decltype(*u8"")>;
    
    enum class not_char8 : unsigned char;
    
    template<typename char8_ = char8_or_char> struct char8_or_not_do { using result = char8_; };
    template<> struct char8_or_not_do<char> { using result = not_char8; };
    
    using char8_or_not = char8_or_not_do<>::result;
    
    template<typename iterator_>
    void copy(iterator_& to, iterator_ const& to_end, char8_or_char const *from, unsigned size) {
        while(to != to_end && size) {
            *to = static_cast<char>(*from++);
            ++to;
            --size;
        }
    }
    
    template<typename iterator_>
    void copy(iterator_& to, iterator_ const& to_end, char from) {
        if(to != to_end) {
            *to = from;
            ++to;
        }
    }

    constexpr unsigned char utf8_first_lookup[0x100] = {
        // 1  2  3  4  5  6  7  8  9  a  b  c  d  e  f
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 0
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 1
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 2
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 3
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 4
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 5
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 6
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 7
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 8
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 9
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // a
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // b
        0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, // c
        2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, // d
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, // e
        4, 4, 4, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // f
    };
    
    template<typename char_>
    char32_t uchar(char_ c) {
        return static_cast<char32_t>(c);
    }
    inline unsigned char uchar(char c) {
        return static_cast<unsigned char>(c);
    }
    inline unsigned char uchar(unsigned char c) {
        return c;
    }
    
    template<typename char_>
    inline unsigned utf8_first(char_ a) {
        auto u = uchar(a);
        return u <= 0xff ? utf8_first_lookup[u] : 0;
    }
    
    template<typename iterator_>
    unsigned utf8_cut(iterator_ i, unsigned size) {
        // do not cut inside a sequence
        unsigned r = 0;
        while(size) {
            auto u = utf8_first(*i);
            if(!u) return 0; // impossible because length()
            if(u > size) break;
            size -= u;
            r += u;
            while(u--) ++i;
        }
        return r;
    }
    
    template<bool>
    struct length_ut8 {};
    
    template<typename iterator_>
    unsigned length(iterator_ begin, iterator_ end, length_ut8<true>) {
        unsigned
            r = 0,
            u = 0,
            uu = 0;
        decltype(uchar(*begin)) c[4];
        while(begin != end && *begin) {
            if(!u) {
                u = uu = utf8_first(*begin);
                if(!u) return 0;
                c[0] = uchar(*begin);
                --u;
            }
            else {
                c[uu - u] = uchar(*begin);
                if(!--u) {
                    // verify!
                    bool ok = false;
                    switch(uu) {
                        case 2:
                            ok = 0x80 <= c[1] && c[1] <= 0xbf;
                            break;
                        case 3:
                            ok = (
                                c[0] == 0xe0 ? 0xa0 <= c[1] && c[1] <= 0xbf :
                                c[0] == 0xed ? 0x80 <= c[1] && c[1] <= 0x9f :
                                0x80 <= c[1] && c[1] <= 0xbf
                            ) &&
                                0x80 <= c[2] && c[2] <= 0xbf;
                            break;
                        case 4:
                            ok = (
                                c[0] == 0xf0 ? 0x90 <= c[1] && c[1] <= 0xbf :
                                c[0] == 0xf4 ? 0x80 <= c[1] && c[1] <= 0x8f :
                                0x80 <= c[1] && c[1] <= 0xbf
                            ) &&
                                0x80 <= c[2] && c[2] <= 0xbf &&
                                0x80 <= c[3] && c[3] <= 0xbf;
                    }
                    if(!ok)
                        return 0;
                }
            }
            ++begin;
            ++r;
        }
        return u ? 0 : r;
    }

    template<typename iterator_>
    unsigned length(iterator_ begin, iterator_ end, length_ut8<false>) {
        unsigned r = 0;
        while(begin != end && *begin) {
            ++begin;
            ++r;
        }
        return r;
    }
    
}



using _::no_const_reference;

using _::char8_or_not;


template<bool utf8_, typename iterator_>
unsigned length(iterator_ begin, iterator_ end) {
    return _::length(begin, end, _::length_ut8<utf8_>());
}

template<bool utf8_, typename char_const_>
unsigned length(char_const_ *begin, char_const_ *end) {
    if(!begin || begin >= end)
        return 0;
    return _::length(begin, end, _::length_ut8<utf8_>());
}

template<bool utf8_, typename char_>
unsigned length(char_ const* cstring) {
    if(!cstring || !*cstring)
        return 0;
    return _::length(cstring, static_cast<char_ const*>(0), _::length_ut8<utf8_>()); // begin != end forever
}



template<unsigned size_ = 0>
struct write_size {
    static constexpr unsigned size = size_;
    template<typename iterator_>
    iterator_ operator()(iterator_ begin, iterator_ /*end*/) const { return begin; }
};



class write_char : public write_size<1>
{
    char my;
    
public:
    write_char(char a) :
        my(a)
    {}
    
    template<typename iterator_>
    iterator_ operator()(iterator_ begin, iterator_ end) const {
        if(my && begin != end) *begin++ = my;
        return begin;
    }
};



template<typename iterator_, typename settings_, unsigned size_ = 0>
class write_string :
    public write_size<size_ ? size_ : settings_::string_size>
{
    iterator_ mybegin;
    unsigned mylength = 0;

public:
    write_string(iterator_ begin, iterator_ end) :
        mybegin(begin),
        mylength(length<settings_::utf8>(begin, end))
    {}

    write_string(iterator_ cstring) :
        mybegin(cstring),
        mylength(length<settings_::utf8>(cstring))
    {}

    template<typename iterator2_>
    iterator2_ operator()(iterator2_ begin, iterator2_ end) const {
        unsigned s = static_cast<unsigned>(end - begin);
        if(s >= mylength)
            s = mylength;
        else if(settings_::utf8)
            s = _::utf8_cut(mybegin, s);
        end = begin + s;
        iterator_ f = mybegin;
        while(begin != end) {
            *begin++ = static_cast<no_const_reference<decltype(*begin)>>(*f);
            ++f;
        }
        return end;
    }
};



// write_range_select is specialized in utf.hpp
template<typename iterator_, typename settings_, unsigned size_ = 0, typename specialize_ = void>
struct write_range_select {
    using result = write_string<iterator_, settings_, size_>;
};

template<
    typename range_,
    typename settings_,
    unsigned size_ = 0,
    typename = decltype(_::make<range_ const&>().begin() == _::make<range_ const&>().end())
>
struct write_range :
    write_range_select<
        no_const_reference<decltype(_::make<range_ const&>().begin())>,
        settings_,
        size_
    >::result
{
    write_range(range_ const& a) :
        write_range_select<
            no_const_reference<decltype(_::make<range_ const&>().begin())>,
            settings_,
            size_
        >::result(a.begin(), a.end())
    {}
};



class write_bool : public write_size<5>
{
    bool my, myuppercase;

public:
    write_bool(bool a, bool uppercase = false) :
        my(a),
        myuppercase(uppercase)
    {}

    template<typename iterator_>
    iterator_ operator()(iterator_ begin, iterator_ end) const {
        if(my)
            _::copy(begin, end, myuppercase ? u8"TRUE" : u8"true", 4);
        else
            _::copy(begin, end, myuppercase ? u8"FALSE" : u8"false", 5);
        return begin;
    }
};



namespace _ {

    constexpr unsigned bytebits(unsigned char a = static_cast<unsigned char>(-1)) {
        return
            a >= 0x7f ? 7 + bytebits(a >> 7) :
            a > 0 ? 1 + bytebits(a >> 1) :
            0;
    }
    
    template<typename iterator_>
    void copy_base_prefix(iterator_& to, iterator_ to_end, unsigned base, bool uppercase) {
        if(base != 10) {
            copy(to, to_end, static_cast<char>(u'0'));
            char c =
                base == 2 ? static_cast<char>(uppercase ? u'B' : u'b') :
                base == 8 ? static_cast<char>(uppercase ? u'O' : u'o') :
                static_cast<char>(uppercase ? u'X' : u'x');
            copy(to, to_end, c);
        }
    }
    
    constexpr char8_or_char digits[2][17] = {
        u8"0123456789abcdef",
        u8"0123456789ABCDEF"
    };
    
}



template<typename type_, typename number_format_ = number_format<>>
class write_int
{
    static constexpr bool is_unsigned = static_cast<type_>(-1) > 0;
    static constexpr unsigned bits = sizeof(type_) * _::bytebits() - (~static_cast<type_>(0) == static_cast<type_>(-1) ? 0 : 1);
    // bits works for unsigned and signed
    // if i is 4 bits, signed min can be
    // -0b1000, 2s complement
    // -0b111, not 2s complement

public:
    static constexpr unsigned
        base = number_format_::base,
        max_digits =
            number_format_::digits > 0 ? number_format_::digits :
            base == 2 ? bits :
            base == 8 ? bits / 3 + (bits % 3 ? 1 : 0) :
            base == 16 ? bits / 4 + (bits % 4 ? 1 : 0) :
            static_cast<unsigned>((bits * 19728ul + 0xfffful) >> 16), // log10(2) * (1 << 16) = 19728.301
        size =
            (!is_unsigned || number_format_::show_plus) + // sign
            (base == 10 || number_format_::hide_base ? 0 : 2) + // 0x
            max_digits;
    
    using char_array = ministr::char_array<size>;

private:
    type_ my;

public:
    write_int(type_ a) :
        my(a)
    {}

    template<typename iterator_>
    iterator_ operator()(iterator_ begin, iterator_ end) const {
        using _::copy;
        bool const minus = !is_unsigned && my < 0;
        if(minus)
            copy(begin, end, static_cast<char>(u'-'));
        else if(number_format_::show_plus)
            copy(begin, end, static_cast<char>(u'+'));
        if(!number_format_::hide_base)
            _::copy_base_prefix(begin, end, base, number_format_::uppercase);
        // write digits, backwards
        type_ constexpr b = static_cast<type_>(base);
        type_ i = my;
        unsigned digits = number_format_::digits ? max_digits : 1;
        if(!number_format_::digits)
            while(i /= b) ++digits;
        if(static_cast<unsigned>(end - begin) < digits)
            return begin;
        iterator_ to = end = begin + digits;
        i = my;
        // if signed 2s complement -min > max
        type_ i0 = static_cast<type_>(i - (i / b) * b);
        i /= b;
        if(minus) {
            i0 *= static_cast<type_>(-1); // avoid warnings for -unsigned
            i *= static_cast<type_>(-1);
        }
        *--to = static_cast<char>(_::digits[!number_format_::lowercase][i0]);
        while(--digits) {
            *--to = static_cast<char>(_::digits[!number_format_::lowercase][i % b]);
            i /= b;
        }
        return end;
    }

    char_array cstring() const {
        char_array r;
        r.write(*this);
        return r;
    }
};



// operator<<(out, char const(&)[size_])
// operator<<(out, char const*)
// is solved by:
// char_const_pointer means the array operator is prefered over char const*
// void_const_pointer is needed to not make char const* go to void const*
// then void_const_pointer_if disables the constructor for char const*
// after that if_bool is needed, a plain bool operator would be used for pointers

template<typename char_>
struct char_const_pointer {
    char_ const *pointer;
    char_const_pointer(char_ const* a) : pointer{a} {}
};

template<typename type_> struct void_const_pointer_if { using result = void_const_pointer_if; };
template<> struct void_const_pointer_if<char> {};
template<> struct void_const_pointer_if<char16_t> {}; // disable here so you know you need utf.hpp
template<> struct void_const_pointer_if<char32_t> {};
template<> struct void_const_pointer_if<wchar_t> {};
template<> struct void_const_pointer_if<char8_or_not> {};

struct void_const_pointer {
    void const *pointer;
    template<typename type_>
    void_const_pointer(type_ const* a, typename void_const_pointer_if<type_>::result = {}) : pointer{a} {}
};

template<typename> struct if_bool;
template<> struct if_bool<bool> { using result = write_bool; };

template<typename p_, typename w_>
out<out<p_, w_>, write_char> operator<<(
    out<p_, w_> const& o,
    char a
) {
    return {o, a};
}

template<typename p_, typename w_, unsigned size_>
out<out<p_, w_>, write_string<char const*, typename out<p_, w_>::settings, size_>> operator<<(
    out<p_, w_> const& o,
    char const (&a)[size_]
) {
    return {o, {a, a + size_}};
}

template<typename p_, typename w_>
out<out<p_, w_>, write_string<char const*, typename out<p_, w_>::settings> > operator<<(
    out<p_, w_> const& o,
    char_const_pointer<char> a
) {
    return {o, {a.pointer}};
}

template<typename p_, typename w_>
out<out<p_, w_>, write_char> operator<<(
    out<p_, w_> const& o,
    char8_or_not a
) {
    return {o, static_cast<char>(a)};
}

template<typename p_, typename w_, unsigned size_>
out<out<p_, w_>, write_string<char8_or_not const*, typename out<p_, w_>::settings, size_>> operator<<(
    out<p_, w_> const& o,
    char8_or_not const (&a)[size_]
) {
    return {o, {a, a + size_}};
}

template<typename p_, typename w_>
out<out<p_, w_>, write_string<char8_or_not const*, typename out<p_, w_>::settings> > operator<<(
    out<p_, w_> const& o,
    char_const_pointer<char8_or_not> a
) {
    return {o, {a.pointer}};
}

template<typename p_, typename w_, typename range_>
out<out<p_, w_>, write_range<range_, typename out<p_, w_>::settings>> operator<<(
    out<p_, w_> const& o,
    range_ const& a
) {
    return {o, a};
}

template<typename p_, typename w_, typename bool_>
out<out<p_, w_>, typename if_bool<bool_>::result> operator<<(
    out<p_, w_> const& o,
    bool_ a
) {
    return {o, {a, out<p_, w_>::number_format::uppercase}};
}

template<typename p_, typename w_>
out<out<p_, w_>, write_int<int, typename out<p_, w_>::number_format>> operator<<(
    out<p_, w_> const& o,
    int a
) {
    return {o, a};
}

template<typename p_, typename w_>
out<out<p_, w_>, write_int<unsigned int, typename out<p_, w_>::number_format>> operator<<(
    out<p_, w_> const& o,
    unsigned int a
) {
    return {o, a};
}

template<typename p_, typename w_>
out<out<p_, w_>, write_int<long, typename out<p_, w_>::number_format>> operator<<(
    out<p_, w_> const& o,
    long a
) {
    return {o, a};
}

template<typename p_, typename w_>
out<out<p_, w_>, write_int<unsigned long, typename out<p_, w_>::number_format>> operator<<(
    out<p_, w_> const& o,
    unsigned long a
) {
    return {o, a};
}

template<typename p_, typename w_>
out<out<p_, w_>, write_int<long long, typename out<p_, w_>::number_format>> operator<<(
    out<p_, w_> const& o,
    long long a
) {
    return {o, a};
}

template<typename p_, typename w_>
out<out<p_, w_>, write_int<unsigned long long, typename out<p_, w_>::number_format>> operator<<(
    out<p_, w_> const& o,
    unsigned long long a
) {
    return {o, a};
}

template<typename p_, typename w_>
out<out<p_, w_>, write_int<decltype(sizeof(0)), number_format_base<typename out<p_, w_>::number_format, 16>>> operator<<(
    out<p_, w_> const& o,
    void_const_pointer a
) {
    return {o, reinterpret_cast<decltype(sizeof(0))>(a.pointer)};
}

// o << size<123>

template<unsigned>
struct size_set;

template<unsigned size_>
size_set<size_>* size() { return 0; }

template<typename p_, typename w_, unsigned size_>
out<out<p_, w_>, write_size<size_>> operator<<(
    out<p_, w_> const& o,
    size_set<size_>* (*)()
) {
    return {o, {}};
}

// o << base<16>

template<unsigned>
struct base_set;

template<unsigned base_>
base_set<base_>* base() { return 0; }

template<typename p_, typename w_>
out<out<p_, w_>, set_number_format<number_format_base<typename out<p_, w_>::number_format, 2>>> operator<<(
    out<p_, w_> const& o,
    base_set<2>* (*)()
) {
    return {o};
}

template<typename p_, typename w_>
out<out<p_, w_>, set_number_format<number_format_base<typename out<p_, w_>::number_format, 8>>> operator<<(
    out<p_, w_> const& o,
    base_set<8>* (*)()
) {
    return {o};
}

template<typename p_, typename w_>
out<out<p_, w_>, set_number_format<number_format_base<typename out<p_, w_>::number_format, 10>>> operator<<(
    out<p_, w_> const& o,
    base_set<10>* (*)()
) {
    return {o};
}

template<typename p_, typename w_>
out<out<p_, w_>, set_number_format<number_format_base<typename out<p_, w_>::number_format, 16>>> operator<<(
    out<p_, w_> const& o,
    base_set<16>* (*)()
) {
    return {o};
}

// o << digits<123>

template<int>
struct digits_set;

template<int digits_>
digits_set<digits_>* digits() { return 0; }

template<typename p_, typename w_, int digits_>
out<out<p_, w_>, set_number_format<number_format_digits<typename out<p_, w_>::number_format, digits_>>> operator<<(
    out<p_, w_> const& o,
    digits_set<digits_>* (*)()
) {
    return {o};
}

// o << no_exponent_min_max<-10, 10>

template<int min_, int max_>
struct no_exponent_min_max_set;

template<int min_, int max_>
no_exponent_min_max_set<min_, max_>* no_exponent_min_max() { return 0; }

template<typename p_, typename w_, int min_, int max_>
out<out<p_, w_>, set_number_format<number_format_no_exponent_min_max<typename out<p_, w_>::number_format, min_, max_>>> operator<<(
    out<p_, w_> const& o,
    no_exponent_min_max_set<min_, max_>* (*)()
) {
    return {o};
}

// o << uppercase
// o << lowercase
// o << mixedcase

template<int> struct case_set;
inline case_set<1>* uppercase() { return 0; }
inline case_set<-1>* lowercase() { return 0; }
inline case_set<0>* mixedcase() { return 0; }

template<typename p_, typename w_, int case_>
out<out<p_, w_>, set_number_format<number_format_case<typename out<p_, w_>::number_format, case_>>> operator<<(
    out<p_, w_> const& o,
    case_set<case_>* (*)()
) {
    return {o};
}

// o << show_plus
// o << hide_plus

template<bool> struct show_plus_set;
inline show_plus_set<true>* show_plus() { return 0; }
inline show_plus_set<false>* hide_plus() { return 0; }

template<typename p_, typename w_, bool show_plus_>
out<out<p_, w_>, set_number_format<number_format_show_plus<typename out<p_, w_>::number_format, show_plus_>>> operator<<(
    out<p_, w_> const& o,
    show_plus_set<show_plus_>* (*)()
) {
    return {o};
}

// o << show_base
// o << hide_base

template<bool> struct hide_base_set;
inline hide_base_set<false>* show_base() { return 0; }
inline hide_base_set<true>* hide_base() { return 0; }

template<typename p_, typename w_, bool hide_base_>
out<out<p_, w_>, set_number_format<number_format_hide_base<typename out<p_, w_>::number_format, hide_base_>>> operator<<(
    out<p_, w_> const& o,
    hide_base_set<hide_base_>* (*)()
) {
    return {o};
}



// bytes
// write bytes as hex, 4 bytes then space

template<unsigned bytes_, typename number_format_ = number_format<>>
class write_bytes
{
    static constexpr unsigned digits = _::bytebits() / 4 + (_::bytebits() % 4 ? 1 : 0);

public:
    static constexpr unsigned size = digits * bytes_ + bytes_ / 4;

private:
    unsigned char const *my;
    unsigned mybytes;

public:
    write_bytes(void const *pointer, unsigned bytes) :
        my(static_cast<unsigned char const*>(pointer)),
        mybytes(pointer ? bytes : 0)
    {}

    template<typename iterator_>
    iterator_ operator()(iterator_ begin, iterator_ end) const {
        auto
            f = my,
            fe = my + mybytes;
        while(f != fe) {
            unsigned space = f == my || (my - f) % 4 ? 0 : 1;
            if(static_cast<unsigned>(end - begin) < digits + space)
                break;
            if(space)
                *begin++ = static_cast<char>(u' ');
            auto t = begin + digits;
            auto u = *f;
            do {
                *--t = static_cast<char>(_::digits[!number_format_::lowercase][u & 0xf]);
                u >>= 4;
            } while(t != begin);
            begin += digits;
            ++f;
        }
        return begin;
    }
};

template<unsigned bytes_>
struct byte_pointer {
    void const *pointer;
    decltype(sizeof(0)) bytes;
    
    byte_pointer(void const* pointer, decltype(sizeof(0)) bytes) :
        pointer(pointer), bytes(bytes)
    {}
};

template<typename p_, typename w_, unsigned bytes_>
out<out<p_, w_>, write_bytes<bytes_, typename out<p_, w_>::number_format>> operator<<(
    out<p_, w_> const& o,
    byte_pointer<bytes_> a
) {
    return {o, {a.pointer, static_cast<unsigned>(a.bytes)}};
}

template<typename p_, typename w_>
out<out<p_, w_>, write_bytes<out<p_, w_>::settings::string_size, typename out<p_, w_>::number_format>> operator<<(
    out<p_, w_> const& o,
    byte_pointer<0> a
) {
    return {o, {a.pointer, static_cast<unsigned>(a.bytes)}};
}

template<unsigned bytes_>
byte_pointer<bytes_> bytes(void const* pointer, decltype(sizeof(0)) size) {
    return {pointer, size};
}

template<unsigned bytes_>
byte_pointer<bytes_> bytes(void const* pointer) {
    return {pointer, bytes_};
}

inline byte_pointer<0> bytes(void const* pointer, decltype(sizeof(0)) size) {
    return {pointer, size};
}

template<typename type_>
byte_pointer<sizeof(type_)> bytes(type_ const& a) {
    return {&a, sizeof(a)};
}

}}
#endif
