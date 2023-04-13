// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_WINDOWS_HPP
#define WATER_WINDOWS_HPP

/*

Include windows.h in a overcomplicated way.

To avoid this, include windows.h yourself or define WATER_NO_WINDOWS_H to not include windows.h at all.

You should define NTDDI_VERSION to NTDDI_WIN7 or some other version (or define _WIN32_WINNT).
This header will not, the default NTDDI_VERSION will probalby be the latest windows version from your windows SDK.
See sdkddkver.h in the windows SDK.

It's 2023 and I could no longer remember why this was the way I have included windows.h for over 10 years.
But it still seems like the best way to do it. Maybe not in 2033.

*/

#ifdef WATER_NO_WINDOWS_H

    #define WATER_WINDOWS_CALLTYPE __stdcall
    // dword_t WATER_WINDOWS_CALLTYPE function(void*);
    
    #define WATER_WINDOWS_FUNCTION(return_type, function, args) extern "C" __declspec(dllimport) return_type WATER_WINDOWS_CALLTYPE function args
    // WATER_WINDOWS_FUNCTION(dword_t, WaitForSingleObject, (void*, dword_t));
        
    #define WATER_WINDOWS_HANDLE(type) struct type ## _struct*
    // typedef WATER_WINDOWS_HANDLE(HINSTANCE) hinstance_t;
        
    #define WATER_WINDOWS_SELECT(windows_h, no_windows_h) no_windows_h
    // dword_t const infinite = WATER_WINDOWS_SELECT(INFINITE, 0xfffffffful)

#else //WATER_NO_WINDOWS_H

    #define WATER_WINDOWS_CALLTYPE WINAPI
    #define WATER_WINDOWS_FUNCTION(return_type, function, args) using ::function
    #define WATER_WINDOWS_HANDLE(type) ::type
    #define WATER_WINDOWS_SELECT(windows_h, no_windows_h) windows_h

    // If windows.h is already included NTDDI_WIN10 from sdkddkver.h is probably defined
    #ifndef NTDDI_WIN10
        #ifdef WATER_COMPILER_MICROSOFT
            #pragma warning(push)
        #endif
        #include <windows.h>
        #ifdef WATER_COMPILER_MICROSOFT
            #pragma warning(pop)
        #endif
        // another way to avoid min max is to define NOMINMAX before including windows.h
        // this is better because it does on define anything?
        #undef min
        #undef max
        // near and far are defined to nothing. This is from 16-bit windows, but it is still defined even on windows for ARM64 in 2023.
        // The windows headers seem to only use uppercase NEAR and FAR, but those are defined to lowercase near and far.
        // So it works to undefine the lowercase ones and re-define the uppercase ones to nothing.
        #undef near
        #undef far
        #ifdef NEAR
            #undef NEAR
            #define NEAR
        #endif
        #ifdef FAR
            #undef FAR
            #define FAR
        #endif
        // rpcndr.h will define small as char and hyper as __int64
        // small does not seem to be used anywhere in the windows SDK, but hyper is and maybe undefining it is wrong.
        #undef small
        #undef hyper
    #endif

#endif //WATER_NO_WINDOWS_H

#include <water/int.hpp>
#include <water/types.hpp>
#include <water/is_no_to.hpp>
namespace water { namespace windows_hpp {

#ifdef WATER_NO_WINDOWS_H

    typedef int bool_t;
    typedef wchar_t char_t;
    
    typedef unsigned short word_t;
    typedef unsigned long dword_t;
    
    typedef int int_t;
    typedef unsigned int uint_t;
    typedef ifel<sizeof(int) == sizeof(void*), int, int_size<sizeof(void*)>> intptr_t;
    typedef to_unsigned<intptr_t> uintptr_t;
    
    typedef long long_t;
    typedef unsigned long ulong_t;
    typedef ifel<sizeof(long) == sizeof(void*), long, int_size<sizeof(void*)>> longptr_t;
    typedef to_unsigned<intptr_t> ulongptr_t;
    
    typedef int_bits_at_least<64> longlong_t;
    typedef to_unsigned<longlong_t> ulonglong_t;
    
    typedef long_t hresult_t;
    typedef ulongptr_t sizet_t;

#else

    typedef ::BOOL bool_t;
    typedef ::WCHAR char_t;
    
    typedef ::WORD word_t;
    typedef ::DWORD dword_t;
    
    typedef ::INT int_t;
    typedef ::UINT uint_t;
    typedef ::INT_PTR intptr_t;
    typedef ::UINT_PTR uintptr_t;
    
    typedef ::LONG long_t;
    typedef ::ULONG ulong_t;
    typedef ::LONG_PTR longptr_t;
    typedef ::ULONG_PTR ulongptr_t;
    
    typedef ::LONGLONG longlong_t;
    typedef ::ULONGLONG ulonglong_t;
    
    typedef ::HRESULT hresult_t;
    typedef ::SIZE_T sizet_t;

#endif

}}
#endif
