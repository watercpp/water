# Changelog

The plan is to write down major or breaking changes here by date, and tag the code with the same date.



# 2023-04-25

### water::memory_track

Breaking changes have been made to `water::memory_track`.

A new readme `water/memory_track/readme.md` and more example code in `water/memory_track/example/`
are far from perfect but should help explain better what it is and how to use it.

The arguments to `memory_track::memory` functions `allocate` and `free` have changed. Code that
calls them needs to be changed. A new alignment argument and a name argument has been added. Look
at `water/memory_track/memory.hpp`.

Similar changes are needed if you replaced the `memory::callback_type` with your own type. Look at
`no_callback` in `water/memory_track/bits.hpp`.

The `tag.name()` has been moved out of the tag and is now a separate argument to the constructors
of `allocator_base` and `allocator` and to the functions `allocator_for` and `allocator_nothrow_for`.
This breaks some uses. Example for `allocator`:

    memory_track::memory<> m;
    using tag = memory_track::memory<>::tag_type;
    
    // Worked before and works now
    allocator<memory_track::memory<>> a1{m, "name"};
    
    // Worked before, breaks now
    allocator<memory_track::memory<>> a2{m, tag{"name"}};
    
    // The tag is the second argument now
    allocator<memory_track::memory<>> a3{m, "name", tag{}};

If you replaced the `memory::tag_type` with your own type, remove the name from it.

Many more changes have been made, but they will hopefully not break existing code.


### Uses of mutexes, condition variables and threads

Things that use a mutex or other thread-related primitives now use the standard variants like
`std::mutex` by default and not `water::threads::mutex`. If you `#define WATER_USE_WATER_THREADS`
the `water::threads` variants are used instead. This change should not break anything.



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