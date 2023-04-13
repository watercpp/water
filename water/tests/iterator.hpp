// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_TESTS_ITERATOR_HPP
#define WATER_TESTS_ITERATOR_HPP
#include <water/iterator.hpp>
#include <water/downgrade_iterators/downgrade_iterators.hpp>

/*

compile time test water/iterator.hpp

*/


#ifndef WATER_NO_STD

namespace water { namespace tests {
    struct iterator_with_std_iterator_traits {};
    struct iterator_with_std_iterator_traits_value {};
}}

namespace std {
    template<>
    struct iterator_traits<water::tests::iterator_with_std_iterator_traits>
    {
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = water::tests::iterator_with_std_iterator_traits_value;
        using difference_type = short;
        using pointer = value_type*;
        using reference = value_type&;
    };

}

#endif


namespace water { namespace tests {

struct iterator_no_tag_1 {
    int operator*();
    int const* operator->();
    int operator-(iterator_no_tag_1) const;
};

struct iterator_no_tag_2 {
    int& operator*();
    int* operator->();
};

struct not_an_iterator {};


#ifndef WATER_NO_STD

// iterator_with_std_iterator_traits
static_assert(equal<iterator_category<iterator_with_std_iterator_traits>, bidirectional_iterator_tag>, "");
static_assert(!is_random_access_iterator<iterator_with_std_iterator_traits>, "");
static_assert(is_bidirectional_iterator<iterator_with_std_iterator_traits>, "");
static_assert(is_forward_iterator<iterator_with_std_iterator_traits>, "");
static_assert(is_input_iterator<iterator_with_std_iterator_traits>, "");
static_assert(!is_output_iterator<iterator_with_std_iterator_traits>, "");
static_assert(iterator_is_const<iterator_with_std_iterator_traits>, "");
static_assert(equal<iterator_value_type<iterator_with_std_iterator_traits>, iterator_with_std_iterator_traits_value>, "");
static_assert(equal<iterator_pointer_type<iterator_with_std_iterator_traits>, iterator_with_std_iterator_traits_value*>, "");
static_assert(equal<iterator_reference_type<iterator_with_std_iterator_traits>, iterator_with_std_iterator_traits_value&>, "");
static_assert(equal<iterator_difference_type<iterator_with_std_iterator_traits>, short>, "");

#endif

// void
static_assert(equal<iterator_category<void>, void>, "");
static_assert(!is_random_access_iterator<void>, "");
static_assert(!is_bidirectional_iterator<void>, "");
static_assert(!is_forward_iterator<void>, "");
static_assert(!is_input_iterator<void>, "");
static_assert(!is_output_iterator<void>, "");
static_assert(iterator_is_const<void>, "");
static_assert(equal<iterator_value_type<void>, void>, "");
static_assert(equal<iterator_pointer_type<void>, void>, "");
static_assert(equal<iterator_reference_type<void>, void>, "");
static_assert(equal<iterator_difference_type<void>, void>, "");

// int
static_assert(equal<iterator_category<int>, void>, "");
static_assert(!is_random_access_iterator<int>, "");
static_assert(!is_bidirectional_iterator<int>, "");
static_assert(!is_forward_iterator<int>, "");
static_assert(!is_input_iterator<int>, "");
static_assert(!is_output_iterator<int>, "");
static_assert(iterator_is_const<int>, "");
static_assert(equal<iterator_value_type<int>, void>, "");
static_assert(equal<iterator_pointer_type<int>, void>, "");
static_assert(equal<iterator_reference_type<int>, void>, "");
static_assert(equal<iterator_difference_type<int>, void>, "");

// not_an_iterator
static_assert(equal<iterator_category<not_an_iterator>, void>, "");
static_assert(!is_random_access_iterator<not_an_iterator>, "");
static_assert(!is_bidirectional_iterator<not_an_iterator>, "");
static_assert(!is_forward_iterator<not_an_iterator>, "");
static_assert(!is_input_iterator<not_an_iterator>, "");
static_assert(!is_output_iterator<not_an_iterator>, "");
static_assert(iterator_is_const<not_an_iterator>, "");
static_assert(equal<iterator_value_type<not_an_iterator>, void>, "");
static_assert(equal<iterator_pointer_type<not_an_iterator>, void>, "");
static_assert(equal<iterator_reference_type<not_an_iterator>, void>, "");
static_assert(equal<iterator_difference_type<not_an_iterator>, void>, "");

// int*
static_assert(equal<iterator_category<int*>, random_access_iterator_tag>, "");
static_assert(is_random_access_iterator<int*>, "");
static_assert(is_bidirectional_iterator<int*>, "");
static_assert(is_forward_iterator<int*>, "");
static_assert(is_input_iterator<int*>, "");
static_assert(!is_output_iterator<int*>, "");
static_assert(!iterator_is_const<int*>, "");
static_assert(equal<iterator_value_type<int*>, int>, "");
static_assert(equal<iterator_pointer_type<int*>, int*>, "");
static_assert(equal<iterator_reference_type<int*>, int&>, "");
static_assert(equal<iterator_difference_type<int*>, ptrdiff_t>, "");

// int const*
static_assert(equal<iterator_category<int const*>, random_access_iterator_tag>, "");
static_assert(is_random_access_iterator<int const*>, "");
static_assert(is_bidirectional_iterator<int const*>, "");
static_assert(is_forward_iterator<int const*>, "");
static_assert(is_input_iterator<int const*>, "");
static_assert(!is_output_iterator<int const*>, "");
static_assert(iterator_is_const<int const*>, "");
static_assert(equal<iterator_value_type<int const*>, int>, "");
static_assert(equal<iterator_pointer_type<int const*>, int const*>, "");
static_assert(equal<iterator_reference_type<int const*>, int const&>, "");
static_assert(equal<iterator_difference_type<int const*>, ptrdiff_t>, "");

// iterator_no_tag_1
static_assert(equal<iterator_category<iterator_no_tag_1>, void>, "");
static_assert(!is_random_access_iterator<iterator_no_tag_1>, "");
static_assert(!is_bidirectional_iterator<iterator_no_tag_1>, "");
static_assert(!is_forward_iterator<iterator_no_tag_1>, "");
static_assert(!is_input_iterator<iterator_no_tag_1>, "");
static_assert(!is_output_iterator<iterator_no_tag_1>, "");
static_assert(iterator_is_const<iterator_no_tag_1>, "");
static_assert(equal<iterator_value_type<iterator_no_tag_1>, int>, "");
static_assert(equal<iterator_pointer_type<iterator_no_tag_1>, int const*>, "");
static_assert(equal<iterator_reference_type<iterator_no_tag_1>, int>, "");
static_assert(equal<iterator_difference_type<iterator_no_tag_1>, int>, "");

// iterator_no_tag_2
static_assert(equal<iterator_category<iterator_no_tag_2>, void>, "");
static_assert(!is_random_access_iterator<iterator_no_tag_2>, "");
static_assert(!is_bidirectional_iterator<iterator_no_tag_2>, "");
static_assert(!is_forward_iterator<iterator_no_tag_2>, "");
static_assert(!is_input_iterator<iterator_no_tag_2>, "");
static_assert(!is_output_iterator<iterator_no_tag_2>, "");
static_assert(!iterator_is_const<iterator_no_tag_2>, "");
static_assert(equal<iterator_value_type<iterator_no_tag_2>, int>, "");
static_assert(equal<iterator_pointer_type<iterator_no_tag_2>, int*>, "");
static_assert(equal<iterator_reference_type<iterator_no_tag_2>, int&>, "");
static_assert(equal<iterator_difference_type<iterator_no_tag_2>, void>, "");

// output<int*>
static_assert(equal<iterator_category<downgrade_iterators::output<int*>>, output_iterator_tag>, "");
static_assert(!is_random_access_iterator<downgrade_iterators::output<int*>>, "");
static_assert(!is_bidirectional_iterator<downgrade_iterators::output<int*>>, "");
static_assert(!is_forward_iterator<downgrade_iterators::output<int*>>, "");
static_assert(!is_input_iterator<downgrade_iterators::output<int*>>, "");
static_assert(is_output_iterator<downgrade_iterators::output<int*>>, "");
static_assert(!iterator_is_const<downgrade_iterators::output<int*>>, "");
static_assert(equal<iterator_value_type<downgrade_iterators::output<int*>>, int>, "");
static_assert(equal<iterator_pointer_type<downgrade_iterators::output<int*>>, void>, "");
static_assert(equal<iterator_reference_type<downgrade_iterators::output<int*>>, typename downgrade_iterators::output<int*>::reference>, "");
static_assert(equal<iterator_difference_type<downgrade_iterators::output<int*>>, void>, "");

// input<int*>
static_assert(equal<iterator_category<downgrade_iterators::input<int*>>, input_iterator_tag>, "");
static_assert(!is_random_access_iterator<downgrade_iterators::input<int*>>, "");
static_assert(!is_bidirectional_iterator<downgrade_iterators::input<int*>>, "");
static_assert(!is_forward_iterator<downgrade_iterators::input<int*>>, "");
static_assert(is_input_iterator<downgrade_iterators::input<int*>>, "");
static_assert(!is_output_iterator<downgrade_iterators::input<int*>>, "");
static_assert(iterator_is_const<downgrade_iterators::input<int*>>, "");
static_assert(equal<iterator_value_type<downgrade_iterators::input<int*>>, int>, "");
static_assert(equal<iterator_pointer_type<downgrade_iterators::input<int*>>, typename downgrade_iterators::input<int*>::pointer>, "");
static_assert(equal<iterator_reference_type<downgrade_iterators::input<int*>>, typename downgrade_iterators::input<int*>::reference>, "");
static_assert(equal<iterator_difference_type<downgrade_iterators::input<int*>>, typename downgrade_iterators::input<int*>::difference_type>, "");

// forward_proxied<int*>
static_assert(equal<iterator_category<downgrade_iterators::forward_proxied<int*>>, forward_iterator_tag>, "");
static_assert(!is_random_access_iterator<downgrade_iterators::forward_proxied<int*>>, "");
static_assert(!is_bidirectional_iterator<downgrade_iterators::forward_proxied<int*>>, "");
static_assert(is_forward_iterator<downgrade_iterators::forward_proxied<int*>>, "");
static_assert(is_input_iterator<downgrade_iterators::forward_proxied<int*>>, "");
static_assert(!is_output_iterator<downgrade_iterators::forward_proxied<int*>>, "");
static_assert(!iterator_is_const<downgrade_iterators::forward_proxied<int*>>, "");
static_assert(equal<iterator_value_type<downgrade_iterators::forward_proxied<int*>>, int>, "");
static_assert(equal<iterator_pointer_type<downgrade_iterators::forward_proxied<int*>>, typename downgrade_iterators::forward_proxied<int*>::pointer>, "");
static_assert(equal<iterator_reference_type<downgrade_iterators::forward_proxied<int*>>, typename downgrade_iterators::forward_proxied<int*>::reference>, "");
static_assert(equal<iterator_difference_type<downgrade_iterators::forward_proxied<int*>>, typename downgrade_iterators::forward_proxied<int*>::difference_type>, "");

// bidirectional<int const*>
static_assert(equal<iterator_category<downgrade_iterators::bidirectional<int const*>>, bidirectional_iterator_tag>, "");
static_assert(!is_random_access_iterator<downgrade_iterators::bidirectional<int const*>>, "");
static_assert(is_bidirectional_iterator<downgrade_iterators::bidirectional<int const*>>, "");
static_assert(is_forward_iterator<downgrade_iterators::bidirectional<int const*>>, "");
static_assert(is_input_iterator<downgrade_iterators::bidirectional<int const*>>, "");
static_assert(!is_output_iterator<downgrade_iterators::bidirectional<int const*>>, "");
static_assert(iterator_is_const<downgrade_iterators::bidirectional<int const*>>, "");
static_assert(equal<iterator_value_type<downgrade_iterators::bidirectional<int const*>>, int>, "");
static_assert(equal<iterator_pointer_type<downgrade_iterators::bidirectional<int const*>>, typename downgrade_iterators::bidirectional<int const*>::pointer>, "");
static_assert(equal<iterator_reference_type<downgrade_iterators::bidirectional<int const*>>, typename downgrade_iterators::bidirectional<int const*>::reference>, "");
static_assert(equal<iterator_difference_type<downgrade_iterators::bidirectional<int const*>>, typename downgrade_iterators::bidirectional<int const*>::difference_type>, "");

// random_access_proxied_const<int*>
static_assert(equal<iterator_category<downgrade_iterators::random_access_proxied_const<int*>>, random_access_iterator_tag>, "");
static_assert(is_random_access_iterator<downgrade_iterators::random_access_proxied_const<int*>>, "");
static_assert(is_bidirectional_iterator<downgrade_iterators::random_access_proxied_const<int*>>, "");
static_assert(is_forward_iterator<downgrade_iterators::random_access_proxied_const<int*>>, "");
static_assert(is_input_iterator<downgrade_iterators::random_access_proxied_const<int*>>, "");
static_assert(!is_output_iterator<downgrade_iterators::random_access_proxied_const<int*>>, "");
static_assert(iterator_is_const<downgrade_iterators::random_access_proxied_const<int*>>, "");
static_assert(equal<iterator_value_type<downgrade_iterators::random_access_proxied_const<int*>>, int>, "");
static_assert(equal<iterator_pointer_type<downgrade_iterators::random_access_proxied_const<int*>>, typename downgrade_iterators::random_access_proxied_const<int*>::pointer>, "");
static_assert(equal<iterator_reference_type<downgrade_iterators::random_access_proxied_const<int*>>, typename downgrade_iterators::random_access_proxied_const<int*>::reference>, "");
static_assert(equal<iterator_difference_type<downgrade_iterators::random_access_proxied_const<int*>>, typename downgrade_iterators::random_access_proxied_const<int*>::difference_type>, "");;

}}
#endif
