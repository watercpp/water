// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_TEST_HPP
#define WATER_TEST_HPP

/*

___water_test(something);

If something is false, this will outupt file, line, function and something then breakpoint.
Define WATER_TEST_FAILED or ___water_test yourself if there is a better way.

*/

#ifndef WATER_TEST_FAILED
	#include <water/trace.hpp>
	#define WATER_TEST_FAILED(file, line, function, code) ::water::trace() << "___water_test failed " << file << ':' << line << ' ' << function << ' ' << code; ___water_breakpoint();
#endif

#ifdef ___water_test
	// done
#elif defined(WATER_COMPILER_MICROSOFT)
	#define ___water_test(...) do { if(__VA_ARGS__) break; WATER_TEST_FAILED(__FILE__, __LINE__, __FUNCSIG__, #__VA_ARGS__); } while(0)
#elif defined(WATER_COMPILER_CLANG) || defined(WATER_COMPILER_GCC)
	#define ___water_test(...) do { if(__VA_ARGS__) break; WATER_TEST_FAILED(__FILE__, __LINE__, __PRETTY_FUNCTION__, #__VA_ARGS__); } while(0)
#else
	#define ___water_test(...) do { if(__VA_ARGS__) break; WATER_TEST_FAILED(__FILE__, __LINE__, __func__, #__VA_ARGS__); } while(0)
#endif

#endif
