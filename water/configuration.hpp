// Copyright 2017-2022 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_CONFIGURATION_HPP
#define WATER_CONFIGURATION_HPP
#ifndef WATER_NO_CONFIGURATION // define to skip this configuration

/*

Everything should configure itself and work on any compiler, hardware or operating system.
If it does not, the problem could be here.

WATER_DEBUG
Define to enable debugging and assertions

WATER_NO_STD
Define if there is no standard C++ library.

WATER_NO_CHEADERS
Define to include C library headers as <header.h> insetead of <cheader>, for example <math.h> instead of <cmath>

WATER_NO_EXCEPTIONS
Define if exceptions are turned off

WATER_COMPILER_CLANG
WATER_COMPILER_GCC
WATER_COMPILER_MICROSOFT
Defined if one of these compilers are detected

WATER_CPU_ARM
WATER_CPU_POWERPC
WATER_CPU_X86
Defined to the number of bits (32 or 64)

WATER_ENDIAN_BIG
WATER_ENDIAN_LITTLE
Almost never used.

WATER_SYSTEM_APPLE
WATER_SYSTEM_LINUX (defined for android)
WATER_SYSTEM_WINDOWS
WATER_SYSTEM_POSIX (some posix things should exist)

WATER_NO_RTTI
Define if run-time type information (typeid, dynamic_cast) is turned off. Almost never used.

*/

// WATER_COMPILER_*

#if \
!defined(WATER_COMPILER_CLANG) && \
!defined(WATER_COMPILER_GCC) && \
!defined(WATER_COMPILER_MICROSOFT)

    #if defined(__clang__)
        #define WATER_COMPILER_CLANG
        #if !defined(WATER_NO_RTTI) && !__has_feature(cxx_rtti)
            #define WATER_NO_RTTI
        #endif
        #if !defined(WATER_NO_EXCEPTIONS) && !__has_feature(cxx_exceptions)
            #define WATER_NO_EXCEPTIONS
        #endif
        
    #elif defined(__GNUC__)
        #define WATER_COMPILER_GCC ((__GNUC__ * 10) + __GNUC_MINOR__) // 4.1 is 41
        #if WATER_COMPILER_GCC >= 41 && !defined(WATER_NO_EXCEPTIONS) && !defined(__EXCEPTIONS)
            #define WATER_NO_EXCEPTIONS
        #endif
        #if WATER_COMPILER_GCC >= 43 && !defined(WATER_NO_RTTI) && !defined(__GXX_RTTI)
            #define WATER_NO_RTTI
        #endif
        
    #elif defined(_MSC_VER)
        #define WATER_COMPILER_MICROSOFT _MSC_VER
        // visual c++ 2003 / 7.1 is 1310
        // visual c++ 2005 / 8.0 is 1400
        #if !defined(WATER_NO_RTTI) && !defined(_CPPRTTI)
            #define WATER_NO_RTTI
        #endif
        #if !defined(WATER_NO_EXCEPTIONS) && !defined(_CPPUNWIND)
            #define WATER_NO_EXCEPTIONS
        #endif
        
    #endif
#endif

// WATER_CPU_*

#if \
!defined(WATER_CPU_ARM) && \
!defined(WATER_CPU_POWERPC) && \
!defined(WATER_CPU_X86)

    #if \
    defined(__aarch64__) || \
    defined(__arm64__) || \
    defined(_M_ARM64) || \
    defined(_M_ARM64EC)
        #define WATER_CPU_ARM 64

    #elif \
    defined(__arm__) || \
    defined(_M_ARM)
        #define WATER_CPU_ARM 32

    #elif \
    defined(__powerpc__) || \
    defined(__POWERPC__) || \
    defined(__ppc__) || \
    defined(__PPC__) || \
    defined(powerc)
        #if \
        defined(__ppc64__) || \
        defined(__LP64__)
            #define WATER_CPU_POWERPC 64
        #else
            #define WATER_CPU_POWERPC 32
        #endif
    
    #elif \
    defined(__amd64__) || \
    defined(__x86_64__) || \
    defined(_M_AMD64) || \
    defined(_M_X64)
        #define WATER_CPU_X86 64
    
    #elif \
    defined(__i386__) || \
    defined(__i386) || \
    defined(_M_IX86) || \
    defined(_X86_) || \
    defined(__INTEL__)
        #define WATER_CPU_X86 32
    
    #endif
#endif

// WATER_SYSTEM_*

#if \
!defined(WATER_SYSTEM_APPLE) && \
!defined(WATER_SYSTEM_LINUX) && \
!defined(WATER_SYSTEM_WINDOWS)
    
    #if \
    defined(linux) || \
    defined(__linux) || \
    defined(__linux__)
        #define WATER_SYSTEM_LINUX

    #elif \
    defined(__APPLE__) || \
    defined(macintosh) || \
    defined(__MACOS__)
        #define WATER_SYSTEM_APPLE

    #elif \
    defined(_WIN32) || \
    defined(_WIN64)
        #define WATER_SYSTEM_WINDOWS

    #endif
#endif

#if \
!defined(WATER_SYSTEM_POSIX)
    #if \
    defined(_XOPEN_SOURCE) || \
    defined(_POSIX_C_SOURCE) || \
    defined(__unix__) || \
    defined(__unix) || \
    defined(unix) || \
    defined(WATER_SYSTEM_LINUX) || \
    defined(WATER_SYSTEM_APPLE)
        #define WATER_SYSTEM_POSIX
    #endif
#endif

// WATER_ENDIAN_*

#if \
!defined(WATER_ENDIAN_BIG) && \
!defined(WATER_ENDIAN_LITTLE)

    #if \
        defined(__BIG_ENDIAN__) || \
        defined(_BIG_ENDIAN) || \
        (defined(__BYTE_ORDER__) && defined(__ORDER_BIG_ENDIAN__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
            #define WATER_ENDIAN_BIG

    #elif \
        defined(__LITTLE_ENDIAN__) || \
        defined(_LITTLE_ENDIAN) || \
        (defined(__BYTE_ORDER__) && defined(__ORDER_LITTLE_ENDIAN__) && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__) || \
        defined(WATER_CPU_X86) || \
        defined(WATER_SYSTEM_WINDOWS)
            #define WATER_ENDIAN_LITTLE

    #endif
#endif

#endif //WATER_NO_CONFIGURATION
#endif //WATER_CONFIGURATION_HPP
