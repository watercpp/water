// Copyright 2018 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_STR_TESTS_DEFAULT_SETTINGS_LOCALE_FLUSH_HPP
#define WATER_STR_TESTS_DEFAULT_SETTINGS_LOCALE_FLUSH_HPP
#include <water/str/tests/bits.hpp>
namespace water { namespace str { namespace tests {

/*

test that the write classes defalt_settings(), locale() and flush() are detected and used

this was broken on clang 5.0.1

*/

namespace default_settings_locale_flush {

    struct base {
        template<typename iterator_>
        void operator()(iterator_ /*begin*/, iterator_ /*end*/) {}
    };

    struct has_default_settings : base {
        bool used = false;
        settings default_settings() {
            used = true;
            return settings{}.base(3).digits(123);
        }
    };

    struct no_default_settings : base {};

    inline void default_settings_test() {
        out<has_default_settings> o1;
        out<no_default_settings> o2;
        ___water_test(o1.used);
        ___water_test(o1.settings().base() == 3 && o1.settings().digits() == 123);
        ___water_test(o2.settings().base() != 3 && o2.settings().digits() != 123);
    }

    struct has_locale : base {
        bool used = false;
        numbers::locale_lowercase locale() {
            used = true;
            return {};
        }
    };

    struct no_locale : base {};

    inline void locale_test() {
        out<has_locale> o1;
        out<no_locale> o2;
        o1 << 123;
        o2 << 123;
        ___water_test(o1.used);
        static_assert(types::equal<decltype(o1.locale()), numbers::locale_lowercase>::result, "");
    }

    struct has_flush : base {
        bool used = false;
        void flush() { used = true; }
    };

    struct no_flush : base {};

    inline void flush_test() {
        out<has_flush> o1;
        out<no_flush> o2;
        o1 << str::flush;
        o2 << str::flush;
        ___water_test(o1.used);
    }

}

inline void default_settings_locale_flush_all() {
    default_settings_locale_flush::default_settings_test();
    default_settings_locale_flush::locale_test();
    default_settings_locale_flush::flush_test();
}

}}}
#endif
