// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_MEMORY_TRACK_STATISTICS_BY_NAME_HPP
#define WATER_MEMORY_TRACK_STATISTICS_BY_NAME_HPP
#include <water/memory_track/statistics.hpp>
#include <water/memory_track/container.hpp>
namespace water { namespace memory_track {

struct statistics_for_name : statistics {
	char const* name = 0; // can be 0
	size_t sort = 0;
	};

class statistics_by_name {
	public:
		using value_type = statistics_for_name; 
	private:
		container<value_type> my;
	public:
		constexpr statistics_by_name() = default;
		statistics_by_name(statistics_by_name const&) = delete;
		statistics_by_name& operator=(statistics_by_name const&) = delete;
		void reset() noexcept {
			my.resize(0);
			}
		value_type* begin() noexcept {
			return my.begin();
			}
		value_type const* begin() const noexcept {
			return my.begin();
			}
		value_type* end() noexcept {
			return my.end();
			}
		value_type const* end() const noexcept {
			return my.end();
			}
		size_t size() const noexcept {
			return my.size();
			}
		value_type* find(char const *a) noexcept {
			auto f = find_do(a);
			if(f.exact) return my.begin() + f.at;
			return 0;
			}
		value_type const* find(char const *a) const noexcept {
			auto f = find_do(a);
			if(f.exact) return my.begin() + f.at;
			return 0;
			}
		value_type find_and_copy(char const *a) const noexcept {
			if(auto r = find(a))
				return *r;
			return {};
			}
		value_type* find_or_add(char const *a) noexcept {
			auto f = find_do(a);
			if(f.exact)
				return my.begin() + f.at;
			value_type v;
			v.name = a;
			v.sort = f.sort;
			return my.insert(f.at, v);
			}
	private:
		size_t sort(char const* c) const noexcept {
			size_t r = 0;
			if(c && *c) {
				size_t l = 0;
				do {
					r ^= (r << 7) ^ static_cast<unsigned char>(*c++);
					++l;
					} while(*++c);
				r += l;
				}
			return r;
			}
		struct find_return {
			size_t sort;
			size_t at;
			bool exact;
			};
		find_return find_do(char const* name) const {
			find_return r{sort(name)};
			if(my.empty())
				return r;
			auto
				i = my.begin(),
				e = my.end();
			while(i != e) {
				auto x = i + (e - i) / 2;
				if(x->sort < r.sort)
					++(i = x);
				else
					e = x;
				}
			auto ii = i;
			while(ii != my.end() && ii->sort == r.sort)
				if(tag(ii->name) == tag(name)) {
					i = ii;
					r.exact = true;
					break;
					}
			r.at = static_cast<size_t>(i - my.begin());
			return r;
			}
	};


}}
#endif
