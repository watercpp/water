// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_NUMBERS_TESTS_WORD_LOCALE_HPP
#define WATER_NUMBERS_TESTS_WORD_LOCALE_HPP
#include <water/numbers/tests/bits.hpp>
namespace water { namespace numbers { namespace tests {

/*

locale where everything is strings

writes numbers like this
MinusHexadecimalOnePointOneFourSevenFifteenGroupOneNineThirteenThirteenGroupSevenFoorteenElevenZeroGroupTwoSpacePowerMinusThreeThreeTwo

*/

struct locale_words
{
    locale_const_string<char>
        mydigits[16] = {
            {"Zero", 4},
            {"One", 3},
            {"Two", 3},
            {"Three", 5},
            {"Four", 4},
            {"Five", 4},
            {"Six", 3},
            {"Seven", 5},
            {"Eight", 5},
            {"Nine", 4},
            {"Ten", 3},
            {"Eleven", 6},
            {"Twelve", 6},
            {"Thirteen", 8},
            {"Foorteen", 8}, // bad spelling because four
            {"Fifteen", 7}
        },
        mypluses[1] = {{"Plus", 4}},
        myminuses[1] = {{"Minus", 5}},
        mygroups[1] = {{"Group", 5}},
        mypoints[1] = {{"Point", 5}},
        myspaces[1] = {{"Space", 5}},
        myinfinities[1] = {{"Infinity", 8}},
        mynans[1] = {{"Nan", 3}},
        mytrues[1] = {{"Yes", 3}},
        myfalses[1] = {{"No", 2}};
    
    locale_const_base<char> mybases[3] = {
        {2, {"Binary", 6}},
        {8, {"Octal", 5}},
        {16, {"Hexadecimal", 11}}
    };
    
    locale_const_exponent<char> myexponents[2] = {
        {10, {"Exponent", 8}},
        {2, {"Power", 5}}
    };
    
    numbers::group mygroup;

public:
    using begin_end_type = begin_end<locale_const_string<char> const*>;

public:
    begin_end_type digits() const {
        return make(mydigits);
    }

    begin_end_type pluses() const {
        return make(mypluses);
    }

    begin_end_type minuses() const {
        return make(myminuses);
    }

    begin_end_type groups() const {
        return make(mygroups);
    }

    begin_end_type points() const {
        return make(mypoints);
    }

    begin_end_type spaces() const {
        return make(myspaces);
    }

    begin_end_type infinities() const {
        return make(myinfinities);
    }

    begin_end_type nans() const {
        return make(mynans);
    }

    begin_end_type trues() const {
        return make(mytrues);
    }

    begin_end_type falses() const {
        return make(myfalses);
    }

    begin_end<locale_const_base<char> const*> bases() const {
        return make(mybases);
    }

    begin_end<locale_const_exponent<char> const*> exponents() const {
        return make(myexponents);
    }

    numbers::group group_function() const {
        return {};
    }

private:
    template<typename type_, size_t size_>
    begin_end<type_ const*> make(type_ const (&a)[size_]) const {
        return {a + 0, a + size_};
    }
};

inline void locale_words_all() {
    char x[10000] = {};
    locale_words locale;
    settings settings_array[] = {
        settings{},
        settings{}.group(true).digits(10).scientific(true).exponent(),
        settings{}.base(2),
        settings{}.base(8).plus(true),
        settings{}.base(10),
        settings{}.base(16).plus(true),
        settings{}.base(2).group(true).space(true).plus(true),
        settings{}.base(8).group(true).space(true),
        settings{}.base(10).group(true).space(true).plus(true),
        settings{}.base(16).group(true).space(true),
        settings{}.base(10).no_exponent().trailing_zeros(true).digits(255),
        settings{}.base(10).scientific(true).exponent(),
        settings{}.base(10).scientific(false).exponent()
    };
    double array[] = {
        0,
        1,
        -1,
        10,
        -10,
        -1.23456789012345678901234567890e-30,
        1.23456789012345678901234567890e10,
        1.2345e100, // scientific mode test
        -1.2345e-100, // scientific mode test
        1.2345e101, // scientific mode test
        -1.2345e-101, // scientific mode test
    };
    for(auto s : settings_array) {
        for(auto a : array) {
            write<double> w(a, s);
            size_t ws = w.template size<char>(locale);
            char *xe = w(x, sizeof(x), locale);
            size_t xs = static_cast<size_t>(xe - x);
            ___water_test(xs);
            ___water_test(ws == xs);
            ___water_test(xe < (x + sizeof(x)) && !*xe);
            //trace << x;
            
            read<double> r(s);
            char *re = r(x, x + sizeof(x), locale);
            size_t ru = r.used();
            size_t rs = static_cast<size_t>(re - x);
            ___water_test(ru);
            ___water_test(ru == rs);
            ___water_test(ru == xs);
            
            //trace << "  wrote " << a << '\n' << "  read  " << static_cast<double>(r);
            
            while(xe != x) *--xe = 0;
        }
    }
}

}}}
#endif
