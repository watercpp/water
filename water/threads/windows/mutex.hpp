// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_WINDOWS_MUTEX_HPP
#define WATER_THREADS_WINDOWS_MUTEX_HPP
#include <water/threads/windows/critical_section.hpp>
#include <water/threads/windows/mutex_event.hpp>
#include <water/threads/windows/mutex_handle.hpp>
#include <water/threads/windows/srwlock.hpp>
#include <water/threads/spin_mutex.hpp>
#include <water/threads/recursive.hpp>
namespace water { namespace threads {

using mutex_list = list<
	#ifndef WATER_THREADS_MUTEX_PREFER_WINDOWS_CRITICAL_SECTION
	srwlock,
	#endif
	critical_section,
	#ifdef WATER_THREADS_MUTEX_PREFER_WINDOWS_CRITICAL_SECTION
	srwlock,
	#endif
	mutex_event,
	mutex_event_named,
	recursive<mutex_event>,
	recursive<mutex_event_named>,
	mutex_handle,
	spin_mutex<>
	>;

}}
#endif
