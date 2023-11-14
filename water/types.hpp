// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_TYPES_HPP
#define WATER_TYPES_HPP
namespace water {

// Read types.md

using size_t = decltype(sizeof(0));

template<typename type_> type_ make_type();



namespace _ {

    template<size_t at_, bool at_less_than_size_, bool at_less_than_4_, typename ...pack_>
    struct at_pack_do {
    };
    
    template<typename p0_, typename ...p_>
    struct at_pack_do<0, true, true, p0_, p_...> {
        using result = p0_;
    };
    
    template<typename p0_, typename p1_, typename ...p_>
    struct at_pack_do<1, true, true, p0_, p1_, p_...> {
        using result = p1_;
    };
    
    template<typename p0_, typename p1_, typename p2_, typename ...p_>
    struct at_pack_do<2, true, true, p0_, p1_, p2_, p_...> {
        using result = p2_;
    };
    
    template<typename p0_, typename p1_, typename p2_, typename p3_, typename ...p_>
    struct at_pack_do<3, true, true, p0_, p1_, p2_, p3_, p_...> {
        using result = p3_;
    };
    
    template<size_t at_, typename p0_, typename p1_, typename p2_, typename p3_, typename ...p_>
    struct at_pack_do<at_, true, false, p0_, p1_, p2_, p3_, p_...> :
        at_pack_do<at_ - 4, true, ((at_ - 4) < 4), p_...>
    {};
    
}

template<size_t at_, typename ...pack_>
using at_pack = typename _::at_pack_do<at_, (at_ < sizeof...(pack_)), (at_ < 4), pack_...>::result;



template<typename a_, typename b_>
bool constexpr equal = false;

template<typename a_>
bool constexpr equal<a_, a_> = true;



namespace _ {
    
    struct ifel_nothing;
    
    template<bool if_, typename true_, typename false_>
    struct ifel_do {
        using result = true_;
    };
    
    template<typename true_, typename false_>
    struct ifel_do<false, true_, false_> {
        using result = false_;
    };
    
    template<typename true_>
    struct ifel_do<false, true_, ifel_nothing> {
    };
    
}

template<bool if_, typename true_, typename false_ = _::ifel_nothing>
using ifel = typename _::ifel_do<if_, true_, false_>::result;



namespace _ {

    template<typename if_, typename else_>
    struct if_not_void_do {
        using result = if_;
    };
    
    template<typename else_>
    struct if_not_void_do<void, else_> {
        using result = else_;
    };
    
    template<>
    struct if_not_void_do<void, void> {
    };
    
}

template<typename if_, typename else_ = void>
using if_not_void = typename _::if_not_void_do<if_, else_>::result;



namespace _ {

    // this is needed so the template arguments of to_void are used
    template<typename ...>
    struct to_void_do {
        using result = void;
    };
    
}

template<typename ...pack_>
using to_void = typename _::to_void_do<pack_...>::result;



namespace _ {

    template<typename ...>
    struct first_do {
    };
    
    template<typename first_, typename ...pack_>
    struct first_do<first_, pack_...> {
        using result = first_;
    };
    
}

template<typename ...pack_>
using first = typename _::first_do<pack_...>::result;



// test

namespace _ { namespace types_hpp_test {

    static_assert(equal<int, int>, "");
    static_assert(!equal<int, bool>, "");
    static_assert(equal<int, decltype(make_type<int>())>, "");
    
    template<int> struct at;
    
    static_assert(equal<at<0>, at_pack<0, at<0>, at<1>, at<2>, at<3>, at<4>, at<5>, at<6>, at<7>, at<8>, at<9>>>, "");
    static_assert(equal<at<1>, at_pack<1, at<0>, at<1>, at<2>, at<3>, at<4>, at<5>, at<6>, at<7>, at<8>, at<9>>>, "");
    static_assert(equal<at<2>, at_pack<2, at<0>, at<1>, at<2>, at<3>, at<4>, at<5>, at<6>, at<7>, at<8>, at<9>>>, "");
    static_assert(equal<at<3>, at_pack<3, at<0>, at<1>, at<2>, at<3>, at<4>, at<5>, at<6>, at<7>, at<8>, at<9>>>, "");
    static_assert(equal<at<4>, at_pack<4, at<0>, at<1>, at<2>, at<3>, at<4>, at<5>, at<6>, at<7>, at<8>, at<9>>>, "");
    static_assert(equal<at<6>, at_pack<6, at<0>, at<1>, at<2>, at<3>, at<4>, at<5>, at<6>, at<7>, at<8>, at<9>>>, "");
    static_assert(equal<at<9>, at_pack<9, at<0>, at<1>, at<2>, at<3>, at<4>, at<5>, at<6>, at<7>, at<8>, at<9>>>, "");
    
    static_assert(equal<short, ifel<true, short>>, "");
    static_assert(equal<short, ifel<true, short, long>>, "");
    static_assert(equal<long, ifel<false, short, long>>, "");
    
    static_assert(equal<int, if_not_void<int>>, "");
    static_assert(equal<int, if_not_void<void, int>>, "");
    
    template<bool if_, typename true_, typename = void>
    struct ifel_valid {
        static bool constexpr result = false; 
    };
    template<bool if_, typename true_>
    struct ifel_valid<if_, true_, to_void<ifel<if_, true_>>> {
        static bool constexpr result = true;
    };
    
    static_assert(ifel_valid<true, int>::result, "");
    static_assert(!ifel_valid<false, int>::result, "");
    
    template<typename if_, typename else_, typename = void>
    struct if_not_void_valid {
        static bool constexpr result = false; 
    };
    template<typename if_, typename else_>
    struct if_not_void_valid<if_, else_, to_void<if_not_void<if_, else_>>> {
        static bool constexpr result = true;
    };
    
    static_assert(if_not_void_valid<int, int>::result, "");
    static_assert(if_not_void_valid<int, void>::result, "");
    static_assert(if_not_void_valid<void, int>::result, "");
    static_assert(!if_not_void_valid<void, void>::result, "");
    
    template<typename a_> first<float, ifel<equal<float, a_>, void>> first_test(a_ const& a);
    template<typename a_> first<void, ifel<!equal<float, a_>, void>> first_test(a_ const& a);
    
    static_assert(equal<decltype(first_test(1.23f)), float>, "");
    static_assert(equal<decltype(first_test(123)), void>, "");
    
}}

}
#endif
