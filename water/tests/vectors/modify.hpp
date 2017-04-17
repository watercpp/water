// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_TESTS_VECTORS_MODIFY_HPP
#define WATER_TESTS_VECTORS_MODIFY_HPP
#include <water/tests/vectors/bits.hpp>
namespace water { namespace tests { namespace vectors {

// modifying functions

struct modify_one {
	template<typename vector_> modify_one(vector_&& a) {
		___water_test(!a.empty());
		vector_ b = a;
		
		b.assign(a.begin(), a.end());
		b.clear();
		b.reserve(a.size());
		b.assign(a.begin(), a.size());
		___water_test(a.size() == b.size());
		b.assign(a.size(), a[0]);
		___water_test(a.size() == b.size());
		b.assign(downgrade_iterators::input_from(a.begin()), downgrade_iterators::input_from(a.end()));
		___water_test(a.size() == b.size());
		b.assign(downgrade_iterators::input_from(a.begin()), a.size());
		___water_test(a.size() == b.size());
		
		// assign nothing
		b.assign(downgrade_iterators::random_access_proxied_const_from(a.begin()), downgrade_iterators::random_access_proxied_const_from(a.begin()));
		___water_test(b.size() == 0);
		b.assign(downgrade_iterators::random_access_proxied_const_from(a.begin()), 0);
		___water_test(b.size() == 0);
		
		b.assign(downgrade_iterators::input_proxied_from(a.begin()), downgrade_iterators::input_proxied_from(a.end()));
		___water_test(a.size() == b.size());
		b.assign(downgrade_iterators::input_proxied_from(a.begin()), a.size());
		___water_test(a.size() == b.size());
		b.assign(downgrade_iterators::forward_proxied_const_from(a.begin()), downgrade_iterators::forward_proxied_const_from(a.end()));
		___water_test(a.size() == b.size());
		b.assign(downgrade_iterators::forward_proxied_const_from(a.begin()), a.size());
		___water_test(a.size() == b.size());
		b.assign(downgrade_iterators::forward_const_from(a.begin()), downgrade_iterators::forward_const_from(a.end()));
		___water_test(a.size() == b.size());
		b.assign(downgrade_iterators::forward_const_from(a.begin()), a.size());
		___water_test(a.size() == b.size());
		b.assign(downgrade_iterators::bidirectional_proxied_const_from(a.begin()), downgrade_iterators::bidirectional_proxied_const_from(a.end()));
		___water_test(a.size() == b.size());
		b.assign(downgrade_iterators::bidirectional_proxied_const_from(a.begin()), a.size());
		___water_test(a.size() == b.size());
		b.assign(downgrade_iterators::bidirectional_const_from(a.begin()), downgrade_iterators::bidirectional_const_from(a.end()));
		___water_test(a.size() == b.size());
		b.assign(downgrade_iterators::bidirectional_const_from(a.begin()), a.size());
		___water_test(a.size() == b.size());
		b.assign(downgrade_iterators::random_access_proxied_const_from(a.begin()), downgrade_iterators::random_access_proxied_const_from(a.end()));
		___water_test(a.size() == b.size());
		b.assign(downgrade_iterators::random_access_proxied_const_from(a.begin()), a.size());
		___water_test(a.size() == b.size());
		b.assign(downgrade_iterators::random_access_const_from(a.begin()), downgrade_iterators::random_access_const_from(a.end()));
		___water_test(a.size() == b.size());
		b.assign(downgrade_iterators::random_access_const_from(a.begin()), a.size());
		___water_test(a.size() == b.size());
		
		b.resize(b.size() * 2, a[0]);
		
		b.emplace_back(a[0]);
		b.emplace(b.begin() + 2, a[0]);
		b.erase(b.begin() + 2);
		
		b.push_back(a[0]);
		b.pop_back();
		
		typename vector_::value_type v = a[0];
		b.push_back(static_cast<typename vector_::value_type&&>(v));
		
		v = a[0];
		b.insert(b.begin() + b.size() / 2, static_cast<typename vector_::value_type&&>(v));
		v = a[0];
		b.insert(b.begin() + b.size() - 1, static_cast<typename vector_::value_type const&>(v));
		b.insert(b.begin() + 1, 3, v);
		
		b.insert(b.end(), b.begin(), b.end());
		b.erase(b.begin() + a.size(), b.end());
		b.insert(b.begin(), a.begin(), a.size());
		b.erase(b.begin(), b.begin() + a.size());
		
		typename vector_::size_type bs = b.size();
		b.insert(b.end() - a.size() / 2, downgrade_iterators::input_from(a.begin()), downgrade_iterators::input_from(a.end()));
		b.insert(b.end() - a.size() / 2, downgrade_iterators::input_from(a.begin()), a.size());
		___water_test(b.size() == bs + a.size() + a.size());
		
		bs = b.size();
		b.insert(b.end() - a.size(), downgrade_iterators::input_proxied_from(a.begin()), downgrade_iterators::input_proxied_from(a.end()));
		b.insert(b.end() - a.size(), downgrade_iterators::input_proxied_from(a.begin()), a.size());
		___water_test(b.size() == bs + a.size() + a.size());
		
		bs = b.size();
		b.insert(b.end() - a.size() / 5, downgrade_iterators::forward_proxied_const_from(a.begin()), downgrade_iterators::forward_proxied_const_from(a.end()));
		b.insert(b.end() - a.size() / 5, downgrade_iterators::forward_proxied_const_from(a.begin()), a.size());
		___water_test(b.size() == bs + a.size() + a.size());
		
		bs = b.size();
		b.insert(b.end(), downgrade_iterators::forward_const_from(a.begin()), downgrade_iterators::forward_const_from(a.end()));
		b.insert(b.end(), downgrade_iterators::forward_const_from(a.begin()), a.size());
		___water_test(b.size() == bs + a.size() + a.size());
		
		bs = b.size();
		b.insert(b.end() - a.size() / 2, downgrade_iterators::bidirectional_proxied_const_from(a.begin()), downgrade_iterators::bidirectional_proxied_const_from(a.end()));
		b.insert(b.end() - a.size() / 2, downgrade_iterators::bidirectional_proxied_const_from(a.begin()), a.size());
		___water_test(b.size() == bs + a.size() + a.size());
		
		bs = b.size();
		b.insert(b.end() - a.size(), downgrade_iterators::bidirectional_const_from(a.begin()), downgrade_iterators::bidirectional_const_from(a.end()));
		b.insert(b.end() - a.size(), downgrade_iterators::bidirectional_const_from(a.begin()), a.size());
		___water_test(b.size() == bs + a.size() + a.size());
		
		bs = b.size();
		b.insert(b.end() - a.size() / 2, downgrade_iterators::random_access_proxied_const_from(a.begin()), downgrade_iterators::random_access_proxied_const_from(a.end()));
		b.insert(b.end() - a.size() / 2, downgrade_iterators::random_access_proxied_const_from(a.begin()), a.size());
		___water_test(b.size() == bs + a.size() + a.size());
		
		bs = b.size();
		b.insert(b.end() - 1, downgrade_iterators::random_access_const_from(a.begin()), downgrade_iterators::random_access_const_from(a.end()));
		b.insert(b.end() - 1, downgrade_iterators::random_access_const_from(a.begin()), a.size());
		___water_test(b.size() == bs + a.size() + a.size());
		
		// insert nothing
		bs = b.size();
		b.insert(b.end() - a.size() / 2, downgrade_iterators::random_access_proxied_const_from(a.begin()), downgrade_iterators::random_access_proxied_const_from(a.begin()));
		b.insert(b.end() - a.size() / 2, downgrade_iterators::random_access_proxied_const_from(a.begin()), 0);
		___water_test(b.size() == bs);
		
		b.resize(b.size() + 1, a[0]);
	
		b.shrink_to_fit();
		
		swap(a, b);
		}
	};

struct modify {
	modify() {
		modify_one{vector<int>(5)};
		value_complex_count c;
		modify_one{vector<value_complex>(5, value_complex(c))};
		___water_test(c.count == 0);
		modify_one{vector<value_simple>(5)};
		}
	};

}}}
#endif
