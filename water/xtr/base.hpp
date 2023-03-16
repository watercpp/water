// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_XTR_BASE_HPP
#define WATER_XTR_BASE_HPP
// no dependency!
namespace water { namespace xtr {

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
// when true all strings are expected to be utf-8. if a string needs to be cut when written it will
// not be cut in the middle of a utf-8 sequence.
//
// end_
// if not 0, each write ends with this. set this to lf to always end with a linebreak. it will be
// added to the end unless it already ends with it.
// this means it does not matter if the string ends with a linebreak or not. and it is possible to
// make it end with two linebreaks, just write 2 linebreaks.
//
// #define the WATER_XTR_SIZE_USED(total_size, used_size) macro to track how much is used and tune
// the string_size_ and extra_size_. Look at water/xtr/tests/size_used.hpp for an example.

template<
    unsigned string_size_ = 40,
    unsigned extra_size_ = 80,
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

template<unsigned base_, typename number_format_ = number_format<>>
using number_format_base = number_format<
    base_,
    number_format_::digits,
    number_format_::no_exponent_min,
    number_format_::no_exponent_max,
    number_format_::flags
>;

template<int digits_, typename number_format_ = number_format<>>
using number_format_digits = number_format<
    number_format_::base,
    digits_,
    number_format_::no_exponent_min,
    number_format_::no_exponent_max,
    number_format_::flags
>;

template<int min_, int max_, typename number_format_ = number_format<>>
using number_format_no_exponent_min_max = number_format<
    number_format_::base,
    number_format_::digits,
    min_,
    max_,
    number_format_::flags
>;

template<unsigned flags_, typename number_format_ = number_format<>>
using number_format_flags = number_format<
    number_format_::base,
    number_format_::digits,
    number_format_::no_exponent_min,
    number_format_::no_exponent_max,
    flags_
>;

template<int case_, typename number_format_ = number_format<>>
using number_format_case = number_format_flags<
    (number_format_::flags & ~number_mixedcase) | (case_ > 0 ? number_uppercase : case_ < 0 ? number_lowercase : 0),
    number_format_
>;

template<bool show_, typename number_format_ = number_format<>>
using number_format_show_plus = number_format_flags<
    show_ ? number_format_::flags | number_show_plus : number_format_::flags & ~number_show_plus,
    number_format_
>;

template<bool hide_, typename number_format_ = number_format<>>
using number_format_hide_base = number_format_flags<
    hide_ ? number_format_::flags | number_hide_base : number_format_::flags & ~number_hide_base,
    number_format_
>;

template<typename number_format_ = number_format<>, typename settings_ = settings<>>
struct configuration {
    using settings = settings_;
    using number_format = number_format_;
};

using configuration_default =
    #ifdef WATER_XTR_CONFIGURATION
    WATER_XTR_CONFIGURATION;
    #else
    configuration<>;
    #endif

template<typename configuration_>
struct configuration_or_default : configuration_ {};

template<>
struct configuration_or_default<void> : configuration_default {};

using number_format_default = configuration_default::number_format;
using settings_default = configuration_default::settings;


using size_t = decltype(sizeof(0));


template<typename function_>
struct unbuffered;


template<typename function_ = void, typename configuration_ = void>
class expression;


template<typename p_, typename w_>
void write(expression<p_, w_>&);


// not initial expression holds a pointer to the previous expression,
// except the second expression holds the first by value.
// this is so the first expression can be created inside a << operator, look at the function class below

template<typename expression_>
class previous
{
    expression_ my;

public:
    explicit previous(expression_& a) :
        my{a}
    {}

    expression_& operator*() {
        return my;
    }
};

template<typename previous1_, typename previous2_, typename write_>
class previous<expression<expression<previous1_, previous2_>, write_>>
{
    using expression_ = expression<expression<previous1_, previous2_>, write_>;
    expression_ *my;

public:
    explicit previous(expression_& a) :
        my{&a}
    {}

    expression_& operator*() {
        return *my;
    }
};



// initial expression with a function (function_ can be a pointer)

template<typename function_, typename configuration_>
class expression
{
public:
    using number_format = typename configuration_or_default<configuration_>::number_format;
    using settings = typename configuration_or_default<configuration_>::settings;
    static constexpr unsigned size = settings::extra_size + (settings::end ? 1 : 0);

private:
    function_ my{};
    
public:

    expression() = default;

    expression(function_ const& a) :
        my{a}
    {}

    void dont_write()
    {}
    
    function_& function() {
        return my;
    }
};


template<typename function_, typename configuration_>
class expression<unbuffered<function_>, configuration_> : public expression<function_, configuration_>
{
public:

    expression() = default;

    expression(function_ const& a) :
        expression<function_, configuration_>{a}
    {}
};



// initial expression without a function

template<typename configuration_>
class expression<void, configuration_>
{
public:
    using number_format = typename configuration_or_default<configuration_>::number_format;
    using settings = typename configuration_or_default<configuration_>::settings;
    static constexpr unsigned size = settings::extra_size + (settings::end ? 1 : 0);
    
public:
    void dont_write()
    {}
};



// not initial expression, with write class

template<typename previous1_, typename previous2_, typename write_>
class expression<expression<previous1_, previous2_>, write_>
{
public:
    using previous_expression = expression<previous1_, previous2_>;
    static constexpr unsigned size = previous_expression::size + write_::size;
    using number_format = typename previous_expression::number_format;
    using settings = typename previous_expression::settings;

private:
    xtr::previous<previous_expression> myprevious;
    write_ mywrite;
    bool mydo_write = true;

public:
    expression(previous_expression& p, write_ const& w) :
        myprevious{p},
        mywrite{w}
    {
        p.dont_write();
    }

    expression(expression&& a) :
        myprevious(a.myprevious),
        mywrite(a.mywrite)
    {
        a.mydo_write = false;
    }

    ~expression() {
        if(mydo_write)
            xtr::write(*this);
    }

    expression(expression const&) = delete;
    expression operator=(expression&&) = delete;
    expression operator=(expression const&) = delete;

    void dont_write() {
        mydo_write = false;
    }

    previous_expression& previous() {
        return *myprevious;
    }

    template<typename to_>
    void write(to_&& to) {
        mywrite(to);
    }
};



// not initial expression, set number format

template<typename format_>
struct set_number_format {
    static constexpr unsigned size = 0;
};

template<typename previous1_, typename previous2_, typename number_format_>
class expression<expression<previous1_, previous2_>, set_number_format<number_format_>>
{
public:
    using previous_expression = expression<previous1_, previous2_>;
    static constexpr unsigned size = previous_expression::size;
    using number_format = number_format_;
    using settings = typename previous_expression::settings;
    
private:
    xtr::previous<previous_expression> myprevious;
    bool mydo_write = true;

public:
    expression(previous_expression& p) :
        myprevious{p}
    {
        p.dont_write();
    }

    expression(expression&& a) :
        myprevious(a.myprevious)
    {
        a.mydo_write = false;
    }

    ~expression() {
        if(mydo_write)
            xtr::write(previous());
    }

    expression(expression const&) = delete;
    expression operator=(expression&&) = delete;
    expression operator=(expression const&) = delete;

    void dont_write() {
        mydo_write = false;
    }

    previous_expression& previous() {
        return *myprevious;
    }
    
    template<typename to_>
    void write(to_&&) {
    }
};



namespace _ {

    template<typename type_> type_ make();
    
    template<typename a_> struct no_const_reference_do { using result = a_; };
    template<typename a_> struct no_const_reference_do<a_&> { using result = a_; };
    template<typename a_> struct no_const_reference_do<a_ const&> { using result = a_; };
    template<typename a_> struct no_const_reference_do<a_ const> { using result = a_; };
    
    template<typename a_> using no_const_reference = typename no_const_reference_do<a_>::result;
    
    
    
    using char8_or_char = no_const_reference<decltype(*u8"")>;
    
    enum class not_char8 : unsigned char;
    
    template<typename char8_ = char8_or_char> struct char8_or_not_do { using result = char8_; };
    template<> struct char8_or_not_do<char> { using result = not_char8; };
    
    using char8_or_not = char8_or_not_do<>::result;
    
    
    
    // copy paste from water::unicode
    
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
    
    template<typename char_, bool = (static_cast<char_>(-1) > 0 && static_cast<char_>(-1) <= 0xff)>
    struct utf8_first_lookup_do {
        static unsigned char at(char_ a) {
            return utf8_first_lookup[a];
        }
    };
    
    template<typename char_>
    struct utf8_first_lookup_do<char_, false> {
        static unsigned char at(char_ a) {
            return 0 <= a && a <= 0xff ? utf8_first_lookup[a] : 0;
        }
    };
    
    template<typename char_>
    inline unsigned char utf8_first_of(char_ a) {
        return utf8_first_lookup_do<char_>::at(a);
    }

    inline unsigned char utf8_first_of(char a) {
        return utf8_first_of(static_cast<unsigned char>(a));
    }

    inline unsigned char utf8_first_of(signed char a) {
        return utf8_first_of(static_cast<unsigned char>(a));
    }
    
    inline unsigned char utf8_first_of(char8_or_not a) {
        return utf8_first_of(static_cast<unsigned char>(a));
    }
    
    template<typename iterator_>
    iterator_ utf8_adjust_end(iterator_ begin, iterator_ end) {
        auto e = end;
        size_t s = 0;
        while(e != begin) {
            --e;
            ++s;
            auto n = utf8_first_of(*e);
            if(n)
                return n > s ? e : e + n;
        }
        return begin;
    }
    
    
    
    template<typename iterator_>
    struct write_to_iterators
    {
        using char_type = no_const_reference<decltype(*make<iterator_>())>; // this does not work for output iterators, utf8_adjust_end also does not work
    
    private:   
        iterator_
            my,
            myend;
        bool mytruncated = false;

    public:
        write_to_iterators(iterator_ begin, iterator_ end) :
            my{begin},
            myend{end}
        {}
        
        write_to_iterators(write_to_iterators const&) = delete;
        write_to_iterators& operator=(write_to_iterators const&) = delete;
    
        template<typename char_>
        void operator()(char_ a) {
            if(my == myend)
                mytruncated = true;
            else {
                *my = static_cast<char_type>(a);
                ++my;
            }
        }
        
        template<typename iterator2_>
        void operator()(iterator2_ begin, size_t size) {
            while(size && static_cast<char_type>(*begin) && my != myend) {
                *my = static_cast<char_type>(*begin);
                ++my;
                ++begin;
                --size;
            }
            if(size && static_cast<char_type>(*begin))
                mytruncated = true;
        }
        
        template<typename iterator2_>
        void operator()(iterator2_ begin, iterator2_ end) {
            while(begin != end && static_cast<char_type>(*begin) && my != myend) {
                *my = static_cast<char_type>(*begin);
                ++my;
                ++begin;
            }
            if(begin != end && static_cast<char_type>(*begin))
                mytruncated = true;
        }
    
        iterator_ at() const {
            return my;
        }
    
        bool truncated() const {
            return mytruncated;
        }
    };
    
    
    
    template<typename function_>
    class write_to_unbuffered
    {
        function_ *my;
        char mylast = 0;

    public:
        explicit write_to_unbuffered(function_& a) :
            my{&a}
        {}
        
        write_to_unbuffered(write_to_unbuffered const&) = delete;
        write_to_unbuffered& operator=(write_to_unbuffered const&) = delete;
    
        template<typename char_>
        void operator()(char_ a) {
            mylast = static_cast<char>(a);
            (*my)(mylast);
        }
        
        template<typename iterator_>
        void operator()(iterator_ begin, size_t size) {
            while(size && static_cast<char>(*begin)) {
                mylast = static_cast<char>(*begin);
                (*my)(mylast);
                ++begin;
                --size;
            }
        }
        
        template<typename iterator_>
        void operator()(iterator_ begin, iterator_ end) {
            while(begin != end && static_cast<char>(*begin)) {
                mylast = static_cast<char>(*begin);
                (*my)(mylast);
                ++begin;
            }
        }
        
        char last() const {
            return mylast;
        }
    };
    
    template<typename function_>
    class write_to_unbuffered<function_*> : public write_to_unbuffered<function_>
    {
    public:
        explicit write_to_unbuffered(function_ *a) :
            write_to_unbuffered<function_>{*a}
        {}
    };
    

    // array version of...
    // template<typename function_, typename configuration_, unsigned size_>
    //  void function(expression<function_, configuration_>& o, char const (&array)[size_], unsigned length) {
    // ...caused compile times on clang to explode.
    
    template<typename function_>
    void function_end_or_not(function_& function, char const* begin, char const* end) {
        function(begin, end);
    }
    
    template<typename function_>
    void function_end_or_not(function_& function, char const* begin, void const*) {
        function(begin);
    }
    
    template<typename function_> char const* function_end_test(function_ *f, decltype((*f)(static_cast<char const*>(0), static_cast<char const*>(0)))*);
    template<typename function_> void const* function_end_test(function_ *f, ...);
    
    
    
    template<typename function_>
    void function(function_& function, char const* begin, char const* end) {
        function_end_or_not(function, begin, static_cast<decltype(function_end_test(&function, 0))>(end));
    }
    
    template<typename function_>
    void function(function_*& function, char const* begin, char const* end) {
        function_end_or_not(*function, begin, static_cast<decltype(function_end_test(function, 0))>(end));
    }
    
    
    
    template<typename configuration_>
    void first_expression_function(expression<void, configuration_>&, char const* /*begin*/, char const* /*end*/) {
    }
    
    template<typename function_, typename configuration_>
    void first_expression_function(expression<function_, configuration_>& x, char const *begin, char const *end) {
        function(x.function(), begin, end);
    }
    
    template<typename previous1_, typename previous2_, typename write_>
    void first_expression_function(expression<expression<previous1_, previous2_>, write_>& x, char const *begin, char const *end) {
        first_expression_function(x.previous(), begin, end);
    }
    
    
    
    template<typename function_, typename configuration_>
    auto& first_expression_function(expression<function_, configuration_>& x) {
        return x.function();
    }
    
    template<typename previous1_, typename previous2_, typename write_>
    auto& first_expression_function(expression<expression<previous1_, previous2_>, write_>& x) {
        return first_expression_function(x.previous());
    }

    
    
    template<typename function_, typename configuration_, typename to_>
    void write_do(expression<function_, configuration_>&, to_&) {
    }
    
    template<typename previous1_, typename previous2_, typename write_, typename to_>
    void write_do(expression<expression<previous1_, previous2_>, write_>& x, to_& to) {
        write_do(x.previous(), to);
        x.write(to);
    }
    
    template<typename p_, typename w_, typename iterator_>
    iterator_ write(expression<p_, w_>& x, iterator_ begin, iterator_ end) {
        if(begin == end)
            return end;
        write_to_iterators<iterator_> to{begin, end};
        write_do(x, to);
        auto at = to.at();
        // if its full, do not end in the middle of a utf8 sequence
        bool truncated = to.truncated();
        auto end_with = static_cast<typename write_to_iterators<iterator_>::char_type>(expression<p_, w_>::settings::end); // avoid visual c++ warning about conditional expression is constant
        if(end_with && at == end && at[-1] != end_with) {
            --at;
            truncated = true;
        }
        if(truncated && expression<p_, w_>::settings::utf8)
            at = utf8_adjust_end(begin, at);
        if(end_with && (at == begin || at[-1] != end_with)) {
            *at = end_with;
            ++at;
        }
        return at;
    }
    
    
    
    template<typename a_>
    struct first_expression_function_type {
        using result = a_;
    };
    
    template<typename a_, typename b_>
    struct first_expression_function_type<expression<a_, b_>> :
        first_expression_function_type<a_>
    {};



    template<
        typename expression_,
        bool empty_ = expression_::size == expression_::settings::extra_size + (expression_::settings::end ? 1 : 0),
        typename function_ = typename first_expression_function_type<expression_>::result
    >
    struct write_if {
        static void do_it(expression_& x) {
            char array[expression_::size + 1]; // add 0 at the end
            auto end = _::write(x, array, array + expression_::size);
            if(end != array) {
                *end = 0;
                first_expression_function(x, array, end);
                #ifdef WATER_XTR_SIZE_USED
                WATER_XTR_SIZE_USED(expression_::size, static_cast<unsigned>(end - array));
                #endif
            }
        }
    };
    
    template<typename expression_, typename function_>
    struct write_if<expression_, false, unbuffered<function_>> {
        static void do_it(expression_& x) {
            // this will not 0-terminate
            write_to_unbuffered<function_> to{first_expression_function(x)};
            write_do(x, to);
            auto end_with = expression_::settings::end; // avoid visual c++ warning about conditional expression is constant
            if(end_with && to.last() != end_with)
                to(end_with);
        }
    };
    
    template<typename expression_, typename function_>
    struct write_if<expression_, true, function_> {
        static void do_it(expression_&) {}
    };
    
    template<typename expression_>
    struct write_if<expression_, false, void> {
        static void do_it(expression_&) {}
    };
    
    
    
    template<typename char_>
    char_* cstring_end(char_ *i) {
        if(i) while(*i) ++i;
        return i;
    }
    
}

using _::char8_or_not;



template<typename p_, typename w_>
void write(expression<p_, w_>& x) {
    _::write_if<expression<p_, w_>>::do_it(x);
}

template<typename p_, typename w_>
void write(expression<p_, w_>&& x) {
    _::write_if<expression<p_, w_>>::do_it(x);
}

template<typename p_, typename w_, typename iterator_>
iterator_ write(expression<p_, w_>& x, iterator_ begin, iterator_ end) {
    return _::write(x, begin, end);
}

template<typename p_, typename w_, typename iterator_>
iterator_ write(expression<p_, w_>&& x, iterator_ begin, iterator_ end) {
    return _::write(x, begin, end);
}



template<unsigned size_ = 0>
struct write_size
{    
    static constexpr unsigned size = size_;
    
    template<typename write_>
    void operator()(write_&&) const {
        // this is used by the operator << size<123>
    }
};



class write_char : public write_size<1>
{
    char my;
    
public:
    write_char(char a) :
        my(a)
    {}
    
    template<typename write_>
    void operator()(write_&& write) const {
        write(my);
    }
};



template<typename iterator_, typename settings_, unsigned size_ = 0>
class write_string :
    public write_size<size_ ? size_ : settings_::string_size>
{
    iterator_ mybegin, myend;

public:
    write_string(iterator_ begin, iterator_ end) :
        mybegin(begin),
        myend(end)
    {}

    write_string(iterator_ cstring) :
        mybegin(cstring),
        myend(_::cstring_end(cstring))
    {}

    template<typename write_>
    void operator()(write_&& write) const {
        write(mybegin, myend);
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
        _::no_const_reference<decltype(_::make<range_ const&>().begin())>,
        settings_,
        size_
    >::result
{
    write_range(range_ const& a) :
        write_range_select<
            _::no_const_reference<decltype(_::make<range_ const&>().begin())>,
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

    template<typename write_>
    void operator()(write_&& write) const {
        if(my)
            write(myuppercase ? u8"TRUE" : u8"true", 4);
        else
            write(myuppercase ? u8"FALSE" : u8"false", 5);
    }
};



namespace _ {

    constexpr unsigned bytebits(unsigned char a = static_cast<unsigned char>(-1)) {
        return
            a >= 0x7f ? 7 + bytebits(a >> 7) :
            a > 0 ? 1 + bytebits(a >> 1) :
            0;
    }
    
    template<typename write_>
    void write_base_prefix(write_&& write, unsigned base, bool uppercase) {
        if(base != 10) {
            write(static_cast<char>(u'0'));
            write(
                base == 2 ? static_cast<char>(uppercase ? u'B' : u'b') :
                base == 8 ? static_cast<char>(uppercase ? u'O' : u'o') :
                static_cast<char>(uppercase ? u'X' : u'x')
            );
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

private:
    type_ my;

public:
    write_int(type_ a) :
        my(a)
    {}

    template<typename write_>
    void operator()(write_&& write) const {
        bool const minus = !is_unsigned && my < 0;
        if(minus)
            write(static_cast<char>(u'-'));
        else if(number_format_::show_plus)
            write(static_cast<char>(u'+'));
        if(!number_format_::hide_base)
            _::write_base_prefix(write, base, number_format_::uppercase);
        // write digits
        type_ constexpr
            b = static_cast<type_>(base),
            minus1 = is_unsigned ? static_cast<type_>(1) : static_cast<type_>(-1); // is_unsigned avoids warnings on math
        type_
            i = my,
            divide = 1;
        unsigned
            digits = 1,
            digits_fixed = number_format_::digits ? max_digits : 0;
        if(i < 0) {
            divide *= minus1;
            i /= b * minus1;
            if(i && digits_fixed != 1) {
                divide *= b;
                ++digits;
            }
        }
        while(i >= b && digits != digits_fixed) {
            i /= b;
            divide *= b;
            ++digits;
        }
        // leading zeros
        while(digits_fixed > digits) {
            write(static_cast<char>(_::digits[!number_format_::lowercase][0]));
            --digits_fixed;
        }
        // digits
        while(digits) {
            unsigned d =
               divide == minus1 ?
               static_cast<unsigned>((my % b) * minus1) : // signed 2s complement -min > max
               static_cast<unsigned>((my / divide) % b);
            write(static_cast<char>(_::digits[!number_format_::lowercase][d]));
            divide /= b;
            --digits;
        }
    }
};



// operator<<(expression, char const(&)[size_])
// operator<<(expression, char const*)
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
auto operator<<(expression<p_, w_>&& x, char a) -> expression<expression<p_, w_>, write_char> {
    return {x, a};
}

template<typename p_, typename w_, size_t size_>
auto operator<<(expression<p_, w_>&& x, char const (&a)[size_]) -> expression<expression<p_, w_>, write_string<char const*, typename expression<p_, w_>::settings, static_cast<unsigned>(size_)>> {
    return {x, {a, a + size_}};
}

template<typename p_, typename w_>
auto operator<<(expression<p_, w_>&& x, char_const_pointer<char> a) -> expression<expression<p_, w_>, write_string<char const*, typename expression<p_, w_>::settings>> {
    return {x, {a.pointer}};
}

template<typename p_, typename w_>
auto operator<<(expression<p_, w_>&& x, char8_or_not a) -> expression<expression<p_, w_>, write_char> {
    return {x, static_cast<char>(a)};
}

template<typename p_, typename w_, size_t size_>
auto operator<<(expression<p_, w_>&& x, char8_or_not const (&a)[size_]) -> expression<expression<p_, w_>, write_string<char8_or_not const*, typename expression<p_, w_>::settings, static_cast<unsigned>(size_)>> {
    return {x, {a, a + size_}};
}

template<typename p_, typename w_>
auto operator<<(expression<p_, w_>&& x, char_const_pointer<char8_or_not> a) -> expression<expression<p_, w_>, write_string<char8_or_not const*, typename expression<p_, w_>::settings>> {
    return {x, {a.pointer}};
}

template<typename p_, typename w_, typename range_>
auto operator<<(expression<p_, w_>&& x, range_ const& a) -> expression<expression<p_, w_>, write_range<range_, typename expression<p_, w_>::settings>> {
    return {x, a};
}

template<typename p_, typename w_, typename bool_>
auto operator<<(expression<p_, w_>&& x, bool_ a) -> expression<expression<p_, w_>, typename if_bool<bool_>::result> {
    return {x, {a, expression<p_, w_>::number_format::uppercase}};
}

template<typename p_, typename w_>
auto operator<<(expression<p_, w_>&& x, int a) -> expression<expression<p_, w_>, write_int<int, typename expression<p_, w_>::number_format>> {
    return {x, a};
}

template<typename p_, typename w_>
auto operator<<(expression<p_, w_>&& x, unsigned int a) -> expression<expression<p_, w_>, write_int<unsigned int, typename expression<p_, w_>::number_format>> {
    return {x, a};
}

template<typename p_, typename w_>
auto operator<<(expression<p_, w_>&& x, long a) -> expression<expression<p_, w_>, write_int<long, typename expression<p_, w_>::number_format>> {
    return {x, a};
}

template<typename p_, typename w_>
auto operator<<(expression<p_, w_>&& x, unsigned long a) -> expression<expression<p_, w_>, write_int<unsigned long, typename expression<p_, w_>::number_format>> {
    return {x, a};
}

template<typename p_, typename w_>
auto operator<<(expression<p_, w_>&& x, long long a) -> expression<expression<p_, w_>, write_int<long long, typename expression<p_, w_>::number_format>> {
    return {x, a};
}

template<typename p_, typename w_>
auto operator<<(expression<p_, w_>&& x, unsigned long long a) -> expression<expression<p_, w_>, write_int<unsigned long long, typename expression<p_, w_>::number_format>> {
    return {x, a};
}

template<typename p_, typename w_>
auto operator<<(expression<p_, w_>&& x, void_const_pointer a) -> expression<expression<p_, w_>, write_int<size_t, number_format_base<16, typename expression<p_, w_>::number_format>>> {
    return {x, reinterpret_cast<size_t>(a.pointer)};
}

// x << size<123>

template<unsigned>
struct size_set;

template<unsigned size_>
size_set<size_>* size() { return 0; }

template<typename p_, typename w_, unsigned size_>
auto operator<<(expression<p_, w_>&& x, size_set<size_>* (*)()) -> expression<expression<p_, w_>, write_size<size_>> {
    return {x, {}};
}

// x << base<16>

template<unsigned>
struct base_set;

template<unsigned base_>
base_set<base_>* base() { return 0; }

template<typename p_, typename w_>
auto operator<<(expression<p_, w_>&& x, base_set<2>* (*)()) -> expression<expression<p_, w_>, set_number_format<number_format_base<2, typename expression<p_, w_>::number_format>>> {
    return {x};
}

template<typename p_, typename w_>
auto operator<<(expression<p_, w_>&& x, base_set<8>* (*)()) -> expression<expression<p_, w_>, set_number_format<number_format_base<8, typename expression<p_, w_>::number_format>>> {
    return {x};
}

template<typename p_, typename w_>
auto operator<<(expression<p_, w_>&& x, base_set<10>* (*)()) -> expression<expression<p_, w_>, set_number_format<number_format_base<10, typename expression<p_, w_>::number_format>>> {
    return {x};
}

template<typename p_, typename w_>
auto operator<<(expression<p_, w_>&& x, base_set<16>* (*)()) -> expression<expression<p_, w_>, set_number_format<number_format_base<16, typename expression<p_, w_>::number_format>>> {
    return {x};
}

// x << digits<123>

template<int>
struct digits_set;

template<int digits_>
digits_set<digits_>* digits() { return 0; }

template<typename p_, typename w_, int digits_>
auto operator<<(expression<p_, w_>&& x, digits_set<digits_>* (*)()) -> expression<expression<p_, w_>, set_number_format<number_format_digits<digits_, typename expression<p_, w_>::number_format>>> {
    return {x};
}

// x << no_exponent_min_max<-10, 10>
// x << exponent
// x << exponent_auto

template<int min_, int max_>
struct no_exponent_min_max_set;

template<int min_, int max_>
no_exponent_min_max_set<min_, max_>* no_exponent_min_max() { return 0; }

inline no_exponent_min_max_set<0, 0>* exponent() { return 0; }

inline no_exponent_min_max_set<0, -1>* exponent_auto() { return 0; }

template<typename p_, typename w_, int min_, int max_>
auto operator<<(expression<p_, w_>&& x, no_exponent_min_max_set<min_, max_>* (*)()) -> expression<expression<p_, w_>, set_number_format<number_format_no_exponent_min_max<min_, max_, typename expression<p_, w_>::number_format>>> {
    return {x};
}



// x << uppercase
// x << lowercase
// x << mixedcase

template<int> struct case_set;
inline case_set<1>* uppercase() { return 0; }
inline case_set<-1>* lowercase() { return 0; }
inline case_set<0>* mixedcase() { return 0; }

template<typename p_, typename w_, int case_>
auto operator<<(expression<p_, w_>&& x, case_set<case_>* (*)()) -> expression<expression<p_, w_>, set_number_format<number_format_case<case_, typename expression<p_, w_>::number_format>>> {
    return {x};
}

// x << show_plus
// x << hide_plus

template<bool> struct show_plus_set;
inline show_plus_set<true>* show_plus() { return 0; }
inline show_plus_set<false>* hide_plus() { return 0; }

template<typename p_, typename w_, bool show_plus_>
auto operator<<(expression<p_, w_>&& x, show_plus_set<show_plus_>* (*)()) -> expression<expression<p_, w_>, set_number_format<number_format_show_plus<show_plus_, typename expression<p_, w_>::number_format>>> {
    return {x};
}

// o << show_base
// o << hide_base

template<bool> struct hide_base_set;
inline hide_base_set<false>* show_base() { return 0; }
inline hide_base_set<true>* hide_base() { return 0; }

template<typename p_, typename w_, bool hide_base_>
auto operator<<(expression<p_, w_>&& x, hide_base_set<hide_base_>* (*)()) -> expression<expression<p_, w_>, set_number_format<number_format_hide_base<hide_base_, typename expression<p_, w_>::number_format>>> {
    return {x};
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

    template<typename write_>
    void operator()(write_&& write) const {
        auto
            f = my,
            fe = my + mybytes;
        while(f != fe) {
            if(f != my && ((my - f) % 4) == 0)
                write(static_cast<char>(u' '));
            auto d = digits;
            while(d) {
                auto x = (*f >> (--d * 4)) & 0xf;
                write(static_cast<char>(_::digits[!number_format_::lowercase][x]));
            }
            ++f;
        }
    }
};

template<unsigned bytes_>
struct byte_pointer {
    void const *pointer;
    size_t bytes;
    
    byte_pointer(void const* pointer, size_t bytes) :
        pointer(pointer),
        bytes(bytes)
    {}
};

template<typename p_, typename w_, unsigned bytes_>
auto operator<<(expression<p_, w_>&& x, byte_pointer<bytes_> a) -> expression<expression<p_, w_>, write_bytes<bytes_, typename expression<p_, w_>::number_format>> {
    return {x, {a.pointer, static_cast<unsigned>(a.bytes)}};
}

template<typename p_, typename w_>
auto operator<<(expression<p_, w_>&& x, byte_pointer<0> a) -> expression<expression<p_, w_>, write_bytes<expression<p_, w_>::settings::string_size, typename expression<p_, w_>::number_format>> {
    return {x, {a.pointer, static_cast<unsigned>(a.bytes)}};
}

template<unsigned bytes_>
byte_pointer<bytes_> bytes(void const* pointer, size_t size) {
    return {pointer, size};
}

template<unsigned bytes_>
byte_pointer<bytes_> bytes(void const* pointer) {
    return {pointer, bytes_};
}

inline byte_pointer<0> bytes(void const* pointer, size_t size) {
    return {pointer, size};
}

template<typename type_>
byte_pointer<sizeof(type_)> bytes(type_ const& a) {
    return {&a, sizeof(a)};
}



// char_array
//
// this converts an expression into a zero terminated char array
//
// it converts to char const*, it is always a cstring even when its empty.
//
// begin() / end() / size() does not include the zero termination, *end() == 0 always
//
// capacity_ is without the zero termination

template<unsigned capacity_>
class char_array
{
public:
    using size_type = unsigned;
    using value_type = char;
    using iterator = char const*;
    using const_iterator = char const*;

private:
    unsigned mysize = 0;
    char my[capacity_ + 1];

public:
    char_array() {
        my[0] = 0;
    }

    template<typename p_, typename w_>
    char_array(expression<p_, w_>& x) {
        x.dont_write();
        auto end = write(x, my, my + capacity_);
        *end = 0;
        mysize = static_cast<unsigned>(end - my);
    }
    
    template<typename p_, typename w_>
    char_array(expression<p_, w_>&& x) :
        char_array{x}
    {}

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
    
    constexpr unsigned capacity() const {
        return capacity_;
    }
};


template<typename p_, typename w_>
auto string(expression<p_, w_>&& x) -> char_array<expression<p_, w_>::size> {
    return x;
}

template<typename p_, typename w_>
auto string(expression<p_, w_>& x) -> char_array<expression<p_, w_>::size> {
    return x;
}

struct string_convert;

constexpr string_convert* string() { return 0; }

template<typename p_, typename w_>
auto operator<<(expression<p_, w_>&& x, string_convert* (*)()) {
    return char_array<expression<p_, w_>::size>{x}; // just return x; and return type after -> made clang warn of copying x
}


// no
//
// usually used like this:
// string(xtr::no << "hello");
// xtr::no << "hello" << xtr::string;

struct no_type {};

constexpr no_type no;

template<typename type_>
auto operator<<(no_type, type_&& b) -> decltype(expression<>{} << b) {
    return expression<>{} << b;
}



// to_buffered
//
// function_ will be called once per expression, and needs to work like one of these:
//   function(char const* begin, char const* end)
//   function(char const* zero_terminated_cstring)

template<typename function_, typename configuration_ = void>
class to_buffered
{
    function_ my{};

public:
    constexpr to_buffered() = default;
    constexpr to_buffered(to_buffered const&) = default;
    constexpr to_buffered(to_buffered&&) = default;
    
    template<typename ...arguments_>
    constexpr to_buffered(arguments_&&... a) :
        my{static_cast<arguments_&&>(a)...}
    {}

    function_& function() {
        return my;
    }

    function_ const& function() const {
        return my;
    }
};

template<typename function_, typename configuration_, typename type_>
auto operator<<(to_buffered<function_, configuration_>& a, type_&& b) -> decltype(expression<function_*, configuration_>{&a.function()} << b) {
    return expression<function_*, configuration_>{&a.function()} << b;
}

template<typename function_, typename configuration_, typename type_>
auto operator<<(to_buffered<function_, configuration_>&& a, type_&& b) -> decltype(expression<function_*, configuration_>{&a.function()} << b) {
    return expression<function_*, configuration_>{&a.function()} << b;
}

template<typename function_, typename configuration_, typename type_>
auto operator<<(to_buffered<function_, configuration_> const& a, type_&& b) -> decltype(expression<function_ const*, configuration_>{&a.function()} << b) {
    return expression<function_ const*, configuration_>{&a.function()} << b;
}



// to_unbuffered
//
// function_ will be called for every character in the expression
//   function(char a)

template<typename function_, typename configuration_ = void>
class to_unbuffered
{
    function_ my{};

public:
    constexpr to_unbuffered() = default;
    constexpr to_unbuffered(to_unbuffered const&) = default;
    constexpr to_unbuffered(to_unbuffered&&) = default;
    
    template<typename ...arguments_>
    constexpr to_unbuffered(arguments_&&... a) :
        my{static_cast<arguments_&&>(a)...}
    {}

    function_& function() {
        return my;
    }

    function_ const& function() const {
        return my;
    }
};

template<typename function_, typename configuration_, typename type_>
auto operator<<(to_unbuffered<function_, configuration_>& a, type_&& b) -> decltype(expression<unbuffered<function_*>, configuration_>{&a.function()} << b) {
    return expression<unbuffered<function_*>, configuration_>{&a.function()} << b;
}

template<typename function_, typename configuration_, typename type_>
auto operator<<(to_unbuffered<function_, configuration_>&& a, type_&& b) -> decltype(expression<unbuffered<function_*>, configuration_>{&a.function()} << b) {
    return expression<unbuffered<function_*>, configuration_>{&a.function()} << b;
}

template<typename function_, typename configuration_, typename type_>
auto operator<<(to_unbuffered<function_, configuration_> const& a, type_&& b) -> decltype(expression<unbuffered<function_ const*>, configuration_>{&a.function()} << b) {
    return expression<unbuffered<function_ const*>, configuration_>{&a.function()} << b;
}



// to_function([](char const* begin, char const* end)) { ... })
// to_function([](char const* cstring) { ... })
// to_function_unbuffered([](char one_at_a_time) { ... })
//
// line_to_function and line_to_function_unbuffered will always end with a line break

namespace _  {

    template<typename a_>
    struct function_pointer_if_function {
        using result = a_;
    };

    template<typename r_, typename ...a_>
    struct function_pointer_if_function<r_(a_...)> {
        using result = r_(*)(a_...);
    };

}

template<typename function_>
using function_or_pointer = typename _::function_pointer_if_function<_::no_const_reference<function_>>::result;

using line_to_configuration = configuration<
    typename configuration_default::number_format,
    settings_end<typename configuration_default::settings, static_cast<char>(u'\n')>
>;

template<typename function_>
auto to_function(function_&& a) -> to_buffered<function_or_pointer<function_>> {
    return static_cast<function_&&>(a);
}

template<typename function_>
auto line_to_function(function_&& a) -> to_buffered<function_or_pointer<function_>, line_to_configuration> {
    return static_cast<function_&&>(a);
}

template<typename function_>
auto to_function_unbuffered(function_&& a) -> to_unbuffered<function_or_pointer<function_>> {
    return static_cast<function_&&>(a);
}

template<typename function_>
auto line_to_function_unbuffered(function_&& a) -> to_unbuffered<function_or_pointer<function_>, line_to_configuration> {
    return static_cast<function_&&>(a);
}



// insert_at_end

template<typename container_>
auto insert_at_end(container_& c) {
    return to_function([&c](char const *b, char const* e) { c.insert(c.end(), b, e); });
}



// to_begin_end

template<typename iterator_>
class to_iterators
{
    iterator_
        mybegin,
        myat,
        myend;
    bool mytruncated = false;

public:
    to_iterators(iterator_ begin, iterator_ end) :
        mybegin{begin},
        myat{begin},
        myend{end}
    {}
    
    iterator_ begin() const {
        return mybegin;
    }
    
    iterator_ end() const {
        // the end of the written string
        return myat;
    }
    
    bool truncated() const {
        return mytruncated;
    }
    
    explicit operator bool() const {
        return mybegin != myat && !mytruncated;
    }
    
    auto size() const {
        return static_cast<size_t>(myat - mybegin);
    }
    
    void operator()(char a) {
        if(myat == myend)
            mytruncated = true;
        else {
            *myat = a;
            ++myat;
        }
    }
};

template<typename iterator_, typename type_>
auto operator<<(to_iterators<iterator_>& a, type_&& b) -> decltype(expression<unbuffered<to_iterators<iterator_>*>>{&a} << b) {
    return expression<unbuffered<to_iterators<iterator_>*>>{&a} << b;
}

template<typename iterator_, typename type_>
auto operator<<(to_iterators<iterator_>&& a, type_&& b) -> decltype(expression<unbuffered<to_iterators<iterator_>*>>{&a} << b) {
    return expression<unbuffered<to_iterators<iterator_>*>>{&a} << b;
}

template<typename iterator_>
to_iterators<iterator_> to_begin_end(iterator_ begin, iterator_ end) {
    return {begin, end};
}

template<typename iterator_>
to_iterators<iterator_> to_begin_end(iterator_ begin, size_t size) {
    return {begin, begin + size};
}

template<typename char_, size_t size_>
to_iterators<char_*> to_begin_end(char_ (&array)[size_]) {
    return {array, array + size_};
}


}}
#endif
