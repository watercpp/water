// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THROW_IF_HPP
#define WATER_THROW_IF_HPP

// do not depend on anything in water

#if !defined(WATER_NO_EXCEPTIONS)
namespace water {
        
// make function signature of throw_if look the same for specialization with this...
template<typename exception_>
inline constexpr bool throw_if_noexcept() {
    return false;
}

template<>
inline constexpr bool throw_if_noexcept<void>() {
    return true;
}

template<typename exception_>
inline void throw_if() noexcept(throw_if_noexcept<exception_>()) {
    throw exception_();
}

template<>
inline void throw_if<void>() noexcept(throw_if_noexcept<void>())
{}

}

#elif defined(WATER_DEBUG)
    
namespace water {
        
template<typename exception_>
inline void throw_if() noexcept {
    ___water_assert(!"exception");
}

template<>
inline void throw_if<void>() noexcept
{}

}

#else

#ifdef WATER_NO_CHEADERS
    #include <stdlib.h>
#else
    #include <cstdlib>
#endif

namespace water {
    
template<typename exception_>
inline void throw_if() noexcept {
    #ifdef WATER_NO_CHEADERS
    abort();
    #else
    std::abort();
    #endif
}

template<>
inline void throw_if<void>() noexcept
{}

}

#endif
#endif
