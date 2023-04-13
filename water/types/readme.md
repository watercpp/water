# water::types

`#include <water/types/types.hpp>`

`water::types` can be useful for template metaprogramming. It's made up of template *metafunctions* 
and *collections*.

The *metafunctions* have a nested type or nontype *result* that is always called `result`
(not `type` or `value` like the STL metafunctions in `<type_traits>`).

*Collections* can contain many *results*, and there are metafunctions to create, modify and traverse 
these *collections*. Something like arrays of metafunctions.

This readme does not explain everything. If you have some reason to do complex metaprogramming, and 
you somehow make it through this readme, you still need to go through the code in `water/types` to 
know what different things do.

## Metafunctions

The technique used throughout `water::types` was created before the C++ 2011 standard added alias
templates (`template<something> using`). Understanding it is fundamental to using it.

With alias templates this example would remove const. volatile and reference from `type_`.
The `no_const`, `no_volatile` and `no_reference` are alias templates:

    template<typename type_> using no_const = ...
    
    template<typename type_>
    struct something {
        using plain_type = no_const<no_volatile<no_reference<type_>>>;
    };

Before the C++ 2011 standard the `no_const`, `no_volatile` and `no_reference` would have to be class
types, and the code would have lots of `typename` and `::type` everywhere:

    template<typename type_> struct no_const { using type = ...  };
    
    template<typename type_>
    struct something {
        using plain_type = typename no_const<
            typename no_volatile<
                typename no_reference<type_>::type
            >::type
        >::type;
    };

With `water::types` only the outermost of the nested templates need `typename` and `::result`.
And it uses the name `result` is instead of `type`:

    template<typename type_> struct no_const { using result = ...  };
    
    template<typename type_>
    struct something {
        using plain_type = typename no_const<no_volatile<no_reference<type_>>>::result;
    };

This avoids most of the mess, and worked with the C++ 1998 standard.

To avoid the `typename` mess, `water::types` differentiate between metafunction types and other
types. There should be no reason to use metafunction templates like `no_volatile` as anything else
than metafunctions.

When a template argument A to metafunction F is another metafunction, F will use the *result* of A
instead of the type A.

For example, `water::types::type` has a type as its `result`, and it knows when its template
argument is another metafunction (itself in this example):

    using is_int_1 = typename types::type<int>::result;
    
    using is_int_2 = typename types::type< types::type< types::type<int> > >::result;

All things in `water::types` produce a `result`. This is like the return value of a function.
The `result` can be a type or a nontype like an integral constant. And all things differentiate
between metafunction types and other types.

    int is_123_a = types::nontype<int, 123>::result;
    
    int is_123_b = types::nontype< types::nontype< types::type<int>, 123> >::result;
    
To make this work, a metafunction needs to tag itself as a `water::types` metafunction, and check if
it's template type arguments are tagged as metafunctions.

This system has some benefits. Unlike alias templates it does not have to evaluate everything.
Conditional metafunctions do not need all possible results to be valid types. And a metafunction can
also *not* produce a result for some argument, and another metafunction like `to_bool` would then
result false.



## Collections

Collections contain zero or more results. `types::list` is a collection

    template<typename ...types_>
    stuct list;

The template arguments can be regular types, or metafunctions. The metafunctions can have a nontype
result, and regular types are converted to metafunctions that result the type.

Every operation (metafunction) that is used on a collection will make a new collection, because they
are types and templates cannot modify types only create new ones.

A type can be a collection and a metafunction with a result at the same time. This is normal. The
types `at`, `insert`, `set` here are collections and metafunctions with a result.

    using list1 = list<char, short, int, long>;
    
    using is_char = at<list1, 0>::reuslt;
    usign is_short = at<list1, 1>::result;
    using is_char_2 = at<at<list1, 100>, -100>::result;
    
    using list2 = insert<list1, nontype<int, 123>, 1>;
    int is_123 = at<list2, 1>::result;
    
    using list3 = set<at<list2, 2>, double>;
    using is_double = at<list3, 0>::result;
    int is_123_again = at<list3, -1>::result;
    int at_offset_2 = offset<list3>::result; 

For more information, look at the list of collection metafunctions below. And the comments in the code.



## Namespace content

These are not all but the most important things in `water::types`

A namespace `water::types::use` can be used to bring the most common `water::types` things into scope.

The utility alias template `result` and template variable `value` gets the result type or result
value from a metafunction.

    using namespace water::types::use;
    
    using type1 = result<ifel<true, int>>;
    using type2 = typename ifel<true, int>::result
    
    int int1 = value<ifel<true, nontype<int, 123>>>;
    int int1 = ifel<true, nontype<int, 123>>>::result;


#### `type<argument>`

- result argument, or if argument is a metafunction that results a type: argument::result


#### `type_plain<argument>`

- result argument always


#### `any_result<argument>`

- if argument is a metafunction, argument::result, otherwise argument.
  (so `any_result<argument>::result` might not be a type)


#### `any_type<argument>`

- if argument is not a metafunction, result argument. if argument::result is a type, result
  argument::result. if argument::result is a nontype result the type of the nontype 


#### `bool_result<bool result>`

- result the bool


#### `false_result`

- same as `bool_result<false>`


#### `true_result`

- same as `bool_result<true>`


#### `nontype<type, type result = 0>`

- result result, except if type is a metafunction and type::result is a nontype then type::result


#### `is_result<argument>`

- result true when argument is a metafunction 


#### `ifel<bool if, typename true, typename false = nothing>`

- result true when if is true, or true::result if true is a metafunction, or no result at all if
  true = types::nothing
- result false when if is false, or false::result if false is a metafunction, or no result at all
  if false = types::nothing


#### `ifel_type<typename if, typename true, typename false = nothing>`

- same as `ifel<to_bool<if>::result, true, false>`


#### `ifel_type_not<typename if, typename true, typename false = nothing>`

- same as `ifel<!to_bool<if>::result, true, false>`


#### `ifel_plain<bool if, typename true, typename false = nothing>`

- same as `ifel`, except it does not check if true or false are metafunctions.


#### `if_not_void<typename first, typename second>`

- if first is not void, or a metafunction with first::result = void, result first (or first::result)
- otherwise, if second is not void, or a metafunction with second::result = void, result second (or
  second::result)
- otherwise it has no result


#### `equal<type, type>`

- result true if the types are equal. if any of them are metafunctions, the result of the
  metafunctions is compared. for nontypes that will compare the nontype values.


#### `equal_plain<type, type>`

- result true if the types are the same. does not check if the types are metafunctions 


#### `to_bool<type>`

- for a metafunction with nontype result, converts the nontype value to bool
- for a metafunction with a type result, true always
- for a type that is not a metafunction, false always

#### `ands<...types>`

- result true if `to_bool<type>::result` is true for all types in the list. it stops on the first
  false.


#### `nots<...types>`

- result true if `to_bool<type>::result` is false for all types in the list. it stops on the first
  true.


#### `ors<...types>`

- result the first type in the list where `to_bool<type>::result` is true. If none is true, it has
  no result.




### Collection

These have better documentation in the code than in this file. Look at the file with the same name
as the template.


#### `list<...types>`

- the list collection


#### `at<typename collection, int offset = 0>`

- Position the collection at the offset.
- Result whatever is in the collection at that offset.
- If the offset is outside of the collection, it has no result.


#### `insert<typename collection, typename insert, int offset = 0>`

- It is a new collection with the insert inserted at offset
- `at<insert<collection, x, 2>, 2>::result` is x


#### `next<typename collection>`

- Same as `at<collection, 1>`


#### `offset<typename collection>`

- The current offset of the collection.
- `offset<at<list<>, 123>>::result` is 123


#### `pull<typename collection>`

- Get a plain collection type out of nested collection templates. Maybe only useful for debugging,
  if the type can be printed or seen as a compiler error.


#### `remove<typename collection, int offset = 0>`

- It is a new collection with the result at offset removed.


#### `set<typename collection, typename set, int offset = 0>`

- It is a new collection with the result at offset set to the new result


#### `size<typename collection>`

- The size of the collection from the current offset to the end.
- `size<list<int, int, int>>::result` is 3
- `size<at<list<int, int, int>, 2>::result` is 1


#### `is_collection<type>`

- result true if type is a collection

