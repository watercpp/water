// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_TYPES1_IFEL_HPP
#define WATER_TYPES1_IFEL_HPP
#include <water/types1/result.hpp>
#include <water/types1/any_result.hpp>
#include <water/types1/to_bool.hpp>
namespace water { namespace types1 {

/*

ifel
if or if-else

if if_ == true, result true_, else result false_
- ifel<true, int>::result is int
- ifel<false, int> has no result
- ifel<false, int, long>::result is long

if true_ or false_ is a result, the result of this is that result
- ifel<true, types1::integer<int, 123> >::result is 123
- ifel<true, ifel<true, int> >::result is int

many ifels can be nested

  char const c = ...;
  typedef
    ifel<c == 'c', char,
    ifel<c == 'i', int,
    ifel<c == 'f', float
    ifel<c == 'b', bool,
    void
    > > > >::result type;

*/

template<bool if_, typename true_, typename false_ = nothing>
struct ifel :
    any_result<true_>
{};

template<typename true_, typename false_>
struct ifel<false, true_, false_> :
    any_result<false_>
{};

template<typename false_>
struct ifel<true, nothing, false_> :
    any_result<>
{};

template<typename true_>
struct ifel<false, true_, nothing> :
    any_result<>
{};

template<bool if2_, typename true2_, typename false2_, typename false_>
struct ifel<true, ifel<if2_, true2_, false2_>, false_> :
    ifel<if2_, true2_, false2_>
{};

template<typename true_, bool if2_, typename true2_, typename false2_>
struct ifel<false, true_, ifel<if2_, true2_, false2_> > :
    ifel<if2_, true2_, false2_>
{};

// ifel_type is like types1::ifel<types1::to_bool<if_>::result, true_, false_>
template<typename if_, typename true_, typename false_ = nothing>
struct ifel_type :
    ifel<to_bool<if_>::result, true_, false_>
{};

template<typename if_, typename true_, typename false_ = nothing>
struct ifel_type_not :
    ifel<!to_bool<if_>::result, true_, false_>
{};


// ifel_plain. like ifel but does not check if types are results.
//
// if if_ == true, result true_, else result false_
// - ifel_plain<true, int>::result is int
// - ifel_plain<false, int> has no result
// - ifel_plain<false, int, long>::result is long
//
// unlike types1::ifel this results the plain true_ or false_, it does not care if true_ or false_ are results
// - ifel_plain<true, types1::enum_result<123> >::result is types1::enum_result<123>
// - ifel<true, types1::enum_result<123> >::result is 123
template<bool if_, typename true_, typename false_ = nothing>
struct ifel_plain :
    type_plain<true_>
{};

template<typename true_, typename false_>
struct ifel_plain<false, true_, false_> :
    type_plain<false_>
{};

template<typename false_>
struct ifel_plain<true, nothing, false_> :
    any_result<>
{};

template<typename true_>
struct ifel_plain<false, true_, nothing> :
    any_result<>
{};


}}
#endif
