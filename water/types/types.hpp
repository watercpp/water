// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_TYPES_TYPES_HPP
#define WATER_TYPES_TYPES_HPP
#include <water/types/ands.hpp>
#include <water/types/any_result.hpp>
#include <water/types/any_type.hpp>
#include <water/types/at.hpp>
#include <water/types/collection.hpp>
#include <water/types/equal.hpp>
#include <water/types/if_not_void.hpp>
#include <water/types/ifel.hpp>
#include <water/types/insert.hpp>
#include <water/types/link.hpp>
#include <water/types/list.hpp>
#include <water/types/next.hpp>
#include <water/types/nots.hpp>
#include <water/types/offset.hpp>
#include <water/types/ors.hpp>
#include <water/types/pull.hpp>
#include <water/types/remove.hpp>
#include <water/types/result.hpp>
#include <water/types/set.hpp>
#include <water/types/size.hpp>
#include <water/types/to_bool.hpp>
namespace water { namespace types {

// using namespace water::types::use;

namespace use {
    using types::ands;
    using types::any_result;
    using types::any_type;
    using types::at;
    using types::at_type;
    using types::bool_result;
    using types::equal;
    using types::equal_plain;
    using types::false_result;
    using types::if_not_void;
    using types::ifel;
    using types::ifel_plain;
    using types::ifel_type;
    using types::ifel_type_not;
    using types::insert;
    using types::is_collection;
    using types::is_result;
    using types::list;
    using types::next;
    using types::nontype;
    using types::nontype_tag;
    using types::nots;
    using types::offset;
    using types::ors;
    using types::pull;
    using types::remove;
    using types::result;
    using types::set;
    using types::size;
    using types::to_bool;
    using types::to_void;
    using types::true_result;
    using types::type;
    using types::type_plain;
    using types::type_tag;
    using types::value;
}

}}
#endif
