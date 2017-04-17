// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_TESTS_ITERATOR_HPP
#define WATER_TESTS_ITERATOR_HPP
#include <water/iterator.hpp>
#include <water/downgrade_iterators/downgrade_iterators.hpp>
namespace water { namespace tests {

/*

compile time test water/iterator.hpp

*/

struct iterator_no_tag_1 {
	int operator*();
	int const* operator->();
	int operator-(iterator_no_tag_1) const;
	};

struct iterator_no_tag_2 {
	int& operator*();
	int* operator->();
	};


int constexpr iterator_all =

	// int*
	types::type_assert<types::equal<iterator_category<int*>, random_access_iterator_tag>>::assert +
	types::type_assert<is_random_access_iterator<int*>>::assert +
	types::type_assert<is_bidirectional_iterator<int*>>::assert +
	types::type_assert<is_forward_iterator<int*>>::assert +
	types::type_assert<is_input_iterator<int*>>::assert +
	types::type_assert<types::nots<is_output_iterator<int*>>>::assert +
	types::type_assert<types::nots<iterator_is_const<int*>>>::assert +
	types::type_assert<types::equal<iterator_value_type<int*>, int>>::assert +
	types::type_assert<types::equal<iterator_pointer_type<int*>, int*>>::assert +
	types::type_assert<types::equal<iterator_reference_type<int*>, int&>>::assert +
	types::type_assert<types::equal<iterator_difference_type<int*>, ptrdiff_t>>::assert +
	
	// int const*
	types::type_assert<types::equal<iterator_category<int const*>, random_access_iterator_tag>>::assert +
	types::type_assert<is_random_access_iterator<int const*>>::assert +
	types::type_assert<is_bidirectional_iterator<int const*>>::assert +
	types::type_assert<is_forward_iterator<int const*>>::assert +
	types::type_assert<is_input_iterator<int const*>>::assert +
	types::type_assert<types::nots<is_output_iterator<int const*>>>::assert +
	types::type_assert<iterator_is_const<int const*>>::assert +
	types::type_assert<types::equal<iterator_value_type<int const*>, int>>::assert +
	types::type_assert<types::equal<iterator_pointer_type<int const*>, int const*>>::assert +
	types::type_assert<types::equal<iterator_reference_type<int const*>, int const&>>::assert +
	types::type_assert<types::equal<iterator_difference_type<int const*>, ptrdiff_t>>::assert +
	
	// iterator_no_tag_1
	types::type_assert<types::equal<iterator_category<iterator_no_tag_1>, void>>::assert +
	types::type_assert<types::nots<is_random_access_iterator<iterator_no_tag_1>>>::assert +
	types::type_assert<types::nots<is_bidirectional_iterator<iterator_no_tag_1>>>::assert +
	types::type_assert<types::nots<is_forward_iterator<iterator_no_tag_1>>>::assert +
	types::type_assert<types::nots<is_input_iterator<iterator_no_tag_1>>>::assert +
	types::type_assert<types::nots<is_output_iterator<iterator_no_tag_1>>>::assert +
	types::type_assert<iterator_is_const<iterator_no_tag_1>>::assert +
	types::type_assert<types::equal<iterator_value_type<iterator_no_tag_1>, int>>::assert +
	types::type_assert<types::equal<iterator_pointer_type<iterator_no_tag_1>, int const*>>::assert +
	types::type_assert<types::equal<iterator_reference_type<iterator_no_tag_1>, int>>::assert +
	types::type_assert<types::equal<iterator_difference_type<iterator_no_tag_1>, int>>::assert +
	
	// iterator_no_tag_2
	types::type_assert<types::equal<iterator_category<iterator_no_tag_2>, void>>::assert +
	types::type_assert<types::nots<is_random_access_iterator<iterator_no_tag_2>>>::assert +
	types::type_assert<types::nots<is_bidirectional_iterator<iterator_no_tag_2>>>::assert +
	types::type_assert<types::nots<is_forward_iterator<iterator_no_tag_2>>>::assert +
	types::type_assert<types::nots<is_input_iterator<iterator_no_tag_2>>>::assert +
	types::type_assert<types::nots<is_output_iterator<iterator_no_tag_2>>>::assert +
	types::type_assert<types::nots<iterator_is_const<iterator_no_tag_2>>>::assert +
	types::type_assert<types::equal<iterator_value_type<iterator_no_tag_2>, int>>::assert +
	types::type_assert<types::equal<iterator_pointer_type<iterator_no_tag_2>, int*>>::assert +
	types::type_assert<types::equal<iterator_reference_type<iterator_no_tag_2>, int&>>::assert +
	types::type_assert<types::equal<iterator_difference_type<iterator_no_tag_2>, void>>::assert +
	
	// output<int*>
	types::type_assert<types::equal<iterator_category<downgrade_iterators::output<int*>>, output_iterator_tag>>::assert +
	types::type_assert<types::nots<is_random_access_iterator<downgrade_iterators::output<int*>>>>::assert +
	types::type_assert<types::nots<is_bidirectional_iterator<downgrade_iterators::output<int*>>>>::assert +
	types::type_assert<types::nots<is_forward_iterator<downgrade_iterators::output<int*>>>>::assert +
	types::type_assert<types::nots<is_input_iterator<downgrade_iterators::output<int*>>>>::assert +
	types::type_assert<is_output_iterator<downgrade_iterators::output<int*>>>::assert +
	types::type_assert<types::nots<iterator_is_const<downgrade_iterators::output<int*>>>>::assert +
	types::type_assert<types::equal<iterator_value_type<downgrade_iterators::output<int*>>, int>>::assert +
	types::type_assert<types::equal<iterator_pointer_type<downgrade_iterators::output<int*>>, void>>::assert +
	types::type_assert<types::equal<iterator_reference_type<downgrade_iterators::output<int*>>, void>>::assert +
	types::type_assert<types::equal<iterator_difference_type<downgrade_iterators::output<int*>>, void>>::assert +
	
	// input<int*>
	types::type_assert<types::equal<iterator_category<downgrade_iterators::input<int*>>, input_iterator_tag>>::assert +
	types::type_assert<types::nots<is_random_access_iterator<downgrade_iterators::input<int*>>>>::assert +
	types::type_assert<types::nots<is_bidirectional_iterator<downgrade_iterators::input<int*>>>>::assert +
	types::type_assert<types::nots<is_forward_iterator<downgrade_iterators::input<int*>>>>::assert +
	types::type_assert<is_input_iterator<downgrade_iterators::input<int*>>>::assert +
	types::type_assert<types::nots<is_output_iterator<downgrade_iterators::input<int*>>>>::assert +
	types::type_assert<iterator_is_const<downgrade_iterators::input<int*>>>::assert +
	types::type_assert<types::equal<iterator_value_type<downgrade_iterators::input<int*>>, int>>::assert +
	types::type_assert<types::equal<iterator_pointer_type<downgrade_iterators::input<int*>>, typename downgrade_iterators::input<int*>::pointer>>::assert +
	types::type_assert<types::equal<iterator_reference_type<downgrade_iterators::input<int*>>, typename downgrade_iterators::input<int*>::reference>>::assert +
	types::type_assert<types::equal<iterator_difference_type<downgrade_iterators::input<int*>>, typename downgrade_iterators::input<int*>::difference_type>>::assert +
	
	// forward_proxied<int*>
	types::type_assert<types::equal<iterator_category<downgrade_iterators::forward_proxied<int*>>, forward_iterator_tag>>::assert +
	types::type_assert<types::nots<is_random_access_iterator<downgrade_iterators::forward_proxied<int*>>>>::assert +
	types::type_assert<types::nots<is_bidirectional_iterator<downgrade_iterators::forward_proxied<int*>>>>::assert +
	types::type_assert<is_forward_iterator<downgrade_iterators::forward_proxied<int*>>>::assert +
	types::type_assert<is_input_iterator<downgrade_iterators::forward_proxied<int*>>>::assert +
	types::type_assert<types::nots<is_output_iterator<downgrade_iterators::forward_proxied<int*>>>>::assert +
	types::type_assert<types::nots<iterator_is_const<downgrade_iterators::forward_proxied<int*>>>>::assert +
	types::type_assert<types::equal<iterator_value_type<downgrade_iterators::forward_proxied<int*>>, int>>::assert +
	types::type_assert<types::equal<iterator_pointer_type<downgrade_iterators::forward_proxied<int*>>, typename downgrade_iterators::forward_proxied<int*>::pointer>>::assert +
	types::type_assert<types::equal<iterator_reference_type<downgrade_iterators::forward_proxied<int*>>, typename downgrade_iterators::forward_proxied<int*>::reference>>::assert +
	types::type_assert<types::equal<iterator_difference_type<downgrade_iterators::forward_proxied<int*>>, typename downgrade_iterators::forward_proxied<int*>::difference_type>>::assert +
	
	// bidirectional<int const*>
	types::type_assert<types::equal<iterator_category<downgrade_iterators::bidirectional<int const*>>, bidirectional_iterator_tag>>::assert +
	types::type_assert<types::nots<is_random_access_iterator<downgrade_iterators::bidirectional<int const*>>>>::assert +
	types::type_assert<is_bidirectional_iterator<downgrade_iterators::bidirectional<int const*>>>::assert +
	types::type_assert<is_forward_iterator<downgrade_iterators::bidirectional<int const*>>>::assert +
	types::type_assert<is_input_iterator<downgrade_iterators::bidirectional<int const*>>>::assert +
	types::type_assert<types::nots<is_output_iterator<downgrade_iterators::bidirectional<int const*>>>>::assert +
	types::type_assert<iterator_is_const<downgrade_iterators::bidirectional<int const*>>>::assert +
	types::type_assert<types::equal<iterator_value_type<downgrade_iterators::bidirectional<int const*>>, int>>::assert +
	types::type_assert<types::equal<iterator_pointer_type<downgrade_iterators::bidirectional<int const*>>, typename downgrade_iterators::bidirectional<int const*>::pointer>>::assert +
	types::type_assert<types::equal<iterator_reference_type<downgrade_iterators::bidirectional<int const*>>, typename downgrade_iterators::bidirectional<int const*>::reference>>::assert +
	types::type_assert<types::equal<iterator_difference_type<downgrade_iterators::bidirectional<int const*>>, typename downgrade_iterators::bidirectional<int const*>::difference_type>>::assert +
	
	// random_access_proxied_const<int*>
	types::type_assert<types::equal<iterator_category<downgrade_iterators::random_access_proxied_const<int*>>, random_access_iterator_tag>>::assert +
	types::type_assert<is_random_access_iterator<downgrade_iterators::random_access_proxied_const<int*>>>::assert +
	types::type_assert<is_bidirectional_iterator<downgrade_iterators::random_access_proxied_const<int*>>>::assert +
	types::type_assert<is_forward_iterator<downgrade_iterators::random_access_proxied_const<int*>>>::assert +
	types::type_assert<is_input_iterator<downgrade_iterators::random_access_proxied_const<int*>>>::assert +
	types::type_assert<types::nots<is_output_iterator<downgrade_iterators::random_access_proxied_const<int*>>>>::assert +
	types::type_assert<iterator_is_const<downgrade_iterators::random_access_proxied_const<int*>>>::assert +
	types::type_assert<types::equal<iterator_value_type<downgrade_iterators::random_access_proxied_const<int*>>, int>>::assert +
	types::type_assert<types::equal<iterator_pointer_type<downgrade_iterators::random_access_proxied_const<int*>>, typename downgrade_iterators::random_access_proxied_const<int*>::pointer>>::assert +
	types::type_assert<types::equal<iterator_reference_type<downgrade_iterators::random_access_proxied_const<int*>>, typename downgrade_iterators::random_access_proxied_const<int*>::reference>>::assert +
	types::type_assert<types::equal<iterator_difference_type<downgrade_iterators::random_access_proxied_const<int*>>, typename downgrade_iterators::random_access_proxied_const<int*>::difference_type>>::assert;

}}
#endif
