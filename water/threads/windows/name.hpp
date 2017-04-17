// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_WINDOWS_NAME_HPP
#define WATER_THREADS_WINDOWS_NAME_HPP
#include <water/threads/windows/bits.hpp>
namespace water { namespace threads {

// water-threads-label-processid-address
class name {
	public:
		static unsigned constexpr
			label_size = 32,
			buffer_size =
				sizeof("water-threads-") + // includes 0 at the end
				label_size + 1 + // label-
				sizeof(GetCurrentProcessId()) * 2 + 1 + // processid-
				sizeof(ulongptr_t) * 2; //pointer
	private:
		wchar_t my[buffer_size];
	public:
		template<size_t size_>
		 name(char const (&label)[size_], void const *pointer) noexcept {
			static_assert(size_ < label_size + 1, "too long");
			___water_assert(pointer);
			wchar_t *t = my;
			char const n[] = "water-threads-", *f = n, *fe = n + sizeof(n) - 1;
			do *t++ = *f; while(++f != fe);
			f = label;
			fe = label + size_ - 1;
			do *t++ = *f; while(++f != fe);
			*t++ = '-';
			put(t, GetCurrentProcessId());
			put(t, reinterpret_cast<ulongptr_t>(pointer));
			*t = 0;
			___water_assert(t < my + buffer_size);
			}
		operator wchar_t const* () const noexcept {
			return my;
			}
	private:
		template<typename unsigned_>
		 static void put(wchar_t *& t, unsigned_ u) noexcept {
			wchar_t
				*b = t,
				*i = t += sizeof(u) * 2;
			do {
				wchar_t d = static_cast<wchar_t>(u & 0xf);
				*--i = d < 10 ? L'0' + d : L'a' + d - 10;
				u >>= 4;
				} while(b != i);
			}
	};

}}
#endif
