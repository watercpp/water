// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_STR_BEGIN_END_HPP
#define WATER_STR_BEGIN_END_HPP
#include <water/str/out.hpp>
namespace water { namespace str {

/*

write to a begin,end range

this can end in an incomplete utf-8 or utf-16 sequence

*/

template<typename iterator_> class
 begin_end {
	public:
		using iterator = iterator_;
		using char_type = typename types::no_const<types::no_reference<decltype(*types::make<iterator_&>())>>::result;
	private:
		iterator
			mybegin {},
			myat {},
			myend {};
	public:
		begin_end() = default;
		begin_end(iterator begin, iterator end) :
			mybegin{begin},
			myat{begin},
			myend{end}
			{}
		iterator begin() const {
			return mybegin;
			}
		iterator end() const {
			return myend;
			}
		iterator at() const {
			return myat;
			}
		void at(iterator a) {
			myat = a;
			}
		template<typename iterator2_> void operator()(iterator2_ begin, iterator2_ end) {
			while(begin != end && myat != myend) {
				*myat = *begin;
				++myat;
				++begin;
				}
			}
	};

}}
#endif
