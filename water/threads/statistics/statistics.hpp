// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_STATISTICS_STATISTICS_HPP
#define WATER_THREADS_STATISTICS_STATISTICS_HPP
#include <water/threads/statistics/list.hpp>
namespace water { namespace threads { namespace statistics {

// this is the "public" interface to the statistics.

inline size_t size() noexcept {
	return list<>::get().size();
	}

inline data* at(size_t a) noexcept {
	// returns 0 if a >= size()
	return list<>::get().at(a);
	}

inline data* find(void const* address) noexcept {
	// find by address of mutex/read_write/semaphore/barrier
	// returns the first, even if there are more with the same address
	return list<>::get().find(address);
	}

inline data* find(name const& n) noexcept {
	// find by name of mutex/read_write/semaphore/barrier
	// returns the first, even if there are more with the same name
	return list<>::get().find(n);
	}

inline data* get(reference& to, void const* address, char const* class_name) noexcept {
	data* r = atomic::get<atomic::none>(to);
	if(r) return r;
	return list<>::get().create(to, address, class_name);
	}

class add {
	reference *myto;
	void const* myaddress;
	char const* myclass;
	signed char
 		mywait = -1,
 		mywake = -1,
 		mytimeout = -1,
 		mycreated = 0;
 	public:
 		add(reference& to, void const* address, char const* class_name) noexcept :
 			myto(&to),
 			myaddress(address),
			myclass(class_name)
			{}
 		~add() noexcept {
 			// do everything here, after locking.
 			// otherwise the atomic operations will affect statistics more?
 			data *d = atomic::get<atomic::none>(*myto);
 			if(!d)
 				d = list<>::get().create(*myto, myaddress, myclass);
 			if(d) {
 				if(mywait != -1) d->wait_add(mywait != 0);
 				if(mywake != -1) d->wake_add(mywake != 0);
 				if(mytimeout != -1) d->timeout_add(mytimeout != 0);
 				if(mycreated) d->wait_object_created_set();
 				}
 			}
 		add(add const&) = delete;
 		add& operator=(add const&) = delete;
 		add& wait(bool good) noexcept {
 			mywait = good;
 			return *this;
 			}
 		add& wake(bool good) noexcept {
 			mywake = good;
 			return *this;
 			}
 		add& timeout(bool good) noexcept {
 			mytimeout = good;
 			return *this;
 			}
 		add& wait_object_created() noexcept {
 			mycreated = 1;
			return *this;
 			}
 		template<typename bool_>
 		 add& wait_object_created(bool_ set) noexcept {
 			mycreated = mycreated || set;
 			return *this;
 			}
 	};

}}}
#endif
