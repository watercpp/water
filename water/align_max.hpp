// Copyright 2017-2018 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_ALIGN_MAX_HPP
#define WATER_ALIGN_MAX_HPP
namespace water {

/*

This could probably be replaced by alignof(std::max_aling_t) from the C standard library, the maximum
alignment of any scalar type.


Memory aligned to this should be aligned for all normal c++ types, but maybe not for some types like SIMD vector types.
If the largest alignment is not a multiple of all other smaller alignments, this will not work

*/

namespace _ {
	
	template<typename a_> constexpr a_ align_max_do(a_ a, a_ b) {
		return a >= b ? a : b;
		}
	template<typename a_, typename... c_> constexpr a_ align_max_do(a_ a, a_ b, c_... c) {
		return align_max_do(a >= b ? a : b, c...);
		}
	
	class align_max_class {
		union types {
			int int_;
			long long_;
			long long long_long;
			float float_;
			double double_;
			long double long_double;
			void* pointer;
			char array[sizeof(void*) * sizeof(void*)];
			int (*function)();
			int (align_max_class::*member_function)();
			int align_max_class::*member;
			} my[2];
		public:
			explicit align_max_class(int i) {
				my[0].pointer = this;
				my[1].int_ = i;
				}
			~align_max_class() {
				(*my[0].function)();
				}
		};

}

unsigned constexpr align_max = static_cast<unsigned>(_::align_max_do(
	alignof(int),
	alignof(long),
	alignof(long long),
	alignof(float),
	alignof(double),
	alignof(long double),
	alignof(void*),
	alignof(char[sizeof(void*) * sizeof(void*)]),
	alignof(int (*)()),
	alignof(int (_::align_max_class::*)()),
	alignof(int _::align_max_class::*),
	alignof(_::align_max_class)
	));

}
#endif
