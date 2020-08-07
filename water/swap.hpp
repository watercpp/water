// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_SWAP_HPP
#define WATER_SWAP_HPP
namespace water {

/*

water::swap_from_swap is useful from within the swap member function of a class:

    class example {
        int my:
    public:
        void swap(example& a) {
            water::swap_from_swap(my, a.my);
        }
    };

*/

namespace _ { namespace swap_hidden_from_argument_dependent_lookup {

    template<typename a_, typename b_>
    void swap(a_& a, b_& b) {
        a_ s(static_cast<a_&&>(a));
        a = static_cast<a_&&>(b);
        b = static_cast<a_&&>(s);
    }

    template<typename t_, decltype(sizeof(0)) s_>
    void swap(t_ (&a)[s_], t_ (&b)[s_]) {
        decltype(sizeof(0)) i = 0;
        do swap(a[i], b[i]); while(++i != s_);
    }

}}

template<typename a_, typename b_>
void swap(a_& a, b_& b) {
    // this should be less specialized than any swap anywhere else worth considering
    _::swap_hidden_from_argument_dependent_lookup::swap(a, b);
}

template<typename type_>
void swap_from_swap(type_& a, type_& b) {
    swap(a, b);
}

}
#endif
