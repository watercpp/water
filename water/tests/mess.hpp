// Copyright 2017-2018 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
/*

define some macros and see what happens

*/

#ifndef WATER_TESTS_MESS
	#define WATER_TESTS_MESS(name) ___ ## name ## _is_a_macro, ___water_tests_mess_hpp =,
	#endif

// iso646
/*
#ifndef and
	#define and WATER_TESTS_MESS(and)
	#endif
#ifndef and_eq
	#define and_eq WATER_TESTS_MESS(and_eq)
	#endif
#ifndef bitand
	#define bitand WATER_TESTS_MESS(bitand)
	#endif
#ifndef bitor
	#define bitor WATER_TESTS_MESS(bitor)
	#endif
#ifndef compl
	#define compl WATER_TESTS_MESS(compl)
	#endif
#ifndef not
	#define not WATER_TESTS_MESS(not)
	#endif
#ifndef not_eq
	#define not_eq WATER_TESTS_MESS(not_eq)
	#endif
#ifndef or
	#define or WATER_TESTS_MESS(or)
	#endif
#ifndef or_eq
	#define or_eq WATER_TESTS_MESS(or_eq)
	#endif
#ifndef xor
	#define xor WATER_TESTS_MESS(xor)
	#endif
#ifndef xor_eq
	#define xor_eq WATER_TESTS_MESS(xor_eq)
	#endif
*/

// windows.h
#ifndef max
	#define max(a, b) WATER_TESTS_MESS(max)
	#endif
#ifndef min
	#define min(a, b) WATER_TESTS_MESS(min)
	#endif
#ifndef far
	#define far WATER_TESTS_MESS(far)
	#endif
#ifndef near
	#define near WATER_TESTS_MESS(far)
	#endif
#ifndef small // char, rpcndr.h
	#define small WATER_TESTS_MESS(small)
	#endif
#ifndef hyper // __int64, rpcndr.h
	#define hyper WATER_TESTS_MESS(hyper)
	#endif
	
// mac AssertMacros.h
#ifndef check
	#define check(a) WATER_TESTS_MESS(check)
	#endif
#ifndef ncheck
	#define ncheck(a) WATER_TESTS_MESS(ncheck)
	#endif
#ifndef check_string
	#define check_string(a) WATER_TESTS_MESS(check_string)
	#endif
#ifndef debug_string
	#define debug_string(a) WATER_TESTS_MESS(debug_string)
	#endif
#ifndef verify
	#define verify(a) WATER_TESTS_MESS(verify)
	#endif
#ifndef nverify
	#define nverify(a) WATER_TESTS_MESS(nverify)
	#endif
#ifndef verify_string
	#define verify_string(a) WATER_TESTS_MESS(verify_string)
	#endif
#ifndef require
	#define require(a) WATER_TESTS_MESS(require)
	#endif
#ifndef nrequire
	#define nrequire(a) WATER_TESTS_MESS(nrequire)
	#endif
#ifndef require_action
	#define require_action(a) WATER_TESTS_MESS(require_action)
	#endif
#ifndef require_quiet
	#define require_quiet(a) WATER_TESTS_MESS(require_quiet)
	#endif
#ifndef require_string
	#define require_string(a) WATER_TESTS_MESS(require_string)
	#endif

// linux and unix things
#ifndef linux
	#define linux WATER_TESTS_MESS(linux)
	#endif
#ifndef unix
	#define unix WATER_TESTS_MESS(unix)
	#endif
