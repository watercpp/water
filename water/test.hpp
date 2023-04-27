// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_TEST_HPP
#define WATER_TEST_HPP

/*

___water_test(something);

If something is false, this will outupt file, line, function and something then breakpoint.
Define WATER_TEST_FAILED or ___water_test yourself if there is a better way.


All compilers seem to have "assignment within conditional expression" warnings. On all compilers this can be avided with:

    if(static_cast<bool>(__VA_ARGS__) == true)

But then Visual C++ 2015 has warning: C4800: 'void *': forcing value to bool 'true' or 'false' (performance warning)
This workaround with a const reference to remove the __VA_ARGS__ from the conditional seems to work:

    auto const& water_test_result = (__VA_ARGS__);
    if(water_test_result)

The Intel C++ older compiler based on EDG and not LLVM is detected as WATER_COMPILER_MICROSOFT on Windows but it does not have __FUNCSIG__

*/

#ifndef WATER_TEST_FAILED
    #include <water/trace.hpp>
    #define WATER_TEST_FAILED(file, line, function, code) ::water::trace << "___water_test failed " << file << ':' << line << ' ' << function << ' ' << code; ___water_breakpoint();
#endif

#ifdef ___water_test
    // done
#elif defined(WATER_COMPILER_MICROSOFT) && !defined(__INTEL_COMPILER)
    #define ___water_test(...) do { auto const& water_test_result = (__VA_ARGS__); if(water_test_result) break; WATER_TEST_FAILED(__FILE__, __LINE__, __FUNCSIG__, #__VA_ARGS__); } while(0)
#elif defined(WATER_COMPILER_CLANG) || defined(WATER_COMPILER_GCC)
    #define ___water_test(...) do { auto const& water_test_result = (__VA_ARGS__); if(water_test_result) break; WATER_TEST_FAILED(__FILE__, __LINE__, __PRETTY_FUNCTION__, #__VA_ARGS__); } while(0)
#else
    #define ___water_test(...) do { auto const& water_test_result = (__VA_ARGS__); if(water_test_result) break; WATER_TEST_FAILED(__FILE__, __LINE__, __func__, #__VA_ARGS__); } while(0)
#endif

namespace water {

    template<typename ...arguments_>
    constexpr void test_unused(arguments_ const&...)
    {}

}

#endif
