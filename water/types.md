# types.hpp

`#include <water/types.hpp>`

water/types.hpp contains template "metafunctions" that are used almost everywhere in the Water C++ library. The file includes nothing else, it has no dependencies. The templates are very simple, but also very useful.



### `ifel<bool if, typename true, typename false = nothing>`

If or if-else. It's like `std::enable_if_t<if, true>` and `std::conditional_t<if, true, false>` in one.

    using is_int  = ifel<true, int, void>;
    using is_void = ifel<false, int, void>;
    
    using is_int = ifel<true, int>;
    using error  = ifel<false, int>; // error


For function overloads:
    
    template<typename iterator_>
    ifel<is_random_access_iterator<iterator_>, size_t> size(iterator_ begin, iterator_ end) {
        return static_cast<size_t>(end - begin);
    }
    
    template<typename iterator_>
    ifel<!is_random_access_iterator<iterator_>, size_t> size(iterator_ begin, iterator_ end) {
        size_t s = 0;
        while(begin != end) {
            ++s;
            ++begin;
        }
        return s;
    }


Nesting:

    int constexpr i = ...;
    
    using x = 
        ifel<i == 1, char,
        ifel<i == 2, short,
        ifel<i == 3, int,
        long
    >>>;



### `if_not_void<typename if, typename else = void>`

If the first type is not `void`, it is that type. Else if the second type is not `void`, it is that type. Otherwise it is an error. Usually used for optional template arguments like this example:

    template<typename type_, typename optional1_ = void, typename optional2_ = void>
    struct example {
        using value_type = type_;
        using option1 = if_not_void<optional1_, default_option1>;
        using option2 = if_not_void<optional2_, default_option2>;
    };



### `at_pack<size_t at, typename ...pack>`

Get a type from the parameter pack. Like `type = pack[at]`. If `at >= sizeof...(pack)` it is an error. 

    template<typename ...pack>
    struct example {
        using at0 = pack_at<0, pack...>;
        using at2 = pack_at<2, pack...>;
    };



### `bool equal<typename type1, typename type2>`

True if type1 and type2 are the same type. It's like `std::is_same<type1, type2>::value`.

    bool constexpr is_true  = equal<int, int>;
    bool constexpr is_false = equal<int, void>;



### `type make_type<type>()`

A function (with no definition) that returns the template argument. Useful together with `decltype`, see the `to_void` example below. Test a type with `decltype` without knowing how to construct the type.



### `to_void<...pack>`

It is `void` always. Like `std::void_t`.

    template<typename type_, typename = void>
    int constexpr example = 1;
    
    template<typename type_>
    int constexpr example<
        type_,
        to_void<
            typename type_::value_type,
            decltype(make_type<type_&>().begin() != make_type<type_&>().end()),
            decltype(*make_type<type_&>().begin()),
            decltype(static_cast<size_t>(make_type<type_&>().size()))
        >
    > = 2;
    
    int is_1 = example<void>;
    int is_2 = example<std::vector<double>>;

The specialization using `to_void` is the most specialized `example` so the compiler will pick that when it can. And for `std::vector` it can, for `void` it cannot.



### `first<...pack>`

The first type of the pack. Like `pack_at<0, pack...>`. If the pack is empty it is an error.

Sometimes it is useful for function arguments: 

    template<typename float_>
    float_ math(float_ a, first<float_> b) {
        return sqrt(pow(a, b));
    }
    
    double d = math(1.23, 2); // without first it would be ambiguos because 1.23 is double and 2 is int
    
Sometimes it is useful to make the first type depend on the other types, a bit like `to_void`:

    template<typename iterator1_, typename iterator2_>
    auto less_than(iterator1_ b1, first<iterator1_> e1, iterator2_ b2, frist<iterator2_> e2) ->
        first<
            bool,
            decltype(b1 != e1),
            decltype(b2 != e2),
            decltype(*b1 < *b2)
        >
    {
        ...
    }

