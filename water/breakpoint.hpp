// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_BREAKPOINT
#include <water/configuration.hpp>

/*

Used only if WATER_DEBUG is defined.

The goal is to make ___water_assert break into the debugger, so you can examine what has happened.
And also make it possible to continue after the breakpoint.

If these do not work, make a function and set a breakpoint in it. Then define WATER_BREAKPOINT to that function

This tries to define WATER_BREAKPOINT to something that breaks into the debugger.
Otherwise it will assert(0) that will probably call abort

With Android Studio 2 until now 2023 raise(SIGSTOP) seems like the best option.
With __builtin_debugtrap() and raise(SIGTRAP) it is not possible to continue from the debugger.

*/

// look for builtin like this because of GCC 9
#ifdef __has_builtin
    #if __has_builtin(__builtin_debugtrap)
        #define WATER_BREAKPOINT_HAS_DEBUGTRAP
    #endif
#endif


#if defined(WATER_COMPILER_MICROSOFT)
    #define WATER_BREAKPOINT __debugbreak()
    // vc8 has no inline asm for x86 64-bit, int3 would work on 32-bit only

#elif defined(WATER_BREAKPOINT_HAS_DEBUGTRAP) && !defined(WATER_SYSTEM_ANDROID)
    #undef WATER_BREAKPOINT_HAS_DEBUGTRAP
    #define WATER_BREAKPOINT __builtin_debugtrap()
    // clang has this

#elif defined(WATER_COMPILER_GCC) && (defined(__i386__) || defined(__amd64__) || defined(__x86_64__))
    #define WATER_BREAKPOINT asm("int3")

#elif defined(WATER_COMPILER_CLANG) && (defined(__i386__) || defined(__amd64__) || defined(__x86_64__))
    #define WATER_BREAKPOINT __asm__("int $3")

#elif defined(WATER_SYSTEM_POSIX)
    #include <signal.h>
    // SIGTRAP is xopen
    #if (defined(_XOPEN_SOURCE) || defined(SIGTRAP)) && !defined(WATER_SYSTEM_ANDROID)
        #define WATER_BREAKPOINT ::raise(SIGTRAP)
    #else
        #define WATER_BREAKPOINT ::raise(SIGSTOP)
    #endif

#elif defined(WATER_SYSTEM_WINDOWS)
    #include <water/windows.hpp>
    namespace water { namespace breakpoint_hpp {
        WATER_WINDOWS_FUNCTION(void, DebugBreak, ());
    }}
    #define WATER_BREAKPOINT ::water::breakpoint_hpp::DebugBreak()

#else
    #ifndef assert // assert.h would redefine it
        #ifdef WATER_NO_CHEADERS
            #include <assert.h>
        #else
            #include <cassert>
        #endif
    #endif
    #define WATER_BREAKPOINT assert(0)

#endif

#endif
