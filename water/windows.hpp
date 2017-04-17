// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_WINDOWS_HPP
#define WATER_WINDOWS_HPP

/*

Include windows.h in a overcomplicated way.

To avoid this, include it yourself or define WATER_NO_WINDOWS_H to not include windows at all.

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

	// If windows.h is already included NTDDI_WIN7 from sdkddkver.h is probably defined. Then avoid this.
	#ifndef NTDDI_WIN7
		#ifndef NTDDI_VERSION
			#define NTDDI_VERSION NTDDI_WIN7
		#endif
		#ifndef WINVER
			#define WINVER (NTDDI_VERSION / 0x10000)
		#endif
		#ifndef _WIN32_WINNT
			#define _WIN32_WINNT (NTDDI_VERSION / 0x10000)
		#endif
		#ifdef WATER_COMPILER_MICROSOFT
			#pragma warning(push)
		#endif
		#include <windows.h>
		#ifdef WATER_COMPILER_MICROSOFT
			#pragma warning(pop)
		#endif
		#undef min 
		#undef max
		#undef near
		#undef far
		#undef small
		#undef hyper
		#ifdef NEAR
			#undef NEAR
			#define NEAR // wonder why i did this?
		#endif
		#ifdef FAR
			#undef FAR
			#define FAR
		#endif
	#endif

#endif //WATER_NO_WINDOWS_H

#ifdef NTDDI_WIN7
	#define WATER_WINDOWS_2000  NTDDI_WIN2K
	#define WATER_WINDOWS_XP    NTDDI_WINXP
	#define WATER_WINDOWS_2003  NTDDI_WS03
	#define WATER_WINDOWS_VISTA NTDDI_VISTA
	#define WATER_WINDOWS_2008  NTDDI_WS08
	#define WATER_WINDOWS_7     NTDDI_WIN7
	#define WATER_WINDOWS_8     NTDDI_WIN8
	#define WATER_WINDOWS_8_1   NTDDI_WINBLUE
	#define WATER_WINDOWS_10    NTDDI_WIN10
#else
	#define WATER_WINDOWS_2000  0x5000000
	#define WATER_WINDOWS_XP    0x5010000
	#define WATER_WINDOWS_2003  0x5020000
	#define WATER_WINDOWS_VISTA 0x6000000
	#define WATER_WINDOWS_2008  0x6000100
	#define WATER_WINDOWS_7     0x6010000
	#define WATER_WINDOWS_8     0x6020000
	#define WATER_WINDOWS_8_1   0x6030000
	#define WATER_WINDOWS_10    0x6030000
#endif

#ifdef WATER_WINDOWS_VERSION
	//done
#elif defined(NTDDI_VERSION)
	#define WATER_WINDOWS_VERSION (NTDDI_VERSION)
#elif defined(WINVER)
	#define WATER_WINDOWS_VERSION ((WINVER) * 0x10000)
#elif defined(_WIN32_WINNT)
	#define WATER_WINDOWS_VERSION ((_WIN32_WINNT) * 0x10000)
#endif

#include <water/int.hpp>
#include <water/types/types.hpp>
namespace water { namespace windows_hpp {

#ifdef WATER_NO_WINDOWS_H

	typedef int bool_t;
	typedef wchar_t char_t;
	
	typedef unsigned short word_t;
	typedef unsigned long dword_t;
	
	typedef int int_t;
	typedef unsigned int uint_t;
	typedef types::ifel<sizeof(int) == sizeof(void*), int, int_size<sizeof(void*)> >::result intptr_t;
	typedef types::to_unsigned<intptr_t>::result uintptr_t;
	
	typedef long long_t;
	typedef unsigned long ulong_t;
	typedef types::ifel<sizeof(long) == sizeof(void*), long, int_size<sizeof(void*)> >::result longptr_t;
	typedef types::to_unsigned<intptr_t>::result ulongptr_t;
	
	typedef int_bits_at_least<64>::result longlong_t;
	typedef types::to_unsigned<longlong_t>::result ulonglong_t;
	
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
