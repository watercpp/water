// Copyright 2017-2018 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_BASE64_BASE64_HPP
#define WATER_BASE64_BASE64_HPP
// no dependency
namespace water { namespace base64 { 

/*

https://en.wikipedia.org/wiki/Base64
https://tools.ietf.org/html/rfc4648

3 8 bit bytes are stored as 4 6 bit base64 characters
out0 = in0 6 high bits
out1 = (in0 2 low bits << 4) | (in1 high 4 bits)
out2 = (in1 4 low bits << 2) | (in2 high 2 bits)
out3 = in2 low 6 bits

if the input is not a multiple of 3, the base64 can be padded with 1 or 2 = to make it 4
AA== 1 byte
AAA= 2 bytes

it is also possible to skip padding. if it ends with
- 1 character = broken
- 2 characters = 1 byte
- 3 characters = 2 bytes

*/

using byte = unsigned char;
using uint32 = char32_t;
using size_t = decltype(sizeof(0));

constexpr byte
	no = 255,
	pad = 0x3d, // =
	encode_table[64] {
		0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4a,0x4b,0x4c,0x4d,0x4e,0x4f,0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5a, // 0-25 'A'-'Z' 0x41-0x5a
		0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6a,0x6b,0x6c,0x6d,0x6e,0x6f,0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7a, // 26-51 'a'-'z' 0x61-0x7a
		0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39, // 52-61, '0'-'9', 0x30-0x39
		0x2b, // 62 '+'
		0x2f // 63 '/'
		},
	decode_table[128] {
		no,no,no,no,no,no,no,no,no,no,no,no,no,no,no,no, //0x00 0x0f
		no,no,no,no,no,no,no,no,no,no,no,no,no,no,no,no, //0x10 0x1f
		no,no,no,no,no,no,no,no,no,no,no,62,no,no,no,63, //0x20 0x2f
		52,53,54,55,56,57,58,59,60,61,no,no,no,no,no,no, //0x30 0x3f
		no, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14, //0x40 0x4f
		15,16,17,18,19,20,21,22,23,24,25,no,no,no,no,no, //0x50 0x5f
		no,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40, //0x60 0x6f
		41,42,43,44,45,46,47,48,49,50,51,no,no,no,no,no  //0x70 0x7f
		};

template<typename char_> inline byte decode(char_ a) {
	unsigned long c = static_cast<unsigned long>(a);
	return c >= 128 ? no : decode_table[c];
	}
template<> inline byte decode<byte>(byte a) {
	return a >= 128 ? no : decode_table[a];
	}
template<> inline byte decode<char>(char a) {
	return decode<byte>(static_cast<byte>(a));
	}
template<> inline byte decode<signed char>(signed char a) {
	return decode<byte>(static_cast<byte>(a));
	}

inline constexpr size_t encode_size(size_t a) {
	// including padding = at the end
	return (a / 3) * 4 + (a % 3 ? 4 : 0);
	}

inline constexpr size_t decode_size(size_t a) {
	// actual size can be 2 bytes less, if a is a multiple of 4 and it ends with ==
	return
		(a & 3) == 1 ? 0 :
		(a / 4) * 3 + ((a & 3) == 3 ? 2 : (a & 3) == 2 ? 1 : 0);
	}
	
template<typename to_> void encode(to_ to, void const *from, size_t bytes) {
	// to(byte, byte, byte = '=', byte = '=');
	// feed this muliple of 3 bytes to encode in parts
	if(!from || !bytes)
		return;
	auto *f = static_cast<byte const*>(from);
	size_t three = bytes / 3;
	while(three) {
		uint32 x = (static_cast<uint32>(f[0]) << 16) | (static_cast<uint32>(f[1]) << 8) | f[2];
		to(
			encode_table[(x >> 18) & 0x3f],
			encode_table[(x >> 12) & 0x3f],
			encode_table[(x >> 6) & 0x3f],
			encode_table[x & 0x3f]
			);
		f += 3;
		--three;
		}
	bytes %= 3;
	if(bytes == 1) {
		to(
			encode_table[(f[0] >> 2) & 0x3f],
			encode_table[(f[0] & 3) << 4]
			);
		}
	else if(bytes == 2) {
		uint32 x = (static_cast<uint32>(f[0]) << 8) | f[1];
		to(
			encode_table[(x >> 10) & 0x3f],
			encode_table[(x >> 4) & 0x3f],
			encode_table[(x & 0xf) << 2]
			);
		}
	}

inline size_t encode(byte *to, void const *from, size_t bytes) {
	// this will always write encode_size(bytes) bytes, it returns that
	byte *t = to;
	encode([&t](byte a, byte b, byte c = pad, byte d = pad) { t[0] = a; t[1] = b; t[2] = c; t[3] = d; t += 4;}, from, bytes);
	return static_cast<size_t>(t - to);
	}

inline size_t encode(char *to, void const *from, size_t bytes) {
	return encode(static_cast<byte*>(static_cast<void*>(to)), from, bytes);
	}

inline size_t encode(void *to, void const *from, size_t bytes) {
	return encode(static_cast<byte*>(to), from, bytes);
	}

template<typename to_> size_t decode(to_ to, void const *from, size_t bytes, bool more = false) {
	// to(byte)
	//
	// returns
	// - the number of bytes decoded
	// - 0 if error or from/bytes was empty
	//
	// feed this multiple of four bytes to decode in parts, more = true if there is more after this
	//
	// to can overwrite from
	//
	if(!from || !bytes)
		return 0;
	size_t
		four = bytes / 4,
		last = bytes % 4;
	auto *f = static_cast<byte const*>(from);
	if(!more) {
		if(last == 1)
			return 0;
		if(!last && f[bytes - 1] == pad) {
			if(f[bytes - 2] == pad) {
				last = 2;
				bytes -= 2;
				}
			else {
				last = 3;
				--bytes;
				}
			--four;
			}
		}
	else if(last)
		return 0;
	bytes = four * 3;
	while(four) {
		byte x[4] {decode(f[0]), decode(f[1]), decode(f[2]), decode(f[3])};
		if(x[0] == no || x[1] == no || x[2] == no || x[3] == no)
			return 0;
		uint32 u =
			(static_cast<uint32>(x[0]) << 18) |
			(static_cast<uint32>(x[1]) << 12) |
			(static_cast<uint32>(x[2]) << 6) |
			x[3];
		to(static_cast<byte>(u >> 16));
		to(static_cast<byte>(u >> 8) & 0xff);
		to(static_cast<byte>(u) & 0xff);
		--four;
		f += 4;
		}
	if(last == 3) {
		byte x[3] {decode(f[0]), decode(f[1]), decode(f[2])};
		if(x[0] == no || x[1] == no || (x[2] & 0x3)) // will work for x[2] == no
			return 0;
		uint32 u =
			(static_cast<uint32>(x[0]) << 10) |
			(static_cast<uint32>(x[1]) << 4) |
			(x[2] >> 2);
		to(static_cast<byte>(u >> 8) & 0xff);
		to(static_cast<byte>(u) & 0xff);
		bytes += 2;
		}
	else if(last == 2) {
		byte x[2] {decode(f[0]), decode(f[1])};
		if(x[0] == no || (x[1] & 0xf))
			return 0;
		to(static_cast<byte>(x[0] << 2) | (x[1] >> 4));
		++bytes;
		}
	return bytes;
	}

inline size_t decode(byte *to, void const* from, size_t bytes, bool more = false) {
	// this will always write decode_size(bytes) or 1 or 2 bytes less
	return decode([&to](byte a) { *to++ = a; }, from, bytes, more);
	}

inline size_t decode(char *to, void const* from, size_t bytes, bool more = false) {
	return decode(static_cast<byte*>(static_cast<void*>(to)), from, bytes, more);
	}

inline size_t decode(void *to, void const* from, size_t bytes, bool more = false) {
	return decode(static_cast<byte*>(to), from, bytes, more);
	}

}}
#endif
