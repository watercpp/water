// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_THREAD_HPP
#define WATER_THREADS_THREAD_HPP
#include <water/threads/configuration.hpp>
#include WATER_THREADS_INCLUDE(thread.hpp)
namespace water { namespace threads {

template<typename class_, void (class_::*function_)()> struct
 member_function {
	using pointer = class_*;
	void operator()(pointer p) { (p->*function_)(); }
	};

template<typename type_, void (*function_)(type_*)> struct
 function {
	using pointer = type_*;
	void operator()(pointer p) { (*function_)(p); }
	};

#if 0

using thread_t = some_type;

struct join_t {};

bool constexpr
	priority_exists = true,
	stack_size_exists = true;

thread_t thread() noexcept;

thread_t thread(join_t a) noexcept;

bool equal(thread_t a, thread_t b) noexcept;

bool join(join_t a) noexcept;

bool dont_join(join_t a) noexcept;

class run_options {
	public:
		run_options& priority(unsigned a) noexcept;
		run_options& stack_size(size_t a) noexcept;
		run_options& qos(qos_t a) noexcept;
	};

template<typename call_> bool run(typename call_::pointer pointer, run_options const& options = {}) noexcept;
template<typename call_> bool run(typename call_::pointer pointer, thread_t& thread, run_options const& options = {}) noexcept;
template<typename call_> bool run(typename call_::pointer pointer, join_t& join, run_options const& options = {}) noexcept;

template<typename function_> bool run(function_& function, run_options const& options = {}) noexcept;
template<typename function_> bool run(function_& function, thread_t& thread, run_options const& options = {}) noexcept;
template<typename function_> bool run(function_& function, join_t& join, run_options const& options = {}) noexcept;
// function must exist as long as the thread runs!

// priority is 1 or more. 0 is an error

unsigned priority_default() noexcept;

class priority {
	public:
		priority() noexcept = default;
		explicit priority(thread_t a) noexcept;
		explicit priority(join_t a) noexcept;
		operator unsigned() const noexcept;
		unsigned min() const noexcept;
		unsigned max() const noexcept;
		bool set(unsigned a) noexcept;
	};

// apple qos
// it matters for A10 processors where qos_default can run on a slow core

enum qos_t {
	qos_error = 0,
	qos_user_interactive,
	qos_user_initiated,
	qos_default,
	qos_utility,
	qos_background,
	qos_unspecified
	};

inline qos_t qos(thread_t) noexcept;
inline qos_t qos(join_t) noexcept;
inline qos_t qos() noexcept;
inline bool qos(qos_t) noexcept;

#endif

}}
#endif
