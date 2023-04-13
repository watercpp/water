# Changelog

The plan is to write down major or breaking changes here by date, and tag the code with the same date.



# 2023-04-12

Most template metafunctions have changed from class types that have a type or integer constant 
`result` to template aliases and template variables. This is a major change to the library itself,
but it should only be a breaking change to users of the parts described below.


### water/types.hpp

New header with simplified metafunctions based on the older ones from the `water::types` namespace.
Read the documentation in `water/types.md`.


### water/is_no_to.hpp

New header with simplified metafunctions for `is_const` and `to_unsigned` etc. The old versions from
the `water::types` namespace have been removed.

They exist mostly so the library works even without the C++ standard library. Use the standard
`<type_traits>` instead when possible.


### water/iterators.hpp

These templates have breaking changes. They no longer have a `result`:

    bool b = is_random_access_iterator<iterator_>;
    bool b = is_bidirectional_iterator<iterator_>:
    bool b = is_forward_iterator<iterator_>;
    bool b = is_input_iterator<iterator_>;
    bool b = is_output_iterator<iterator_>;
    bool b = iterator_is_const<iterator_>;

    using t = iterator_category<iterator_>;
    using t = iterator_value_type<iterator_>;
    using t = iterator_pointer_type<iterator_>;
    using t = iterator_reference_type<iterator_>;
    using t = iterator_difference_type<iterator_>;

The old versions with `result` looked like this:

    bool b = is_random_access_iterator<iterator_>::result;
    using t = iterator_category<iterator_>::result;



### water::unicode

These templates have breaking changes just like `water/iterators.hpp`. They no longer have a `result`:
    
    unsigned u = utf_from_char<char_>;
    unsigned u = utf_from_iterator<iterator_>;
    
    using t = char_from_utf<8>;



### water/type_traits.hpp

Breaking changes, but this file exists only for building without the C++ standard library. Use the
standard `<type_traits>` instead.



### water::types

Lots of things have been removed and replaced with metafunctions in `water/is_no_to.hpp`. Some
things have been removed without replacement. The rest the library now use `water/types.hpp` and
`water/is_no_to.hpp` instead of `water::types`.

The `water::types` namespace remains as a tool for more complex metaprogramming. All the core parts
are still there. Plus a new readme: `water/types/readme.md`.

Code that breaks has to be modified, in one of two ways:
- The old version is in `water/types1/types.hpp` and `water::types1` namespace. Include that header
  instead and use that namespace. This is hopefully easy using multi-file search and replace in a
  text editor.
- Switch to using the new templates from `water/types.hpp` and `water/is_no_to.hpp`
  (Or `<type_traits>`).



# 2023-04-07

This changelog did not exist