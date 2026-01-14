// Copyright 2026 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_VOID_RETURN_HPP
#define WATER_VOID_RETURN_HPP
namespace water {

/*

Useful if some function might return void or it might return a value.

    void function1() {};
    bool function2() { return true; };

    bool is_false = (function1(), void_return(false));
    bool is_true  = (function2(), void_return(false));

For example in a template function where the template argument can optionally return something that
converts to true or false to signal success or failure:

    template<typename write_>
    bool write_all_data(write_&& write) {
        while(true) {
            std::vector<unsigned char> data = get_more_data();
            if(data.empty())
                return true;
            auto ok = (write(data), void_return(true));
            if(!ok)
                return false;
        }
    }

Notice the extra parenthesis. They are needed, this uses an overloaded comma operator.

*/


template<typename type_ = void>
class void_return_value
{
    type_ my{};

public:
    
    explicit constexpr void_return_value(type_ const& a) :
        my{a}
    {}

    constexpr void_return_value() = default;
    
    constexpr operator type_ const&() const noexcept {
        return my;
    }
};

template<>
class void_return_value<void>
{};



template<typename type_>
constexpr void_return_value<type_> void_return(type_ const& a) {
    return void_return_value<type_>{a};
}

template<typename type_>
constexpr void_return_value<type_> void_return() {
    return {};
}

constexpr void_return_value<> void_return() noexcept {
    return {};
}



template<typename anything_, typename void_>
constexpr anything_&& operator,(anything_&& a, void_return_value<void_> const&) noexcept {
    return static_cast<anything_&&>(a);
}



namespace _ { namespace void_return_hpp_test {

    constexpr void void_function() {};
    constexpr bool bool_function() { return true; };

    static_assert((void_function(), void_return(false)) == false, "");
    static_assert((bool_function(), void_return(false)) == true, "");

}}

}
#endif
