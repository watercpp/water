// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_MACROS_HPP
#define WATER_MACROS_HPP
#include <water/configuration.hpp>

/*

Macros for debugging and assertions. Define WATER_DEBUG to enable them.

These macros use lowercase names beginning with 3 underscores, like ___water_something, because all
uppercase macros tend to take focus away from the surrounding code. The C++ standard 17.4.3.1.2 says
names containing double underscore are reserved for the implementation. If these macros are ever a
problem, it should be easy to search for and replace them.

All of the macros can be overridden by defining them before water/macros.hpp is included.

*/

#ifndef ___water_assert
    #ifndef WATER_DEBUG
        #define ___water_assert(...)
    #else
        #if \
        defined(WATER_COMPILER_CLANG) || \
        defined(WATER_COMPILER_GCC)
            #define ___water_assert(...) if(!(__VA_ARGS__)) { ::water::assert_trace("___water_assert(" #__VA_ARGS__ ")", __FILE__, __LINE__, __PRETTY_FUNCTION__); ___water_breakpoint(); }
        #elif \
        defined(WATER_COMPILER_MICROSOFT)
            #define ___water_assert(...) if(!(__VA_ARGS__)) { ::water::assert_trace("___water_assert(" #__VA_ARGS__ ")", __FILE__, __LINE__, __FUNCSIG__); ___water_breakpoint(); }
        #else
            #define ___water_assert(...) if(!(__VA_ARGS__)) { ::water::assert_trace("___water_assert(" #__VA_ARGS__ ")", __FILE__, __LINE__, __func__); ___water_breakpoint(); }
        #endif
        #define WATER_MACROS_ASSERT_TRACE
    #endif
#endif

#ifndef ___water_breakpoint
    #ifndef WATER_DEBUG
        #define ___water_breakpoint()
    #else
        #include <water/breakpoint.hpp>
        #define ___water_breakpoint() WATER_BREAKPOINT
    #endif
#endif

#ifndef ___water_debug
    #ifdef WATER_DEBUG
        #define ___water_debug(...) __VA_ARGS__
        #define ___water_final(...)
    #else
        #define ___water_debug(...)
        #define ___water_final(...) __VA_ARGS__
    #endif
#endif

#ifdef WATER_MACROS_ASSERT_TRACE
    #undef WATER_MACROS_ASSERT_TRACE
    #ifdef WATER_ASSERT_TRACE
    namespace water {
        inline void assert_trace(char const *what, char const *file, unsigned long line, char const *function) noexcept {
            WATER_ASSERT_TRACE(what, file, line, function);
        }
    }
    #else
    #include <water/trace.hpp>
    namespace water {
        inline void assert_trace(char const *what, char const *file, unsigned long line, char const *function) noexcept {
            // ___water_assert(something)
            //   somefile.cxx:1234
            //   void somefunction()
            //
            // do not rewrite this to use array-template arguments instead of char-pointers, compile-times will be long.
            trace << xtr::reserve<2048> << what << "\n  " << file << ':' << line << "\n  " << function;
        }
    }
    #endif
#endif
#endif
