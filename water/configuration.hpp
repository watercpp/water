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
Defined if one of these compilers are detected.

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
    
    // The intel compilers will be detected as one of the three above depending on the
    // compiler version and operating system. This is fine.
        
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

#endif //WATER_NO_CONFIGURATION
#endif //WATER_CONFIGURATION_HPP
