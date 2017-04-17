// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_WINDOWS_ONCE_HPP
#define WATER_THREADS_WINDOWS_ONCE_HPP
#include <water/threads/windows/bits.hpp>
namespace water { namespace threads {

class once {
	init_once_t my{};
	public:
		constexpr once() noexcept = default;
		once(once const&) = delete;
		once& operator=(once const&) = delete;
		template<typename function_>
		 void operator()(function_ a) {
			panic_if_not(InitOnceExecuteOnce(&my, &function<function_>, &a, 0));
			}
	private:
		template<typename function_>
		 static bool_t WATER_WINDOWS_CALLTYPE function(init_once_t*, void *pointer, void**) noexcept {
			(*static_cast<function_*>(pointer))();
			return 1;
			}
	};

}}
#endif
