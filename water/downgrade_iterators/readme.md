# water::downgrade_iterators

Downgrade iterators to some other iterator category. This is useful for testing,
to test that an algorithm works with a specific iterator category.

There is one class for each standard iterator category (except contiguous):

- `bidirectional`
- `forward`
- `input`
- `output`
- `random_access`

Most classes have configuration options to make the iterator const, or to use
proxy classes for the pointer and reference types.

## Example: Downgrade vector iterators to input iterators

	#include <water/downgrade_iterators/downgrade_iterators.hpp>
	
	std::vector<int> v;
	water::downgrade_iterators::input<std::vector<int>::iterator>
		begin{v.begin()},
		end{v.end()};
	int sum = 0;
	while(begin != end) {
		sum += *begin;
		++begin;
		}
		

There are function to create iterators with less typing:

	auto i = water::downgrade_iterators::input_from(v.begin());