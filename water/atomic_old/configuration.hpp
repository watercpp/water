// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_ATOMIC_OLD_CONFIGURATION_HPP
#define WATER_ATOMIC_OLD_CONFIGURATION_HPP
#include <water/water.hpp>

// to use a new underlying implementation with water atomic
// - modify this header to know how to select it, or...
// - include the underlying before including anything else in water::atomic_old
//   and define WATER_ATOMIC_OLD_UNDERLYING to the fully qualified namespace of
//   the underlying

#ifdef WATER_ATOMIC_OLD_UNDERLYING
namespace water { namespace atomic_old {
	namespace underlying = WATER_ATOMIC_OLD_UNDERLYING;
}}
		
// atomic_ops library, you must define WATER_ATOMIC_OLD_HAS_ATOMIC_OPS
#elif defined(WATER_ATOMIC_OLD_HAS_ATOMIC_OPS)
#include <water/atomic_old/atomic_ops/atomic_ops.hpp>
namespace water { namespace atomic_old {
	namespace underlying = atomic_ops;
}}

#elif defined(WATER_ATOMIC_OLD_HAS_GCC) || (defined(WATER_COMPILER_GCC) && WATER_COMPILER_GCC >= 41) || defined(WATER_COMPILER_CLANG)
#include <water/atomic_old/sync/sync.hpp>
namespace water { namespace atomic_old {
	namespace underlying = sync;
}}

// Mac OS X 10.4 or later
#elif defined(WATER_SYSTEM_APPLE)
#include <water/atomic_old/osatomic/osatomic.hpp>
namespace water { namespace atomic_old {
	namespace underlying = osatomic;
}}

// Visual C++ 2005 or later
#elif defined(WATER_COMPILER_MICROSOFT) && WATER_COMPILER_MICROSOFT >= 1400
#include <water/atomic_old/interlocked_visual_cpp/interlocked_visual_cpp.hpp>
namespace water { namespace atomic_old {
	namespace underlying = interlocked_visual_cpp;
}}

// Windows API Interlocked
#elif defined(WATER_SYSTEM_WINDOWS)
#include <water/atomic_old/interlocked/interlocked_windows.hpp>
namespace water { namespace atomic_old {
	namespace underlying = interlocked_windows;
}}

#else
#define WATER_NO_ATOMIC
namespace water { namespace atomic_old { namespace underlying {
	
	// dummy underlying to make it possible to select via templates

	typedef long int_t;

	inline void barrier() {}

	template<typename t_, int b_> struct
	 atomic {
		enum has_e { has = 0 };
		};

	template<typename t_> struct
	 atomic_type_from {
		typedef void result;
		};
	
}}}

#endif
#endif
