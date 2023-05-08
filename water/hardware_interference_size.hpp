// Copyright 2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_HARDWARE_INTERFERENCE_SIZE_HPP
#define WATER_HARDWARE_INTERFERENCE_SIZE_HPP
#include <water/water.hpp>
#include <water/align_max.hpp>

/*

2023-05-02
The Android NDK C++ library defines __cpp_lib_hardware_interference_size in <version> but it does not have the constants.
75% of this file is a workaround for that. If/when the constants are added in the future this file will use them.

iterator_traits is used to put functions inside the std namespace wihtout breaking the standard.
The "using namespace" in the functions makes that namespace appear as if it was part of the global namespace.
This means the functions will pick std::something before water::_::hardware_interference_size::something.

*/

#ifndef WATER_NO_STD
#include <new>
#if defined(__cpp_lib_hardware_interference_size) && !defined(WATER_HARDWARE_INTERFERENCE_SIZE_WORKAROUND) && (!defined(__STDC_HOSTED__) || __STDC_HOSTED__)
    #define WATER_HARDWARE_INTERFERENCE_SIZE_WORKAROUND
#endif
#endif

#ifdef WATER_HARDWARE_INTERFERENCE_SIZE_WORKAROUND
#include <iterator>

namespace water { namespace _ { namespace hardware_interference_size {
    
    struct specialize;
    
    size_t constexpr
        hardware_constructive_interference_size = 0,
        hardware_destructive_interference_size = 0;

}}}

namespace std {

    //size_t constexpr hardware_constructive_interference_size = 1234;

    template<>
    struct iterator_traits<water::_::hardware_interference_size::specialize> : iterator_traits<void>
    {
        static constexpr std::size_t constructive() {
            using namespace water::_::hardware_interference_size;
            return hardware_constructive_interference_size;
        }
        
        static constexpr std::size_t destructive() {
            using namespace water::_::hardware_interference_size;
            return hardware_destructive_interference_size;
        }
    };

}

#endif

namespace water {


namespace _ { namespace hardware_interference_size {

    size_t constexpr
    #ifdef WATER_HARDWARE_INTERFERENCE_SIZE_WORKAROUND
        constructive_std = std::iterator_traits<specialize>::constructive(),
        destructive_std = std::iterator_traits<specialize>::destructive();
    #elif !defined(WATER_NO_STD) && defined(__cpp_lib_hardware_interference_size)
        constructive_std = std::hardware_constructive_interference_size,
        destructive_std = std::hardware_destructive_interference_size;
    #else
        constructive_std = 0,
        destructive_std = 0;
    #endif
    
    constexpr size_t other(size_t target, size_t a = align_allocations) {
        return a < target ? other(target, a * 2) : a;
    } 
    
    size_t constexpr
    #if defined(__GCC_CONSTRUCTIVE_SIZE) && defined(__GCC_DESTRUCTIVE_SIZE)
        constructive_other = __GCC_CONSTRUCTIVE_SIZE,
        destructive_other = __GCC_DESTRUCTIVE_SIZE;
    #else
        constructive_other = other(64),
        destructive_other = other(128);
    #endif
}}


size_t constexpr hardware_constructive_interference_size =
    _::hardware_interference_size::constructive_std ?
    _::hardware_interference_size::constructive_std :
    _::hardware_interference_size::constructive_other;

size_t constexpr hardware_destructive_interference_size =
    _::hardware_interference_size::destructive_std ?
    _::hardware_interference_size::destructive_std :
    _::hardware_interference_size::destructive_other;


constexpr size_t round_up_to_hardware_destructive_interference_size(size_t a) {
    return a % hardware_destructive_interference_size ?
        (a / hardware_destructive_interference_size + 1) * hardware_destructive_interference_size :
        a;
}


}
#endif
