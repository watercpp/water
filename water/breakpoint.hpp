// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_BREAKPOINT
#include <water/configuration.hpp>

/*

Used only if WATER_DEBUG is defined.

If these do not work, make a function and set a breakpoint in it. Then define WATER_BREAKPOINT to that function

This tries to define WATER_BREAKPOINT to something that breaks into the debugger.
Otherwise it will assert(0) that will probably call abort;

*/

#if \
defined(WATER_COMPILER_MICROSOFT)
    #define WATER_BREAKPOINT __debugbreak()
    // vc8 has no inline asm for x86 64-bit, int3 would work on 32-bit only

#elif \
defined(WATER_CPU_X86) && \
defined(WATER_COMPILER_GCC)
    #define WATER_BREAKPOINT asm("int3")

#elif \
defined(WATER_CPU_X86) && \
defined(WATER_COMPILER_CLANG)
    #define WATER_BREAKPOINT __asm__("int $3")

#elif \
defined(WATER_SYSTEM_POSIX)
    #include <signal.h>
    // SIGTRAP is xopen
    #if \
    defined(_XOPEN_SOURCE) || \
    defined(SIGTRAP)
        #define WATER_BREAKPOINT ::raise(SIGTRAP)
    #else
        #define WATER_BREAKPOINT ::raise(SIGSTOP)
    #endif

#elif \
defined(WATER_SYSTEM_WINDOWS)
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
