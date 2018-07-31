// Copyright 2018 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_BASE64_TESTS_ALL_HPP
#define WATER_BASE64_TESTS_ALL_HPP
#include <water/water.hpp>
#include <water/test.hpp>
#include <water/base64/base64.hpp>
namespace water { namespace base64 { namespace tests {

inline bool equal(void const* a, void const* b, size_t s) {
	auto
		aa = static_cast<byte const*>(a),
		ae = aa + s,
		bb = static_cast<byte const*>(b);
	while(aa != ae && *aa == *bb) {
		++aa;
		++bb;
		}
	return aa == ae;
	}

template<size_t size_> void size() {
	byte
		original[size_],
		decode2[size_],
		encode1[encode_size(size_)],
		encode2[encode_size(size_)];
	for(auto& a : original)
		a = static_cast<byte>((&a - original) & 0xff);
	
	// encode
	size_t s = encode(encode1, original, size_);
	___water_test(s == encode_size(size_));
	size_t used = s;
	switch(size_ % 3) {
		case 0:
			___water_test(decode_size(s) == size_);
			___water_test(encode1[s - 1] != pad);
			___water_test(encode1[s - 2] != pad);
			break;
		case 1:
			___water_test(decode_size(s) == size_ + 2);
			___water_test(encode1[s - 1] == pad);
			___water_test(encode1[s - 2] == pad);
			used -= 2;
			break;
		case 2:
			___water_test(decode_size(s) == size_ + 1);
			___water_test(encode1[s - 1] == pad);
			___water_test(encode1[s - 2] != pad);
			used -= 1;
			break;
		}
	size_t verify = used;
	while(verify)
		___water_test(decode(encode1[--verify]) != no);
	
	// decode encoded
	s = decode(decode2, encode1, s);
	___water_test(s == size_);
	___water_test(equal(original, decode2, size_));
	
	// decode encoded, without pad at end
	s = decode(decode2, encode1, used);
	___water_test(s == size_);
	___water_test(equal(original, decode2, size_));
	
	// encode decoded again
	s = encode(encode2, decode2, s);
	___water_test(s == encode_size(size_));
	___water_test(equal(encode1, encode2, encode_size(size_)));
	
	// encode in parts
	size_t
		part = 3 * 8,
		left = size_,
		to = 0,
		from = 0;
	do {
		if(part > left) part = left;
		to += encode(encode1 + to, original + from, part);
		from += part;
		left -= part;
		} while(left);
	___water_test(to == encode_size(size_));
	___water_test(equal(encode1, encode2, encode_size(size_)));

	// decode in parts
	part = 4 * 8;
	left = encode_size(size_);
	to = 0;
	from = 0;
	do {
		if(part > left) part = left;
		to += decode(decode2 + to, encode1 + from, part, left != part);
		from += part;
		left -= part;
		} while(left);
	___water_test(to == size_);
	___water_test(equal(original, decode2, size_));
	
	}

inline void all() {
	size<1>();
	size<2>();
	size<3>();
	size<4>();
	size<5>();
	size<6>();
	size<7>();
	size<8>();
	size<3 * 8 - 1>();
	size<3 * 8 + 1>();
	size<123>();
	size<546>();
	size<1024>();
	size<1023>();
	size<1022>();
	size<1021>();
	size<1020>();
	};

}}}
#endif
